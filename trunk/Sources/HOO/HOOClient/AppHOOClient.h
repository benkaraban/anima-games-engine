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
#ifndef APPHOOCLIENT_H_
#define APPHOOCLIENT_H_

#ifdef LM_DEVMODE
#include <Core/Debug/FPSCounter.h>
#endif

#include <Window/Application.h>
#include <Window/WinHandler.h>

#include <HOOClient/IAppState.h>
#include <HOOClient/GUI/WidgetFactory.h>
#include <HOOClient/OutGameMusic.h>

class AppHOOClient : public Window::Application
{
public:
    AppHOOClient();
    virtual ~AppHOOClient();

protected:
    virtual bool initialise();
    virtual void update(double elapsed);
    virtual void render();
    virtual void release();
    virtual bool finished();

    virtual Renderer::IRenderer &   getRenderer() { return *_appContext.pRenderer; }
    virtual Window::InputManager &  getInputManager() { return *_appContext.pInput; }
    virtual Window::WinHandler &    getWinManager() { return *_appContext.pWinHandler; }

    void setState(EAppState state);

    AppContext              _appContext;
    Ptr<IAppState>          _states[EAppState_COUNT];
    Ptr<IAppState>          _pState;
#ifdef LM_DEVMODE
    Core::FPSCounter            _debugFPS;
    Ptr<Renderer::IHUD>         _pDebugHUD;
    Ptr<Renderer::IText>        _pDebugText;
    double                      _debugTimeUpdate;
    double                      _debugTimeRender;
    double                      _debugTimeFlip;
    Ptr<Renderer::IRectangle>   _pDebugRectUpdate;
    Ptr<Renderer::IRectangle>   _pDebugRectRender;
    Ptr<Renderer::IRectangle>   _pDebugRectFlip;
#endif
};

#endif /* APPTVUNIVERSE_H_ */
