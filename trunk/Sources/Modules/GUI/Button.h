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
#ifndef GUI_BUTTON_H_
#define GUI_BUTTON_H_

#include <GUI/Widget.h>
#include <GUI/Label.h>
#include <GUI/Picture.h>

namespace GUI
{
struct LM_API_GUI ButtonState
{
    ButtonState();

    String          picName;
    Core::Vector4f  picColor;
    
    Core::Vector2f  posDelta;

    Core::Vector4f  textColor;
};

LM_ENUM_5(EButtonRenderState,
          BS_DEFAULT,
          BS_DISABLED,
          BS_DISABLED_MOUSE_OVER,
          BS_MOUSE_OVER,
          BS_PUSHED);

LM_ENUM_7(EButtonInternalState,
          BIS_DEFAULT,
          BIS_DISABLED,
          BIS_DISABLED_MOUSE_OVER,
          BIS_MOUSE_OVER,
          BIS_PUSHED_OVER,
          BIS_PUSHED_NOT_OVER,
          BIS_PUSHED_BY_HOTKEY);

struct LM_API_GUI ButtonStyle
{
    ButtonStyle();

    String          text;
    String          fontName;
    int32           fontSize;
    bool            bold;
    bool            italic;
    bool            shadow;

    ButtonState     state[EButtonRenderState_COUNT];
};

class LM_API_GUI Button : public Widget
{
public:
    Button(const Ptr<ResourcePool> & pResPool,
           const Ptr<Renderer::IHUD> & pHUD,
           const Ptr<Window::InputManager> & pInput);

    virtual ~Button(){}

    void setStyle(const ButtonStyle & style);
    const ButtonStyle & getStyle() const;

    void setHotKey(int32 key1);
    void setHotKey(int32 key1, int32 key2);

    void setText(const String & text);
    const String & getText();

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

    void setRenderState(EButtonRenderState rs);
    void updateRenderState();

    Ptr<Window::InputManager>  _pInput;
    Ptr<Picture>               _pPicture;
    Ptr<Label>                 _pLabel;

    ButtonStyle           _style;
    EButtonRenderState    _curRenderState;
    EButtonInternalState  _curState;

    int32                   _hotKey1;
    int32                   _hotKey2;
};
}

#endif

