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
#include "SceneWater.h"

#include <Core/Logger.h>
#include <Core/Math/Transform.h>

#include <Assets/Tool.h>
#include <Assets/GeometricPrimitive.h>

#include <Core/Math/Color.h>

#include <Workflow/ModelImporterX.h>
#include <Workflow/TexCompression.h>

SceneWater::SceneWater(const TestSceneData & data) :
    ITestScene(data)
{
    _quakeCamSpeed = 1000.0;
    setCamera(CAMERA_QUAKE);

    // Pour faire des bench, mettre la caméra comme ça
    _camQuake.setPosition(Core::Vector3f(0.0, 1000.0, 0.0));
    _camQuake.turnUpDown(f_PI_DIV_2);

    updateCamera();
}

const float MESH_DIMENSION = 10000.0;

bool SceneWater::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        // Main light
        Renderer::MainLightSettings light(_pScene->getLightSettings());
        light.sunAngleToZenith = 1.29f;
        light.sunAngleToNorth  = 0.20f;
        _pScene->setLightSettings(light);

        // Fog
        Renderer::FogSettings fog(Core::Vector4f(1.0, 0.0, 0.0, 0.0), 100.0, 2000.0, 0.0);
        _pScene->setFogSettings(fog);

        // Teapot
        {
            Workflow::ModelImporterX importX;
            Ptr<Core::InputStream> pInputModel(pVFS->readFile(L"/Data/TUData/Modeles/teapot.x"));
            Ptr<Assets::Model> pModelTeapot(importX.importModel(*pInputModel));

            int32 idTeapotMesh = pModelTeapot->getMeshId(L"Teapot01");
            Ptr<Assets::ModelMesh> pMesh = pModelTeapot->getMeshPtr(idTeapotMesh);

            TextureGrabber texGrabber(pVFS, _pRenderer);
            Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(pMesh, texGrabber);

            Ptr<Renderer::IMeshInstance> pInst = _pScene->createMeshInstance(pRenderMesh);

            Core::Matrix4f world(Core::TRANSLATION, Core::Vector3f(0.0f, 20.0f, 0.0f));
            pInst->setWorldMatrix(world);
        }

        // Modèle
        {
            Ptr<Core::InputStream> pInputImage(pVFS->readFile(L"/Data/TUData/Textures/Sand00.jpg"));
            Ptr<Images::Image> pImage (new Images::Image);
            pImage->load((Core::SeekableInputStream &)*pInputImage);
            Workflow::TextureOptions options;

            Ptr<Assets::Texture> pTexture(Workflow::generateTexture(options, pImage));
            Ptr<Renderer::ITextureMap> pGrassTexture(_pRenderer->uploadTexture(pTexture));

            Ptr<Assets::ModelMesh> pMesh(new Assets::ModelMesh(0, L"Terrain"));
            Ptr<Assets::VerticeSet> pVert(pMesh->getLOD(Assets::LOD_HIGH));

            Assets::createPlane(*pVert, 2.0f*MESH_DIMENSION, 2.0f*MESH_DIMENSION, 181, 181, 20.0f, 20.0f);
            pMesh->cleanup();

            TextureGrabber texGrabber(pVFS, _pRenderer);
            Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(pMesh, texGrabber);

            // Instances

            Ptr<Renderer::IMeshInstance> pTerrain = _pScene->createMeshInstance(pRenderMesh);
            {
                Renderer::Material mat;
                mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                mat.pDiffuseMap  = pGrassTexture;

                pTerrain->setMaterial(mat);

                Core::Matrix4f worldR(Core::ROTATION, 0.2f, Core::ROT_X_PITCH);
                Core::Matrix4f worldT(Core::TRANSLATION, Core::Vector3f(0.0f, -700.0f, 0.0f));
                pTerrain->setWorldMatrix(worldT * worldR);

                Ptr<Renderer::IAABox> pBox = _pScene->createAABox();
                pBox->setBox(pTerrain->getBoundingBox());
                _boxes.push_back(pBox);
            }
        }


        // Textures

        Ptr<Renderer::ITextureMap> pTexMap00(loadTexture(pVFS, L"/Data/TUData/Textures/Water2.tga", Workflow::TEXTURE_NORMALMAP));
        Ptr<Renderer::ITextureMap> pTexMap01(loadTexture(pVFS, L"/Data/TUData/Textures/Water1.jpg", Workflow::TEXTURE_NORMALMAP));

        // Water
        static const float WATER_SIZE = 1.7f * MESH_DIMENSION;

        _pWater = _pScene->createWaterPatch(WATER_SIZE, WATER_SIZE);
        _pScene->setWaterLevel(-200.0);

        Renderer::WaterLayer layer0;
        layer0.pNormalMap = pTexMap00;

        layer0.scaleU = 11333.3f;
        layer0.scaleV = 11333.3f;
        layer0.speedU =  113.3f;
        layer0.speedV = -0.00f;
        layer0.amplitude = 0.1f;

        Renderer::WaterLayer layer1;
        layer1.pNormalMap = pTexMap00;

        layer1.scaleU = 11333.3f;
        layer1.scaleV = 11333.3f;
        layer1.speedU = -113.3f;
        layer1.speedV =  113.3f;
        layer1.amplitude = 0.1f;

        Renderer::WaterLayer layer2;
        layer2.scaleU = 1.0;
        layer2.scaleV = 1.0;
        layer2.speedU = 0.00;
        layer2.speedV = 0.00;
        layer2.amplitude = 1.0;

        _pWater->setLayer(0, layer0);
        _pWater->setLayer(1, layer1);
        _pWater->setLayer(2, layer2);

        _pWater->setDiffuse(Core::cvtSRGB2Linear(Core::Vector4f(0.0f, 0.10f, 0.15f, 1.0f)));
        _pWater->setSpecular(Core::cvtSRGB2Linear(Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f)));
        _pWater->setShininess(32.0f);
        _layer = 0;

        Ptr<Renderer::IAABox> pBox = _pScene->createAABox();
        pBox->setBox(_pWater->getBoundingBox());
        _boxes.push_back(pBox);

        setBoxesVisible(false);

        // SkySphere
        Ptr<Renderer::ITextureMap> pSkyTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));
        Renderer::SkysphereSettings sky(pSkyTexture, -0.05f);
        _pScene->setSkysphereSettings(sky);

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 10.0, 5000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising water scene : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}

void SceneWater::setWaveInfos(const Renderer::WaterLayer & layer)
{
    INF << L"-------- " << Core::toString(_layer) << L"\n";
    INF << L"Scale = " << Core::toString(layer.scaleU) << L"," << Core::toString(layer.scaleV) << L"\n";
    INF << L"Speed = " << Core::toString(layer.speedU) << L"," << Core::toString(layer.speedV) << L"\n";
    INF << L"Ampli = " << Core::toString(layer.amplitude) << L"\n";
    _pWater->setLayer(_layer, layer);
}

void SceneWater::update(double elapsed)
{
    ITestScene::update(elapsed);

    Renderer::WaterLayer layer;
    _pWater->getLayer(_layer, layer);

    if(_pInput->isKeyTyped('1'))
    {
        _layer = 0;
        _pWater->getLayer(_layer, layer); 
        setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped('2'))
    {
        _layer = 0;
        _pWater->getLayer(_layer, layer); 
        setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped('3'))
    {
        _layer = 0;
        _pWater->getLayer(_layer, layer); 
        setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped('4'))
    {
        _layer = 0;
        _pWater->getLayer(_layer, layer); 
        setWaveInfos(layer);
    }

    if(_pInput->isKeyTyped(VK_UP))
    {
        layer.scaleV += 0.1f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_DOWN))
    {
        layer.scaleV -= 0.1f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_LEFT))
    {
        layer.scaleU -= 0.1f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_RIGHT))
    {
        layer.scaleU += 0.1f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_NUMPAD4))
    {
        layer.speedU -= 0.001f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_NUMPAD6))
    {
        layer.speedU += 0.001f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_NUMPAD2))
    {
        layer.speedV -= 0.001f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_NUMPAD8))
    {
        layer.speedV += 0.001f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_ADD))
    {
        layer.amplitude += 0.1f; setWaveInfos(layer);
    }
    if(_pInput->isKeyTyped(VK_SUBTRACT))
    {
        layer.amplitude = std::max(0.0f, layer.amplitude - 0.1f); setWaveInfos(layer);
    }
}

void SceneWater::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


