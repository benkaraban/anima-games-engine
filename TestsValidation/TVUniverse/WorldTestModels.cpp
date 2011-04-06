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

#include <Packing/TexCompressionOnTheFly.h>
#include <Packing/TerrainTextureGen.h>

#include <Workflow/TexCompression.h>

#include "WorldTestModels.h"

#include <math.h>

using namespace Core;
using namespace Universe;

//-----------------------------------------------------------------------------
void WorldTestModels::initalise(const Ptr<Renderer::IRenderer> & pRenderer, 
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


    // Teapot

    _pModel = _pWorld->createModel(L"/TUData/Models/simplescene.mdl");
    _pModel->setWorldPosition(Core::Vector3f(0.0f, 0.0f, 60.0f));

    // Cam�ra

    _camQuake.setPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    _pCamera = _pWorld->createPerspectiveCamera(f_PI * 0.25f, 1.0f, 1.0f, 4000.0f);
}
//------------------------------------------------------------------------------
void WorldTestModels::update(double elapsed)
{
    if(_pInput->isKeyTyped('R'))
    {
        _pWorld->getRessourcesPool()->reloadModel(L"/TUData/Models/simplescene.mdl");
        _pModelReload = _pWorld->createInstance(L"/TUData/Models/simplescene.mdl");
        _pModelReload->setWorldPosition(Core::Vector3f(0.0f, 30.0f, 60.0f));
    }

    IWorldTest::update(elapsed);
}
//-----------------------------------------------------------------------------
