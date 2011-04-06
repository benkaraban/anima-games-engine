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
#ifndef GUI_PROGRESSBAR_H_
#define GUI_PROGRESSBAR_H_

#include <GUI/ResourcePool.h>
#include <GUI/Widget.h>

#include <Renderer/IRectangle.h>

namespace GUI
{
LM_ENUM_2(EProgressBarOrient,
          PROGRESSBAR_LEFT_TO_RIGHT,
          PROGRESSBAR_RIGHT_TO_LEFT);


class LM_API_GUI ProgressBar : public Widget
{
public:
    ProgressBar(const Ptr<ResourcePool> & pResPool,
                const Ptr<Renderer::IHUD> & pHUD);

    void setBorderColor(const Core::Vector4f & color);
    void setOrient(EProgressBarOrient orient);
    void setBarInteriorPosition(const Core::Vector2f & pos);
    void setBarInterior(const String & pic);
    void setBarBorder(const String & pic);
    void setProgress(float p);
    void setBorderVisible(bool v) { _pRectBorder->setVisible(v); }

    const Core::Vector4f & setBorderColor() const { return _borderColor; }
    EProgressBarOrient getOrient() const { return _orient; }
    const String & getBarInterior() const;
    const String & getBarBorder() const;
    float getProgress() const { return _progress; }
    bool isBorderVisible() const { return _pRectBorder->isVisible() ; }

    virtual void pack();

protected:
    void internalUpdateBar();

    virtual void onVisibilityChanged();
    virtual void onEnableChanged();
    virtual void onActiveChanged();
    virtual void onPositionChanged();
    virtual void onPriorityChanged(int32 p);
    virtual void onSizeChanged();
    virtual void onColorChanged();
    virtual void onKill();

    Ptr<ResourcePool>          _pResPool;
    Ptr<Renderer::IRectangle>  _pRectInterior;
    Ptr<Renderer::IRectangle>  _pRectBorder;
    Core::Vector4f             _borderColor;
    Core::Vector2f             _barInteriorPosition;
    EProgressBarOrient         _orient;
    String                     _barInterior;
    String                     _barBorder;
    Renderer::PictureInfos     _picInterior;
    float                      _progress;
};
}

#endif

