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
#include <Renderer/Common/Font.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
Font::Font(const Ptr<Gfx::IDevice> & pDevice, const String & name, int32 height, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _name(name),
    _height(height)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
Font::~Font()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
Ptr<Gfx::IFont> Font::getInternalFont(bool bold, bool italic)
{
    if(bold)
        if(italic)
            return _pFontBoldItalic;
        else
            return _pFontBold;
    else
        if(italic)
            return _pFontItalic;
        else
            return _pFontNormal;
}
//-----------------------------------------------------------------------------
void Font::onDeviceLost()
{
    _pFontNormal = null;
    _pFontBold = null;
    _pFontBoldItalic = null;
    _pFontItalic = null;
}
//-----------------------------------------------------------------------------
void Font::getTextSize(Core::Vector2f & size, const String & text, bool bold, bool italic)
{
    int32 sx = 0;
    int32 sy = 0;
    Ptr<Gfx::IFont> pFont(getInternalFont(bold, italic));
    if(pFont != null)
        pFont->getTextSize(text, sx, sy);
    size.x = float(sx);
    size.y = float(sy);
}
//-----------------------------------------------------------------------------
void Font::onDeviceReset()
{
    Gfx::FontDesc desc;
    desc.name = _name;
    desc.pixHeight = _height;
    desc.bold = false;
    desc.italic = false;

    _pFontNormal = _pDevice->createFont(desc);

    desc.bold = true;
    _pFontBold = _pDevice->createFont(desc);

    desc.italic = true;
    _pFontBoldItalic = _pDevice->createFont(desc);

    desc.bold = false;
    _pFontItalic = _pDevice->createFont(desc);
}
//-----------------------------------------------------------------------------
}
