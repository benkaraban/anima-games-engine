/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include <Core/Math/MathTools.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/System/Files.h>

#include <Packing/TexCompressionOnTheFly.h>
#include <Packing/TerrainTextureGen.h>

#include <Workflow/TexCompression.h>

#include "WorldTestTerrain.h"

#include <math.h>

using namespace Core;
using namespace Universe;

//-----------------------------------------------------------------------------
void WorldTestTerrain::initalise(const Ptr<Renderer::IRenderer> & pRenderer, 
                                 const Ptr<Core::VirtualFileSystem> & pVFS, 
                                 const Ptr<Window::InputManager> & pInput,
                                 const Ptr<Audio::IDevice> & pDevice)
{
    _pRenderer = pRenderer;
    _pVFS = pVFS;
    _pWorld = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS));
    _pWorld->getRessourcesPool()->setOnTheFlyTextureGenerator(Ptr<IOnTheFlyTextureGenerator>(new Packing::OnTheFlyTextureGenerator()));
    _pInput = pInput;
    _camSpeed = 800.0;

    // Ciel

    _pWorld->setSkyTexture(L"/TUData/Textures/FullskiesSunset0024_1_S.tex");
    _pWorld->setSkyColor(Core::Vector4f(1.0, 1.0, 1.0, 0.0));
    _pWorld->setSkyHorizon(-0.25);

    // Fog

    _pWorld->setFogColor(Core::Vector4f(0.15f, 0.1f, 0.1f, 0.0f));
    _pWorld->setFogRange(75.0, 500.0);
    _pWorld->setFogSkyOpacity(0.0);

    // Light

    Renderer::MainLightSettings light = _pWorld->getMainLightSettings();
    light.ambient  = Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    light.diffuse0 = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    light.diffuse1 = Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    light.diffuse2 = Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    light.specular = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    light.mulIntensity = 1.0f;
    light.sunAngleToZenith = 0.0f;
    light.sunAngleToNorth = 1.0f;
    light.dynamicShadowOpacity = 0.5f;
    light.dynamicShadowMiddle = 10.0f;
    light.dynamicShadowMiddleBlend = 5.0f;
    light.dynamicShadowEnd = 50.0f;
    light.dynamicShadowEndBlend = 0.0f;
    light.bloomThreshold = 0.0f;
    light.bloomFactor = 0.0f;
    light.occlusionContrast = 0.0f;
    light.occlusionLuminosity = 0.0f;

    _pWorld->setMainLightSettings(light);


    // Terrain

    static const float S = 1.0f;

    Ptr<NodeTerrain> pTerrain = _pWorld->createTerrain(L"/TUDataRaw/Terrains/Terrain00/terrain-model.tmdl");
    pTerrain->setLocalPosition(Core::Vector3f(100.0f, -100.0f, 0.0f));
    pTerrain->setLocalScale(Core::Vector3f(S, S, S));
    pTerrain->setDiffuse(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    pTerrain->setSpecular(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    pTerrain->setGlow(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    pTerrain->setShininess(8.0f);

    Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument(L"doc"));
    Ptr<Core::XMLNode> pXMLNode(new Core::XMLNode(L"node"));

    Core::List<NodeTerrain::Layer> & layers = pTerrain->getLayers();

    layers.resize(3);

    layers[0].diffuseTexName = L"Data/TUData/Terrains/Terrain00/Layers/layer-0-diffuse.jpg";
    layers[0].normalTexName  = L"Data/TUData/Terrains/Terrain00/Layers/layer-0-normal.jpg";
    layers[0].maskTexName    = L"";
    layers[0].width    = 100.0f;
    layers[0].height   = 100.0f;
    layers[0].angle    = 0.0f;
    layers[0].color    = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    layers[1].diffuseTexName = L"Data/TUData/Terrains/Terrain00/Layers/layer-1-diffuse.jpg";
    layers[1].normalTexName  = L"Data/TUData/Terrains/Terrain00/Layers/layer-1-normal.jpg";
    layers[1].maskTexName    = L"Data/TUData/Terrains/Terrain00/Layers/layer-1-mask.jpg";
    layers[1].width    = 20.0f;
    layers[1].height   = 20.0f;
    layers[1].angle    = 0.0f;
    layers[1].color    = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    layers[2].diffuseTexName = L"Data/TUData/Terrains/Terrain00/Layers/layer-2-diffuse.jpg";
    layers[2].normalTexName  = L"Data/TUData/Terrains/Terrain00/Layers/layer-2-normal.jpg";
    layers[2].maskTexName    = L"Data/TUData/Terrains/Terrain00/Layers/layer-2-mask.jpg";
    layers[2].width    = 50.0f;
    layers[2].height   = 50.0f;
    layers[2].angle    = 0.0f;
    layers[2].color    = Core::Vector4f(1.0f, 0.3f, 0.3f, 1.0f);

    //pXMLDoc->addNode(pXMLNode);
    //_pWorld->exportXML(pXMLNode, pTerrain);
    //pXMLDoc->saveDocument(L"Local/terrain.xml");

    //
    //pTerrain->kill();
    //pTerrain = LM_DEBUG_PTR_CAST<NodeTerrain>(_pWorld->importXML(pXMLNode));
    //_pTerrain = pTerrain;

    // Teapot

    Ptr<Universe::NodeMesh> pMesh(_pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01"));

    pMesh->setWorldPosition(Core::Vector3f(0.0, 0.0, 50.0));
    pMesh->setLocalScale(Core::Vector3f(0.1f, 0.1f, 0.1f));

    // Pas une texture compressÈe dans le VFS !
    pMesh->setDiffuseTexture(L"Data/TUData/Textures/Sand00.jpg");
    pMesh->setNormalTexture(L"Data/TUData/Textures/stones01-normal.jpg");

    Renderer::Material mat = pMesh->getMeshInstance()->getMaterial();
    mat.specular = Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    pMesh->getMeshInstance()->setMaterial(mat);

    _pTeapot = pMesh;

    // CamÈra

    _camQuake.setPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    _pCamera = _pWorld->createPerspectiveCamera(f_PI * 0.25f, 1.0f, 1.0f, 4000.0f);
}
//------------------------------------------------------------------------------
void WorldTestTerrain::generateLayers(bool useGlobal, float weightGlobal)
{
    if(useGlobal)
        _pTerrain->setGlobalNormalTexture(L"Data/TUData/Terrains/Terrain00/NormalMapSmall.jpg");
    else
        _pTerrain->setGlobalNormalTexture(L"");

    _pTerrain->setGlobalNormalWeight(weightGlobal);

    System::createDirectory(L"Local/blocks-textures");

    Ptr<Core::VFSDirectMapping> pLocalVFS(new Core::VFSDirectMapping());
    pLocalVFS->linkDirectory(L".");

    Packing::TerrainTextureGenOptions options;
    options.pPool = _pWorld->getRessourcesPool();
    options.pRenderer = _pRenderer;
    options.pVFS = pLocalVFS;
    options.path = L"/Local/blocks-textures";
    options.prefix = L"test-";
    options.compressTextures = false;
    options.pTerrain = _pTerrain;
    options.pObserver = Ptr<Core::IProgressObs>(new Core::ProgressPrint());

    options.resolution.push_back(256);

    Packing::generateTerrainTexture(options);
}
//------------------------------------------------------------------------------
void WorldTestTerrain::update(double elapsed)
{
    if     (_pInput->isKeyTyped('1')) generateLayers(false, 0.0f);
    else if(_pInput->isKeyTyped('2')) generateLayers(true,  0.3f);
    else if(_pInput->isKeyTyped('3')) generateLayers(true,  1.0f);

    IWorldTest::update(elapsed);
}
//-----------------------------------------------------------------------------
