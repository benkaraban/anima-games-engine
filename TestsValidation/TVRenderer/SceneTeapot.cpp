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
#include <Workflow/ModelOptimize.h>
#include <Workflow/TexCompression.h>
#include "SceneTeapot.h"

SceneTeapot::SceneTeapot(const TestSceneData & data) :
    ITestScene(data),
    _worldMatTeapot(Core::IDENTITY)
{
    _camOrbit.zoomInOut(-400.0);
    updateCamera();
}

bool SceneTeapot::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        // Textures

        Ptr<Renderer::ITextureMap> pNormalTextureDXT5(loadPackedTexture(pVFS, L"/Data/AssetsPacked/TUData/Textures/Water2.tex"));
        Ptr<Renderer::ITextureMap> pDiffuseTexture(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-diffuse.jpg", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pSpecularTexture(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-specular.jpg", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pNormalTexture(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-normal.jpg", Workflow::TEXTURE_NORMALMAP));
        Ptr<Renderer::ITextureMap> pDiffuseAlphaTex(loadTexture(pVFS, L"/Data/TUData/Textures/decal00.tga", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pDecalTexture(loadTexture(pVFS, L"/Data/TUData/Textures/part00.tga", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pSkyTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));

        Ptr<Renderer::ITextureMap> pIceNormal(loadTexture(pVFS, L"/Data/TUData/Textures/ice-01-height.jpg", Workflow::TEXTURE_HEIGHTMAP));
        Ptr<Renderer::ITextureMap> pIceDiffuse(loadTexture(pVFS, L"/Data/TUData/Textures/ice-01-diffuse.jpg", Workflow::TEXTURE_COLORMAP));

        // Sky

        _pScene->setSkysphereSettings(Renderer::SkysphereSettings(pSkyTexture, -0.25));


        // Modèle

        Workflow::ModelImporterX importX;
        Ptr<Core::InputStream> pInputModel(pVFS->readFile(L"/Data/TUData/Modeles/teapot.x"));
        Ptr<Assets::Model> pModelTeapot(importX.importModel(*pInputModel));

        int32 idTeapotMesh = pModelTeapot->getMeshId(L"Teapot01");

        if(idTeapotMesh == Assets::UNDEFINED_NODE_ID)
            throw Core::Exception(L"Model does not contain objet 'Teapot01'");

        Ptr<Assets::ModelMesh> pMesh = pModelTeapot->getMeshPtr(idTeapotMesh);

        INF << L"Vertices  : " << Core::toString(pMesh->getVertexCount()) << L"\n";
        INF << L"Triangles : " << Core::toString(pMesh->getTriangleCount()) << L"\n";

        Workflow::ModelPackingOptions modelOptions;

        Workflow::packModel(modelOptions, *pModelTeapot);

        INF << L"Vertices  : " << Core::toString(pMesh->getVertexCount()) << L"\n";
        INF << L"Triangles : " << Core::toString(pMesh->getTriangleCount()) << L"\n";

        TextureGrabber texGrabber(pVFS, _pRenderer);
        Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(pMesh, texGrabber);

        // Lumière principale

        Renderer::MainLightSettings settings;
        settings.ambient  = Core::Vector4f(0.05f, 0.05f, 0.05f, 0.0f);
        settings.diffuse  = Core::Vector4f(0.50f, 0.50f, 0.50f, 0.0f);
        settings.specular = Core::Vector4f(0.80f, 0.80f, 0.80f, 0.0f);
        settings.bloomThreshold = 0.1f;
        settings.bloomFactor = 0.0f;
        //settings.bloomThreshold = 0.1f;
        //settings.bloomFactor = 5.0f;

        _pScene->setLightSettings(settings);

        // Instances

        _pMeshInstanceTeapot01 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot02 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot03 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot04 = _pScene->createMeshInstance(pRenderMesh);

        _pMeshInstanceTeapot05 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot06 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot07 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot08 = _pScene->createMeshInstance(pRenderMesh);

        _pMeshInstanceTeapot09 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot10 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot11 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot12 = _pScene->createMeshInstance(pRenderMesh);

        _pMeshInstanceTeapot13 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot14 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot15 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot16 = _pScene->createMeshInstance(pRenderMesh);

        _pMeshInstanceTeapot17 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot18 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot19 = _pScene->createMeshInstance(pRenderMesh);
        _pMeshInstanceTeapot20 = _pScene->createMeshInstance(pRenderMesh);

        Renderer::Material mat1;
        mat1.glowColor = Core::cvtSRGB2Linear(Core::Vector4f(0.42f, 0.64f, 0.87f, 1.00f));
        mat1.diffuse   = Core::cvtSRGB2Linear(Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f));
        mat1.specular  = Core::cvtSRGB2Linear(Core::Vector4f(0.40f, 0.40f, 0.40f, 1.00f));
        mat1.shininess = 45.0f;
        //mat1.zBias     = 1.0f;

        Renderer::Material mat2;
        mat2.diffuse     = Core::cvtSRGB2Linear(Core::Vector4f(0.80f, 0.80f, 0.80f, 1.00f));
        mat2.specular    = Core::cvtSRGB2Linear(Core::Vector4f(0.20f, 0.20f, 0.20f, 1.00f));
        mat2.shininess   = 16.0f;
        mat2.pDiffuseMap = pDiffuseTexture;

        Renderer::Material mat3(mat2);
        mat3.specular     = Core::cvtSRGB2Linear(Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f));
        mat3.shininess    = 16.0f;
        mat3.pSpecularMap = pSpecularTexture;

        Renderer::Material mat4;
        mat4.diffuse     = Core::cvtSRGB2Linear(Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f));
        mat4.specular    = Core::cvtSRGB2Linear(Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f));
        mat4.shininess   = 45.0f;
        mat4.refracColor = Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 1.0f, 1.00f, 0.60f));

        _pMeshInstanceTeapot01->setMaterial(mat1);
        _pMeshInstanceTeapot02->setMaterial(mat2);
        _pMeshInstanceTeapot03->setMaterial(mat3);
        _pMeshInstanceTeapot04->setMaterial(mat4);

        mat1.glowColor  = Core::cvtSRGB2Linear(Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f));
        mat1.diffuse    = Core::cvtSRGB2Linear(Core::Vector4f(0.42f, 0.64f, 0.87f, 1.00f));
        mat1.zBias      = 0.0f;
        mat1.pNormalMap = null;//pNormalTextureDXT5;
        mat2.pNormalMap = pNormalTexture;
        mat3.pNormalMap = pNormalTexture;

        mat4.diffuse     = Core::cvtSRGB2Linear(Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f));
        mat4.specular    = Core::cvtSRGB2Linear(Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f));
        mat4.shininess   = 12.0f;
        mat4.pDiffuseMap = pIceDiffuse;
        mat4.refracColor = Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 1.0f, 1.00f, 0.75f));

        _pMeshInstanceTeapot05->setMaterial(mat1);
        _pMeshInstanceTeapot06->setMaterial(mat2);
        _pMeshInstanceTeapot07->setMaterial(mat3);
        _pMeshInstanceTeapot08->setMaterial(mat4);

        mat1.diffuse.a = 0.5;
        mat2.diffuse.a = 0.5;
        mat3.pDiffuseMap = pDiffuseAlphaTex;
        mat3.transDiffuseTex = true;
        mat4.pNormalMap  = pIceNormal;

        _pMeshInstanceTeapot09->setMaterial(mat1);
        _pMeshInstanceTeapot10->setMaterial(mat2);
        _pMeshInstanceTeapot11->setMaterial(mat3);
        _pMeshInstanceTeapot12->setMaterial(mat4);

        mat1.diffuse      = Core::cvtSRGB2Linear(Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f));
        mat1.reflexions   = Core::cvtSRGB2Linear(Core::Vector4f(0.42f, 0.64f, 0.87f, 1.00f));
        mat1.pNormalMap   = null;

        mat2.diffuse      = Core::cvtSRGB2Linear(Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f));
        mat2.reflexions   = Core::cvtSRGB2Linear(Core::Vector4f(0.80f, 0.80f, 0.80f, 1.00f));
        mat2.pDiffuseMap  = null;

        mat3.reflexions   = Core::cvtSRGB2Linear(Core::Vector4f(0.80f, 0.80f, 0.80f, 1.00f));
        mat3.diffuse      = Core::cvtSRGB2Linear(Core::Vector4f(0.40f, 0.40f, 0.40f, 1.00f));
        mat3.specular     = Core::cvtSRGB2Linear(Core::Vector4f(0.40f, 0.40f, 0.40f, 1.00f));
        mat3.pDiffuseMap  = pDiffuseTexture;
        mat3.pSpecularMap = null;
        mat3.transDiffuseTex = false;

        mat4.reflexions  = Core::cvtSRGB2Linear(Core::Vector4f(0.5f));

        _pMeshInstanceTeapot13->setMaterial(mat1);
        _pMeshInstanceTeapot14->setMaterial(mat2);
        _pMeshInstanceTeapot15->setMaterial(mat3);
        _pMeshInstanceTeapot16->setMaterial(mat4);

        // CUSTOM
        mat1.matType = Assets::MAT_CUSTOM;
        mat2.matType = Assets::MAT_CUSTOM;
        mat3.matType = Assets::MAT_CUSTOM;
        mat4.matType = Assets::MAT_CUSTOM;

        mat1.customMode  = Assets::CM_ADD;
        mat1.customColor = Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
        mat1.pCustomMap0 = pDecalTexture;
        mat1.opColor01   = Assets::OP_MUL;
        mat1.pCustomMap1 = pDecalTexture;
        mat1.duv0        = Core::Vector2f(0.1f, 0.2f);
        mat1.duv1        = Core::Vector2f(0.2f, 0.1f);

        mat2.customMode  = Assets::CM_LERP;
        mat2.customColor = Core::Vector4f(1.0f, 0.3f, 0.3f, 1.0f);
        mat2.pCustomMap0 = pDecalTexture;
        mat2.opColor01   = Assets::OP_ADD;
        mat2.pCustomMap1 = pDecalTexture;
        mat2.duv0        = Core::Vector2f(0.1f, 0.2f);
        mat2.duv1        = Core::Vector2f(0.2f, 0.1f);
        mat2.colorFactor = 0.5f;
        mat2.glowFactor  = 1.0f;

        mat3.customMode  = Assets::CM_LERP;
        mat3.customColor = Core::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
        mat3.pCustomMap0 = pDiffuseTexture;
        mat3.opColor01   = Assets::OP_MUL;
        mat3.pCustomMap1 = pDecalTexture;
        mat3.opColor12   = Assets::OP_ADD;
        mat3.pCustomMap2 = pDecalTexture;
        mat3.duv1        = Core::Vector2f(0.1f, 0.2f);
        mat3.duv2        = Core::Vector2f(0.2f, 0.1f);

        mat4.customMode  = Assets::CM_REFRAC;
        mat4.customColor = Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f);
        //mat4.customColor = Core::Vector4f(1.0f);
        mat4.pCustomMap0 = pNormalTexture;
        mat4.pCustomMap1 = pNormalTextureDXT5;
        mat4.pCustomMap2 = pNormalTextureDXT5;
        mat4.duv1        = Core::Vector2f(0.02f, 0.005f);
        mat4.duv2        = Core::Vector2f(0.002f, 0.01f);
        mat4.refracScale = 10.0f;
        mat4.zBias       = 0.0f;

        _pMeshInstanceTeapot17->setMaterial(mat1);
        _pMeshInstanceTeapot18->setMaterial(mat2);
        _pMeshInstanceTeapot19->setMaterial(mat3);
        _pMeshInstanceTeapot20->setMaterial(mat4);

        Core::Matrix4f world01(Core::TRANSLATION, -300.0,  150.0, 0.0);
        Core::Matrix4f world02(Core::TRANSLATION, -100.0,  150.0, 0.0);
        Core::Matrix4f world03(Core::TRANSLATION,  100.0,  150.0, 0.0);
        Core::Matrix4f world04(Core::TRANSLATION,  300.0,  150.0, 0.0);

        Core::Matrix4f world05(Core::TRANSLATION, -300.0,   50.0, 0.0);
        Core::Matrix4f world06(Core::TRANSLATION, -100.0,   50.0, 0.0);
        Core::Matrix4f world07(Core::TRANSLATION,  100.0,   50.0, 0.0);
        Core::Matrix4f world08(Core::TRANSLATION,  300.0,   50.0, 0.0);

        Core::Matrix4f world09(Core::TRANSLATION, -300.0,  -50.0, 0.0);
        Core::Matrix4f world10(Core::TRANSLATION, -100.0,  -50.0, 0.0);
        Core::Matrix4f world11(Core::TRANSLATION,  100.0,  -50.0, 0.0);
        Core::Matrix4f world12(Core::TRANSLATION,  300.0,  -50.0, 0.0);

        Core::Matrix4f world13(Core::TRANSLATION, -300.0, -150.0, 0.0);
        Core::Matrix4f world14(Core::TRANSLATION, -100.0, -150.0, 0.0);
        Core::Matrix4f world15(Core::TRANSLATION,  100.0, -150.0, 0.0);
        Core::Matrix4f world16(Core::TRANSLATION,  300.0, -150.0, 0.0);

        Core::Matrix4f world17(Core::TRANSLATION, -300.0, -250.0, 0.0);
        Core::Matrix4f world18(Core::TRANSLATION, -100.0, -250.0, 0.0);
        Core::Matrix4f world19(Core::TRANSLATION,  100.0, -250.0, 0.0);
        Core::Matrix4f world20(Core::TRANSLATION,  300.0, -250.0, 0.0);

        //_pMeshInstanceTeapot01->kill();
        //_pMeshInstanceTeapot02->kill();
        //_pMeshInstanceTeapot03->kill();
        //_pMeshInstanceTeapot04->kill();
        //_pMeshInstanceTeapot05->kill();
        //_pMeshInstanceTeapot06->kill();
        //_pMeshInstanceTeapot07->kill();
        //_pMeshInstanceTeapot08->kill();
        //_pMeshInstanceTeapot09->kill();
        //_pMeshInstanceTeapot10->kill();
        //_pMeshInstanceTeapot11->kill();
        //_pMeshInstanceTeapot12->kill();
        //_pMeshInstanceTeapot13->kill();
        //_pMeshInstanceTeapot14->kill();
        //_pMeshInstanceTeapot15->kill();
        //_pMeshInstanceTeapot16->kill();

        _pMeshInstanceTeapot01->setWorldMatrix(world01);
        _pMeshInstanceTeapot02->setWorldMatrix(world02);
        _pMeshInstanceTeapot03->setWorldMatrix(world03);
        _pMeshInstanceTeapot04->setWorldMatrix(world04);
        _pMeshInstanceTeapot05->setWorldMatrix(world05);
        _pMeshInstanceTeapot06->setWorldMatrix(world06);
        _pMeshInstanceTeapot07->setWorldMatrix(world07);
        _pMeshInstanceTeapot08->setWorldMatrix(world08);
        _pMeshInstanceTeapot09->setWorldMatrix(world09);
        _pMeshInstanceTeapot10->setWorldMatrix(world10);
        _pMeshInstanceTeapot11->setWorldMatrix(world11);
        _pMeshInstanceTeapot12->setWorldMatrix(world12);
        _pMeshInstanceTeapot13->setWorldMatrix(world13);
        _pMeshInstanceTeapot14->setWorldMatrix(world14);
        _pMeshInstanceTeapot15->setWorldMatrix(world15);
        _pMeshInstanceTeapot16->setWorldMatrix(world16);
        _pMeshInstanceTeapot17->setWorldMatrix(world17);
        _pMeshInstanceTeapot18->setWorldMatrix(world18);
        _pMeshInstanceTeapot19->setWorldMatrix(world19);
        _pMeshInstanceTeapot20->setWorldMatrix(world20);

        _boxes.push_back(_pScene->createAABox());
        _boxes.push_back(_pScene->createAABox());

        _boxes[0]->setBox(_pMeshInstanceTeapot01->getBoundingBox());
        _boxes[1]->setBox(_pMeshInstanceTeapot02->getBoundingBox());

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

void SceneTeapot::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}



