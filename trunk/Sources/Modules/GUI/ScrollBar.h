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
#ifndef GUI_SCOLLBAR_H_
#define GUI_SCOLLBAR_H_

#include <GUI/ResourcePool.h>
#include <GUI/Widget.h>

#include <Renderer/IRectangle.h>

namespace GUI
{
LM_ENUM_3(EScrollInternalState,
          SIS_DEFAULT,
          SIS_MOUSE_OVER_BAR,
          SIS_MOUSE_GRIP_BAR
          );

class LM_API_GUI VScrollBar : public Widget
{
public:
    VScrollBar(const Ptr<ResourcePool> & pResPool,
               const Ptr<Renderer::IHUD> & pHUD,
               const Ptr<Window::InputManager> & pInput);

    void setBackgroundPicture(const String & pic);
    void setBarPicture(const String & pic);
    void setBackgroundColor(const Core::Vector4f & pic);
    void setBarColor(const Core::Vector4f & pic);
    void setBarHighlightColor(const Core::Vector4f & pic);
    void setRange(int32 minv, int32 maxv, int32 barSize);
    void setValue(int32 v);

    const String & getBackgroundPicture() const;
    const String & getBarPicture() const;
    int32 getValue() const { return _value; }

    virtual void pack();

protected:
    virtual void onVisibilityChanged();
    virtual void onEnableChanged();
    virtual void onActiveChanged();
    virtual void onPositionChanged();
    virtual void onPriorityChanged(int32 p);
    virtual void onSizeChanged();
    virtual void onColorChanged();
    virtual void onKill();

    virtual void updateDisplay();
    virtual void update(double elapsed);

    Ptr<ResourcePool>          _pResPool;
    Ptr<Window::InputManager>  _pInput;

    Ptr<Renderer::IRectangle>  _pRectBack;
    Ptr<Renderer::IRectangle>  _pRectBar;

    String                     _backPicName;
    String                     _barPicName;

    Core::Vector4f             _backColor;
    Core::Vector4f             _barColor;
    Core::Vector4f             _barHighColor;

    int32                      _minv;
    int32                      _maxv;
    int32                      _barSize;

    EScrollInternalState       _state;
    int32                      _value;
    float                      _mouseDY;
};

class LM_API_GUI HScrollBar : public Widget
{
public:
    HScrollBar(const Ptr<ResourcePool> & pResPool,
               const Ptr<Renderer::IHUD> & pHUD,
               const Ptr<Window::InputManager> & pInput);

    void setBackgroundPicture(const String & pic);
    void setBarPicture(const String & pic);
    void setBackgroundColor(const Core::Vector4f & pic);
    void setBarColor(const Core::Vector4f & pic);
    void setBarHighlightColor(const Core::Vector4f & pic);
    void setRange(int32 minv, int32 maxv, int32 barSize);
    void setValue(int32 v);
    void setValuePercent(float percent);

    const String & getBackgroundPicture() const;
    const String & getBarPicture() const;
    int32 getValue() const { return _value; }
    float getValuePercent() const;

    virtual void pack();

protected:
    virtual void onVisibilityChanged();
    virtual void onEnableChanged();
    virtual void onActiveChanged();
    virtual void onPositionChanged();
    virtual void onPriorityChanged(int32 p);
    virtual void onSizeChanged();
    virtual void onColorChanged();
    virtual void onKill();

    virtual void updateDisplay();
    virtual void update(double elapsed);

    Ptr<ResourcePool>          _pResPool;
    Ptr<Window::InputManager>  _pInput;

    Ptr<Renderer::IRectangle>  _pRectBack;
    Ptr<Renderer::IRectangle>  _pRectBar;

    String                     _backPicName;
    String                     _barPicName;

    Core::Vector4f             _backColor;
    Core::Vector4f             _barColor;
    Core::Vector4f             _barHighColor;

    int32                      _minv;
    int32                      _maxv;
    int32                      _barSize;

    EScrollInternalState       _state;
    int32                      _value;
    float                      _mouseDX;
};

}

#endif

