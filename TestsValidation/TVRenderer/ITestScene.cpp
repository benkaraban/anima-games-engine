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
#include <Core/Logger.h>
#include <Assets/SkeletonPose.h>
#include <Window/Tools.h>
#include <TVTools.h>
#include "ITestScene.h"

TextureGrabber::TextureGrabber(const Ptr<Core::VirtualFileSystem> & pVFS,
                               const Ptr<Renderer::IRenderer> & pRenderer) :
    _pVFS(pVFS),
    _pRenderer(pRenderer)
{
};

Ptr<Renderer::ITextureMap> TextureGrabber::getTexture(const String & name)
{
    if(name == String::EMPTY)
    {
        return null;
    }
    else if(_textures2D.count(name) > 0)
    {
        return _textures2D[name];
    }
    else
    {
        Ptr<Assets::Texture> pTexture(new Assets::Texture());
        _pVFS->read(name, *pTexture);

        Ptr<Renderer::ITextureMap> pResult = _pRenderer->uploadTexture(pTexture);
        _textures2D[name] = pResult;
        return pResult;
    }
};

Ptr<Renderer::ICubeTextureMap> TextureGrabber::getCubeTexture(const String & name)
{
    return Ptr<Renderer::ICubeTextureMap>(null);
};

ITestScene::ITestScene(const TestSceneData & data) :
    _pRenderer(data.pRenderer),
    _pInput(data.pInput),
    _initialised(false),
    _sensibilityRot(0.01f),
    _sensibilityXY(0.2f),
    _sensibilityZ(10.0f),
    _quakeCamSpeed(200.0f),
    _sensibilityLightColor(0.05f),
    _camType(CAMERA_ORBITAL),
    _pCameraCtrl(NULL),
    _camOrbit(Core::Vector3f(0.0, 0.0, 0.0), 200.0f),
    _camQuake(Core::Vector3f(0.0, 0.0, -200.0)),
    _matWorldCamera(Core::IDENTITY),
    _showBoxes(false)
{
    _pCameraCtrl = &_camOrbit;
    updateCamera();
}


void ITestScene::updateCamera()
{
    _matWorldCamera = _pCameraCtrl->getWorldMatrix();
}

void ITestScene::setCamera(ECameraType camType)
{
    _camType = camType;
    switch(_camType)
    {
    case CAMERA_ORBITAL: _pCameraCtrl = &_camOrbit; break;
    case CAMERA_QUAKE:   _pCameraCtrl = &_camQuake; break;
    }
    updateCamera();
}


void ITestScene::update(double elapsed)
{
    _pCamera->setAspectRatio(float(_pRenderer->getDefaultView()->getWidth())/float(std::max(1, _pRenderer->getDefaultView()->getHeight())));
    _pScene->update(elapsed);

    // Camera --------------------------------------------------------------

    if(_camType == CAMERA_QUAKE)
        Window::updateQuakeCamera(_camQuake, *_pInput, _quakeCamSpeed, _sensibilityRot, elapsed);
    else
        Window::updateOrbitalCamera(_camOrbit, *_pInput, _sensibilityXY, _sensibilityRot, _sensibilityZ, elapsed);

    updateCamera();

    // Lighting ------------------------------------------------------------

    Renderer::MainLightSettings lighting = _pScene->getLightSettings();
    Renderer::RendererSettings settings = _pRenderer->getRenderSettings();
    Renderer::DebugSettings debug = _pRenderer->getDebugSettings();
    Renderer::SkysphereSettings sky = _pScene->getSkysphereSettings();
    
    TVTools::updateLighting(lighting, *_pInput, _sensibilityLightColor);
    TVTools::updateRenderSettings(settings, *_pInput);
    TVTools::updateDebugSettings(debug, *_pInput);
    TVTools::updateSky(sky, *_pInput);

    _pScene->setLightSettings(lighting);
    _pScene->setSkysphereSettings(sky);
    _pRenderer->setRenderSettings(settings);
    _pRenderer->setDebugSettings(debug);


    if(_pInput->isKeyTyped('B'))
    {
        _showBoxes = !_showBoxes;
        setBoxesVisible(_showBoxes);
    }
    else if(_pInput->isKeyTyped('M'))
    {
        INF << L"-------------------------------------------------\n";
        INF << _pRenderer->getRenderStats();
    }
    else if(_pInput->isKeyTyped('R'))
    {
        _pRenderer->onDeviceLost();
        _pRenderer->onDeviceReset();
    }
    else if(_pInput->isKeyTyped('C'))
    {
        setCamera(ECameraType((_camType + 1) % ECameraType_COUNT));
    }
}

void ITestScene::setBoxesVisible(bool flag)
{
    for(Core::List<Ptr <Renderer::IAABox> >::const_iterator ipBox = _boxes.begin(); ipBox != _boxes.end(); ++ipBox)
    {
        if(*ipBox != null)
        {
            (*ipBox)->setColor(Core::Vector4f(0.0, 1.0, 0.0, 1.0));
            (*ipBox)->setVisible(flag);
        }
    }
}

Ptr<Renderer::ITextureMap> ITestScene::uploadTexture(const Ptr<Images::Image> & pImage, Workflow::ETextureType type)
{
    Workflow::TextureOptions options;
    options.texType = type;

    Ptr<Assets::Texture> pTexture(Workflow::generateTexture(options, pImage));
    return _pRenderer->uploadTexture(pTexture);
}


Ptr<Renderer::ITextureMap> ITestScene::loadTexture(const Ptr<Core::VirtualFileSystem> & pVFS, const String & fileName, Workflow::ETextureType type)
{
    Ptr<Core::InputStream> pInputImage(pVFS->readFile(fileName));

    if(pInputImage == null)
        throw Core::Exception(L"Unable to open file " + fileName);

    Ptr<Images::Image> pImage(new Images::Image);
    pImage->load((Core::SeekableInputStream &)*pInputImage);

    Workflow::TextureOptions options;
    options.texType = type;

    Ptr<Assets::Texture> pTexture(Workflow::generateTexture(options, pImage));
    return _pRenderer->uploadTexture(pTexture);
}

Ptr<Renderer::ITextureMap> ITestScene::loadPackedTexture(const Ptr<Core::VirtualFileSystem> & pVFS, const String & fileName)
{
    Ptr<Core::InputStream> pInput(pVFS->readFile(fileName));

    if(pInput == null)
        throw Core::Exception(L"Unable to open file " + fileName);

    Ptr<Assets::Texture> pTexture(new Assets::Texture());
    pTexture->read(*pInput);

    return _pRenderer->uploadTexture(pTexture);
}

void ITestScene::loadSkeleton(SkeletonInfos & infos, const Ptr<Core::VirtualFileSystem> & pVFS, 
                              const String & fileName, Renderer::ITextureGrabber & texGrabber,
                              bool skin)
{
    Ptr<Core::InputStream> pInput(pVFS->readFile(fileName));

    if(pInput == null)
        throw Core::Exception(L"Unable to open file " + fileName);

    infos.pModel = Ptr<Assets::Model>(new Assets::Model());
    infos.pModel->read(*pInput);

    infos.meshes.resize(infos.pModel->getMeshCount());
    infos.skins.resize(infos.pModel->getMeshCount());
    infos.nodes.resize(infos.pModel->getNodeCount());

    LM_ASSERT(infos.pModel->getSkeletonPtr() != null);
    infos.pSkeleton = _pScene->createSkeletonInstance(infos.pModel->getSkeletonPtr());

    for(int32 iMesh=0; iMesh < infos.meshes.size(); iMesh++)
    {
        if(infos.pModel->getMeshPtr(iMesh)->isSkin() && skin)
            infos.skins[iMesh] = _pRenderer->uploadSkinMesh(infos.pModel->getMeshPtr(iMesh), texGrabber);
        else
            infos.meshes[iMesh] = _pRenderer->uploadMesh(infos.pModel->getMeshPtr(iMesh), texGrabber);
    }

    for(int32 iNode=0; iNode < infos.nodes.size(); iNode++)
    {
        int32 meshId = infos.pModel->getNode(iNode).getMeshId();

        if(meshId != Assets::UNDEFINED_MESH_ID)
        {
            Assets::Material amat = infos.pModel->getMesh(meshId).getMaterial();
            Renderer::Material mat(amat);

            mat.pDiffuseMap  = (amat.diffuseTexName != String::EMPTY) ? loadPackedTexture(pVFS, amat.diffuseTexName) : null;
            mat.pSpecularMap = (amat.specularTexName != String::EMPTY) ? loadPackedTexture(pVFS, amat.specularTexName) : null;
            mat.pNormalMap   = (amat.normalTexName != String::EMPTY) ? loadPackedTexture(pVFS, amat.normalTexName) : null;

            if(infos.pModel->getMesh(meshId).isSkin() && skin)
            {
                Ptr<Renderer::ISkinMeshInstance> pSkin(_pScene->createSkinMeshInstance(infos.skins[meshId], infos.pSkeleton));
                pSkin->setMaterial(mat);
            }
            else
            {
                infos.nodes[iNode] = _pScene->createMeshInstance(infos.meshes[meshId]);
                infos.nodes[iNode]->setMaterial(mat);
            }
        }
    }

    animateSkeleton(infos, 0, 0.0f);
}

void ITestScene::animateSkeleton(SkeletonInfos & infos, int32 animId, float key)
{
    Assets::ModelPose pose;

    if(animId < infos.pModel->getAnimationCount())
        infos.pModel->getPose(pose, animId, key);
    else
        infos.pModel->getDefaultPose(pose);

    Ptr<Assets::SkeletonPose> pSkelPose(new Assets::SkeletonPose());
    infos.pModel->getSkeleton().getSkeletonPose(*pSkelPose, pose);
    infos.pSkeleton->setPose(pSkelPose);

    Core::Matrix4f skelMat(infos.pSkeleton->getWorldMatrix());

    for(int32 ii=0; ii < infos.nodes.size(); ii++)
    {
        if(infos.nodes[ii] != null)
            infos.nodes[ii]->setWorldMatrix(skelMat * pose.getNodeMatrix(ii));
    }
}

