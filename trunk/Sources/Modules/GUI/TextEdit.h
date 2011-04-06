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
#ifndef GUI_TEXTEDIT_H_
#define GUI_TEXTEDIT_H_

#include <GUI/Widget.h>
#include <GUI/Label.h>

namespace GUI
{
LM_ENUM_3(ETextEditInternalState,
          TEIS_DEFAULT,
          TEIS_MOUSE_OVER,
          TEIS_MOUSE_OVER_DOWN);

class LM_API_GUI TextEdit : public Label
{
public:
    TextEdit(const Ptr<ResourcePool> & pResPool,
             const Ptr<Renderer::IHUD> & pHUD,
             const Ptr<Window::InputManager> & pInput,
             Focus * pFocus);

    void setPassword(bool pass);
    void setMaxLength(int32 maxLength);
    virtual void setText(const String & text);
    
    bool isPassword() const { return _isPassword; }
    int32 getMaxLength() const { return _maxLength; }
    virtual const String & getText() const { return _editText; }

    virtual void focus();
    virtual void gainFocus();
    virtual void loseFocus();

    virtual void update(double elapsed);
    
protected:
    virtual void onVisibilityChanged();
    virtual void onPriorityChanged(int32 p);
    virtual void clampVisibleText();
    
    int32 mouseIndex();
    int32 prevWord(int32 ii);
    int32 nextWord(int32 ii);
    void insert(const String & text);
    void removeSelection();
    void removeChar(bool back);

    Ptr<Window::InputManager>   _pInput;
    Focus *                     _pFocus;
    ETextEditInternalState      _state;

    int32                       _maxLength;
    bool                        _isPassword;

    Ptr<Renderer::IText>        _pCursor;
    int32                       _cursorPos;
    float                       _cursorTime;

    Ptr<Renderer::IRectangle>   _pSelection;
    int32                       _iSelStart;
    int32                       _iSelEnd;
    int32                       _selectionCursor;
    bool                        _selectionActive;
    bool                        _selectionByWord;

    String                      _editText;
    String                      _displayText;

    int32                       _iVisibleStart;
    int32                       _iVisibleEnd;
    bool                        _preferForwardVisible;

    bool                        _returnTyped;
};
}

#endif

