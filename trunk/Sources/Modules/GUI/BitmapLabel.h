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
#ifndef GUI_BITMAP_LABEL_H_
#define GUI_BITMAP_LABEL_H_

#include <GUI/Label.h>

#include <Renderer/IRectangle.h>

namespace GUI
{
class LM_API_GUI BitmapLabel : public Widget
{
public:
    BitmapLabel(const Ptr<ResourcePool> & pResPool,
                const Ptr<Renderer::IHUD> & pHUD);

    void setText(const String & text);
    void setFont(const String & font);
    void setHLayout(EHoriLayout layout);
    void setVLayout(EVertLayout layout);
    void setLayouts(EHoriLayout hl, EVertLayout vl) { setHLayout(hl); setVLayout(vl); }
    void setZoomFactor(float zoom);

    const String & getText() const { return _text; }
    const String & getFont() const { return _fontName; }
    EHoriLayout getHLayout() const { return _hLayout; }
    EVertLayout getVLayout() const { return _vLayout; }
    float getZoomFactor() const { return _zoomFactor; }
    const Core::Vector2f & getTextSize() const { return _textSize; }

protected:
    virtual void onVisibilityChanged();
    virtual void onEnableChanged();
    virtual void onActiveChanged();
    virtual void onPositionChanged();
    virtual void onPriorityChanged(int32 p);
    virtual void onSizeChanged();
    virtual void onColorChanged();
    virtual void onKill();

    virtual void updateText();
    virtual void updateDisplay();

    Ptr<ResourcePool>                       _pResPool;
    Ptr<Renderer::IHUD>                     _pHUD;
    Ptr<BitmapFont>                         _pFont;
    
    String                                  _fontName;
    String                                  _text;
    EHoriLayout                             _hLayout;
    EVertLayout                             _vLayout;

    float                                   _zoomFactor;
    Core::Vector2f                          _textSize;

    Core::List<Ptr<Renderer::IRectangle> >  _chars;
};
}

#endif

