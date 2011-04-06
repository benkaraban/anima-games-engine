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
#include "SceneCrashTest.h"

#include <Core/Logger.h>
#include <Core/StopWatch.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Math/Random.h>
#include <Core/Math/MathTools.h>

#include <Assets/Tool.h>
#include <Assets/GeometricPrimitive.h>

#include <Core/Math/Color.h>

#include <Workflow/ModelImporterX.h>
#include <Workflow/ModelOptimize.h>
#include <Workflow/TexCompression.h>

const float MESH_DIMENSION   = 12.0;

SceneCrashTest::SceneCrashTest(const TestSceneData & data) :
    ITestScene(data),
    _alpha(0.0f)
{
    _camQuake.setPosition(Core::Vector3f(0.0f, 0.0f, -20.0f));
    _camOrbit.setDistance(20.0f);
    _quakeCamSpeed = 10.0f;
    _sensibilityXY = 0.02f;
    _sensibilityZ = 1.0f;
}

bool SceneCrashTest::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
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
            light.diffuse  = Core::Vector4f(0.50f, 0.50f, 0.50f, 0.00f);
            light.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
            light.sunAngleToZenith = 1.14f;
            light.sunAngleToNorth = 5.02f;

            light.sunAngleToZenith = 0.8f * f_PI_DIV_2;
            light.sunAngleToZenith = 0.1f;
            light.sunAngleToNorth = 0.0;

            _pScene->setLightSettings(light);
        }

        // Black texture
        {
            Ptr<Images::Image> pImage(new Images::Image(Core::ARGB_U8, 1, 1));
            byte * pData = pImage->getData();
            pData[0] = 0x00;
            pData[1] = 0x00;
            pData[2] = 0x00;
            pData[3] = 0xFF;

            Workflow::TextureOptions options;
            options.texType = Workflow::TEXTURE_COLORMAP;
            Ptr<Assets::Texture> pTexture = Workflow::generateTexture(options, pImage);
            _pBlackTex = _pRenderer->uploadTexture(pTexture);
        }

        // Dynamic lights
        {
            _pLight1 = _pScene->createPointLight();
            _pLight1->setRadius(9.0);

            _pLight1->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)));
            _pLight1->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.2f, 0.2f, 1.0f)));
            _pLight1->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.4f, 0.4f, 1.0f)));

            _pLight2 = _pScene->createPointLight();
            _pLight2->setRadius(9.0);

            _pLight2->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)));
            _pLight2->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(0.6f, 0.6f, 1.0f, 1.0f)));
            _pLight2->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));

            _pLight3 = _pScene->createPointLight();
            _pLight3->setRadius(9.0);

            _pLight3->setAmbientColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.05f, 0.0f, 1.0f)));
            _pLight3->setDiffuseColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f)));
            _pLight3->setSpecularColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));

            // Decals pour visualiser les lumières

            Ptr<Renderer::ITextureMap> pDecalTexture(loadTexture(pVFS, L"/Data/TUData/Textures/part00.tga", Workflow::TEXTURE_COLORMAP));

            _pDecal1 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);
            _pDecal2 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);
            _pDecal3 = _pScene->createDecal(pDecalTexture, Renderer::DECAL_LERP);

            _pDecal1->setSize(1.0f);
            _pDecal2->setSize(1.0f);
            _pDecal3->setSize(1.0f);

            _pDecal1->setColor(Core::cvtSRGB2Linear(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.50f)));
            _pDecal2->setColor(Core::cvtSRGB2Linear(Core::Vector4f(0.6f, 0.6f, 1.0f, 0.50f)));
            _pDecal3->setColor(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 1.0f, 0.0f, 0.50f)));
        }

        // Modèles
        {
            Workflow::ModelImporterX importX;
            Ptr<Core::InputStream> pInputModel(pVFS->readFile(L"/Data/TUData/Modeles/cylinder.x"));
            Ptr<Assets::Model> pModel(importX.importModel(*pInputModel));
            _pCylinderMesh = pModel->getMeshPtr(0);

            Workflow::generateTBN(*pModel, false);

            // Cylindre 1
            {
                const float S = 1.25f;
                Core::Matrix4f matS(Core::SCALE, Core::Vector3f(S, S, S));
                Core::Matrix4f matT(Core::TRANSLATION, Core::Vector3f(3.0f, 1.0f, -0.5f));
                Core::Matrix4f matR(Core::ROTATION, 0.7f, Core::ROT_X_PITCH);
                Core::Matrix4f matW(matT * (matR * matS));

                Renderer::Material mat;
                mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                mat.shininess = 22.0f;
                mat.dynamicShadowCaster = true;
                mat.dynamicShadowOpacity = 0.75f;
                mat.pDiffuseMap = loadTexture(pVFS, L"/Data/TUData/Textures/cylinder2.psd", Workflow::TEXTURE_COLORMAP);
                mat.pNormalMap = loadPackedTexture(pVFS, L"/Data/AssetsPacked/TUData/Textures/stones01-normal.tex");

                Ptr<Renderer::IMesh> pMesh = _pRenderer->uploadMesh(_pCylinderMesh, texGrabber);
                _pCylinder1 = _pScene->createMeshInstance(pMesh);
                _pCylinder1->setMaterial(mat);
                _pCylinder1->setWorldMatrix(matW);
            }

            // Cylindre 2
            {
                const float S = 1.25f;
                Core::Matrix4f matS(Core::SCALE, Core::Vector3f(S, S, S));
                Core::Matrix4f matT(Core::TRANSLATION, Core::Vector3f(0.0f, -2.0f, 0.0f));
                Core::Matrix4f matR(Core::ROTATION, 0.0f, Core::ROT_X_PITCH);
                Core::Matrix4f matW(matT * (matR * matS));

                Renderer::Material mat;
                mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                mat.pLightMap = _pBlackTex;
                mat.dynamicShadowCaster = true;
                mat.dynamicShadowOpacity = 0.75f;
                
                mat.pDiffuseMap = loadTexture(pVFS, L"/Data/TUData/Textures/cylinder2.psd", Workflow::TEXTURE_COLORMAP);
                mat.pNormalMap = loadPackedTexture(pVFS, L"/Data/AssetsPacked/TUData/Textures/stones01-normal.tex");

                Ptr<Renderer::IMesh> pMesh = _pRenderer->uploadMesh(_pCylinderMesh, texGrabber);
                _pCylinder2 = _pScene->createMeshInstance(pMesh);
                _pCylinder2->setMaterial(mat);
                _pCylinder2->setWorldMatrix(matW);
            }

            // Terrain
            {
                _pTerrainMesh = Ptr<Assets::ModelMesh>(new Assets::ModelMesh(0, L"Terrain"));
                Ptr<Assets::VerticeSet> pVert(_pTerrainMesh->getLOD(Assets::LOD_HIGH));

                Assets::createPlane(*pVert, 2.0f*MESH_DIMENSION, 2.0f*MESH_DIMENSION, 80, 80, 1.0f, 0.0f);
                _pTerrainMesh->cleanup();

                TextureGrabber texGrabber(pVFS, _pRenderer);
                Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(_pTerrainMesh, texGrabber);

                _pTerrain = _pScene->createMeshInstance(pRenderMesh);
                _pTerrain->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.0f, -4.5f, 0.0f));
                {
                    Renderer::Material mat;
                    mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                    mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                    mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                    mat.pDiffuseMap = loadTexture(pVFS, L"/Data/TUData/Textures/Sand00.jpg", Workflow::TEXTURE_COLORMAP);
                    mat.pNormalMap = loadPackedTexture(pVFS, L"/Data/AssetsPacked/TUData/Textures/stones01-normal.tex");
                    mat.dynamicShadowReceiver = true;
                    mat.internalTextureBorder = true;

                    _pTerrain->setMaterial(mat);

                    Ptr<Renderer::IAABox> pBox = _pScene->createAABox();
                    pBox->setBox(_pTerrain->getBoundingBox());
                    _boxes.push_back(pBox);
                }
            }
        }

        // SkySphere
        {
            Ptr<Renderer::ITextureMap> pDiffuseTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));
            Renderer::SkysphereSettings sky(pDiffuseTexture, -0.25);
            _pScene->setSkysphereSettings(sky);
        }

        setBoxesVisible(false);

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 1.0, 1000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising grass scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

void SceneCrashTest::update(double elapsed)
{
    ITestScene::update(elapsed);

    _alpha += 0.75f * float(elapsed);

    float a1 = _alpha + (0.0f / 3.0f) * f_PI_MUL_2;
    float a2 = _alpha + (1.0f / 3.0f) * f_PI_MUL_2;
    float a3 = _alpha + (2.0f / 3.0f) * f_PI_MUL_2;

    static const float R = 5.0f;

    _pLight1->setPosition(Core::Vector3f(R * cos(a1), -2.0f, R * sin(a1)));
    _pLight2->setPosition(Core::Vector3f(R * cos(a2), -2.0f, R * sin(a2)));
    _pLight3->setPosition(Core::Vector3f(R * cos(a3), -2.0f, R * sin(a3)));

    Core::Matrix4f world1(Core::TRANSLATION, _pLight1->getPosition());
    Core::Matrix4f world2(Core::TRANSLATION, _pLight2->getPosition());
    Core::Matrix4f world3(Core::TRANSLATION, _pLight3->getPosition());

    _pDecal1->setWorldMatrix(world1);
    _pDecal2->setWorldMatrix(world2);
    _pDecal3->setWorldMatrix(world3);
}

void SceneCrashTest::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


