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
#include <Core/Math/MathTools.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/System/Files.h>
#include <Core/Logger.h>

#include <Packing/TexCompressionOnTheFly.h>
#include <Packing/LightmapGen.h>
#include <Packing/WorldPak.h>

#include <Workflow/TexCompression.h>

#include "WorldTestLightMaps.h"

#include <math.h>

#define PACK_LEVEL      0
#define ADD_CHARACTER   1

using namespace Core;
using namespace Universe;

//-----------------------------------------------------------------------------
void WorldTestLightMaps::initalise(const Ptr<Renderer::IRenderer> & pRenderer, 
                                   const Ptr<Core::VirtualFileSystem> & pVFS, 
                                   const Ptr<Window::InputManager> & pInput, 
                                   const Ptr<Audio::IDevice> & pDevice)
{
    try
    {
        _pRenderer = pRenderer;
        _pVFS = pVFS;
        _pWorld = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS));
        _pWorld->getRessourcesPool()->setOnTheFlyTextureGenerator(Ptr<IOnTheFlyTextureGenerator>(new Packing::OnTheFlyTextureGenerator()));
        _pInput = pInput;
        _camSpeed = 250.0;

        //Ptr<Core::InputStream> pWorldInput(_pVFS->readFile(L"/HOO/Levels/ViaOlympia/ViaOlympia3.world"));
        //Ptr<Core::InputStream> pWorldInput(_pVFS->readFile(L"/Tests/vo-test-lm.world"));
        Ptr<Core::InputStream> pWorldInput(_pVFS->readFile(L"/Tests/vo-test-om-pak.world"));
        //Ptr<Core::InputStream> pWorldInput(_pVFS->readFile(L"/Tests/terrain-lm.world"));
        //Ptr<Core::InputStream> pWorldInput(_pVFS->readFile(L"/Tests/dtc.world"));
        Ptr<Core::XMLDocument> pXML(new Core::XMLDocument(*pWorldInput));
        _pWorld->importWorldXML(*pXML);

    #if PACK_LEVEL
        {
            Packing::LightmapsGenOptions options;

            options.pWorld = _pWorld;
            options.path = L"/Temp/";
            options.prefix = L"lm-";
            options.compressTextures = false;
            options.debugCopy = false;
            options.pObserver = Ptr<Core::IProgressObs>(new Core::ProgressPrint());
            options.defaultResolution = 512;
            
            Packing::generateLightmaps(options);
        }
        {
            Packing::WorldPackingOptions options;
            options.path = L"/Tests/vo-tv-pak";
            options.pWorld = _pWorld;
            options.textureAtlasSize = 4096;
            options.batchSize = 3000;
            options.border = 2;
            options.pObserver = Ptr<Core::IProgressObs>(new Core::ProgressPrint());

            Packing::packWorld(options);
        }
    #endif

    #if ADD_CHARACTER
        Ptr<Universe::Node> pStartNode(_pWorld->getFirstNodeWithTag(L"PlayersPosition"));
        {
            Ptr<Universe::NodeSkeleton> pCharacter(_pWorld->createModel(L"/HOO/Characters/Silene/Silene.mdl"));
            pCharacter->animate(L"idle", 0.0f, 0.0f, 1.0f, true);
            pCharacter->setLocalPosition(Core::Vector3f(0.0f, 0.0f, -12.0f));
            pStartNode->addChild(pCharacter, false);
        }
        {
            Ptr<Universe::NodeSkeleton> pCharacter(_pWorld->createModel(L"/HOO/Characters/Silene/Silene.mdl"));
            pCharacter->animate(L"idle", 0.0f, 0.0f, 1.0f, true);
            pCharacter->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 12.0f));
            pCharacter->localYaw(f_PI);
            pStartNode->addChild(pCharacter, false);
        }
    #endif

        // Cam�ra
        Ptr<Universe::Node> pNode(_pWorld->getFirstNodeWithTag(L"CAM_IDLE"));

        _camQuake.setPosition(pNode->getWorldPosition());
        _camQuake.turnLeftRight(0.85f * f_PI);
        //_camQuake.
        _pCamera = _pWorld->createPerspectiveCamera(f_PI * 0.25f, 1.0f, 1.0f, 4000.0f);
    }
    catch(Core::Exception & e)
    {
        ERR << L"Error : " << e.getMessage() << L"\n";
    }
}
//------------------------------------------------------------------------------
void WorldTestLightMaps::update(double elapsed)
{
    IWorldTest::update(elapsed);

    if(_pInput->isKeyTyped('C'))
    {
        if(_pWorld->getSkyTexture() == L"")
            _pWorld->setSkyTexture(L"/Tests/skybox.ctex");
        else
            _pWorld->setSkyTexture(L"");
    }
}
//-----------------------------------------------------------------------------
