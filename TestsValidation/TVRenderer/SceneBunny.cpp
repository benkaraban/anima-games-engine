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

#include "SceneBunny.h"

#include <Core/Logger.h>
#include <Core/Math/Transform.h>

#include <Assets/Tool.h>

#include <Core/Math/Color.h>

#include <Workflow/ModelImporterX.h>
#include <Workflow/ModelOptimize.h>
#include <Workflow/TexCompression.h>

SceneBunny::SceneBunny(const TestSceneData & data) :
    ITestScene(data),
    _worldMatTeapot(Core::IDENTITY),
    _angle(0.0)
{

}

bool SceneBunny::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        TextureGrabber texGrabber(pVFS, _pRenderer);
        Workflow::ModelImporterX importX;
        Ptr<Core::InputStream> pInputModel(pVFS->readFile(L"/Data/TUData/Modeles/bunny-teapot.x"));
        Ptr<Assets::Model> pModel(importX.importModel(*pInputModel));

        Workflow::ModelPackingOptions modelOptions;
        Workflow::packModel(modelOptions, *pModel);

        int32 meshCount = pModel->getMeshCount();
        int32 nodeCount = pModel->getNodeCount();

        Core::List<Ptr<Renderer::IMesh> >         meshes;
        Core::List<Ptr<Renderer::IMeshInstance> > nodes;

        for(int32 ii=0; ii < meshCount; ii++)
            meshes.push_back(_pRenderer->uploadMesh(pModel->getMeshPtr(ii), texGrabber));

        nodes.resize(nodeCount);

        for(int32 ii=0; ii < nodeCount; ii++)
        {
            int32 meshId = pModel->getNode(ii).getMeshId();
            if(meshId != Assets::UNDEFINED_MESH_ID)
                nodes[ii] = _pScene->createMeshInstance(meshes[meshId]);
        }

        Assets::ModelPose pose;
        pModel->getDefaultPose(pose);


        for(int32 ii=0; ii < nodeCount; ii++)
        {
            if(nodes[ii] != null)
            {
                nodes[ii]->setWorldMatrix(pose.getNodeMatrix(ii));
                Ptr<Renderer::IAABox> pBox = _pScene->createAABox();
                pBox->setBox(nodes[ii]->getBoundingBox());
                _boxes.push_back(pBox);
            }
        }

        int32 idBunny = pModel->getNodeId(L"bunny");
        int32 idTeapot1 = pModel->getNodeId(L"teapot1");
        int32 idTeapot2 = pModel->getNodeId(L"teapot2");
        int32 idHoly = pModel->getNodeId(L"holy");

        if(idBunny != Assets::UNDEFINED_NODE_ID)
        {
            Renderer::Material mat;
            mat.diffuse   = Core::cvtSRGB2Linear(Core::Vector4f(0.60f, 0.60f, 0.60f, 1.00f));
            mat.specular  = Core::cvtSRGB2Linear(Core::Vector4f(0.40f, 0.40f, 0.40f, 1.00f));
            mat.shininess = 10.0f;

            nodes[idBunny]->setMaterial(mat);
        }

        if(idTeapot1 != Assets::UNDEFINED_NODE_ID)
        {
            Renderer::Material mat;
            mat.diffuse    = Core::cvtSRGB2Linear(Core::Vector4f(0.40f, 0.00f, 0.00f, 1.00f));
            mat.specular   = Core::cvtSRGB2Linear(Core::Vector4f(0.60f, 0.60f, 0.60f, 1.00f));
            mat.shininess  = 45.0f;
            mat.reflexions = Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.7f, 0.7f, 1.0f));

            nodes[idTeapot1]->setMaterial(mat);
        }

        if(idTeapot2 != Assets::UNDEFINED_NODE_ID)
        {
            Renderer::Material mat;
            mat.diffuse    = Core::cvtSRGB2Linear(Core::Vector4f(0.42f, 0.64f, 0.87f, 1.00f));
            mat.specular   = Core::cvtSRGB2Linear(Core::Vector4f(0.40f, 0.40f, 0.40f, 1.00f));
            mat.shininess  = 45.0f;
            mat.glowColor  = Core::Vector4f(0.25f, 0.25f, 0.5f, 1.00f);
            mat.glowExtent = Core::Vector3f(5.0f, 5.0f, 5.0f);

            nodes[idTeapot2]->setMaterial(mat);
        }

        if(idHoly != Assets::UNDEFINED_NODE_ID)
        {
            Renderer::Material mat;
            mat.diffuse   = Core::cvtSRGB2Linear(Core::Vector4f(0.80f, 0.80f, 0.10f, 1.00f));
            mat.specular  = Core::cvtSRGB2Linear(Core::Vector4f(0.80f, 0.80f, 0.80f, 1.00f));
            mat.shininess = 80.0f;

            nodes[idHoly]->setMaterial(mat);
        }

        setBoxesVisible(false);

        // Fog
        Renderer::FogSettings fog(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.3f, 0.3f, 1.0f)), 100.0f, 2000.0f, 0.0f);
        _pScene->setFogSettings(fog);

        // Lumière principale

        Renderer::MainLightSettings settings;
        settings.ambient  = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.0f);
        settings.diffuse  = Core::Vector4f(0.65f, 0.65f, 0.65f, 0.0f);
        settings.specular = Core::Vector4f(0.80f, 0.80f, 0.80f, 0.0f);

        _pScene->setLightSettings(settings);

        // SkySphere

        Ptr<Renderer::ITextureMap> pSkyTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));
        _pScene->setSkysphereSettings(Renderer::SkysphereSettings(pSkyTexture, -0.25));

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 1.0, 10000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising teapot scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

void SceneBunny::render()
{
    Core::Vector3f translation;
    Core::Quaternionf rotation;
    Core::Vector3f scale;
    Core::Quaternionf scaleOrientation;

    //pour le moment on récupère la translation de manière un poil brutal
    _matWorldCamera.getTransformSafe(translation, rotation, scale, scaleOrientation);

    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


