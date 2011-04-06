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
#include <Images/Operator.h>

#include <Packing/PicLoaderOnTheFly.h>

#include <Workflow/TexCompression.h>

namespace Packing
{
//-----------------------------------------------------------------------------
OnTheFlyPictureGenerator::OnTheFlyPictureGenerator(const Ptr<Core::VirtualFileSystem> & pVFS,
                                                   const Ptr<Renderer::IRenderer>     & pRenderer) : 
    _pVFS(pVFS),
    _pRenderer(pRenderer)
{
}
//-----------------------------------------------------------------------------
Renderer::PictureInfos OnTheFlyPictureGenerator::getPicture(const String & name)
{
    Ptr<Core::InputStream> pInput = _pVFS->readFile(name);

    if(pInput == null)
        throw Core::Exception(L"unable to open file '" + name + L"'");

    Ptr<Images::Image> pSrcImage(new Images::Image());
    pSrcImage->load((Core::SeekableInputStream &)*pInput);

    int32 w1 = pSrcImage->getWidth();
    int32 h1 = pSrcImage->getHeight();

    int32 w2 = Core::nextPowerOfTwo(w1);
    int32 h2 = Core::nextPowerOfTwo(h1);
    
    Ptr<Images::Image> pPicImage(new Images::Image(Core::ARGB_U8, w2, h2));

    Ptr<Images::View> pView1(new Images::View(pSrcImage));
    Ptr<Images::View> pView2(new Images::View(pPicImage, 0, 0, w1, h1));

    Images::copy(*pView1, *pView2);

    Workflow::TextureOptions options;
    options.format = Assets::TEX_FORMAT_ARGB8;
    options.texType = Workflow::TEXTURE_COLORMAP;
    options.generateMipmap = false;

    Ptr<Assets::Texture> pTexture = Workflow::generateTexture(options, pPicImage);

    Renderer::PictureInfos result;
    result.pTexture = _pRenderer->uploadTexture(pTexture);
    result.width = w1;
    result.height = h1;
    result.u1 = 0.0f;
    result.u2 = float(w1) / float(w2);
    result.v1 = 0.0f;
    result.v2 = float(h1) / float(h2);

    return result;
}
//-----------------------------------------------------------------------------
}
