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
#ifndef GUI_PICTURE_H_
#define GUI_PICTURE_H_

#include <GUI/ResourcePool.h>
#include <GUI/Widget.h>

#include <Renderer/IRectangle.h>

namespace GUI
{
class LM_API_GUI Picture : public Widget
{
public:
    Picture(const Ptr<ResourcePool> & pResPool,
            const Ptr<Renderer::IHUD> & pHUD);

    void setPicture(const String & pic);
    const String & getPicture() const;

    void setPictureColor(const Core::Vector4f & color) { _picColor = color; onColorChanged(); }
    const Core::Vector4f & getPictureColor() const { return _picColor; }

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

    Ptr<ResourcePool>          _pResPool;
    Ptr<Renderer::IRectangle>  _pRect;
    String                     _picName;
    Core::Vector4f             _picColor;
};
}

#endif

