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
#ifndef HUP_WINDOWUPDATER_H_
#define HUP_WINDOWUPDATER_H_

using std::min;
using std::max;
#include <windows.h>

#include <Core/Standard.h>

//-----------------------------------------------------------------------------
LM_ENUM_3 (EProgressBarState,
           EPB_STATE_CLEAR,
           EPB_STATE_LOOP,
           EPB_STATE_WITH_LIMIT)

class WHU
{
public:
    friend LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
    friend bool errorQuestionDlg (const Core::String& title, const Core::String& text);
    friend void messageDlg (const Core::String& title, const Core::String& text);

    static void createInstance(HINSTANCE hInst);
    static WHU& Inst();
    ~WHU();
    
    bool createMainWindows(const String& title);
    void setProgress(float p);
    void setProgressBarState(EProgressBarState state);
    void setTextSatutBar(const String& text);
    void setTextEdit(const String& text);

private:
    void create(HWND hWnd);
    bool createBitmap();
    bool createProgressBar();
    bool createOkButton();
    bool createCancelButton();
    bool createTextEdit();
    bool createStatusBar();

    void exit();

public:
    WHU(HINSTANCE hInst);


    HBITMAP             _hImage;
    HINSTANCE _hInst;

    HWND _hWnd;
    HWND _hWndPBText;
    HWND _hWndPB;
    HWND _hWndBmp;
    HWND _hWndButOk;
    HWND _hWndButCancel;
    HWND _hWndEdit;
    HWND _hwndStatus;
};
//-----------------------------------------------------------------------------
void messageDlg (const Core::String& title, const Core::String& text);
bool errorQuestionDlg (const Core::String& title, const Core::String& text);
//-----------------------------------------------------------------------------

#endif