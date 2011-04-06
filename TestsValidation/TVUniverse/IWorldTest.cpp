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
#include <Core/Logger.h>
#include <Window/Tools.h>
#include <TVTools.h>
#include "IWorldTest.h"

IWorldTest::IWorldTest() : _isInitialised(false), _camSpeed(3.0)
{
}

void IWorldTest::update(double elapsed)
{
    if(_pWorld != null)
    {
        Renderer::MainLightSettings lighting = _pWorld->getMainLightSettings();
        Renderer::RendererSettings settings = _pWorld->getRenderer()->getRenderSettings();
        Renderer::DebugSettings debug = _pWorld->getRenderer()->getDebugSettings();
        
        //TVTools::updateLighting(lighting, *_pInput, 0.05f);
        TVTools::updateRenderSettings(settings, *_pInput);
        TVTools::updateDebugSettings(debug, *_pInput);

        _pWorld->setMainLightSettings(lighting);
        _pWorld->getRenderer()->setRenderSettings(settings);
        _pWorld->getRenderer()->setDebugSettings(debug);

        if(_pInput->isKeyTyped('M'))
        {
            INF << L"-------------------------------------------------\n";
            INF << _pWorld->getRenderer()->getRenderStats();
        }

        if(_pCamera != null && _pInput != null)
        {
            Window::updateQuakeCamera(_camQuake, *_pInput, _camSpeed, 0.005, elapsed);
            _pCamera->setLocalMatrix(_camQuake.getWorldMatrix());
        }

        _pWorld->update(elapsed);
    }
}

void IWorldTest::render(const Ptr<Renderer::IRenderView> & pView)
{
    if(_pWorld != null && _pCamera != null)
    {
        float ratio = pView->getWidth() / std::max(1.0f, float(pView->getHeight()));
        _pCamera->getCamera()->setAspectRatio(ratio);

        if(_pWorld2 != null)
        {
            Core::List<Ptr<Universe::World> > worlds;
            worlds.push_back(_pWorld);
            worlds.push_back(_pWorld2);
            Universe::World::render(pView, _pCamera, worlds);
        }
        else
        {
            _pWorld->render(pView, _pCamera);
        }
    }
}
