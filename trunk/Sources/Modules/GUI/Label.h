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
#ifndef GUI_LABEL_H_
#define GUI_LABEL_H_

#include <GUI/ResourcePool.h>
#include <GUI/Widget.h>

#include <Renderer/IText.h>

namespace GUI
{
LM_ENUM_3(EHoriLayout, HL_LEFT, HL_RIGHT, HL_CENTER);
LM_ENUM_3(EVertLayout, VL_UP,   VL_DOWN,  VL_CENTER);

struct LabelStyle
{
    LabelStyle();

    String           fontName;
    int32            fontSize;
    Core::Vector4f   color;
    Core::Vector4f   shadowColor;
    bool             italic;
    bool             bold;
    bool             shadow;

    EHoriLayout      hLayout;
    EVertLayout      vLayout;
};


class LM_API_GUI Label : public Widget
{
public:
    Label(const Ptr<ResourcePool> & pResPool,
          const Ptr<Renderer::IHUD> & pHUD);

    virtual void setText(const String & text);
    
    void setFontName(const String & font);
    void setFontSize(int32 size);

    void setTextColor(const Core::Vector4f & color);
    void setShadowColor(const Core::Vector4f & color);

    void setBold(bool bold);
    void setItalic(bool italic);
    void setShadow(bool shadow);

    void setHLayout(EHoriLayout layout);
    void setVLayout(EVertLayout layout);
    void setLayouts(EHoriLayout hl, EVertLayout vl) { setHLayout(hl); setVLayout(vl); }

    //-------------------------------------------------------------------------

    virtual const String & getText() const { return _text; }

    const String & getFontName() const { return _fontName; }
    int32 getFontSize() const { return _fontSize; }

    const Core::Vector4f & getTextColor() const { return _textColor; }
    const Core::Vector4f & getShadowColor() const { return _shadowColor; }

    bool isBold() const { return _bold; }
    bool isItalic() const { return _italic; }
    bool hasShadow() const { return _shadow; }

    EHoriLayout getHLayout() const { return _hLayout; }
    EVertLayout getVLayout() const { return _vLayout; }

    void setStyle(const LabelStyle & style);
    void getStyle(LabelStyle & style);

    void fitText(bool fitX = true, bool fitY = false);

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

    Ptr<ResourcePool>     _pResPool;
    Ptr<Renderer::IText>  _pText1;
    Ptr<Renderer::IText>  _pText2;

    String                _text;
    String                _fontName;
    int32                 _fontSize;
    EHoriLayout           _hLayout;
    EVertLayout           _vLayout;
    Core::Vector4f        _textColor;
    Core::Vector4f        _shadowColor;
    bool                  _italic;
    bool                  _bold;
    bool                  _shadow;
};
}

#endif

