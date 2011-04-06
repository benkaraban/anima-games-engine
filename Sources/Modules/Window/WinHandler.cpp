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
#include <Window/WinHandler.h>
#include <winsafe.h>

namespace Window
{
//-----------------------------------------------------------------------------
WinHandler::WinHandler(const Ptr<InputManager> & pInput) :
    _pInput(pInput),
    _width(1),
    _height(1),
    _isActive(true),
    _isMinimized(false)
{
}
//-----------------------------------------------------------------------------
bool WinHandler::onPaint(void * pHandle)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hWnd(static_cast<HWND>(pHandle));
    hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onDestroy(void * pHandle)
{
    PostQuitMessage(0);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onLeftButtonDoubleClick(void * pHandle)
{
    _pInput->mouseLB(true);
    _pInput->mouseLBDoubleClick();
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onLeftButtonDown(void * pHandle)
{
    SetCapture((HWND)pHandle);
    _pInput->mouseLB(true);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onLeftButtonUp(void * pHandle)
{
    SetCapture(NULL);
    _pInput->mouseLB(false);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onMiddleButtonDown(void * pHandle)
{
    _pInput->mouseMB(true);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onMiddleButtonUp(void * pHandle)
{
    _pInput->mouseMB(false);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onRightButtonDown(void * pHandle)
{
    _pInput->mouseRB(true);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onRightButtonUp(void * pHandle)
{
    _pInput->mouseRB(false);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onMouseMove(void * pHandle, int32 x, int32 y, int32 flags)
{
    _pInput->mouseMove(int16(x), int16(y));
    _pInput->mouseBoutons(flags);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onMouseWheel(void * pHandle, int32 roll, int32 flags)
{
    _pInput->mouseWheel(roll);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onKeyDown(void * pHandle, int32 keyCode)
{
    _pInput->keyDown(keyCode);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onKeyUp(void * pHandle, int32 keyCode)
{
    _pInput->keyUp(keyCode);
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onResize(void * pHandle, int32 width, int32 height)
{
    _width = width;
    _height = height;
    return true;
}
//-----------------------------------------------------------------------------
bool WinHandler::onActivate(void * pHandle, bool active)
{
    if(active != _isActive)
        if(active)
            INF << L"Window activation\n";
        else
            INF << L"Window deactivation\n";

    _isActive = active;
    return false;
}
//-----------------------------------------------------------------------------
bool WinHandler::onMinimize(void * pHandle, bool minimized)
{
    if(minimized != _isMinimized)
        if(minimized)
            INF << L"Window minimized\n";
        else
            INF << L"Window restored\n";

    _isMinimized = minimized;
    return false;
}
//-----------------------------------------------------------------------------
bool WinHandler::onChar(void * pHandle, wchar_t c)
{
    _pInput->character(c);
    return true;
}
//-----------------------------------------------------------------------------
int32 WinHandler::getWindowWidth() const
{
    return _width;
}
//-----------------------------------------------------------------------------
int32 WinHandler::getWindowHeight() const
{
    return _height;
}
//-----------------------------------------------------------------------------
bool WinHandler::isActive() const
{
    return _isActive;
}
//-----------------------------------------------------------------------------
bool WinHandler::isMinimized() const
{
    return _isMinimized;
}
//-----------------------------------------------------------------------------
}

