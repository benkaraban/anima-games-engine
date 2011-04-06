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
#ifndef CHRONORAGE_CHRONORAGE_H_
#define CHRONORAGE_CHRONORAGE_H_

#include <Core/Debug/FPSCounter.h>

#include <Modes/IGameState.h>


namespace ChronoRage
{

class LM_API_CHR ChronoRage : public Window::Application
{
public:
    ChronoRage();
    virtual ~ChronoRage();

    virtual void run();
    const Ptr<Window::Window> & getWindow() { return _appContext.pRenderWindow; }

protected:
    virtual bool initialise();
    virtual void release();
    virtual void update(double elapsed);
    virtual void render();

    virtual Renderer::IRenderer &   getRenderer() { return *_appContext.pRenderer; }
    virtual Window::InputManager &  getInputManager() { return *_appContext.pInputManager; }
    virtual Window::WinHandler &    getWinManager() { return *_appContext.pWinHandler; }

    void initGameState();
    virtual bool pauseWhenInactive() { return true; }

protected:
    EGameStateMode              _currentMode;
    AppContext                  _appContext;

    Ptr<IGameState>             _pGameState;

#if LM_DISPLAY_FPS
    Core::FPSCounter            _fps;
    Ptr<Renderer::IHUD>         _pHUD;
    Ptr<Renderer::IFont>        _pFontFPS;
    Ptr<Renderer::IText>        _pTextFPS;
    Ptr<Renderer::IText>        _pTextFoesNumber;
    Ptr<Renderer::IText>        _pTextPowerLevel;
    Ptr<Renderer::IText>        _pTextTimeWarpNumber;
    bool                        _showRenderStats;
#endif
};

}//namespace ChronoRage

#endif/*CHRONORAGE_CHRONORAGE_H_*/