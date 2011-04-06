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
#include <VoxelFont.h>

#include <Assets/GeometricPrimitive.h>
#include <Core/TGA.h>
#include <Core/Math/Random.h>
#include <Universe/Tasks/Translate.h>

namespace ChronoRage
{
VoxelFont::VoxelFont(const VoxelFontInfos & infos) : _infos(infos)
{
    for(int32 ii=0; ii < infos.characters.size(); ii++)
    {
        int32 iu = ii % infos.uRes;
        int32 iv = ii / infos.uRes;

        int32 x = iu * infos.charWidth;
        int32 y = iv * infos.charHeight;

        Ptr<Core::Bitmap> pBmpChar = Core::crop(infos.pBitmap, x, y, infos.charWidth, infos.charHeight);

        Ptr<VoxelSpriteTemplate> pTemplate(new VoxelSpriteTemplate(pBmpChar, infos.cubeSize, infos.padding, infos.glowFactor));
        _chars[infos.characters[ii]] = pTemplate;
    }

    _charSpacing = infos.charSpacing * (infos.cubeSize + infos.padding);
    _charHeight = infos.charHeight * (infos.cubeSize + infos.padding);
}

Ptr<VoxelSpriteTemplate> VoxelFont::getTemplate(wchar_t c) const
{
    Core::Map<wchar_t, Ptr<VoxelSpriteTemplate> >::const_iterator iChar = _chars.find(c);

    if(iChar == _chars.end())
        return null;
    else
        return iChar->second;
}

}//namespace ChronoRage