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
#include <Core/Math/Color.h>
#include <Workflow/ModelImporterX.h>
#include <Workflow/TexCompression.h>
#include "SceneLights.h"

SceneLights::SceneLights(const TestSceneData & data) :
    ITestScene(data),
    _worldMatTeapot(Core::IDENTITY),
    _angle(0.0),
    _alpha1(0.00),
    _alpha2(1.50),
    _alpha3(2.00),
    _alpha4(3.00)
{
    updateCamera();
}

bool SceneLights::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        // Texture

        Ptr<Renderer::ITextureMap> pDiffuseTexture(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-diffuse.jpg", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pNormalTexture(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-normal.jpg", Workflow::TEXTURE_NORMALMAP));
        Ptr<Renderer::ITextureMap> pDecalTexture(loadTexture(pVFS, L"/Data/TUData/Textures/part00.tga", Workflow::TEXTURE_COLORMAP));

        // Modèle

        Workflow::ModelImporterX importX;
        Ptr<Core::InputStream> pInputModel(pVFS->readFile(L"/Data/TUData/Modeles/teapot.x"));
        Ptr<Assets::Model> pModelTeapot(importX.importModel(*pInputModel));

        int32 idTeapotMesh = pModelTeapot->getMeshId(L"Teapot01");

        if(idTeapotMesh == Assets::UNDEFINED_NODE_ID)
            throw Core::Exception(L"Model does not contain objet 'Teapot01'");

        Ptr<Assets::ModelMesh> pMesh = pModelTeapot->getMeshPtr(idTeapotMesh);

        for(int32 ii=0; ii < pMesh->getLODCount(); ii++)
        {
            Assets::VerticeSet & vset = pMesh->getLODByID(ii);
            Assets::makeTBNBasis(vset, false);
        }

        TextureGrabber texGrabber(pVFS, _pRenderer);
        Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(pMesh, texGrabber);

        // Lumière principale

        Renderer::MainLightSettings settings;
        settings.ambient = Core::Vector4f(0.0, 0.0, 0.0, 0.0);
        settings.diffuse = Core::Vector4f(0.0, 0.0, 0.0, 0.0);
        settings.specular = Core::Vector4f(0.0, 0.0, 0.0, 0.0);

        _pScene->setLightSettings(settings);

        // Lumières additionnelles

        _pLight1 = _pScene->createPointLight();
        _pLight1->setRadius(150.0);

        _pLight1->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)));
        _pLight1->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.2f, 0.2f, 1.0f)));
        _pLight1->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.4f, 0.4f, 1.0f)));

        _pLight2 = _pScene->createPointLight();
        _pLight2->setRadius(150.0);

        _pLight2->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)));
        _pLight2->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(0.6f, 0.6f, 1.0f, 1.0f)));
        _pLight2->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));

        _pLight3 = _pScene->createPointLight();
        _pLight3->setRadius(150.0);

        _pLight3->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.05f, 0.0f, 1.0f)));
        _pLight3->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f)));
        _pLight3->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));

        _pLight4 = _pScene->createPointLight();
        _pLight4->setRadius(150.0);

        _pLight4->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)));
        _pLight4->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 1.0f, 1.0f, 1.0f)));
        _pLight4->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));

        // Decals pour visualiser les lumières

        _pDecal1 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);
        _pDecal2 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);
        _pDecal3 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);
        _pDecal4 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);

        _pDecal1->setSize(10.0);
        _pDecal2->setSize(10.0);
        _pDecal3->setSize(10.0);
        _pDecal4->setSize(10.0);

        _pDecal1->setColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.50f)));
        _pDecal2->setColor(Core::cvtSRGB2Linear(Core::Vector4f(0.6f, 0.6f, 1.0f, 0.50f)));
        _pDecal3->setColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 1.0f, 0.0f, 0.50f)));
        _pDecal4->setColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 1.0f, 1.0f, 0.50f)));

        // Instances

        _pMeshInstanceTeapot1 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot2 = _pScene->createMeshInstance(pRenderMesh);

        Renderer::Material mat1;
        mat1.diffuse    = Core::cvtSRGB2Linear(Core::Vector4f(0.42f, 0.64f, 0.87f, 1.00f));
        mat1.specular   = Core::cvtSRGB2Linear(Core::Vector4f(0.40f, 0.40f, 0.40f, 1.00f));
        mat1.shininess  = 45.0f;
        mat1.pNormalMap = pNormalTexture;

        Renderer::Material mat2;
        mat2.diffuse     = Core::cvtSRGB2Linear(Core::Vector4f(0.90f, 0.80f, 0.80f, 1.00f));
        mat2.specular    = Core::cvtSRGB2Linear(Core::Vector4f(0.20f, 0.20f, 0.20f, 1.00f));
        mat2.shininess   = 90.0f;
        mat2.pDiffuseMap = pDiffuseTexture;
        mat2.pNormalMap  = pNormalTexture;

        _pMeshInstanceTeapot1->setMaterial(mat1);
        _pMeshInstanceTeapot2->setMaterial(mat2);

        Core::Matrix4f world1(Core::TRANSLATION, -100.0, 0.0, 0.0);
        Core::Matrix4f world2(Core::TRANSLATION,  100.0, 0.0, 0.0);

        _pMeshInstanceTeapot1->setWorldMatrix(world1);
        _pMeshInstanceTeapot2->setWorldMatrix(world2);

        _boxes.push_back(_pScene->createAABox());
        _boxes.push_back(_pScene->createAABox());

        _boxes[0]->setBox(_pMeshInstanceTeapot1->getBoundingBox());
        _boxes[1]->setBox(_pMeshInstanceTeapot2->getBoundingBox());

        // Spline

        Core::CRSpline spline;
        spline.getPoints().push_back(Core::Vector3f(0.0f, 0.0f, 0.0f));
        spline.getPoints().push_back(Core::Vector3f(0.0f, 0.0f, 0.0f));
        spline.getPoints().push_back(Core::Vector3f(0.0f, 0.0f, 0.0f));
        spline.getPoints().push_back(Core::Vector3f(100.0f, 50.0f, 50.0f));
        spline.getPoints().push_back(Core::Vector3f(150.0f, 150.0f, -100.0f));
        spline.getPoints().push_back(Core::Vector3f(250.0f, 50.0f, 0.0f));
        spline.getPoints().push_back(Core::Vector3f(250.0f, 50.0f, 0.0f));
        spline.getPoints().push_back(Core::Vector3f(250.0f, 50.0f, 0.0f));

        _pSpline = _pScene->createSpline();
        _pSpline->setResolution(200);
        _pSpline->setColor(Core::Vector4f(0.5f, 0.5f, 1.0f, 1.0f));
        _pSpline->setSpline(spline);
        _pSpline->setWorldMatrix(world1);

        setBoxesVisible(false);

        _pCamera = _pScene->createPerspectiveCamera(f_PI_DIV_4, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 1.0, 1000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising teapot scene : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}

void SceneLights::update(double elapsed)
{
    ITestScene::update(elapsed);

    _alpha1 += 0.50f * float(elapsed);
    _alpha2 += 1.35f * float(elapsed);
    _alpha3 += 0.95f * float(elapsed);
    _alpha4 += 0.65f * float(elapsed);

    _pLight1->setPosition(Core::Vector3f(0.0f, 100.0f * sin(_alpha1), 0.0f));
    _pLight2->setPosition(Core::Vector3f(100.0f + 100.0f * cos(_alpha2), 0.0f, 100.0f * sin(_alpha2)));
    _pLight3->setPosition(Core::Vector3f(100.0f, 100.0f * sin(_alpha3), 100.0f * cos(_alpha3)));
    _pLight4->setPosition(Core::Vector3f(300.0f + 100.0f * cos(_alpha4), 0.0f, 100.0f * sin(_alpha4)));

    Core::Matrix4f world1(Core::TRANSLATION, _pLight1->getPosition());
    Core::Matrix4f world2(Core::TRANSLATION, _pLight2->getPosition());
    Core::Matrix4f world3(Core::TRANSLATION, _pLight3->getPosition());
    Core::Matrix4f world4(Core::TRANSLATION, _pLight4->getPosition());

    _pDecal1->setWorldMatrix(world1);
    _pDecal2->setWorldMatrix(world2);
    _pDecal3->setWorldMatrix(world3);
    _pDecal4->setWorldMatrix(world4);
}

void SceneLights::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


