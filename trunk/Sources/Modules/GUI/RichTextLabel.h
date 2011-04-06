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
#ifndef GUI_RICHTEXTLABEL_H_
#define GUI_RICHTEXTLABEL_H_

#include <GUI/ResourcePool.h>
#include <GUI/Widget.h>
#include <GUI/RichTextParser.h>

#include <Renderer/IText.h>

namespace GUI
{
class LM_API_GUI RichTextLabel : public Widget
{
public:
    RichTextLabel(const Ptr<ResourcePool> & pResPool,
                  const Ptr<Renderer::IHUD> & pHUD);

    virtual void setText(const String & text);

    // Styles par d�faut
    void setFontName(const String & font);
    void setFontSize(int32 size);
    void setHLayout(EHoriLayout layout) { _hLayout = layout; };
    void setVLayout(EVertLayout layout) { _vLayout = layout; };
    void setLayouts(EHoriLayout hl, EVertLayout vl) { setHLayout(hl); setVLayout(vl); }

    const String & getFontName() const { return _fontName; }
    int32 getFontSize() const { return _fontSize; }

    void setShadow(bool shadow) { _shadow = shadow; }
    bool getShadow() const { return _shadow; }

    void setAutoCenterVContent(bool flag);
    bool isAutoCenterVContent() const { return _autoCenterVContent; }

    virtual void setFirstLine(int32 line);
    virtual int32 getFirstLine() const;
    virtual int32 getMaxFirstLine() const;
    virtual int32 getLineCount() const;
    virtual int32 getVisibleLineCount() const;

    float getYLine() const { return _yLine; }
    //void setStyle(const LabelStyle & style);
    //void getStyle(LabelStyle & style);

protected:
    //virtual void onVisibilityChanged();
    //virtual void onEnableChanged();
    //virtual void onActiveChanged();
    //virtual void onPositionChanged();
    //virtual void onPriorityChanged(int32 p);
    //virtual void onSizeChanged();
    //virtual void onColorChanged();
    //virtual void onKill();

    struct StyleState
    {
        String              tag;
        ERichTextTokenType  tagType;
        LabelStyle          style;
        EHoriLayout         hLayout;
        EVertLayout         vLayout;
    };

    virtual void nextLine(const StyleState & state);
    virtual bool fitInLine(const StyleState & state, const String & text, const Ptr<Renderer::IFont> & pFont);
    virtual void addToLine(const StyleState & state, const String & text, const Ptr<Renderer::IFont> & pFont);

    virtual void generateImage(const StyleState & state, const String & name);
    virtual void generateText(const StyleState & state, const String & text);
    virtual void generateLines();
    virtual void clearLines();
    virtual void computeMaxFirstLine();

    void updateText();

    void pushStyle(Core::List<StyleState> & stack, const RichTextToken & token);

    Ptr<ResourcePool>     _pResPool;
    Ptr<Renderer::IHUD>   _pHUD;

    String                    _text;
    String                    _fontName;
    int32                     _fontSize;
    EHoriLayout               _hLayout;
    EVertLayout               _vLayout;
    bool                      _shadow;
    bool                      _autoCenterVContent;
    Core::List<RichTextToken> _tokens;
    Core::List<Ptr<Widget> >  _lines;
    Core::List<float>         _baseY;
    Ptr<Widget>               _pCurLine;
    float                     _yLine;
    int32                     _firstLine;
    int32                     _maxFirstLine;
    float                     _lastLineY;
    float                     _maxDY;
};
}

#endif

