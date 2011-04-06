/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
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
#include <Renderer/SM2/SceneSM2.h>
#include <Renderer/Common/Decal.h>
#include <Renderer/Common/Camera.h>
#include <Assets/SkeletonPose.h>
#include <Core/Logger.h>
#include <Core/Math/GeometryIntersect.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
SceneSM2::SceneSM2(const Ptr<Gfx::IDevice> & pDevice, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _pSkysphere(new Skysphere(pDevice, 16, pResLib)),
    _mustUpdateReflexions(true),
    _time(0.0),
    _waterLevel(0.f)
{
    _pStreamFreeForm = Ptr<StreamBuffer>(new StreamBuffer(_pDevice, _pResLib, sizeof(FreeFormVertex), 100000, 120000));
}
//-----------------------------------------------------------------------------
Ptr<IPerspectiveCamera> SceneSM2::createPerspectiveCamera(float fov, float ratio,
                                                          float near, float far)
{
    return Ptr<IPerspectiveCamera>(new PerspectiveCamera(fov, ratio, near, far));
}
//-----------------------------------------------------------------------------
Ptr<IOrthoCamera> SceneSM2::createOrthoCamera(float left, float right,
                                              float bottom, float top,
                                              float near, float far)
{
    return Ptr<IOrthoCamera>(new OrthoCamera(left, right, bottom, top, near, far));
}
//-----------------------------------------------------------------------------
Ptr<IMeshInstance> SceneSM2::createMeshInstance(const Ptr<IMesh> & pMesh)
{
    Ptr<MeshInstanceSM2> pResult(new MeshInstanceSM2(pMesh, _pResLib));
    _meshes.push_back(pResult);
    return pResult;
}
//-----------------------------------------------------------------------------
void SceneSM2::getAllMeshes(Core::List<MeshInstanceSM2 *> & meshes)
{
    for(int32 ii=0; ii < _meshes.size(); ii++)
        meshes.push_back(_meshes[ii].get());
}
//-----------------------------------------------------------------------------
void SceneSM2::getMeshes(Core::List<MeshInstanceSM2 *> & meshes, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _meshes.size(); ii++)
        if(_meshes[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_meshes[ii]->getBoundingBox(), frustum))
            meshes.push_back(_meshes[ii].get());
}
//-----------------------------------------------------------------------------
void SceneSM2::getSkinMeshes(Core::List<SkinMeshInstanceSM2 *> & meshes, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _skinMeshes.size(); ii++)
        if(_skinMeshes[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_skinMeshes[ii]->getBoundingBox(), frustum))
            meshes.push_back(_skinMeshes[ii].get());
}
//-----------------------------------------------------------------------------
Ptr<ISkeletonInstance> SceneSM2::createSkeletonInstance(const Ptr<Assets::Skeleton> & pSkeleton)
{
    Ptr<SkeletonInstance> pResult(new SkeletonInstance(pSkeleton));
    _skeletons.push_back(pResult);
    return pResult;
}
//-----------------------------------------------------------------------------
Ptr<ISkinMeshInstance> SceneSM2::createSkinMeshInstance(const Ptr<ISkinMesh> & pMesh,
                                                        const Ptr<ISkeletonInstance> & pSkeleton)
{
    Ptr<SkinMeshInstanceSM2> pResult(new SkinMeshInstanceSM2(pMesh, pSkeleton, _pResLib));
    _skinMeshes.push_back(pResult);
    Ptr<ISkinMeshInstance> pResult2(pResult);
    return pResult2;
}
//-----------------------------------------------------------------------------
Ptr<IPointLight> SceneSM2::createPointLight()
{
    Ptr<PointLight> pResult(new PointLight());
    _pointLights.push_back(pResult);
    return pResult;
}
//-----------------------------------------------------------------------------
void SceneSM2::getPointLights(Core::List<PointLight *> & lights, const Core::Frustumf & frustum)
{
    lights.clear();
    for(int32 ii=0; ii < _pointLights.size(); ii++)
        lights.push_back(_pointLights[ii].get());
}
//-----------------------------------------------------------------------------
Ptr<IGrassCluster> SceneSM2::createGrassCluster(const Core::List<GrassPatch> & patches,
                                                const Ptr<ITextureMap> & pTexture)
{
    Ptr<GrassCluster> result(new GrassCluster(_pDevice, patches, pTexture, _pResLib));
    _grass.push_back(result);
    return result;
}
//-----------------------------------------------------------------------------
void SceneSM2::getGrassClusters(Core::List<GrassCluster *> & grass, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _grass.size(); ii++)
        if(_grass[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_grass[ii]->getBoundingBox(), frustum))
            grass.push_back(_grass[ii].get());
}
//-----------------------------------------------------------------------------
Ptr<IBillboardGroup> SceneSM2::createBillboardGroup(int32 maxSize,
                                                    const Ptr<ITextureMap> & pTexture,
                                                    EBillboardMode mode)
{
    Ptr<BillboardGroup> result(new BillboardGroup(maxSize, mode, pTexture, _pDevice, _pResLib));
    _bills.push_back(result);
    return result;
}
//-----------------------------------------------------------------------------
void SceneSM2::getBillboardGroups(Core::List<BillboardGroup *> & bills, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _bills.size(); ii++)
        if(_bills[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_bills[ii]->getBoundingBox(), frustum))
            bills.push_back(_bills[ii].get());
}
//-----------------------------------------------------------------------------
Ptr<IDecal> SceneSM2::createDecal(const Ptr<ITextureMap> & pTexture, EDecalMode mode)
{
    Ptr<Decal> result(new Decal(pTexture, mode));
    _decals.push_back(result);
    return result;
}
//-----------------------------------------------------------------------------
Ptr<IFreeForm> SceneSM2::createFreeForm(const Ptr<ITextureMap> & pTexture, EFreeFormMode mode, bool isLongLife)
{
    Ptr<FreeForm> result;
    if(isLongLife)
        result = Ptr<FreeForm>(new FreeForm(1000, 1000, mode, pTexture, _pDevice, _pResLib));
    else
        result = Ptr<FreeForm>(new FreeForm(1000, 1000, mode, pTexture, _pStreamFreeForm));
    
    _freeForms.push_back(result);
    return result;
}
//-----------------------------------------------------------------------------
void SceneSM2::getDecals(Core::List<Decal *> & decals, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _decals.size(); ii++)
        if(_decals[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_decals[ii]->getBoundingBox(), frustum))
            decals.push_back(_decals[ii].get());
}
//-----------------------------------------------------------------------------
void SceneSM2::getFreeForms(Core::List<FreeForm *> & freeForms, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _freeForms.size(); ii++)
        if(_freeForms[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_freeForms[ii]->getBoundingBox(), frustum))
            freeForms.push_back(_freeForms[ii].get());
}
//-----------------------------------------------------------------------------
void SceneSM2::getSplines(Core::List<Spline *> & splines, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _splines.size(); ii++)
        if(_splines[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_splines[ii]->getBoundingBox(), frustum))
            splines.push_back(_splines[ii].get());
}
//-----------------------------------------------------------------------------
Ptr<IWaterPatch> SceneSM2::createWaterPatch(float width, float height)
{
    Ptr<WaterPatch> pWater(new WaterPatch(width, height));
    pWater->setWaterLevel(_waterLevel);
    _water.push_back(pWater);

    return pWater;
}
//-----------------------------------------------------------------------------
void SceneSM2::getWaterPatches(Core::List<WaterPatch *> & water, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _water.size(); ii++)
        if(_water[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_water[ii]->getBoundingBox(), frustum))
            water.push_back(_water[ii].get());
}
//-----------------------------------------------------------------------------
Ptr<IAABox> SceneSM2::createAABox()
{
    Ptr<AABox> result(new AABox());
    _boxes.push_back(result);
    return result;
}
//-----------------------------------------------------------------------------
void SceneSM2::getAABoxes(Core::List<AABox *> & boxes, const Core::Frustumf & frustum)
{
    for(int32 ii=0; ii < _boxes.size(); ii++)
        if(_boxes[ii]->isVisible() && Core::intersect_AABoxFrustum_Fast(_boxes[ii]->getBox(), frustum))
            boxes.push_back(_boxes[ii].get());
}
//-----------------------------------------------------------------------------
const SkysphereSettings & SceneSM2::getSkysphereSettings()
{
    return _pSkysphere->getSettings();
}
//-----------------------------------------------------------------------------
void SceneSM2::setSkysphereSettings(const SkysphereSettings & settings)
{
    if(settings != _pSkysphere->getSettings())
    {
        _pSkysphere->update(settings);
        _mustUpdateReflexions = true;
    }
}
//-----------------------------------------------------------------------------
Ptr<ISpline> SceneSM2::createSpline()
{
    Ptr<Spline> pResult(new Spline());
    _splines.push_back(pResult);
    return pResult;
}
//-----------------------------------------------------------------------------
const Skysphere & SceneSM2::getSkysphere() const
{
    return *_pSkysphere;
}
//-----------------------------------------------------------------------------
const MainLightSettings & SceneSM2::getLightSettings() const
{
    return _lightSettings;
}
//-----------------------------------------------------------------------------
void SceneSM2::setLightSettings(const MainLightSettings & settings)
{
    if(settings != _lightSettings)
    {
        _lightSettings = settings;
        _mustUpdateReflexions = true;
    }
}
//-----------------------------------------------------------------------------
const FogSettings & SceneSM2::getFogSettings()
{
    return _fogSettings;
}
//-----------------------------------------------------------------------------
void SceneSM2::setFogSettings(const FogSettings & settings)
{
    _fogSettings = settings;
}
//-----------------------------------------------------------------------------
void SceneSM2::cleanup()
{
#define CLEAN_LIST(list) \
    for(int32 ii=0; ii < list.size(); ) \
    { \
        if(list[ii]->isAlive()) \
            ii++; \
        else \
            list.qerase(ii); \
    }

    CLEAN_LIST(_meshes);
    CLEAN_LIST(_skinMeshes);
    CLEAN_LIST(_skeletons);
    CLEAN_LIST(_decals);
    CLEAN_LIST(_freeForms);
    CLEAN_LIST(_bills);
    CLEAN_LIST(_pointLights);
    CLEAN_LIST(_grass);
    CLEAN_LIST(_water);
    CLEAN_LIST(_boxes);
    CLEAN_LIST(_splines);

    // Remove PointLight influence data
    for(int32 ii=0; ii < _meshes.size(); ii++)
    {
        LightingData & lighting = _meshes[ii]->getLightingData();
        lighting.pointLights.clear();
    }

    for(int32 ii=0; ii < _skinMeshes.size(); ii++)
    {
        LightingData & lighting = _skinMeshes[ii]->getLightingData();
        lighting.pointLights.clear();
    }

#undef CLEAN_LIST
}
//-----------------------------------------------------------------------------
bool SceneSM2::mustUpdateReflexions() const
{
    return _mustUpdateReflexions;
}
//-----------------------------------------------------------------------------
void SceneSM2::globalReflexionsUpdated()
{
    _mustUpdateReflexions = false;
}
//-----------------------------------------------------------------------------
void SceneSM2::update(double elapsed)
{
    _time += elapsed;
}
//-----------------------------------------------------------------------------
double SceneSM2::getTime() const
{
    return _time;
}
//-----------------------------------------------------------------------------
void SceneSM2::setWindSettings(int32 windId, const WindSettings & settings)
{
    LM_ASSERT(windId <= WIND_COUNT);
    _windSettings[windId] = settings;
}
//-----------------------------------------------------------------------------
const WindSettings & SceneSM2::getWindSettings(int32 windId) const
{
    LM_ASSERT(windId <= WIND_COUNT);
    return _windSettings[windId];
}
//-----------------------------------------------------------------------------
float SceneSM2::getWaterLevel() const
{
    return _waterLevel;
}
//-----------------------------------------------------------------------------
void SceneSM2::setWaterLevel(float level)
{
    _waterLevel = level;
    for(Core::List<Ptr<WaterPatch> >::const_iterator ipWater = _water.begin(); ipWater != _water.end(); ++ipWater)
        (*ipWater)->setWaterLevel(level);
}
//-----------------------------------------------------------------------------
void SceneSM2::getAABox(Core::AABoxf & box) const
{
    box.clear();

    for(int32 ii=0; ii < _meshes.size(); ii++)
        box.setUnion(_meshes[ii]->getBoundingBox());

    for(int32 ii=0; ii < _skinMeshes.size(); ii++)
        box.setUnion(_skinMeshes[ii]->getBoundingBox());

    for(int32 ii=0; ii < _decals.size(); ii++)
        box.setUnion(_decals[ii]->getBoundingBox());

    for(int32 ii=0; ii < _bills.size(); ii++)
        box.setUnion(_bills[ii]->getBoundingBox());

    for(int32 ii=0; ii < _grass.size(); ii++)
        box.setUnion(_grass[ii]->getBoundingBox());

    for(int32 ii=0; ii < _water.size(); ii++)
        box.setUnion(_water[ii]->getBoundingBox());
}
//-----------------------------------------------------------------------------
}
