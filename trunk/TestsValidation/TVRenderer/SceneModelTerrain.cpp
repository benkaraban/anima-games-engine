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
#include "SceneModelTerrain.h"

#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Math/Random.h>
#include <Core/Math/MathTools.h>

#include <Assets/Tool.h>

#include <Core/Math/Color.h>
#include <Images/Operator.h>

#include <Workflow/TerrainModelGen.h>
#include <Workflow/ModelOptimize.h>

#define FLAT_TERRAIN     0
#define SAVE_TERRAIN     0
#define GENERATE_NORMALS 0
#define GENERATE_LAYERS  0

const float MESH_DIMENSION = 1000.0f;

SceneModelTerrain::SceneModelTerrain(const TestSceneData & data) :
    ITestScene(data)
{
    _camQuake.setPosition(Core::Vector3f(0.0f, 0.0f, -20.0f));
    _camOrbit.setDistance(20.0f);
    _quakeCamSpeed = 400.0f;
    _sensibilityXY = 0.02f;
    _sensibilityZ = 1.0f;

    setCamera(CAMERA_QUAKE);
    _camQuake.setPosition(Core::Vector3f(0.0, 2900.0, 0.0));
    _camQuake.turnUpDown(f_PI_DIV_2);
    updateCamera();

    _camQuake.setPosition(Core::Vector3f(0.0, 900.0, 0.0));
    _quakeCamSpeed = 800.0f;
    updateCamera();
}

bool SceneModelTerrain::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
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
            light.shadowMiddle = 20.0f;
            light.shadowMiddleBlend = 6.0f;
            light.shadowEnd = 58.0f;
            light.shadowEndBlend = 5.0f;

            _pScene->setLightSettings(light);
        }

#if FLAT_TERRAIN
        {
            Ptr<Assets::ModelMesh> pTerrainMesh(new Assets::ModelMesh(0, L"Terrain"));
            Ptr<Assets::VerticeSet> pVert(pTerrainMesh->getLOD(Assets::LOD_HIGH));

            Assets::createPlane(*pVert, 2.0f*MESH_DIMENSION, 2.0f*MESH_DIMENSION, 80, 80, 1.0f, 0.0f);
            pTerrainMesh->cleanup();

            TextureGrabber texGrabber(pVFS, _pRenderer);
            Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(pTerrainMesh, texGrabber);

            Ptr<Renderer::IMeshInstance> pTerrain = _pScene->createMeshInstance(pRenderMesh);
            pTerrain->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.0f, 0.0f, 0.0f));

            lmd.box = lmn.box = pTerrain->getBoundingBox();

            Ptr<Images::Image> pImageDiffuse = _pRenderer->generateLayerMap(lmd, pTerrain);
            Ptr<Images::Image> pImageNormal  = _pRenderer->generateLayerMap(lmn, pTerrain);

            Renderer::Material mat;
            mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
            mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
            mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
            mat.pDiffuseMap = uploadTexture(pImageDiffuse, Workflow::TEXTURE_COLORMAP);
            mat.pNormalMap  = uploadTexture(pImageNormal, Workflow::TEXTURE_NORMALMAP);

            pTerrain->setMaterial(mat);
        }
#else
        // Génération terrain
        {
            Ptr<Core::InputStream> pInputHeightMap(pVFS->readFile(L"/Data/TUData/Terrains/Terrain00/HeightMap.exr"));
            Ptr<Core::InputStream> pInputNormalMap(pVFS->readFile(L"/Data/TUData/Terrains/Terrain00/NormalMap.jpg"));

            LM_ASSERT(pInputHeightMap != null);
            LM_ASSERT(pInputNormalMap != null);

            Workflow::TMGenInput gen;
            {
                Ptr<Images::Image> pHeightMapRGBF32 (new Images::Image);
                pHeightMapRGBF32->load2((Core::SeekableInputStream &)*pInputHeightMap);
                gen.pHeightMap = Ptr<Images::Image>(new Images::Image(Core::GRAY_F32, pHeightMapRGBF32->getWidth(), pHeightMapRGBF32->getHeight()));
                gen.LODCount   = 4;
                gen.blockWidth = 21;
                gen.blendScaleFactor = 0.7f;
                gen.maxHeight   = 178.0f;
                gen.terrainSize = Core::Vector2f(2400.0f, 2400.0f);

                Images::convert(pHeightMapRGBF32, gen.pHeightMap);
#if !GENERATE_NORMALS
                gen.pNormalMap = Ptr<Images::Image>(new Images::Image);
                gen.pNormalMap->load2((Core::SeekableInputStream &)*pInputNormalMap);
#endif
            }

            INF << L"Generate terrain...\n";
            _pModel = Workflow::generateTerrainModel(gen);
#if GENERATE_NORMALS
            gen.pNormalMap->saveFile(L"Data/TUData/Terrains/Terrain00/GeneratedNormalMap.jpg");
#endif
            INF << L"Generate terrain...done\n";

#if SAVE_TERRAIN
            INF << L"Optimize terrain...\n";
            Workflow::ModelPackingOptions modelOptions;
            Workflow::packModel(modelOptions, *_pModel);
            pVFS->write(L"/Data/TUData/Terrains/Terrain00/terrain-model.tmdl", *_pModel);
            INF << L"Optimize terrain...done\n";
#endif
        }
        
        // SkySphere
        {
            Ptr<Renderer::ITextureMap> pSkyTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));
            Renderer::SkysphereSettings sky(pSkyTexture, -0.05f);
            _pScene->setSkysphereSettings(sky);
        }

        Renderer::LayerMapInfos lmd;
        Renderer::LayerMapInfos lmn;

        lmd.width  = lmn.width = 256;
        lmd.height = lmn.height = 256;
        lmd.isNormalMap = false;
        lmn.isNormalMap = true;

        lmn.layers.resize(1);

        Core::AABoxf box;

        for(int32 ii=0; ii < _pModel->getMeshCount(); ii++)
            box.setUnion(_pModel->getMesh(ii).getBoundingBox());

        float sx = box.getSize().x;
        float sz = box.getSize().z;

        lmn.layers[0].generateU = lmn.layers[0].generateU = Core::Vector4f(1.0f / sx, 0.000f, 0.000f, -0.5f * sx);
        lmn.layers[0].generateV = lmn.layers[0].generateV = Core::Vector4f(0.000f, 0.000f, 1.0f / sz, -0.5f * sz);

#if GENERATE_LAYERS
        lmn.layers[0].pSourceTexture = loadTexture(pVFS, L"/Data/TUData/Terrains/Terrain00/NormalMap.jpg", Workflow::TEXTURE_NORMALMAP);
#endif

        // Modèle -> Renderer
        {
            int32 meshCount = _pModel->getMeshCount();
            int32 nodeCount = _pModel->getNodeCount();

            Core::List<Ptr<Renderer::IMesh> >     meshes;
            meshes.resize(meshCount);

            for(int32 ii=0; ii < meshCount; ii++)
            {
                lmd.box.setUnion(_pModel->getMeshPtr(ii)->getBoundingBox());
                meshes[ii] = _pRenderer->uploadMesh(_pModel->getMeshPtr(ii), texGrabber);
            }

            lmn.box = lmd.box;

            _boxes.resize(nodeCount);
            _nodes.resize(nodeCount);

            for(int32 ii=0; ii < nodeCount; ii++)
            {
                int32 meshId = _pModel->getNode(ii).getMeshId();

                if(meshId != Assets::UNDEFINED_MESH_ID)
                {
                    _nodes[ii] = _pScene->createMeshInstance(meshes[meshId]);
                    _boxes[ii] = _pScene->createAABox();
                    _boxes[ii]->setBox(_nodes[ii]->getBoundingBox());

#if GENERATE_LAYERS
                    INF << L"Creating layered textures for " << _pModel->getNode(ii).getName() << L"\n";
                    {
                        Ptr<Images::Image> pImageDiffuse(new Images::Image(Images::ARGB_U8, lmd.width, lmd.height));
                        Ptr<Images::Image> pImageNormal(new Images::Image(Images::ARGB_U8, lmn.width, lmn.height));

                        lmd.pData = pImageDiffuse->getData();
                        lmn.pData = pImageNormal->getData();

                        _pRenderer->generateLayerMap(lmd, _nodes[ii]);
                        _pRenderer->generateLayerMap(lmn, _nodes[ii]);

                        Renderer::Material mat;
                        mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                        mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                        mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                        //mat.pDiffuseMap = uploadTexture(pImageDiffuse, Workflow::TEXTURE_COLORMAP);
                        mat.pNormalMap  = uploadTexture(pImageNormal, Workflow::TEXTURE_NORMALMAP);
                        mat.clampTextureBorder = true;

                        _nodes[ii]->setMaterial(mat);
                    }
#else
                    Renderer::Material mat;
                    mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                    mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                    mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                    mat.pDiffuseMap = loadTexture(pVFS, L"/Data/TUData/Textures/border64.png", Workflow::TEXTURE_COLORMAP);
                    mat.clampTextureBorder = true;

                    _nodes[ii]->setMaterial(mat);
#endif
                }
            }

            Assets::ModelPose pose;
            _pModel->getDefaultPose(pose);

            for(int32 ii=0; ii < nodeCount; ii++)
            {
                if(_nodes[ii] != null)
                    _nodes[ii]->setWorldMatrix(pose.getNodeMatrix(ii));
            }
        }
#endif

        setBoxesVisible(false);

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 1.0, 3000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising terrain scene : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}

void SceneModelTerrain::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


