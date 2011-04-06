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
#include "SceneLightMap2.h"

#include <Core/Logger.h>
#include <Core/StopWatch.h>
#include <Core/System/Files.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Math/Random.h>
#include <Core/Math/MathTools.h>

#include <Assets/Tool.h>
#include <Assets/GeometricPrimitive.h>

#include <Core/Math/Color.h>

#include <Workflow/ModelImporterX.h>
#include <Workflow/TexCompression.h>

const float MESH_DIMENSION   = 12.0;

SceneLightMap2::SceneLightMap2(const TestSceneData & data) :
    ITestScene(data)
{
    _camQuake.setPosition(Core::Vector3f(0.0f, 10.0f, 0.0f));
    _camOrbit.setObservedPoint(Core::Vector3f(0.5f, 0.125f, 0.5f));
    _camOrbit.setDistance(2.0f);
    _quakeCamSpeed = 10.0f;
    _sensibilityXY = 0.001f;
    _sensibilityZ = 0.04f;
}

bool SceneLightMap2::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
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
            light.sunAngleToZenith = 0.00f;
            light.sunAngleToNorth = 5.02f;

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

        // Modèles
        {
            // Sphere
            {
                _pSphereMesh = Ptr<Assets::ModelMesh>(new Assets::ModelMesh(0, L"Terrain"));
                Ptr<Assets::VerticeSet> pVert(_pSphereMesh->getLOD(Assets::LOD_HIGH));

                Assets::createUVSphere(*pVert, 0.2f, 12, 12, false);
                _pSphereMesh->cleanup();

                TextureGrabber texGrabber(pVFS, _pRenderer);
                Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(_pSphereMesh, texGrabber);

                _pSphere = _pScene->createMeshInstance(pRenderMesh);
                _pSphere->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.55f, 0.15f, 0.53f));

                Renderer::Material mat;
                mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                mat.pLightMap = _pBlackTex;

                _pSphere->setMaterial(mat);
            }

            // Terrain
            {
                _terrains.resize(4);

                _pTerrainMesh = Ptr<Assets::ModelMesh>(new Assets::ModelMesh(0, L"Terrain"));
                Ptr<Assets::VerticeSet> pVert(_pTerrainMesh->getLOD(Assets::LOD_HIGH));

                Assets::createPlane(*pVert, 0.5f, 0.5f, 4, 4, 1.0f, 0.0f);
                _pTerrainMesh->cleanup();

                TextureGrabber texGrabber(pVFS, _pRenderer);
                Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(_pTerrainMesh, texGrabber);

                _terrains[0] = _pScene->createMeshInstance(pRenderMesh);
                _terrains[1] = _pScene->createMeshInstance(pRenderMesh);
                _terrains[2] = _pScene->createMeshInstance(pRenderMesh);
                _terrains[3] = _pScene->createMeshInstance(pRenderMesh);

                _terrains[0]->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.25f, 0.0f,  0.25f));
                _terrains[1]->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.75f, 0.0f,  0.25f));
                _terrains[2]->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.75f, 0.0f,  0.75f));
                _terrains[3]->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.25f, 0.0f,  0.75f));


                {
                    Renderer::Material mat;
                    mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                    mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                    mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                    mat.internalTextureBorder = true;

                    _terrains[0]->setMaterial(mat);
                    _terrains[1]->setMaterial(mat);
                    _terrains[2]->setMaterial(mat);
                    _terrains[3]->setMaterial(mat);

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

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 0.1f, 100.0f);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising grass scene : " << exception.getMessage() << L"\n";
        Core::System::errorMessageBox(exception.getMessage());
        result = false;
    }

    return result;
}

void SceneLightMap2::update(double elapsed)
{
    ITestScene::update(elapsed);

    if(_pInput->isKeyTyped('X'))
    {
        Core::StopWatch sw;


        Ptr<Core::IProgressObs> pObs(new Core::ProgressPrint());
        Renderer::LightMapInfos lm;
        lm.pScene = _pScene;
        lm.width = lm.height = 32;
        lm.internalTextureBorder = true;

        // Sphere static
        {
            Renderer::Material mat = _pSphere->getMaterial();
            mat.staticLighting = true;
            _pSphere->setMaterial(mat);
        }

        // Terrain
        {
            Core::List<Ptr<Images::Image> > images;
            images.resize(4);

            // Set static

            for(int32 ii=0; ii < _terrains.size(); ii++)
            {
                Renderer::Material mat = _terrains[ii]->getMaterial();
                mat.staticLighting = true;
                mat.pLightMap = _pBlackTex;
                _terrains[ii]->setMaterial(mat);
            }

            // Gen lightmap

            for(int32 ii=0; ii < _terrains.size(); ii++)
            {
                images[ii] = Ptr<Images::Image>(new Images::Image(Core::ARGB_U8, lm.width, lm.height));
                lm.pObs = Ptr<Core::IProgressObs>(new Core::SubProgressObs(pObs, float(ii) / float(_terrains.size()), float(ii + 1) / float(_terrains.size())));

                Ptr<Core::Bitmap> pBitmap = _pRenderer->generateLightMap(lm, _terrains[ii]);

                if(lm.debugPositionBitmap)
                    Images::toImage(lm.pPositionBitmap)->saveFile(L"Local/lm-terrainn" + Core::toString(ii) + L".tga");

                if(lm.debugNormalBitmap)
                    Images::toImage(lm.pNormalBitmap)->saveFile(L"Local/lm-terrainp" + Core::toString(ii) + L".tga");

                images[ii] = Images::toImage(pBitmap);
                images[ii]->saveFile(L"Local/lm-terrain" + Core::toString(ii) + L".tga");
            }

            // Set lightmap

            for(int32 ii=0; ii < _terrains.size(); ii++)
            {
                Renderer::Material mat = _terrains[ii]->getMaterial();
                mat.pLightMap = uploadTexture(images[ii], Workflow::TEXTURE_COLORMAP);
                _terrains[ii]->setMaterial(mat);
            }
        }

        // Sphere dynamic
        {
            Renderer::Material mat = _pSphere->getMaterial();
            mat.staticLighting = false;
            _pSphere->setMaterial(mat);
        }

        INF << sw.elapsed().toString() << L"\n";
    }
    else if(_pInput->isKeyTyped('B'))
    {
        for(int32 ii=0; ii < _terrains.size(); ii++)
        {
            Renderer::Material mat = _terrains[ii]->getMaterial();
            mat.staticLighting = !mat.staticLighting;
            _terrains[ii]->setMaterial(mat);
        }
    }
}

void SceneLightMap2::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


