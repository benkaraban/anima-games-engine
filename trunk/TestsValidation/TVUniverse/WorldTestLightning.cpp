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
#include "WorldTestLightning.h"

static const bool VISIBLE_BOUNDING_BOXES = false;

//-----------------------------------------------------------------------------
void WorldTestLightning::initalise(const Ptr<Renderer::IRenderer> & pRenderer, 
                                    const Ptr<Core::VirtualFileSystem> & pVFS, 
                                    const Ptr<Window::InputManager> & pInput, 
                                    const Ptr<Audio::IDevice> & pDevice)
{
    _pRenderer = pRenderer;
    _pVFS = pVFS;
    _pAudioDevice = pDevice;
    _pWorld = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS, null, _pAudioDevice));
    _pInput = pInput;
    
    // Sun

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

    // Ciel

    _pWorld->setSkyTexture(L"/TUData/Textures/FullskiesSunset0024_1_S.tex");
    _pWorld->setSkyColor(Core::Vector4f(1.0, 1.0, 1.0, 0.0));
    _pWorld->setSkyHorizon(-0.25);

    // Fog

    _pWorld->setFogColor(Core::Vector4f(0.2f, 0.2f, 0.2f, 0.0f));
    _pWorld->setFogRange(10.0f, 20.0f);
    _pWorld->setFogSkyOpacity(0.0f);

    // Storm

    Ptr<Universe::NodeStorm> pStorm = _pWorld->createStorm();

    static const float PERIOD = 0.002f;
    pStorm->setGenerationTime(0.0, 0.1);
    pStorm->setTailSphereSize(Core::Vector3f(4.0f, 0.0f, 4.0f));
    pStorm->setMaxCount(1000);
    pStorm->setFertility(5);
    pStorm->setPeriod(PERIOD, PERIOD);
    pStorm->setResolution(30);
    pStorm->setEntropy(0.15f);
    pStorm->setHeadWidth(0.2f);
    pStorm->setTailWidth(0.05f);
    pStorm->setFadeInTime(0.0f);
    pStorm->setDuration(0.1f);
    pStorm->setFadeOutTime(0.05f);
    pStorm->setColor(Core::Vector4f(0.4f, 0.4f, 1.0f, 1.0f), Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f));
    pStorm->setGlow(Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f), Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f));

    //static const float PERIOD = 0.02f;
    //pStorm->setMaxCount(1000);
    //pStorm->setPeriod(PERIOD, PERIOD);
    //pStorm->setResolution(30);
    //pStorm->setEntropy(0.15f);
    //pStorm->setHeadWidth(0.125f);
    //pStorm->setTailWidth(0.125f);
    //pStorm->setFadeInTime(0.0f);
    //pStorm->setDuration(0.2f);
    //pStorm->setFadeOutTime(0.1f);
    //pStorm->setColor(Core::Vector4f(0.4f, 0.4f, 1.0f, 1.0f), Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f));
    //pStorm->setGlow(Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f), Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f));

    //pStorm->setWorldPosition(Core::Vector3f(-10.0f, 0.0f, 0.0f));
    //pStorm->setBoundingBoxVisible(true);

    pStorm->addTag(L"STORM");

    // Ghost
    //pStorm->setPeriod(0.5f, 0.5f);
    //pStorm->setFadeInTime(0.5f);
    //pStorm->setFadeOutTime(0.5f);
    //pStorm->setResolution(5);
    //pStorm->setHeadWidth(1.0);
    //pStorm->setTailWidth(1.0);
    //pStorm->setEntropy(0.05f);
    //pStorm->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
    //pStorm->setGlow(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));


    {
        Core::XMLDocument xmlDocument;
        _pWorld->exportWorldXML(xmlDocument);
        if(!xmlDocument.saveDocument(L"./Local/tv-universe00.world.xml"))
            LM_ASSERT(false);

        Core::XMLDocument xmlDocument2;
        if(!xmlDocument2.loadDocument(L"./Local/tv-universe00.world.xml"))
            LM_ASSERT(false);

        _pWorld->importWorldXML(xmlDocument2);

        pStorm = LM_DEBUG_PTR_CAST<Universe::NodeStorm>(_pWorld->getFirstNodeWithTag(L"STORM"));
    }


    // Caméra

    _camQuake.setPosition(Core::Vector3f(0.0f, 0.0f, -50.0f));
    _pCamera = _pWorld->createPerspectiveCamera(f_PI * 0.25f, 1.0f, 0.1f, 200.0f);
    _pStorm = pStorm;
}

void WorldTestLightning::update(double elapsed)
{
    IWorldTest::update(elapsed);

    if(_pInput->isKeyTyped('X'))
    {
        _pStorm->restart();
    }
    else if(_pInput->isKeyTyped('W'))
    {
        _pWorld->runInline(L"world:getFirstNodeWithTag('STORM'):setGenerationTime(0.0, 0.1)");
        _pWorld->runInline(L"world:getFirstNodeWithTag('STORM'):killAtEnd(true)");
        _pWorld->runInline(L"world:getFirstNodeWithTag('STORM'):restart()");
    }
}
//-----------------------------------------------------------------------------
