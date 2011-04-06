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
#include "SceneCOLLADA.h"

#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Math/Random.h>
#include <Core/Math/MathTools.h>
#include <Core/StopWatch.h>

#include <Assets/Tool.h>
#include <Assets/SkeletonPose.h>

#include <Core/Math/Color.h>

#include <Workflow/COLLADA/ModelImporterCOLLADA.h>
#include <Workflow/ModelImporterOBJ.h>
#include <Workflow/ModelImporterX.h>
#include <Workflow/ModelOptimize.h>
#include <Workflow/TexCompression.h>

const float MESH_DIMENSION   = 150.0;

#define PACKING_ON 0

SceneCOLLADA::SceneCOLLADA(const TestSceneData & data) :
    ITestScene(data)
{
    _camQuake.setPosition(Core::Vector3f(0.0f, 0.0f, -20.0f));
    _quakeCamSpeed = 5.0f;
    _sensibilityXY = 0.02f;
    _sensibilityZ = 0.5f;
    _key = 0.0f;
}

bool SceneCOLLADA::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        // VFS2
        Ptr<Core::VFSDirectMapping> pVFS2 = Ptr<Core::VFSDirectMapping>(new Core::VFSDirectMapping());
        pVFS2->linkDirectoryThrow(L"Data/AssetsPacked");

        TextureGrabber texGrabber(pVFS2, _pRenderer);
        _pScene = _pRenderer->createScene();

        // Fog
        {
            Renderer::FogSettings fog(Core::Vector4f(0.0, 0.0, 0.0, 0.0), 10.0, 200.0, 0.0);
            _pScene->setFogSettings(fog);
        }

        // Light
        {
            Renderer::MainLightSettings light;
            light.ambient  = Core::Vector4f(0.05f, 0.05f, 0.05f, 0.00f);
            light.diffuse  = Core::Vector4f(0.70f, 0.70f, 0.70f, 0.00f);
            light.specular = Core::Vector4f(0.70f, 0.70f, 0.70f, 0.00f);
            light.sunAngleToNorth = 0.7f;
            light.sunAngleToZenith = 0.5f;
            light.shadowMiddle = 20.0f;
            light.shadowMiddleBlend = 6.0f;
            light.shadowEnd = 58.0f;
            light.shadowEndBlend = 5.0f;

            _pScene->setLightSettings(light);
        }

        
        // SkySphere
        {
            Ptr<Renderer::ITextureMap> pSkyTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));
            _pScene->setSkysphereSettings(Renderer::SkysphereSettings(pSkyTexture, -0.25));
        }

        _pVFS = pVFS;
        initModel(L"/Data/TUData/Modeles/simpleanim.dae");

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 1.0, 5000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising grass scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

Ptr<Assets::Model> SceneCOLLADA::loadModel(const String & modelName)
{
    Ptr<Assets::Model> pModel;
    String modelNameUp(modelName);
    modelNameUp.toUpper();

    Ptr<Core::InputStream> pInputModel(_pVFS->readFile(modelName));

    if(modelNameUp.endsWith(L".DAE"))
    {
        Workflow::ModelImporterCOLLADA importerCOLLADA;
        pModel = importerCOLLADA.importModel(*pInputModel);
    }
    else if(modelNameUp.endsWith(L".OBJ"))
    {
        Workflow::ModelImporterOBJ importerOBJ;
        pModel = importerOBJ.importModel(*pInputModel);
    }
    else if(modelNameUp.endsWith(L".X"))
    {
        Workflow::ModelImporterX importerX;
        pModel = importerX.importModel(*pInputModel);
    }
    else if(modelNameUp.endsWith(L".MDL"))
    {
        pModel = Ptr<Assets::Model>(new Assets::Model());
        pModel->read(*pInputModel);
    }
    else
    {
        throw Core::Exception(L"Unsupported import format ");
    }

    return pModel;
}

void SceneCOLLADA::initModel(const String & modelName, const String & modelName2)
{
    // Nettoyage

    for(int32 ii=0; ii < _skinNodes.size(); ii++)
        if(_skinNodes[ii] != null)
            _skinNodes[ii]->kill();

    for(int32 ii=0; ii < _boxes.size(); ii++)
        if(_boxes[ii] != null)
            _boxes[ii]->kill();

    for(int32 ii=0; ii < _nodes.size(); ii++)
        if(_nodes[ii] != null)
            _nodes[ii]->kill();

    _nodes.clear();
    _skinNodes.clear();
    _boxes.clear();
    _pSkeleton = null;
    _pModel = null;


    // Chargement fichier
    _pModel = loadModel(modelName);

    if(modelName2 != String::EMPTY)
    {
        Ptr<Assets::Model> pModel2(loadModel(modelName2));
        _pModel->mergeAnimations(*pModel2);
    }

    INF << "-----------------------------------------------\n";
    INF << "Model animations :\n";

    for(int32 ii=0; ii < _pModel->getAnimationCount(); ii++)
        INF << " - " << _pModel->getAnimationName(ii) << L"\n";


#if PACKING_ON
    Workflow::ModelPackingOptions packOptions;
    packOptions.pObs = Ptr<Core::IProgressObs>(new Core::ProgressPrint());
    Workflow::packModel(packOptions, *_pModel);
#endif

    TextureGrabber texGrabber(_pVFS, _pRenderer);
    _pSkeleton = _pScene->createSkeletonInstance(_pModel->getSkeletonPtr());

    int32 meshCount = _pModel->getMeshCount();
    int32 nodeCount = _pModel->getNodeCount();

    Core::List<Ptr<Renderer::IMesh> >     meshes;
    Core::List<Ptr<Renderer::ISkinMesh> > skins;

    meshes.resize(meshCount);
    skins.resize(meshCount);

    for(int32 ii=0; ii < meshCount; ii++)
    {
        if(_pModel->getMeshPtr(ii)->isSkin())
            skins[ii] = _pRenderer->uploadSkinMesh(_pModel->getMeshPtr(ii), texGrabber);
        else
            meshes[ii] = _pRenderer->uploadMesh(_pModel->getMeshPtr(ii), texGrabber);
    }

    _boxes.resize(nodeCount);
    _nodes.resize(nodeCount);
    _skinNodes.resize(nodeCount);

    Renderer::Material mat;
    mat.diffuse     = Core::cvtSRGB2Linear(Core::Vector4f(0.50f, 0.70f, 1.00f, 1.00f));
    mat.specular    = Core::cvtSRGB2Linear(Core::Vector4f(0.10f, 0.10f, 0.10f, 1.00f));
    mat.shininess   = 8.0f;

    for(int32 ii=0; ii < nodeCount; ii++)
    {
        int32 meshId = _pModel->getNode(ii).getMeshId();

        if(meshId != Assets::UNDEFINED_MESH_ID)
        {
            Ptr<Renderer::IAABox> pBox = _pScene->createAABox();

            if(_pModel->getMesh(meshId).isSkin())
            {
                _skinNodes[ii] = _pScene->createSkinMeshInstance(skins[meshId], _pSkeleton);
                _skinNodes[ii]->setMaterial(mat);
                pBox->setBox(_skinNodes[ii]->getBoundingBox());
            }
            else
            {
                _nodes[ii] = _pScene->createMeshInstance(meshes[meshId]);
                _nodes[ii]->setMaterial(mat);
                pBox->setBox(_nodes[ii]->getBoundingBox());
            }

            _boxes[ii] = pBox;
        }
    }

    setBoxesVisible(false);

    Assets::ModelPose pose;
    _pModel->getDefaultPose(pose);

    for(int32 ii=0; ii < nodeCount; ii++)
    {
        if(_nodes[ii] != null)
        {
            _nodes[ii]->setWorldMatrix(pose.getNodeMatrix(ii));
        }
    }

    _camOrbit = Core::CameraOrbital(Core::Vector3f(0.0, 0.0, 0.0), 20.0f);
    _animId = 0;
    updateCamera();
}

void SceneCOLLADA::update(double elapsed)
{
    ITestScene::update(elapsed);

    if     (_pInput->isKeyTyped('1')) initModel(L"/Data/TUData/Modeles/Test-orientation.DAE");
    else if(_pInput->isKeyTyped('2')) initModel(L"/Data/TUData/Modeles/Test-orientation2.DAE");
    else if(_pInput->isKeyTyped('3')) initModel(L"/Data/TUData/Modeles/simplescene.DAE");
    else if(_pInput->isKeyTyped('4')) initModel(L"/Data/TUData/Modeles/simplescene.OBJ");
    else if(_pInput->isKeyTyped('5')) initModel(L"/Data/TUData/Modeles/simpleanim.DAE", L"/Data/TUData/Modeles/simpleanim2.DAE");
    else if(_pInput->isKeyTyped('6')) initModel(L"/Data/TUData/Modeles/Arene_tentures.DAE");
    else if(_pInput->isKeyTyped('7')) initModel(L"/Data/TUData/Modeles/sam1.DAE");
    else if(_pInput->isKeyTyped('8')) initModel(L"/Data/TUData/Modeles/sam1.X");
    else if(_pInput->isKeyTyped('9')) initModel(L"/Data/AssetsPacked/TUData/Models/simplescene.mdl");
    else if(_pInput->isKeyTyped('0')) _animId = (_animId + 1) % std::max(1, _pModel->getAnimationCount());


    if(_pModel->getAnimationCount() >= 1 && _pSkeleton != null)
    {
        Assets::ModelPose pose;
        _pModel->getPose(pose, _animId, _key);

        Ptr<Assets::SkeletonPose> pSkelPose(new Assets::SkeletonPose());
        _pModel->getSkeleton().getSkeletonPose(*pSkelPose, pose);
        _pSkeleton->setPose(pSkelPose);

        for(int32 ii=0; ii < _nodes.size(); ii++)
        {
            if(_skinNodes[ii] != null)
            {
                Core::AABoxf box(_skinNodes[ii]->getBoundingBox());
                _boxes[ii]->setBox(_skinNodes[ii]->getBoundingBox());
                _boxes[ii]->setColor(Core::Vector4f(0.0f, 1.0f, 1.0f, 1.0f));
            }
            if(_nodes[ii] != null)
            {
                _nodes[ii]->setWorldMatrix(pose.getNodeMatrix(ii));
                _boxes[ii]->setBox(_nodes[ii]->getBoundingBox());
            }
        }

        _key += float(elapsed) / _pModel->getAnimationDuration(_animId);
        while(_key > 1.0f)
            _key -= 1.0f;
    }

    for(int32 ii=0; ii < _nodes.size(); ii++)
    {
        if(_skinNodes[ii] != null)
        {
            _boxes[ii]->setBox(_skinNodes[ii]->getBoundingBox());
            _boxes[ii]->setColor(Core::Vector4f(0.0f, 1.0f, 1.0f, 1.0f));
        }
        if(_nodes[ii] != null)
        {
            _boxes[ii]->setBox(_nodes[ii]->getBoundingBox());
        }
    }
}

void SceneCOLLADA::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


