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
#ifndef WINDOW_APPLICATION_H_
#define WINDOW_APPLICATION_H_

#include <Core/Standard.h>
#include <Gfx/GfxType.h>
#include <Renderer/IRenderer.h>

#include <Window/InputManager.h>
#include <Window/WinHandler.h>
#include <Window/Window.h>

namespace Window
{
class LM_API_WIN Application : public Core::Object
{
public:
    Application();
    virtual ~Application() {};

    virtual void setMainArgs(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
    virtual void run();

    HINSTANCE getInstance() { return _hInst; };
    const String & getCommandLine() { return _commandLine; };

protected:
    virtual bool initialise() { return true; };
    virtual void release() {};

    virtual void update(double elapsed) {};
    virtual void render() {};
    virtual bool finished() { return false; }
    virtual bool pauseWhenInactive() { return false; }

    virtual Renderer::IRenderer &   getRenderer() = 0;
    virtual InputManager &          getInputManager() = 0;
    virtual WinHandler &            getWinManager() = 0;

private:
    void internalUpdate(double elapsed);

    HINSTANCE   _hInst;
    HINSTANCE   _hPrevInstance;
    String      _commandLine;
    int         _nCmdShow;

    Gfx::EDeviceState   _deviceState;

    Core::List<double> _elapseds;
};
}

#endif /* APPLICATION_H_ */
