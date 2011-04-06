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
#include "SceneLayers.h"

#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Math/Random.h>
#include <Core/Math/MathTools.h>

#include <Assets/Tool.h>

#include <Core/Math/Color.h>
#include <Images/Operator.h>

const float MESH_DIMENSION = 1000.0f;

SceneLayers::SceneLayers(const TestSceneData & data) :
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

bool SceneLayers::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
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

        // Chargement terrain
        {
            _pModel = Ptr<Assets::TerrainModel>(new Assets::TerrainModel());
            pVFS->read(L"/Data/TUData/Terrains/Terrain00/terrain-model.tmdl", *_pModel);
        }
        
        // SkySphere
        {
            Ptr<Renderer::ITextureMap> pSkyTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));
            Renderer::SkysphereSettings sky(pSkyTexture, -0.05f);
            _pScene->setSkysphereSettings(sky);
        }

        // Modèle -> Renderer
        {
            int32 meshCount = _pModel->getMeshCount();
            int32 nodeCount = _pModel->getNodeCount();

            Core::List<Ptr<Renderer::IMesh> >     meshes;
            meshes.resize(meshCount);

            for(int32 ii=0; ii < meshCount; ii++)
                meshes[ii] = _pRenderer->uploadMesh(_pModel->getMeshPtr(ii), texGrabber);

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


                    Renderer::Material mat;
                    mat.emissive = Core::Vector4f(0.00f, 0.00f, 0.00f, 1.00f);
                    mat.diffuse  = Core::Vector4f(1.00f, 1.00f, 1.00f, 1.00f);
                    mat.specular = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.00f);
                    mat.dynamicShadowCaster = false;
                    mat.dynamicShadowReceiver = true;
                    mat.clampTextureBorder = true;
                    mat.internalTextureBorder = true;

                    _nodes[ii]->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, Core::Vector3f(100.0f, -500.0f, 0.0f)));
                    _nodes[ii]->setMaterial(mat);
                }
            }
        }

        _pVFS = pVFS;
        setBoxesVisible(false);

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 1.0, 3000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising terrain scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

void SceneLayers::generateLayers(bool diffuseOn, bool normalOn, bool detailOn, int32 size)
{
    int32 iN = 0;

    Core::AABoxf box;

    for(int32 ii=0; ii < _pModel->getMeshCount(); ii++)
        box.setUnion(_pModel->getMesh(ii).getBoundingBox());

    float sx = box.getSize().x;
    float sz = box.getSize().z;

    const Core::Vector4f LAYER1U(0.005f, 0.000f,  0.000f, 0.000f);
    const Core::Vector4f LAYER1V(0.000f, 0.000f, -0.005f, 0.000f);

    const Core::Vector4f LAYER2U(0.005f, 0.000f,  0.000f, 0.000f);
    const Core::Vector4f LAYER2V(0.000f, 0.000f, -0.005f, 0.000f);

    const Core::Vector4f LAYER3U(0.005f, 0.000f,  0.000f, 0.000f);
    const Core::Vector4f LAYER3V(0.000f, 0.000f, -0.005f, 0.000f);

    Renderer::LayerMapInfos lmd;
    Renderer::LayerMapInfos lmn;

    lmd.box = lmn.box = box;

    if(diffuseOn)
    {
        lmd.width = lmd.height = size;
        lmd.isNormalMap = false;

        // Layer 1
        {
            Renderer::LayerMapInfos::LayerInfo layer1;
            layer1.generateU = LAYER1U;
            layer1.generateV = LAYER1V;
            layer1.pSourceTexture = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-0-diffuse.jpg", Workflow::TEXTURE_COLORMAP);

            lmd.layers.push_back(layer1);
        }

        // Layer 2
        {
            Renderer::LayerMapInfos::LayerInfo layer2;
            layer2.generateU = LAYER2U;
            layer2.generateV = LAYER2V;
            layer2.pSourceTexture = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-1-diffuse.jpg", Workflow::TEXTURE_COLORMAP);
            layer2.pMaskTexture   = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-1-mask.jpg", Workflow::TEXTURE_COLORMAP);

            lmd.layers.push_back(layer2);
        }

        // Layer 3
        {
            Renderer::LayerMapInfos::LayerInfo layer3;
            layer3.generateU = LAYER3U;
            layer3.generateV = LAYER3V;
            layer3.pSourceTexture = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-2-diffuse.jpg", Workflow::TEXTURE_COLORMAP);
            layer3.pMaskTexture   = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-2-mask.jpg", Workflow::TEXTURE_COLORMAP);

            lmd.layers.push_back(layer3);
        }
    }

    if(normalOn || detailOn)
    {
        lmn.width = lmn.height = size;
        lmn.isNormalMap = true;
        lmn.useDetailLayer = detailOn;

        if(detailOn)
        {
            Renderer::LayerMapInfos::LayerInfo detailLayer;
            detailLayer.pSourceTexture = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/NormalMapSmall.jpg", Workflow::TEXTURE_NORMALMAP);
            detailLayer.color = Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f);

            float dx = 2.0f / float(detailLayer.pSourceTexture->getSourceTexture()->getWidth());
            float dy = 2.0f / float(detailLayer.pSourceTexture->getSourceTexture()->getHeight());

            detailLayer.generateU = Core::Vector4f(1.0f / sx, 0.000f, 0.000f,  0.5f + dx);
            detailLayer.generateV = Core::Vector4f(0.000f, 0.000f, -1.0f / sz,  0.5f + dy);

            lmn.detailLayer = detailLayer;
        }

        if(normalOn)
        {
            // Layer 1
            {
                Renderer::LayerMapInfos::LayerInfo layer1;
                layer1.generateU = LAYER1U;
                layer1.generateV = LAYER1V;
                //layer1.pSourceTexture = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-0-normal.jpg", Workflow::TEXTURE_NORMALMAP);
                layer1.pSourceTexture = loadPackedTexture(_pVFS, L"/Data/AssetsPacked/Tests/layers/layer-0-normal.tex");
                

                lmn.layers.push_back(layer1);
            }

            // Layer 2
            {
                Renderer::LayerMapInfos::LayerInfo layer2;
                layer2.generateU = LAYER2U;
                layer2.generateV = LAYER2V;
                //layer2.pSourceTexture = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-1-normal.jpg", Workflow::TEXTURE_NORMALMAP);
                layer2.pSourceTexture = loadPackedTexture(_pVFS, L"/Data/AssetsPacked/Tests/layers/layer-1-normal.tex");
                layer2.pMaskTexture   = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-1-mask.jpg", Workflow::TEXTURE_COLORMAP);

                lmn.layers.push_back(layer2);
            }

            // Layer 3
            {
                Renderer::LayerMapInfos::LayerInfo layer3;
                layer3.generateU = LAYER3U;
                layer3.generateV = LAYER3V;
                //layer3.pSourceTexture = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-2-normal.jpg", Workflow::TEXTURE_NORMALMAP);
                layer3.pSourceTexture = loadPackedTexture(_pVFS, L"/Data/AssetsPacked/Tests/layers/layer-2-normal.tex");
                layer3.pMaskTexture   = loadTexture(_pVFS, L"/Data/TUData/Terrains/Terrain00/Layers/layer-2-mask.jpg", Workflow::TEXTURE_COLORMAP);

                lmn.layers.push_back(layer3);
            }
        }
    }

    // Modèle -> Renderer
    for(int32 ii=0; ii < _nodes.size(); ii++)
    {
        if(_nodes[ii] != null)
        {
            INF << L"Creating layered textures for " << _pModel->getNode(ii).getName() << L"\n";
            if(diffuseOn)
            {
                Ptr<Core::Bitmap> pBitmap = _pRenderer->generateLayerMap(lmd, _nodes[ii]);
                Ptr<Images::Image> pImageDiffuse(new Images::Image(pBitmap));

                pImageDiffuse->saveFile(L"Local/Layers/" + _pModel->getNode(ii).getName() + L".tga");

                Renderer::Material mat = _nodes[ii]->getMaterial();
                mat.pDiffuseMap = uploadTexture(pImageDiffuse, Workflow::TEXTURE_COLORMAP);
                _nodes[ii]->setMaterial(mat);
            }
            else
            {
                Renderer::Material mat = _nodes[ii]->getMaterial();
                mat.pDiffuseMap = null;
                _nodes[ii]->setMaterial(mat);
            }

            if(normalOn || detailOn)
            {
                Ptr<Core::Bitmap> pBitmap = _pRenderer->generateLayerMap(lmn, _nodes[ii]);
                Ptr<Images::Image> pImageNormal(new Images::Image(pBitmap));

                pImageNormal->saveFile(L"Local/Layers/" + _pModel->getNode(ii).getName() + L".tga");

                Renderer::Material mat = _nodes[ii]->getMaterial();
                mat.pNormalMap  = uploadTexture(pImageNormal, Workflow::TEXTURE_NORMALMAP);
                _nodes[ii]->setMaterial(mat);
            }
            else
            {
                Renderer::Material mat = _nodes[ii]->getMaterial();
                mat.pNormalMap  = null;
                _nodes[ii]->setMaterial(mat);
            }
        }
    }
}

void SceneLayers::update(double elapsed)
{
    ITestScene::update(elapsed);

    if     (_pInput->isKeyTyped('1')) generateLayers(true,  false, false, 128);
    else if(_pInput->isKeyTyped('2')) generateLayers(false, true,  false, 256);
    else if(_pInput->isKeyTyped('3')) generateLayers(false, false, true,  256);
    else if(_pInput->isKeyTyped('4')) generateLayers(false, true,  true,  256);
    else if(_pInput->isKeyTyped('5')) generateLayers(true,  true,  true,  256);
    else if(_pInput->isKeyTyped('6')) generateLayers(true,  false, true,  256);
}

void SceneLayers::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


