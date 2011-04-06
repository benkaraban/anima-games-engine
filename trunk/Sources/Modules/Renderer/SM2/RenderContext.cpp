/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <Renderer/SM2/RenderContext.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
RenderContext::RenderContext(
        const Ptr<IRenderTarget> & pTarget,
        const Core::List<Ptr<SceneSM2> > & scenes,
        const Ptr<ICamera> & pCamera,
        int32 width,
        int32 height,
        float time,
        int32 flags
)
:
    _pRenderTarget(pTarget.get()),
    _viewportWidth(width),
    _viewportHeight(height),
    _light(scenes[0]->getLightSettings()),
    _fog(scenes[0]->getFogSettings(), pCamera->getNearPlane(), pCamera->getFarPlane()),
    _time(time),
    _waterLevel(scenes[0]->getWaterLevel()),
    _isDynamicShadowOn((flags & CTX_DYNAMIC_SHADOW_ON) != 0),
    _pSky(&scenes[0]->getSkysphere())
{
    const Ptr<SceneSM2> & pScene = scenes[0];

    pCamera->getWorldMatrix(_world);
    pCamera->getViewMatrix(_view);
    pCamera->getProjectionMatrix(_proj);

    if(flags & WATER_CONTEXT)
    {
        Core::Matrix4f reflexionMat(
                1.0f,  0.0f,  0.0f,  0.0f,
                0.0f, -1.0f,  0.0f,  2.0f * _waterLevel,
                0.0f,  0.0f,  1.0f,  0.0f,
                0.0f,  0.0f,  0.0f,  1.0f);
        _view = _view * reflexionMat;
    }

    _fog._color *= _light.getGlobalFade();
    _viewProj = _proj * _view;

    _eye = _world.getTVector();
    _eyeDir = _world.getZVector();

    for(int32 ii=0; ii < WIND_COUNT; ii++)
        _windSettings[ii] = pScene->getWindSettings(ii);

    Core::matrixToFrustum(_viewProj, _frustum);

    Core::List<MeshInstanceSM2 *> meshes;
    Core::List<SkinMeshInstanceSM2 *> skinMeshes;

    for(int32 ii=0; ii < scenes.size(); ii++)
    {
        scenes[ii]->getMeshes(meshes, _frustum);
        scenes[ii]->getSkinMeshes(skinMeshes, _frustum);
        scenes[ii]->getDecals(_decals, _frustum);
        scenes[ii]->getFreeForms(_freeForms, _frustum);
        scenes[ii]->getBillboardGroups(_bills, _frustum);
        scenes[ii]->getGrassClusters(_grass, _frustum);
        scenes[ii]->getWaterPatches(_water, _frustum);
        scenes[ii]->getAABoxes(_boxes, _frustum);
        scenes[ii]->getSplines(_splines, _frustum);
    }

    for(int32 ii=0; ii < meshes.size(); ii++)
    {
        if(meshes[ii]->getMaterial().matType == Assets::MAT_STANDARD)
            _meshes.push_back(meshes[ii]);
        else
            _customMeshes.push_back(meshes[ii]);
    }

    for(int32 ii=0; ii < skinMeshes.size(); ii++)
    {
        if(skinMeshes[ii]->getMaterial().matType == Assets::MAT_STANDARD)
            _skinMeshes.push_back(skinMeshes[ii]);
        else
            _customSkinMeshes.push_back(skinMeshes[ii]);
    }

    _isReflectionMapNeeded = false;

    for(int32 ii=0; ii < _water.size(); ii++)
        _isReflectionMapNeeded |= _water[ii]->isReflective();

    // Dynamic shadow map -----------------------------------------------------

    _lightWorld.setZVec(_light.getDirection(), Core::Vector3f(0.0, 1.0, 0.0));
    _lightView = _lightWorld.getInverted();

    Core::AABoxf lightWorldBox;
    
    bool dynamicShadowCaster   = false;
    bool dynamicShadowReceiver = false;

    for(int32 iMesh=0; iMesh < _meshes.size(); iMesh++)
    {
        const Material & material = _meshes[iMesh]->getRenderableMaterial();

        if(material.dynamicShadowCaster || material.dynamicShadowReceiver)
            lightWorldBox.setUnion(_meshes[iMesh]->getBoundingBox());

        dynamicShadowCaster = dynamicShadowCaster || material.dynamicShadowCaster;
        dynamicShadowReceiver = dynamicShadowReceiver || material.dynamicShadowReceiver;
    }

    for(int32 iMesh=0; iMesh < _skinMeshes.size(); iMesh++)
    {
        const Material & material = _skinMeshes[iMesh]->getRenderableMaterial();

        if(material.dynamicShadowCaster || material.dynamicShadowReceiver)
            lightWorldBox.setUnion(_skinMeshes[iMesh]->getBoundingBox());

        dynamicShadowCaster = dynamicShadowCaster || material.dynamicShadowCaster;
        dynamicShadowReceiver = dynamicShadowReceiver || material.dynamicShadowReceiver;
    }

    _isDynamicShadowMapNeeded = dynamicShadowReceiver;
    _isDynamicShadowOn = _isDynamicShadowOn && _isDynamicShadowMapNeeded;

    if(_isDynamicShadowOn)
    {
        float near0 = pCamera->getNearPlane();
        float far0  = near0 + pScene->getLightSettings().dynamicShadowMiddle;
        float near1 = far0  - pScene->getLightSettings().dynamicShadowMiddleBlend;
        float far1  = near0 + pScene->getLightSettings().dynamicShadowEnd;

        getShadowSplit(_lightProj0, _dynamicShadowSplitSize0, near0, far0, pCamera);
        getShadowSplit(_lightProj1, _dynamicShadowSplitSize1, near1, far1, pCamera);
    }
    else
    {
        _lightProj0.setIdentity();
        _lightProj1.setIdentity();
        
        _dynamicShadowSplitSize0 = 0.0f;
        _dynamicShadowSplitSize1 = 0.0f;
    }

    _lightViewProj0 = _lightProj0 * _lightView;
    _lightViewProj1 = _lightProj1 * _lightView;

    // Static shadow map  -----------------------------------------------------
    {
        // Récupère les 8 points du Frustum
        Core::List<Core::Vector3f> pts;

        Core::Matrix4f invSplitViewProj = _viewProj.getInverted();

        pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f, -1.0f,  0.0f, 1.0f)).homogeneousDiv());
        pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f, -1.0f,  0.0f, 1.0f)).homogeneousDiv());
        pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f,  1.0f,  0.0f, 1.0f)).homogeneousDiv());
        pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f,  1.0f,  0.0f, 1.0f)).homogeneousDiv());
        pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv());
        pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv());
        pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv());
        pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv());

        // Bounding box des 8 points du Frustum split dans l'espace light view
        Core::AABoxf lightViewBox;

        for(int32 ii=0; ii < pts.size(); ii++)
            lightViewBox.addPoint(_lightView.apply(pts[ii]));


        // World box de tous les opbjets de la scène
        Core::List<MeshInstanceSM2 *> allMeshes;
        Core::AABoxf worldBox;
        scenes[0]->getAllMeshes(allMeshes);

        for(int32 ii=0; ii < allMeshes.size(); ii++)
            worldBox.setUnion(allMeshes[ii]->getBoundingBox());

        worldBox.setInter(lightViewBox);

        Core::AABoxf lvb(worldBox, _lightView); // Light view box
        const Core::Vector3f & p1 = lvb.min;
        const Core::Vector3f & p2 = lvb.max;
        _staticLightProj.setOrthoProj(p1.x, p2.x, p1.y, p2.y, p1.z, p2.z);
        _staticLightViewProj = _staticLightProj * _lightView;
    }

    setStats();
}
//-----------------------------------------------------------------------------
RenderContext::RenderContext(
    ELightmapGenerationContext ctx, 
    const Ptr<IRenderTarget> & pTarget,
    const Core::List<Ptr<SceneSM2> > & scenes,
    const Ptr<ICamera> & pCamera,
    const Ptr<IMeshInstance> & pMesh,
    int32 width,
    int32 height,
    float time
)
:
    _pRenderTarget(pTarget.get()),
    _viewportWidth(width),
    _viewportHeight(height),
    _light(scenes[0]->getLightSettings()),
    _fog(scenes[0]->getFogSettings(), pCamera->getNearPlane(), pCamera->getFarPlane()),
    _time(time),
    _waterLevel(scenes[0]->getWaterLevel()),
    _isDynamicShadowOn(false),
    _pSky(&scenes[0]->getSkysphere())
{
    const Ptr<SceneSM2> & pScene = scenes[0];

    pCamera->getWorldMatrix(_world);
    pCamera->getViewMatrix(_view);
    pCamera->getProjectionMatrix(_proj);

    _eye = _world.getTVector();
    _eyeDir = _world.getZVector();
    _isReflectionMapNeeded = false;
    _isDynamicShadowMapNeeded = false;
    _isDynamicShadowOn = false;
    _viewProj = _proj * _view;
    Core::matrixToFrustum(_viewProj, _frustum);

    for(int32 ii=0; ii < WIND_COUNT; ii++)
        _windSettings[ii] = pScene->getWindSettings(ii);

    _meshes.push_back(LM_DEBUG_CAST<MeshInstanceSM2*>(pMesh.get()));

    _lightWorld.setZVec(_light.getDirection(), Core::Vector3f(0.0, 1.0, 0.0));
    _lightView = _lightWorld.getInverted();

    // Static shadow map  -----------------------------------------------------
    {
        Core::List<MeshInstanceSM2 *> allMeshes;
        Core::AABoxf worldBoxA;
        scenes[0]->getAllMeshes(allMeshes);

        for(int32 ii=0; ii < allMeshes.size(); ii++)
            worldBoxA.setUnion(allMeshes[ii]->getBoundingBox());

        Core::AABoxf lvbA(worldBoxA, _lightView); // Light view box


        Core::AABoxf worldBoxB(pMesh->getBoundingBox());
        Core::AABoxf lvbB(worldBoxB, _lightView);

        const Core::Vector3f & p1A = lvbA.min;
        const Core::Vector3f & p2A = lvbA.max;

        const Core::Vector3f & p1B = lvbB.min;
        const Core::Vector3f & p2B = lvbB.max;

        _staticLightProj.setOrthoProj(p1B.x, p2B.x, p1B.y, p2B.y, p1A.z, p2A.z);
        //_staticLightProj.setOrthoProj(p1A.x, p2A.x, p1A.y, p2A.y, p1A.z, p2A.z);
        _staticLightViewProj = _staticLightProj * _lightView;
    }

    setStats();
}
//-----------------------------------------------------------------------------
RenderContext::RenderContext(EStaticShadowContext, const RenderContext & context, const Core::List<Ptr<SceneSM2> > & scenes) :
    _pRenderTarget(context.getRenderTarget()),
    _viewportWidth(context.getViewportWidth()),
    _viewportHeight(context.getViewportHeight()),
    _lightWorld(context.getLightWorld()),
    _lightView(context.getLightView()),
    _lightProj0(context.getLightProj0()),
    _lightProj1(context.getLightProj1()),
    _lightViewProj0(context.getLightViewProj0()),
    _lightViewProj1(context.getLightViewProj1()),
    _dynamicShadowSplitSize0(context.getDynamicShadowSplitSize0()),
    _dynamicShadowSplitSize1(context.getDynamicShadowSplitSize1()),
    _world(context.getLightWorld()),
    _view(context.getLightView()),
    _proj(context.getStaticLightProj()),
    _viewProj(context.getStaticLightViewProj()),
    _eye(context.getEye()),
    _light(context.getLight()),
    _fog(context.getFog()),
    _time(context.getTime()),
    _waterLevel(context.getWaterLevel()),
    _isReflectionMapNeeded(false),
    _isDynamicShadowOn(context.isDynamicShadowOn()),
    _pSky(context.getSkysphere())
{
    for(int32 ii=0; ii < WIND_COUNT; ii++)
        _windSettings[ii] = context.getWindSettings(ii);

    Core::matrixToFrustum(_viewProj, _frustum);
    _meshes.clear();
    scenes[0]->getAllMeshes(_meshes);

    // TODO
    //for(int32 iMesh=0; iMesh < skinMeshes.size(); iMesh++)
    //    if(skinMeshes[iMesh]->getRenderableMaterial().dynamicShadowCaster && !skinMeshes[iMesh]->getRenderableMaterial().dynamicShadowReceiver)
    //        _skinMeshes.push_back(skinMeshes[iMesh]);

    _isDynamicShadowMapNeeded = false;
    _isDynamicShadowOn = false;

    setStats();
}
//-----------------------------------------------------------------------------
RenderContext::RenderContext(EDynamicShadowContext, const RenderContext & context, const Core::List<Ptr<SceneSM2> > & scenes, int32 split) :
    _pRenderTarget(context.getRenderTarget()),
    _viewportWidth(context.getViewportWidth()),
    _viewportHeight(context.getViewportHeight()),
    _lightWorld(context.getLightWorld()),
    _lightView(context.getLightView()),
    _lightProj0(context.getLightProj0()),
    _lightProj1(context.getLightProj1()),
    _lightViewProj0(context.getLightViewProj0()),
    _lightViewProj1(context.getLightViewProj1()),
    _dynamicShadowSplitSize0(context.getDynamicShadowSplitSize0()),
    _dynamicShadowSplitSize1(context.getDynamicShadowSplitSize1()),
    _world(context.getLightWorld()),
    _view(context.getLightView()),
    _eye(context.getEye()),
    _light(context.getLight()),
    _fog(context.getFog()),
    _time(context.getTime()),
    _waterLevel(context.getWaterLevel()),
    _isReflectionMapNeeded(false),
    _isDynamicShadowOn(context.isDynamicShadowOn()),
    _pSky(context.getSkysphere())
{
    //const Ptr<SceneSM2> & pScene = scenes[0];

    switch(split)
    {
    case 0:
        _proj = context.getLightProj0();
        _viewProj = context.getLightViewProj0();
        break;
    case 1:
        _proj = context.getLightProj1();
        _viewProj = context.getLightViewProj1();
        break;
    default:
        LM_ASSERT(false);
        break;
    }

    for(int32 ii=0; ii < WIND_COUNT; ii++)
        _windSettings[ii] = context.getWindSettings(ii);

    Core::matrixToFrustum(_viewProj, _frustum);

    _meshes.clear();
    _skinMeshes.clear();

    Core::List<MeshInstanceSM2 *>     meshes;
    Core::List<SkinMeshInstanceSM2 *> skinMeshes;

    for(int32 ii=0; ii < scenes.size(); ii++)
    {
        scenes[ii]->getMeshes(meshes, _frustum);
        scenes[ii]->getSkinMeshes(skinMeshes, _frustum);
    }

    for(int32 iMesh=0; iMesh < meshes.size(); iMesh++)
        if(meshes[iMesh]->getRenderableMaterial().dynamicShadowCaster && !meshes[iMesh]->getRenderableMaterial().dynamicShadowReceiver
            && !meshes[iMesh]->getRenderableMaterial().staticLighting)
            _meshes.push_back(meshes[iMesh]);

    for(int32 iMesh=0; iMesh < skinMeshes.size(); iMesh++)
        if(skinMeshes[iMesh]->getRenderableMaterial().dynamicShadowCaster && !skinMeshes[iMesh]->getRenderableMaterial().dynamicShadowReceiver)
            _skinMeshes.push_back(skinMeshes[iMesh]);

    _isDynamicShadowMapNeeded = !_meshes.empty() || !_skinMeshes.empty();
    _isDynamicShadowOn = _isDynamicShadowOn && _isDynamicShadowMapNeeded;

    setStats();
}
//-----------------------------------------------------------------------------
void RenderContext::getShadowSplit(Core::Matrix4f & lightProj, float & splitSize, float near, float far, const Ptr<ICamera> & pCamera)
{
    near = std::max(0.001f, near);

    Core::Matrix4f splitProj;
    pCamera->getProjectionMatrixSplit(splitProj, near, far);

    Core::Matrix4f splitViewProj = splitProj * _view;
    Core::Matrix4f invSplitViewProj = splitViewProj.getInverted();

    // Récupère les 8 points du Frustum split
    Core::List<Core::Vector3f> pts;

    pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f, -1.0f,  0.0f, 1.0f)).homogeneousDiv());
    pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f, -1.0f,  0.0f, 1.0f)).homogeneousDiv());
    pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f,  1.0f,  0.0f, 1.0f)).homogeneousDiv());
    pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f,  1.0f,  0.0f, 1.0f)).homogeneousDiv());
    pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv());
    pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv());
    pts.push_back(invSplitViewProj.apply(Core::Vector4f(-1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv());
    pts.push_back(invSplitViewProj.apply(Core::Vector4f( 1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv());

    // Bounding box des 8 points du Frustum split dans l'espace light view
    Core::AABoxf lightViewBox;

    for(int32 ii=0; ii < pts.size(); ii++)
        lightViewBox.addPoint(_lightView.apply(pts[ii]));
        
    Core::Vector3f center = lightViewBox.getCenter();
    Core::Vector3f size = lightViewBox.getSize();

    splitSize = 0.5f * std::max(size.x, size.y);

    lightProj.setOrthoProj(center.x - splitSize, center.x + splitSize, 
                           center.y - splitSize, center.y + splitSize,
                           center.z - 2.0f * size.z -20.0f, center.z + 0.5f*size.z);
}
//-----------------------------------------------------------------------------
void RenderContext::setStats()
{
    _stats.clear();
    _stats << L"visible obj {";
    if(!_meshes.empty())     _stats << L" meshes :" << Core::toStringPadSpaces(_meshes.size(), 3);
    if(!_skinMeshes.empty()) _stats << L" skins :" << Core::toStringPadSpaces(_skinMeshes.size(), 3);
    if(!_decals.empty())     _stats << L" decals : " << Core::toStringPadSpaces(_decals.size(), 3);
    if(!_bills.empty())      _stats << L" bills : " << Core::toStringPadSpaces(_bills.size(), 3);
    if(!_grass.empty())      _stats << L" grass : " << Core::toStringPadSpaces(_grass.size(), 3);
    if(!_water.empty())      _stats << L" watter : " << Core::toStringPadSpaces(_water.size(), 3);
    if(!_boxes.empty())      _stats << L" boxes : " << Core::toStringPadSpaces(_boxes.size(), 3);
    if(!_splines.empty())    _stats << L" splines : " << Core::toStringPadSpaces(_splines.size(), 3);
    if(!_freeForms.empty())  _stats << L" fforms : " << Core::toStringPadSpaces(_freeForms.size(), 3);
    _stats << L"}\n";
}
//-----------------------------------------------------------------------------
}
