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
#ifndef GUI_CHECKBOX_H_
#define GUI_CHECKBOX_H_

#include <GUI/Widget.h>
#include <GUI/Label.h>
#include <GUI/Picture.h>

namespace GUI
{
struct LM_API_GUI CheckBoxState
{
    CheckBoxState();

    Core::Vector4f  picColor;
    Core::Vector2f  posDelta;
    Core::Vector4f  textColor;
};

LM_ENUM_4(ECheckBoxRenderState,
          CBS_DEFAULT,
          CBS_DISABLED,
          CBS_MOUSE_OVER,
          CBS_PUSHED);

LM_ENUM_5(ECheckBoxInternalState,
          CBIS_DEFAULT,
          CBIS_DISABLED,
          CBIS_MOUSE_OVER,
          CBIS_PUSHED_OVER,
          CBIS_PUSHED_NOT_OVER);

struct LM_API_GUI CheckBoxStyle
{
    CheckBoxStyle();

    String          picChecked;
    String          picUnchecked;
    String          text;
    String          fontName;
    int32           fontSize;
    bool            bold;
    bool            italic;
    bool            shadow;
    float           padding;

    CheckBoxState   state[ECheckBoxRenderState_COUNT];
};

class LM_API_GUI CheckBox : public Widget
{
public:
    CheckBox(const Ptr<ResourcePool> & pResPool,
             const Ptr<Renderer::IHUD> & pHUD,
             const Ptr<Window::InputManager> & pInput);

    void setStyle(const CheckBoxStyle & style);
    const CheckBoxStyle & getStyle() const;

    void setText(const String & text);
    const String & getText();

    bool isChecked() const { return _isChecked; }
    void setChecked(bool checked);

    virtual void update(double elapsed);
    
protected:
    virtual void onVisibilityChanged();
    virtual void onEnableChanged();
    virtual void onActiveChanged();
    virtual void onPositionChanged();
    virtual void onPriorityChanged(int32 p);
    virtual void onSizeChanged();
    virtual void onColorChanged();
    virtual void onKill();

    virtual void internalNotify(EEvent ev);

    void setRenderState(ECheckBoxRenderState rs);
    void updateRenderState();

    Ptr<Window::InputManager>  _pInput;
    Ptr<Picture>               _pPicture;
    Ptr<Label>                 _pLabel;

    CheckBoxStyle           _style;
    ECheckBoxRenderState    _curRenderState;
    ECheckBoxInternalState  _curState;
    bool                    _isChecked;
};
}

#endif

