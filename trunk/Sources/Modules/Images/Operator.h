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
#ifndef IMAGES_OPERATORS2_H_
#define IMAGES_OPERATORS2_H_

#include <Images/Image.h>

namespace Images
{

LM_ENUM_1(EBlurType,
          BLUR_GAUSSIAN)

/**
 * Le fichier contient la declaration des operations sur une Image ou sur une View
 */
//-----------------------------------------------------------------------------
LM_API_IMG void convert(const Ptr<Image>& pSrc, Ptr<Image>& pDst);
//-----------------------------------------------------------------------------
LM_API_IMG void resize(const Ptr<Image>& pSrc, Ptr<Image>& pDst, const EInterpolationType inter = INTER_BICUBIC);
LM_API_IMG void resize(const View& src, View& dst, const EInterpolationType inter = INTER_BICUBIC);
//-----------------------------------------------------------------------------
LM_API_IMG void resizeByStep(const Ptr<Image>& pSrc, Ptr<Image>& pDst, const EInterpolationType inter = INTER_BICUBIC);
//-----------------------------------------------------------------------------
LM_API_IMG void fill(Ptr<Image>& pImg, const Pixel& color);
LM_API_IMG void fill(View& view, const Pixel& color);
LM_API_IMG void setToZero(Ptr<Image>& pImg);
LM_API_IMG void setToZero(View& view);
//-----------------------------------------------------------------------------
LM_API_IMG void minMax(const Ptr<Image>& pImg, Pixel& colorMin, Pixel& colorMax);
LM_API_IMG void minMax(View& view, Pixel& colorMin, Pixel& colorMax);
//-----------------------------------------------------------------------------
LM_API_IMG void copy(const Ptr<Image>& pSrc, Ptr<Image>& pDst);
LM_API_IMG void copy(const View& src, View& dst);
//-----------------------------------------------------------------------------
//TODO gerer le scale en float
LM_API_IMG void blur(const Ptr<Image>& pSrc, Ptr<Image>& pDst, const int scale, const EBlurType blur = BLUR_GAUSSIAN);
LM_API_IMG void blur(const View& src, View& dst, const int scale, const EBlurType blur = BLUR_GAUSSIAN);
//-----------------------------------------------------------------------------




/**
 *  Fonction utilise par la class Images
 */
LM_API_IMG void resize(const Image& src, Image& dst, const EInterpolationType inter = INTER_BICUBIC);
} // namespace Images

#endif //IMAGES_OPERATORS2_H_
