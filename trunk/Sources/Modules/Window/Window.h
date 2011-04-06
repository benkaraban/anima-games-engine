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
#ifndef WINDOW_WINDOW_H_
#define WINDOW_WINDOW_H_

#include <Core/Map.h>
#include <Core/Bitmap.h>

namespace Window
{
//-----------------------------------------------------------------------------
const int32 FLAG_LBUTTON = 0x00000001;
const int32 FLAG_MBUTTON = 0x00000002;
const int32 FLAG_RBUTTON = 0x00000004;
const int32 FLAG_CTRL_KEY = 0x00000008;
const int32 FLAG_SHIFT_KEY = 0x00000010;

//-----------------------------------------------------------------------------
class LM_API_WIN IWindowMessageHandler
{
public:
    virtual ~IWindowMessageHandler() {};
    virtual bool onPaint(void * pHandle) = 0;
    virtual bool onDestroy(void * pHandle) = 0;
    virtual bool onLeftButtonDoubleClick(void * pHandle) = 0;
    virtual bool onLeftButtonDown(void * pHandle) = 0;
    virtual bool onLeftButtonUp(void * pHandle) = 0;
    virtual bool onMiddleButtonDown(void * pHandle) = 0;
    virtual bool onMiddleButtonUp(void * pHandle) = 0;
    virtual bool onRightButtonDown(void * pHandle) = 0;
    virtual bool onRightButtonUp(void * pHandle) = 0;
    virtual bool onMouseMove(void * pHandle, int32 x, int32 y, int32 flags) = 0;
    virtual bool onMouseWheel(void * pHandle, int32 roll, int32 flags) = 0;
    virtual bool onKeyDown(void * pHandle, int32 keyCode) = 0;
    virtual bool onKeyUp(void * pHandle, int32 keyCode) = 0;
    virtual bool onChar(void * pHandle, wchar_t c) = 0;
    virtual bool onResize(void * pHandle, int32 width, int32 height) = 0;
    virtual bool onActivate(void * pHandle, bool active) = 0;
    virtual bool onMinimize(void * pHandle, bool minimized) = 0;
};

//-----------------------------------------------------------------------------
class LM_API_WIN Window : public Core::Object
{
public:
    Window(void * hInst, const String & title, int32 left, int32 top, int32 width, int32 height,
           bool isFullscreen, const Ptr<IWindowMessageHandler> & pWinHandler, void * hIcon = NULL);

    ~Window();

    void destroy();

    void toggleFullscreen(bool fullscreen, int32 width, int32 height);
    void resize(int32 width, int32 height);

    void setCursor(const Ptr<Core::Bitmap> & pCursor, int32 xSpot, int32 ySpot);

    virtual String getTitle() const {return _title;};
    virtual int32 getWidth() const {return _width;};
    virtual int32 getHeight() const {return _height;};
    virtual bool isFullscreen() const {return _isFullscreen;};
    virtual void * getHandle() const {return _hwnd;};

    struct CursorData
    {
        CursorData(const Ptr<Core::Bitmap> & pBitmap, int32 xs, int32 ys) : pBitmap(pBitmap), xSpot(xs), ySpot(ys)
        {}

        Ptr<Core::Bitmap>   pBitmap;
        int32               xSpot;
        int32               ySpot;

        bool operator < (const CursorData & cd) const
        {
            if(pBitmap != cd.pBitmap)
                return pBitmap < cd.pBitmap;
            else
                if(xSpot != cd.xSpot)
                    return xSpot < cd.xSpot;
                else
                    return ySpot < cd.ySpot;
        }
    };

protected:
    void *      _hinstance;
    void *      _hwnd;
    String      _title;
    int32       _top;
    int32       _left;
    int32       _width;
    int32       _height;
    bool        _isFullscreen;

    Core::Map<CursorData, void*>  _cursors;
};
}

#endif
