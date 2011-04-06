/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include <Gfx/GfxDx9/FontDX9.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
FontDX9::FontDX9(IDirect3DDevice9 * pDevice, const FontDesc & desc) :
    _desc(desc),
    _pFont(NULL)
{
    HDC hDC = GetDC(NULL);
    int nLogPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC(NULL, hDC);

    int height = -MulDiv(desc.pixHeight, nLogPixelsY, 72);

    HRESULT hresult = D3DXCreateFontW(
        pDevice,
        height,
        0, // Width
        desc.bold ? FW_BOLD : FW_NORMAL,
        1, // MipLevels
        desc.italic,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        PROOF_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        desc.name.c_str(),
        &_pFont
        );

    LM_HANDLE_D3D_ERROR(hresult, L"D3DXCreateFontW");
}
//-----------------------------------------------------------------------------
FontDX9::~FontDX9()
{
    _pFont->Release();
}
//-----------------------------------------------------------------------------
void FontDX9::drawText(const String & text, int32 x, int32 y, const Core::Vector4f & color)
{
    RECT rect;
    SetRect(&rect, x, y, 0, 0);

    // Cette horreur contre nature est du au fait que l'affichage des textes par D3D se fait
    // forcément en blending SRC_ALPHA / ONE_MINUS_SRC_ALPHA alors que l'on gère le reste
    // en prémultiplication alpha ONE / ONE_MINUS_SRC_ALPHA. On effectue donc ce hack affreux
    // pour retrouver plus ou moins la même chose.
    float m = 1.0f / ((color.a < 0.001f) ? 1.0f : color.a);
    D3DCOLOR col = D3DXCOLOR(m * color.x, m * color.y, m * color.z, color.w);

    _pFont->DrawTextW(NULL, text.data(), text.size(), &rect, DT_NOCLIP, col);
}
//-----------------------------------------------------------------------------
void FontDX9::drawClippedText(const String & text, int32 x, int32 y, int32 clipx, int32 clipy, const Core::Vector4f & color)
{
    RECT rect;
    SetRect(&rect, x, y, x + clipx, y + clipy);

    // cf. drawText
    float m = 1.0f / ((color.a < 0.001f) ? 1.0f : color.a);
    D3DCOLOR col = D3DXCOLOR(m * color.x, m * color.y, m * color.z, color.w);
    _pFont->DrawTextW(NULL, text.data(), text.size(), &rect, 0, col);
}
//-----------------------------------------------------------------------------
const FontDesc & FontDX9::getDesc() const 
{ 
    return _desc; 
}
//-----------------------------------------------------------------------------
void FontDX9::getTextSize(const String & text, int32 & width, int32 & height)
{
    if(text.size() > 0 && text[text.size() - 1] == L' ')
    {
        String text2(text);
        text2[text2.size() - 1] = L'.';
        getTextSize(text2, width, height);
    }
    else if(text.size() == 0)
    {
        String text2(L".");
        text2[text2.size() - 1] = L'.';
        int32 w = 0;
        width = 0;
        getTextSize(text2, w, height);
    }
    else
    {
        RECT rect;
        SetRect(&rect, 0, 0, 0, 0);

        D3DCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
        _pFont->DrawTextW(NULL, text.data(), text.size(), &rect, DT_CALCRECT, col);

        width = rect.right + 1;
        height = rect.bottom + 1;
    }
}
//-----------------------------------------------------------------------------
}
