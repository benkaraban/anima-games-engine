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
#include <Assets/Tool.h>
#include <Assets/SkeletonPose.h>
#include <Images/Image.h>
#include <Core/Math/Color.h>
#include <Workflow/ModelImporterX.h>
#include <Workflow/TexCompression.h>
#include <Workflow/ModelOptimize.h>
#include "SceneAnimation.h"

SceneAnimation::SceneAnimation(const TestSceneData & data) :
    ITestScene(data),
    _key(0.0),
    _alpha1(0.0)
{
}

bool SceneAnimation::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        // Chargement modèle

        TextureGrabber texGrabber(pVFS, _pRenderer);
        Workflow::ModelImporterX importX;
        Ptr<Core::InputStream> pInputModel(pVFS->readFile(L"/Data/TUData/Modeles/skinned.X"));
        Ptr<Assets::Model> pModel(importX.importModel(*pInputModel));

        Workflow::ModelPackingOptions modelOptions;
        modelOptions.maxBonesPerBlendingGroup = 4;

        Workflow::packModel(modelOptions, *pModel);


        // Chargement textures

        Ptr<Renderer::ITextureMap> pDiffuseTexture(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-diffuse.jpg", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pNormalTexture(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-normal.jpg", Workflow::TEXTURE_NORMALMAP));
        Ptr<Renderer::ITextureMap> pDecalTexture(loadTexture(pVFS, L"/Data/TUData/Textures/part00.tga", Workflow::TEXTURE_COLORMAP));

        // Création modèle

        _pModel = pModel;
        _pSkeleton = _pScene->createSkeletonInstance(pModel->getSkeletonPtr());

        int32 meshCount = pModel->getMeshCount();
        int32 nodeCount = pModel->getNodeCount();

        Core::List<Ptr<Renderer::IMesh> >     meshes;
        Core::List<Ptr<Renderer::ISkinMesh> > skins;

        meshes.resize(meshCount);
        skins.resize(meshCount);

        for(int32 ii=0; ii < meshCount; ii++)
        {
            if(pModel->getMeshPtr(ii)->isSkin())
                skins[ii] = _pRenderer->uploadSkinMesh(pModel->getMeshPtr(ii), texGrabber);
            else
                meshes[ii] = _pRenderer->uploadMesh(pModel->getMeshPtr(ii), texGrabber);
        }

        _boxes.resize(nodeCount);
        _nodes.resize(nodeCount);
        _skinNodes.resize(nodeCount);

        Renderer::Material mat;
        mat.diffuse     = Core::cvtSRGB2Linear(Core::Vector4f(0.50f, 0.70f, 1.00f, 1.00f));
        mat.specular    = Core::cvtSRGB2Linear(Core::Vector4f(0.10f, 0.10f, 0.10f, 1.00f));
        mat.shininess   = 8.0f;
        mat.pDiffuseMap = pDiffuseTexture;
        mat.pNormalMap  = pNormalTexture;

        for(int32 ii=0; ii < nodeCount; ii++)
        {
            int32 meshId = pModel->getNode(ii).getMeshId();

            if(meshId != Assets::UNDEFINED_MESH_ID)
            {
                Ptr<Renderer::IAABox> pBox = _pScene->createAABox();

                if(pModel->getMesh(meshId).isSkin())
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

        Assets::ModelPose pose;
        pModel->getDefaultPose(pose);

        for(int32 ii=0; ii < nodeCount; ii++)
        {
            if(_nodes[ii] != null)
            {
                _nodes[ii]->setWorldMatrix(pose.getNodeMatrix(ii));
            }
        }

        setBoxesVisible(true);

        // Lumière

        _pLight1 = _pScene->createPointLight();
        _pLight1->setRadius(150.0);

        _pLight1->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)));
        _pLight1->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.2f, 0.2f, 1.0f)));
        _pLight1->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.4f, 0.4f, 1.0f)));

        // Lumière principale

        Renderer::MainLightSettings settings;
        settings.ambient  = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.0f);
        settings.diffuse  = Core::Vector4f(0.10f, 0.10f, 0.10f, 0.0f);
        settings.specular = Core::Vector4f(0.10f, 0.10f, 0.10f, 0.0f);

        _pScene->setLightSettings(settings);

        // Decals pour visualiser les lumières

        _pDecal1 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);
        _pDecal1->setSize(10.0);
        _pDecal1->setColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.50f)));

        // Caméra

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 1.0, 1000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising teapot scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}


void SceneAnimation::update(double elapsed)
{
    ITestScene::update(elapsed);

    if(_pModel->getAnimationCount() >= 1)
    {
        Assets::ModelPose pose;
        _pModel->getPose(pose, 0, _key);

        Ptr<Assets::SkeletonPose> pSkelPose(new Assets::SkeletonPose());
        _pModel->getSkeleton().getSkeletonPose(*pSkelPose, pose);
        _pSkeleton->setPose(pSkelPose);

        for(int32 ii=0; ii < _nodes.size(); ii++)
        {
            if(_skinNodes[ii] != null)
            {
                _boxes[ii]->setBox(_skinNodes[ii]->getBoundingBox());
            }
            if(_nodes[ii] != null)
            {
                _nodes[ii]->setWorldMatrix(pose.getNodeMatrix(ii));
                _boxes[ii]->setBox(_nodes[ii]->getBoundingBox());
            }
        }

        _key += float(elapsed) / _pModel->getAnimationDuration(0);
        while(_key > 1.0f)
            _key -= 1.0f;
    }

    static const float RADIUS = 50.0;
    _alpha1 += 0.50f * float(elapsed);
    _pLight1->setPosition(Core::Vector3f(RADIUS * cos(_alpha1), 50.0, RADIUS * sin(_alpha1)));

    Core::Matrix4f world1(Core::TRANSLATION, _pLight1->getPosition());
    _pDecal1->setWorldMatrix(world1);
}

void SceneAnimation::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}



