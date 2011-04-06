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
#include <Core/StopWatch.h>
#include <Core/DateAndTime.h>
#include <Core/RT/Thread.h>
#include <Window/Application.h>

#define LOG_UPDATE_TIMES 0

namespace Window
{
//-----------------------------------------------------------------------------
Application::Application() :
    _hInst(0),
    _hPrevInstance(0),
    _nCmdShow(0),
    _deviceState(Gfx::EDeviceState(-1))
{
}
//-----------------------------------------------------------------------------
void Application::setMainArgs(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    _hInst = hInst;
    _hPrevInstance = hPrevInstance;
    _commandLine = String(String8(lpCmdLine));
    _nCmdShow = nCmdShow;
}
//-----------------------------------------------------------------------------
void Application::run()
{
    if(initialise())
    {
        Core::TimeValue prevTime(Core::Clock::universalTime());
        MSG msg = {0};

        Core::StopWatch sw;

        while(WM_QUIT != msg.message && !finished())
        {
            if(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            else
            {
                double elapsed = sw.elapsed().ftotalSeconds();
                sw.reset();
                internalUpdate(elapsed);
            }
        }
    }

    release();
}
//-----------------------------------------------------------------------------
void Application::internalUpdate(double elapsed)
{
#if LOG_UPDATE_TIMES
    _elapseds.push_back(elapsed);

    if(_elapseds.size() > 1000)
    {
        for(int32 ii=0; ii< _elapseds.size(); ii++)
            INF << Core::toString(_elapseds[ii]) << L"\n";
        _elapseds.clear();
    }
#endif

    // TODO decoupe
    if(!pauseWhenInactive() || getWinManager().isActive())
        update(elapsed);
    getInputManager().update();

    Renderer::IRenderer & renderer = getRenderer();
    Gfx::EDeviceState deviceState = renderer.getDeviceState();

    if(deviceState != _deviceState)
    {
        INF << L"Device state changed to " << Gfx::toString(deviceState) << L"\n";

        switch(deviceState)
        {
        case Gfx::DS_DEVICE_LOST:
            renderer.onDeviceLost();
            break;
        case Gfx::DS_DEVICE_NOT_RESET:
            if(_deviceState != Gfx::DS_DEVICE_LOST)
            {
                WAR << L"Device is in state DS_DEVICE_NOT_RESET but never was DS_DEVICE_LOST, let's do as if it were lost first\n";
                renderer.onDeviceLost();
            }
            renderer.onDeviceReset();
            break;
        case Gfx::DS_DEVICE_ERROR:
            ERR << L"Device is in error state, exiting\n";
            PostQuitMessage(0);
            break;
        }

        _deviceState = deviceState;
    }
    else if(_deviceState == Gfx::DS_DEVICE_NOT_RESET)
    {
        ERR << L"Device is still not reset, even after doing a reset, exiting\n";
        PostQuitMessage(0);
    }

    switch(_deviceState)
    {
    case Gfx::DS_DEVICE_OK:
    {
        if(getWinManager().isMinimized())
        {
            Core::Thread::sleep(20);
        }
        else
        {
            render();

            if(!getWinManager().isActive())
                Core::Thread::sleep(2);
        }
        break;
    }
    case Gfx::DS_DEVICE_LOST:
    case Gfx::DS_DEVICE_NOT_RESET:
    {
        Core::Thread::sleep(20);
        break;
    }
    case Gfx::DS_DEVICE_ERROR:
        break;
    }
}
//-----------------------------------------------------------------------------
}

