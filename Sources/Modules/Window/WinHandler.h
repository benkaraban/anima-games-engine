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
#ifndef WINHANDLER_H_
#define WINHANDLER_H_

#include <Window/Window.h>
#include <Window/InputManager.h>

namespace Window
{
class LM_API_WIN WinHandler : public Core::Object, public IWindowMessageHandler
{
public:
    WinHandler(const Ptr<InputManager> & pInput);

    virtual bool onPaint(void * pHandle);
    virtual bool onDestroy(void * pHandle);

    virtual bool onLeftButtonDoubleClick(void * pHandle);
    virtual bool onLeftButtonDown(void * pHandle);
    virtual bool onLeftButtonUp(void * pHandle);

    virtual bool onMiddleButtonDown(void * pHandle);
    virtual bool onMiddleButtonUp(void * pHandle);

    virtual bool onRightButtonDown(void * pHandle);
    virtual bool onRightButtonUp(void * pHandle);

    virtual bool onMouseMove(void * pHandle, int32 x, int32 y, int32 flags);
    virtual bool onMouseWheel(void * pHandle, int32 roll, int32 flags);
    virtual bool onKeyDown(void * pHandle, int32 keyCode);
    virtual bool onKeyUp(void * pHandle, int32 keyCode);
    virtual bool onChar(void * pHandle, wchar_t c);
    virtual bool onResize(void * pHandle, int32 width, int32 height);
    virtual bool onActivate(void * pHandle, bool active);
    virtual bool onMinimize(void * pHandle, bool minimized);

    virtual int32 getWindowWidth() const;
    virtual int32 getWindowHeight() const;
    virtual bool isActive() const;
    virtual bool isMinimized() const;

protected:
    Ptr<InputManager>           _pInput;
    int32                       _width;
    int32                       _height;
    bool                        _isActive;
    bool                        _isMinimized;
};

}

#endif /* WINHANDLER_H_ */
