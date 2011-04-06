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
#include "SceneLightMap.h"

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

SceneLightMap::SceneLightMap(const TestSceneData & data) :
    ITestScene(data)
{
    _camQuake.setPosition(Core::Vector3f(0.0f, 0.0f, -20.0f));
    _camOrbit.setDistance(20.0f);
    _quakeCamSpeed = 10.0f;
    _sensibilityXY = 0.02f;
    _sensibilityZ = 1.0f;
    updateCamera();
}

bool SceneLightMap::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
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
            light.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 0.00f);
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
            pData[0] = 0xFF;
            pData[1] = 0xFF;
            pData[2] = 0xFF;
            pData[3] = 0xFF;

            Workflow::TextureOptions options;
            options.texType = Workflow::TEXTURE_COLORMAP;
            Ptr<Assets::Texture> pTexture = Workflow::generateTexture(options, pImage);
            _pBlackTex = _pRenderer->uploadTexture(pTexture);
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
                //mat.specular = Core::Vector4f(1.00f, 1.00f, 1.00f, 0.00f);
                mat.shininess = 22.0f;
                mat.pLightMap = _pBlackTex;
                mat.dynamicShadowCaster = true;
                mat.dynamicShadowOpacity = 0.75f;
                mat.pDiffuseMap = loadTexture(pVFS, L"/Data/TUData/Textures/cylinder2.psd", Workflow::TEXTURE_COLORMAP);
                mat.pNormalMap = loadTexture(pVFS, L"/Data/TUData/Textures/cylinder2-height.psd", Workflow::TEXTURE_HEIGHTMAP);
                //mat.pLightMap = loadTexture(pVFS, L"/Local/lm-cylinder.tga", Workflow::TEXTURE_COLORMAP);
                //mat.staticLighting = true;

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
                //mat.pLightMap = loadTexture(pVFS, L"/Local/lm-cylinder2-from-1024.psd", Workflow::TEXTURE_COLORMAP);
                //mat.staticLighting = true;

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
                    mat.pLightMap = _pBlackTex;
                    mat.dynamicShadowReceiver = true;

                    _pTerrain->setMaterial(mat);

                    Ptr<Renderer::IAABox> pBox = _pScene->createAABox();
                    pBox->setBox(_pTerrain->getBoundingBox());
                    _boxes.push_back(pBox);
                }
            }
        } // Modèles
#if 1
            {
                bool v = true;
                {
                    Renderer::Material mat = _pCylinder1->getMaterial();
                    v = !mat.staticLighting;
                    mat.staticLighting = v;
                    mat.dynamicShadowCaster = !v;
                    _pCylinder1->setMaterial(mat);

                    mat = _pCylinder2->getMaterial();
                    mat.staticLighting = v;
                    mat.dynamicShadowCaster = !v;
                    _pCylinder2->setMaterial(mat);

                    mat = _pTerrain->getMaterial();
                    mat.staticLighting = v;
                    mat.dynamicShadowCaster = !v;
                    _pTerrain->setMaterial(mat);
                }
            }
#endif

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

void SceneLightMap::generateLightmaps(bool reinit)
{
    Core::StopWatch sw;

    {
        Renderer::Material mat = _pCylinder1->getMaterial();
        mat.staticLighting = true;
        mat.dynamicShadowCaster = false;
        if(reinit)
            mat.pLightMap = null;
        _pCylinder1->setMaterial(mat);
    }
    {
        Renderer::Material mat = _pCylinder2->getMaterial();
        mat.staticLighting = true;
        mat.dynamicShadowCaster = false;
        if(reinit)
            mat.pLightMap = null;
        _pCylinder2->setMaterial(mat);
    }
    {
        Renderer::Material mat = _pTerrain->getMaterial();
        mat.staticLighting = true;
        mat.dynamicShadowCaster = false;
        if(reinit)
            mat.pLightMap = null;
        _pTerrain->setMaterial(mat);
    }

    Ptr<Core::IProgressObs> pObs(new Core::ProgressPrint());
    Renderer::LightMapInfos lm;
    lm.pScene = _pScene;
    lm.width = lm.height = 64;
    lm.internalTextureBorder = false;

    Ptr<Images::Image> pImageT;
    Ptr<Images::Image> pImage1;
    Ptr<Images::Image> pImage2;

    // Terrain
    {
        lm.pObs = Ptr<Core::IProgressObs>(new Core::SubProgressObs(pObs, 0.0f, 1.0f / 3.0f));

        Ptr<Core::Bitmap> pBitmapT = _pRenderer->generateLightMap(lm, _pTerrain);
        pImageT = Images::toImage(pBitmapT);
        pImageT->saveFile(L"Local/lm-terrain.tga");
    }

    // Cylinder 1
    {
        lm.pObs = Ptr<Core::IProgressObs>(new Core::SubProgressObs(pObs, 1.0f / 3.0f, 2.0f / 3.0f));
        
        Ptr<Core::Bitmap> pBitmap1 = _pRenderer->generateLightMap(lm, _pCylinder1);
        pImage1 = Images::toImage(pBitmap1);
        pImage1->saveFile(L"Local/lm-cylinder1.tga");
    }

    // Cylinder 2
    {
        lm.pObs = Ptr<Core::IProgressObs>(new Core::SubProgressObs(pObs, 2.0f / 3.0f, 3.0f / 3.0f));
        
        Ptr<Core::Bitmap> pBitmap2 = _pRenderer->generateLightMap(lm, _pCylinder2);
        pImage2 = Images::toImage(pBitmap2);
        pImage2->saveFile(L"Local/lm-cylinder2.tga");
    }

    // Assign lightmaps
    {
        Renderer::Material mat = _pTerrain->getMaterial();
        mat.pLightMap = uploadTexture(pImageT, Workflow::TEXTURE_COLORMAP);
        _pTerrain->setMaterial(mat);
    }
    {
        Renderer::Material mat = _pCylinder1->getMaterial();
        mat.pLightMap = uploadTexture(pImage1, Workflow::TEXTURE_COLORMAP);
        _pCylinder1->setMaterial(mat);
    }
    {
        Renderer::Material mat = _pCylinder2->getMaterial();
        mat.pLightMap = uploadTexture(pImage2, Workflow::TEXTURE_COLORMAP);
        _pCylinder2->setMaterial(mat);
    }

    INF << L"\nElapsed : " << sw.elapsed().toString() << L"\n";
}

void SceneLightMap::update(double elapsed)
{
    ITestScene::update(elapsed);

    if(_pInput->isKeyTyped('1'))
    {
        Renderer::MainLightSettings light = _pScene->getLightSettings();

        light.model    = Renderer::LIGHTING_MODEL_CLEAR_SKY;
        light.sunColor = Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
        light.skyColor = Core::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
        light.lightReflexion  = 0.0f;
        light.globalIntensity = 0.5f;

        _pScene->setLightSettings(light);

        generateLightmaps(true);
    }
    else if(_pInput->isKeyTyped('2'))
    {
        Renderer::MainLightSettings light = _pScene->getLightSettings();

        light.model    = Renderer::LIGHTING_MODEL_FOGGY;
        light.sunColor = Core::Vector4f(1.0f, 0.6f, 0.5f, 1.0f);
        light.skyColor = Core::Vector4f(0.5f);
        light.lightReflexion  = 0.20f;
        light.globalIntensity = 0.95f;

        _pScene->setLightSettings(light);

        generateLightmaps(true);
    }
    else if(_pInput->isKeyTyped('3'))
    {
        Renderer::MainLightSettings light = _pScene->getLightSettings();

        light.model    = Renderer::LIGHTING_MODEL_CLOUDY;
        light.sunColor = Core::Vector4f(1.0f, 0.6f, 0.5f, 1.0f);
        light.skyColor = Core::Vector4f(0.5f);
        light.lightReflexion  = 0.20f;
        light.globalIntensity = 0.95f;

        _pScene->setLightSettings(light);

        generateLightmaps(true);
    }
    else if(_pInput->isKeyTyped('4'))
    {
        Renderer::MainLightSettings light = _pScene->getLightSettings();

        light.model    = Renderer::LIGHTING_MODEL_CLEAR_SKY;
        light.sunColor = Core::Vector4f(1.0f, 0.6f, 0.5f, 1.0f);
        light.skyColor = Core::Vector4f(0.5f);
        light.lightReflexion  = 0.20f;
        light.globalIntensity = 0.85f;

        _pScene->setLightSettings(light);

        Renderer::SkysphereSettings sky = _pScene->getSkysphereSettings();
        sky.sphericity = 1.0f;
        _pScene->setSkysphereSettings(sky);

        generateLightmaps(true);
    }
    else if(_pInput->isKeyTyped('5'))
    {
        Renderer::MainLightSettings light = _pScene->getLightSettings();

        light.model    = Renderer::LIGHTING_MODEL_CLEAR_SKY;
        light.sunColor = Core::Vector4f(1.0f, 0.6f, 0.5f, 1.0f);
        light.skyColor = Core::Vector4f(0.5f);
        light.lightReflexion  = 0.20f;
        light.globalIntensity = 0.95f;

        _pScene->setLightSettings(light);

        Renderer::SkysphereSettings sky = _pScene->getSkysphereSettings();
        sky.sphericity = 0.1f;
        _pScene->setSkysphereSettings(sky);

        generateLightmaps(true);
    }
    else if(_pInput->isKeyTyped('X'))
    {
        generateLightmaps(false);
    }
    else if(_pInput->isKeyTyped('V'))
    {
        bool v = true;
        {
            Renderer::Material mat = _pCylinder1->getMaterial();
            v = !mat.staticLighting;
            mat.staticLighting = v;
            mat.dynamicShadowCaster = !v;
            _pCylinder1->setMaterial(mat);

            mat = _pCylinder2->getMaterial();
            mat.staticLighting = v;
            mat.dynamicShadowCaster = !v;
            _pCylinder2->setMaterial(mat);

            mat = _pTerrain->getMaterial();
            mat.staticLighting = v;
            mat.dynamicShadowCaster = !v;
            _pTerrain->setMaterial(mat);
        }
    }
    else if(_pInput->isKeyTyped('W'))
    {
        Renderer::RadiosityQuery query;
        query.pScene = _pScene;
        query.positions.push_back(_camQuake.getPosition());
        query.directions.push_back(_camQuake.getDirection());
        query.screenShots = true;

        Core::List<Core::Vector4f> colors;

        _pRenderer->getRadiosity(query, colors);
        INF << Core::toString(colors[0]) << L"\n";
    }
}

void SceneLightMap::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


