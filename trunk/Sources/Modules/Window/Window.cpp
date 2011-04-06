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
#include <Core/Bitmap.h>

#include <Window/Window.h>
#include <Window/JoystickDI.h>

#include <winsafe.h>

namespace Window
{
//-----------------------------------------------------------------------------
static Core::Map<int32, Ptr<IWindowMessageHandler> > winMap;
//-----------------------------------------------------------------------------
static int32 getMouseFlags(WPARAM wParam)
{
    int32 flags = 0;

    if(wParam & MK_LBUTTON) flags |= FLAG_LBUTTON;
    if(wParam & MK_MBUTTON) flags |= FLAG_MBUTTON;
    if(wParam & MK_RBUTTON) flags |= FLAG_RBUTTON;
    if(wParam & MK_CONTROL) flags |= FLAG_CTRL_KEY;
    if(wParam & MK_SHIFT) flags |= FLAG_SHIFT_KEY;

    return flags;
}
//-----------------------------------------------------------------------------
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool messageHandled = false;
    int32 id = int32(hWnd);

    if(message == WM_CREATE)
    {
        CREATESTRUCTW * cs = reinterpret_cast<CREATESTRUCTW *>(lParam);
        Ptr<IWindowMessageHandler> * ppWinHandler = reinterpret_cast<Ptr<IWindowMessageHandler> *>(cs->lpCreateParams);
        winMap[id] = *ppWinHandler;
    }

    Core::Map<int32, Ptr<IWindowMessageHandler> >::iterator ipHandler = winMap.find(id);

    if(ipHandler != winMap.end())
    {
        switch(message)
        {
        case WM_SHOWWINDOW:
            INF << L"Show window : " << Core::toString(wParam) << L"   " << Core::toString(lParam) << L"\n";
            break;


        case WM_PAINT:
            messageHandled = ipHandler->second->onPaint(hWnd);
            break;

        case WM_CHAR:
        case WM_UNICHAR:
            messageHandled = ipHandler->second->onChar(hWnd, wchar_t(wParam));
            break;

        case WM_HOTKEY:
            messageHandled = false;
            break;

        case WM_ACTIVATEAPP:
            messageHandled = ipHandler->second->onActivate(hWnd, wParam != 0);
            break;

        case WM_SIZE:
            if(wParam == SIZE_MINIMIZED)
                messageHandled = ipHandler->second->onMinimize(hWnd, true);
            else if(wParam == SIZE_RESTORED)
                messageHandled = ipHandler->second->onMinimize(hWnd, false);

            messageHandled = ipHandler->second->onResize(hWnd, LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_DESTROY:
            messageHandled = ipHandler->second->onDestroy(hWnd);
            break;

        case WM_KEYDOWN:
            messageHandled = ipHandler->second->onKeyDown(hWnd, wParam);
            break;

        case WM_KEYUP:
            messageHandled = ipHandler->second->onKeyUp(hWnd, wParam);
            break;

        case WM_LBUTTONDBLCLK:
            messageHandled = ipHandler->second->onLeftButtonDoubleClick(hWnd);
            break;

        case WM_LBUTTONDOWN:
            messageHandled = ipHandler->second->onLeftButtonDown(hWnd);
            break;

        case WM_LBUTTONUP:
            messageHandled = ipHandler->second->onLeftButtonUp(hWnd);
            break;

        case WM_MBUTTONDOWN:
            messageHandled = ipHandler->second->onMiddleButtonDown(hWnd);
            break;

        case WM_MBUTTONUP:
            messageHandled = ipHandler->second->onMiddleButtonUp(hWnd);
            break;

        case WM_RBUTTONDOWN:
            messageHandled = ipHandler->second->onRightButtonDown(hWnd);
            break;

        case WM_RBUTTONUP:
            messageHandled = ipHandler->second->onRightButtonUp(hWnd);
            break;

        case WM_MOUSEMOVE:
            messageHandled = ipHandler->second->onMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam), getMouseFlags(wParam));
            break;

        case WM_MOUSEWHEEL:
            messageHandled = ipHandler->second->onMouseWheel(hWnd, int32(int16(HIWORD(wParam))) / WHEEL_DELTA, getMouseFlags(wParam));
            break;

        case WM_SYSKEYDOWN:
            messageHandled = (wParam == VK_MENU);
            break;
        }
    }

    if(messageHandled)
        return 0;
    else
        return DefWindowProcW(hWnd, message, wParam, lParam);
 }
//-----------------------------------------------------------------------------
void clientResize(HWND hWnd, int32 nWidth, int32 nHeight)
{
    RECT rcClient, rcWindow;
    POINT ptDiff;
    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(hWnd,rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}
//-----------------------------------------------------------------------------
static const int32 COMMON_FLAGS             = WS_VISIBLE | WS_CLIPCHILDREN;
static const int32 FULLSCREEN_MODE_FLAGS    = COMMON_FLAGS | WS_POPUP;
#ifdef LM_DEVMODE
static const int32 WINDOWED_MODE_FLAGS      = COMMON_FLAGS | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
#else
static const int32 WINDOWED_MODE_FLAGS      = COMMON_FLAGS | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
#endif
//-----------------------------------------------------------------------------
Window::Window(
        void * hInst,
        const String & title,
        int32 left,
        int32 top,
        int32 width,
        int32 height,
        bool isFullscreen,
        const Ptr<IWindowMessageHandler> & pWinHandler,
        void * hIcon)
: _hinstance(hInst),
_hwnd(NULL),
_title(title),
_top(top),
_left(left),
_width(width),
_height(height),
_isFullscreen(isFullscreen)
{
    INF << L"Creating window " << Core::toString(width) << L"x" << Core::toString(height) << L" @ "
    << Core::toString(top) << L"," << Core::toString(left) << L" fullscreen : " << Core::strYesNo(isFullscreen) << L"\n";
    
    DWORD dwStyle = isFullscreen ? FULLSCREEN_MODE_FLAGS : WINDOWED_MODE_FLAGS;

    if(isFullscreen)
    {
        _top = 0;
        _left = 0;
    }

    if(hIcon == NULL)
        hIcon = LoadIcon(0, IDI_APPLICATION);

    WNDCLASSW wc =
    {   CS_DBLCLKS, WndProc, 0, 0, HINSTANCE(hInst),
        HICON(hIcon), NULL,
        (HBRUSH)GetStockObject(BLACK_BRUSH), 0, L"AGWin"
    };
    RegisterClassW(&wc);

    _hwnd = CreateWindowW(L"AGWin", title.c_str(), dwStyle, left, top, width, height, NULL, 0, HINSTANCE(hInst),
            const_cast<void*>(reinterpret_cast<const void*>(&pWinHandler)));

    if(_hwnd == NULL)
    {
        ERR << "Unable to create window. Error : " << Core::toString(int32(GetLastError())) << L"\n";
    }
    else
    {
        clientResize((HWND)_hwnd, _width, _height);
    }
}
//-----------------------------------------------------------------------------
void Window::toggleFullscreen(bool fullscreen, int32 width, int32 height)
{
    if(fullscreen)
    {
        _left = 0;
        _top = 0;
    }

    if(fullscreen && !_isFullscreen) // window -> fullscreen
    {
        SetWindowLong((HWND)_hwnd, GWL_STYLE, FULLSCREEN_MODE_FLAGS);
        SetWindowPos((HWND)_hwnd, HWND_NOTOPMOST, _left, _top, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED);
        clientResize((HWND)_hwnd, width, height);
    }
    else if(!fullscreen && _isFullscreen) // fullscreen -> window
    {
        SetWindowLong((HWND)_hwnd, GWL_STYLE, WINDOWED_MODE_FLAGS);
        SetWindowPos((HWND)_hwnd, HWND_NOTOPMOST, _left, _top, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED);
        clientResize((HWND)_hwnd, width, height);
    }
    else if(_width != width || _height != height)
    {
        clientResize((HWND)_hwnd, width, height);
    }

    _isFullscreen = fullscreen;
    _width = width;
    _height = height;
}
//-----------------------------------------------------------------------------
Window::~Window()
{
    destroy();
}
//-----------------------------------------------------------------------------
void Window::destroy()
{
    if(_hwnd != NULL)
    {
        Core::Map<int32, Ptr<IWindowMessageHandler> >::iterator ipHandler = winMap.find(int32(_hwnd));

        if(ipHandler != winMap.end())
            winMap.erase(ipHandler);

        DestroyWindow(HWND(_hwnd));
    }
    _hwnd = 0;
}
//-----------------------------------------------------------------------------
HCURSOR createAlphaCursor(const Window::CursorData & cd)
{
    int32 width = cd.pBitmap->getWidth();
    int32 height = cd.pBitmap->getHeight();

    BITMAPV5HEADER bi;

    if(cd.pBitmap->getFormat() != Core::ARGB_U8)
        throw Core::Exception(L"Incorrect bitmap format passed to CreateCursor");

    ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
    bi.bV5Size          = sizeof(BITMAPV5HEADER);
    bi.bV5Width         = width;
    bi.bV5Height        = height;
    bi.bV5Planes        = 1;
    bi.bV5BitCount      = 32;
    bi.bV5Compression   = BI_BITFIELDS;
    bi.bV5RedMask       = 0x00FF0000;
    bi.bV5GreenMask     = 0x0000FF00;
    bi.bV5BlueMask      = 0x000000FF;
    bi.bV5AlphaMask     = 0xFF000000; 

    HDC hdc = GetDC(NULL);

    void * pBits = NULL;
    HBITMAP colorBmp = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (void **)&pBits, NULL, 0);

    int32 rowStep = 4 * width;
    byte * pTarget = (byte*)pBits;
    byte * pSource = ((byte*)cd.pBitmap->getData()) + rowStep * (height - 1);

    for(int32 y=0; y < height; y++)
    {
        memcpy(pTarget, pSource, rowStep);
        pSource -= rowStep;
        pTarget += rowStep;
    }

    HBITMAP maskBmp = CreateBitmap(width, height, 1, 1, NULL);

    ICONINFO ii;
    ii.fIcon    = FALSE;
    ii.xHotspot = cd.xSpot;
    ii.yHotspot = cd.ySpot;
    ii.hbmMask  = maskBmp;
    ii.hbmColor = colorBmp;

    HCURSOR cursor = CreateIconIndirect(&ii);

    DeleteObject(colorBmp);          
    DeleteObject(maskBmp); 

    return cursor;
}
//-----------------------------------------------------------------------------
void Window::resize(int32 width, int32 height)
{
    _width = width;
    _height = height;
    clientResize((HWND)_hwnd, width, height);
}
//-----------------------------------------------------------------------------
void Window::setCursor(const Ptr<Core::Bitmap> & pCursor, int32 xSpot, int32 ySpot)
{
    if(pCursor == null)
    {
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    else
    {
        HCURSOR cursor = NULL;
        CursorData cd(pCursor, xSpot, ySpot);
        Core::Map<CursorData, void*>::const_iterator iCursor = _cursors.find(cd);

        if(iCursor != _cursors.end())
        {
            cursor = (HCURSOR)iCursor->second;
        }
        else
        {
            cursor = createAlphaCursor(cd);
            _cursors[cd] = cursor;
        }

        SetCursor(cursor);
    }
}
//-----------------------------------------------------------------------------
}
