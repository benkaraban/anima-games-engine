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
#ifndef RENDERER_TOOLS_H_
#define RENDERER_TOOLS_H_

#include <Core/List.h>
#include <Core/Bitmap.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector.h>
#include <Renderer/Settings.h>
#include <Gfx/IDriver.h>
#include <Gfx/GfxType.h>

namespace Renderer
{
LM_API_RDR Ptr<Core::Bitmap> borderDiffuse(const Ptr<Core::Bitmap> & pSrcBMP);
LM_API_RDR void getCubeMapMatrix(Gfx::ECubeMapFace face, Core::Matrix4f & mat, const Core::Vector3f & pos);
LM_API_RDR void setSampler(Gfx::RSSamplerDesc & sampler, EFilterLevel level);

LM_API_RDR void                     filterResolutions(const Core::List<Gfx::DisplayMode> & modes,
                                                      const Gfx::DisplayMode &             initialMode,
                                                      Core::List<Resolution> &             allRes,
                                                      Core::List<Resolution> &             goodRes,
                                                      bool                                 logOn);
LM_API_RDR void                     filterResolutions(Core::List<Resolution> & resolutions, int32 minWidth, int32 minHeight);
LM_API_RDR String                   toStringRatio(float r);
LM_API_RDR Gfx::EMultiSamplingType  cvt(EMSAALevel msaa);
LM_API_RDR EMSAALevel               cvt(Gfx::EMultiSamplingType msaa);
LM_API_RDR Gfx::DisplayMode         bestFit(const Ptr<Gfx::IDriver> & pDriver, int32 id, int32 width, int32 height, Gfx::EDisplayPixelFormat pf);
}

#endif /* TOOLS_H_ */
