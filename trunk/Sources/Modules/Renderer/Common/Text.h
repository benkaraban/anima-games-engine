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
#ifndef RENDERER_TEXT_H_
#define RENDERER_TEXT_H_

#include <Renderer/IText.h>

namespace Renderer
{
class LM_API_RDR Text : public IText, public Core::Object
{
public:
    Text(const Ptr<IFont> & pFont, const String & text, int32 priority = 0);

    virtual void setPosition(const Core::Vector2f & position);
    virtual const Core::Vector2f & getPosition() const;

    virtual void setClipRect(const Core::Vector2f & clip);
    virtual const Core::Vector2f & getClipRect() const;

    virtual void setPriority(int32 p);
    virtual int32 getPriority() const;

    virtual void getTextSize(Core::Vector2f & size);

    virtual void setText(const String & text);
    virtual const String & getText() const;

    virtual void setColor(const Core::Vector4f & color);
    virtual const Core::Vector4f & getColor() const;

    virtual void setFont(const Ptr<IFont> & pFont);
    virtual Ptr<IFont> getFont() const;

    virtual void setBold(bool flag);
    virtual bool isBold() const;

    virtual void setItalic(bool flag);
    virtual bool isItalic() const;

    virtual void setVisible(bool flag);
    virtual bool isVisible() const;

    virtual void kill();
    virtual bool isAlive() const;

protected:
    Ptr<IFont>          _pFont;
    String              _text;
    Core::Vector4f      _color;
    Core::Vector2f      _position;
    Core::Vector2f      _clipRect;
    int32               _priority;
    bool                _isBold;
    bool                _isItalic;
    bool                _isAlive;
    bool                _isVisible;    
};
}

#endif
