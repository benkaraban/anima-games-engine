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
#include "WindowUpdater.h"
#include <windowsx.h> 
#include "resource.h"
#include <olectl.h>
#include <ole2.h>

#include "AppUpdater.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

WHU* pInst = NULL;
const int32 PROGRESS_BAR_NB_STEP = 100;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

const int ID_BUT_CANCEL       = 1053;
const int TEXT_EDIT_HEIGHT    = 40;
const int PROGRESS_BAR_HEIGHT = GetSystemMetrics(SM_CYVSCROLL);
const int BUTTON_HEIGHT       = 40;
const int BUTTON_MARGE_HEIGHT = 20;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uMessage)
    {
        case WM_CREATE:
        {
            WHU::Inst().create(hWnd);
            break;
        }

        case WM_COMMAND:
        {
            switch (GET_WM_COMMAND_ID(wParam,lParam))
            {
                case ID_BUT_CANCEL:
                {
                   WHU::Inst().exit();
                   break;
                }
            }
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
    }

    return DefWindowProcW(hWnd,uMessage,wParam,lParam);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HBITMAP LoadImageFromResource(UINT resourceId, LPTSTR resourceType)
{
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(resourceId), resourceType);
    if (!hRes) return NULL;

    HGLOBAL pMem = LoadResource(NULL, hRes);
    if (!pMem) return NULL;

    DWORD dwSize = SizeofResource(NULL, hRes);

    HGLOBAL pGlobal = GlobalAlloc(0, dwSize);
    memcpy(pGlobal, pMem, dwSize);

    LPSTREAM pStream = NULL;
    CreateStreamOnHGlobal(pGlobal, FALSE, &pStream);

    LPPICTURE pPicture = NULL;
    OleLoadPicture(pStream, dwSize, TRUE, IID_IPicture, (LPVOID*)&pPicture);

    GlobalFree(pGlobal);
    pStream->Release();

    //Make a copy of bitmap with given Bit per pixel value
    HBITMAP hBmp = NULL;
    pPicture->get_Handle((unsigned int*)&hBmp);
    HBITMAP bitmap = (HBITMAP)CopyImage(hBmp, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);

    pPicture->Release();
    return bitmap;
} 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
WHU::WHU(HINSTANCE hInst)
{
    _hInst  = hInst;
    _hImage = LoadImageFromResource(IDB_TITLE, "ID_BITMAP_TITLE");
}
//-----------------------------------------------------------------------------
WHU::~WHU()
{
    if (_hWnd)
        DestroyWindow(_hWnd);

    delete pInst;
    pInst = NULL;
}
//-----------------------------------------------------------------------------
void WHU::createInstance(HINSTANCE hInst)
{
    if (pInst==NULL)
    {
        pInst = new WHU(hInst);
    }
}
//-----------------------------------------------------------------------------
WHU& WHU::Inst()
{
    return *pInst;
}
//-----------------------------------------------------------------------------
bool WHU::createMainWindows(const String& title)
{
    int iWidth  = 500;
    int iHeight = TEXT_EDIT_HEIGHT + 2*PROGRESS_BAR_HEIGHT + 2*BUTTON_MARGE_HEIGHT + BUTTON_HEIGHT + 50;

    if (_hImage)
    {
        BITMAP    bm;
        GetObject(WHU::Inst()._hImage, sizeof(BITMAP), &bm); 
        iWidth   = bm.bmWidth;
        iHeight += bm.bmHeight;
    }

    WNDCLASSW wndWc;
        wndWc.style          = 0;
        wndWc.lpfnWndProc    = (WNDPROC) WndProc;
        wndWc.cbClsExtra     = 0;
        wndWc.cbWndExtra     = 0;
        wndWc.hInstance      = _hInst;
        wndWc.hIcon          = NULL;
        wndWc.hCursor        = LoadCursor(0, IDC_ARROW);
        wndWc.hbrBackground  = (HBRUSH)COLOR_WINDOW;
        wndWc.lpszMenuName   = NULL;
        wndWc.lpszClassName  = title.c_str();

    if (!RegisterClassW(&wndWc)) return false;

    //centre de l'ecran
    int iSw = (WORD)GetSystemMetrics(SM_CXSCREEN);
    int iSh = (WORD)GetSystemMetrics(SM_CYSCREEN);
    RECT rc = { (iSw - iWidth)/2, (iSh - iHeight)/2, iWidth, iHeight };

    HWND hWnd = CreateWindowW(wndWc.lpszClassName, wndWc.lpszClassName,
                      WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX,
                      rc.left,rc.top, iWidth,iHeight,
                      NULL, NULL, _hInst, NULL);

    ShowWindow(hWnd, TRUE);
    return true;
}
//-----------------------------------------------------------------------------
void WHU::create(HWND hWnd)
{
    _hWnd = hWnd;

    createCancelButton();
    createProgressBar();
    createBitmap();
    createOkButton();
    createCancelButton();
    createTextEdit();
    createStatusBar();
}
//-----------------------------------------------------------------------------
bool WHU::createOkButton()
{
    if (!_hWnd) return false;

    RECT rcClient;
    GetClientRect(_hWnd, &rcClient); 

    int32 xPos = 80;
    int32 yPos = 2*PROGRESS_BAR_HEIGHT+TEXT_EDIT_HEIGHT + BUTTON_MARGE_HEIGHT;
    int32 width = 100;
    int32 height = BUTTON_HEIGHT;

    if (WHU::Inst()._hImage)
    {
        BITMAP    bm;
        GetObject(WHU::Inst()._hImage, sizeof(BITMAP), &bm); 
        xPos = bm.bmWidth/4;
        yPos += bm.bmHeight;
    }

    _hWndButOk = CreateWindowExW (0, WC_BUTTONW, L"OK",
         WS_VISIBLE | WS_DISABLED | WS_CHILD | BS_PUSHBUTTON, xPos, yPos, width, height, _hWnd, NULL, _hInst, NULL);

    String text = AppUpdater::Inst().getDicoText(L"launch");
    SendMessageW(_hWndButOk, WM_SETTEXT, 0, (LPARAM)text.c_str());
    
    return true;
}
//-----------------------------------------------------------------------------
bool WHU::createCancelButton()
{
    if (!_hWnd) return false;

    RECT rcClient;
    GetClientRect(_hWnd, &rcClient); 

    int32 xPos   = 80;
    int32 yPos   = 2*PROGRESS_BAR_HEIGHT+TEXT_EDIT_HEIGHT + BUTTON_MARGE_HEIGHT;
    int32 width  = 100;
    int32 height = BUTTON_HEIGHT;

    if (_hImage)
    {
        BITMAP    bm;
        GetObject(_hImage, sizeof(BITMAP), &bm); 
        xPos = bm.bmWidth - bm.bmWidth/4-width;
        yPos += bm.bmHeight;
    }

    _hWndButCancel = CreateWindowExW (0, L"BUTTON", 0,
         WS_CHILD, xPos, yPos, width, height, _hWnd, (HMENU)ID_BUT_CANCEL, _hInst, NULL);

    String text = AppUpdater::Inst().getDicoText(L"cancel");
    SendMessageW(_hWndButCancel, WM_SETTEXT, 0, (LPARAM)text.c_str());

    ShowWindow(_hWndButCancel, SW_SHOW);
    return true;
}
//-----------------------------------------------------------------------------
bool WHU::createProgressBar()
{
    if (!_hWnd) return false;

    int32 starty = TEXT_EDIT_HEIGHT;

    if (WHU::Inst()._hImage)
    {
        BITMAP    bm;
        GetObject(WHU::Inst()._hImage, sizeof(BITMAP), &bm); 
        starty += bm.bmHeight;
    }

    RECT rcClient;
    GetClientRect(_hWnd, &rcClient); 
    int cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 

    _hWndPBText = CreateWindowExW(0, WC_STATICW,
        NULL, WS_CHILD | WS_VISIBLE | ES_CENTER,
        rcClient.left, starty, rcClient.right, PROGRESS_BAR_HEIGHT, 
        _hWnd, (HMENU) 0, NULL, NULL);

    _hWndPB = CreateWindowExW(0, PROGRESS_CLASSW,
        NULL, WS_CHILD | WS_VISIBLE,
        rcClient.left, starty+PROGRESS_BAR_HEIGHT, rcClient.right, PROGRESS_BAR_HEIGHT, 
        _hWnd, (HMENU) 0, NULL, NULL);

    

    if (!_hWndPB || !_hWndPBText) return false;

    return true;
}
//-----------------------------------------------------------------------------
void WHU::setProgressBarState(EProgressBarState state)
{
    switch(state)
    {
    case EPB_STATE_CLEAR:
        SetWindowLongPtrW(_hWndPB, GWL_STYLE, WS_CHILD | WS_VISIBLE);
        SendMessageW(_hWndPB, PBM_SETRANGE, 0, MAKELPARAM(0, PROGRESS_BAR_NB_STEP));
        SendMessageW(_hWndPB, PBM_SETPOS, 0, MAKELPARAM(0, 0));
        break;
    case EPB_STATE_LOOP:
        SetWindowLongPtrW(_hWndPB, GWL_STYLE, PBS_MARQUEE | WS_CHILD | WS_VISIBLE);
        SendMessageW(_hWndPB,(UINT) PBM_SETMARQUEE,(WPARAM) TRUE,(LPARAM)50 );
        break;
    case EPB_STATE_WITH_LIMIT:
        SetWindowLongPtrW(_hWndPB, GWL_STYLE, WS_CHILD | WS_VISIBLE);
        SendMessageW(_hWndPB, PBM_SETRANGE, 0, MAKELPARAM(0, PROGRESS_BAR_NB_STEP));
        break;
    }
}
//-----------------------------------------------------------------------------
bool WHU::createBitmap()
{
    if (!_hWnd) return false;
    if (!WHU::Inst()._hImage) return false;

    _hWndBmp = CreateWindowExW (WS_EX_STATICEDGE, WC_STATICW, NULL,
        WS_CHILD | WS_VISIBLE | SS_BITMAP, 0, 0, 0, 0, _hWnd, 0, NULL, NULL);

    if (!_hWndBmp) return false;

    SendMessageW(_hWndBmp, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)WHU::Inst()._hImage);

    return true;
}
//-----------------------------------------------------------------------------
bool WHU::createTextEdit()
{
    int32 starty = 50;

    if (WHU::Inst()._hImage)
    {
        BITMAP    bm;
        GetObject(WHU::Inst()._hImage, sizeof(BITMAP), &bm); 
        starty = bm.bmHeight;
    }

    RECT rcClient;
    GetClientRect(_hWnd, &rcClient); 

    _hWndEdit = CreateWindowExW(0, WC_EDITW, L"",        
                                WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_READONLY, 
                                0, starty, rcClient.right-rcClient.left, TEXT_EDIT_HEIGHT,
                                _hWnd, NULL, _hInst, NULL);       

    return true;
}
//-----------------------------------------------------------------------------
bool WHU::createStatusBar()
{
    _hwndStatus = CreateWindowExW(
            0,                       // no extended styles
            STATUSCLASSNAMEW,         // name of status bar class
            L"",                    // no text when first created
            WS_CHILD | WS_VISIBLE,   // creates a visible child window
            0, 0, 0, 0,              // ignores size and position
            _hWnd,                   // handle to parent window
            NULL,                    // child window identifier
            _hInst,                  // handle to application instance
            NULL);                   // no window creation data

    return true;
}
//-----------------------------------------------------------------------------
void WHU::setProgress(float p)
{
    if (!_hWndPB) return;

    SendMessageW(_hWndPB, PBM_SETPOS, int(p*PROGRESS_BAR_NB_STEP), MAKELPARAM(0, 0));

    String text = Core::toString(p*100,0) + L"%";
    SendMessageW(_hWndPBText, WM_SETTEXT, 0, (LPARAM)text.c_str());
}
//-----------------------------------------------------------------------------
void WHU::exit()
{
    EndDialog(_hWnd, FALSE);
    AppUpdater::Inst().exit();
}
//-----------------------------------------------------------------------------
void WHU::setTextSatutBar(const String& text)
{
    LM_ASSERT(_hwndStatus!=NULL);

    SendMessageW(_hwndStatus, WM_SETTEXT, 0, (LPARAM)text.c_str());
}
//-----------------------------------------------------------------------------
void WHU::setTextEdit(const String& text)
{
    LM_ASSERT(_hWndEdit!=NULL);
    SendMessageW(_hWndEdit, WM_SETTEXT, 0, (LPARAM)text.c_str());
}
//-----------------------------------------------------------------------------
bool errorQuestionDlg (const Core::String& title, const Core::String& text)
{
    int ret = MessageBoxW(WHU::Inst()._hWnd, text.c_str(), title.c_str(), MB_APPLMODAL | MB_RETRYCANCEL | MB_ICONERROR);

    return ret==IDRETRY;
}
//-----------------------------------------------------------------------------
void messageDlg (const Core::String& title, const Core::String& text)
{
    int ret = MessageBoxW(WHU::Inst()._hWnd, text.c_str(), title.c_str(), MB_APPLMODAL | MB_OK | MB_ICONERROR);
}
//-----------------------------------------------------------------------------