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
#ifndef RENDERER_RENDERTOOLS_H_
#define RENDERER_RENDERTOOLS_H_

#include <Core/List.h>
#include <Renderer/IFreeForm.h>

namespace Renderer
{
LM_ENUM_4(ETrailMode, 
          TRAIL_STANDARD_UNORM, 
          TRAIL_STANDARD_NORM,
          TRAIL_TANGENT_LOCAL,
          TRAIL_TANGENT_VIEW);

struct TrailConfig
{
    float   headWidth;
    float   tailWidth;

    float   headU;
    float   tailU;

    float   duStart;

    bool    tailClamp;
    bool    tailFade;

    Core::Vector4f headColor;
    Core::Vector4f tailColor;

    Core::Vector4f headGlow;
    Core::Vector4f tailGlow;

    ETrailMode     mode;
    Core::Vector3f viewVec;
};

LM_API_RDR void generateTrail(Core::List<Renderer::FreeFormVertex> & vert, 
                              Core::List<uint16> & ind, 
                              const Core::List<Core::Vector3f> & positions, 
                              const TrailConfig & trail,
                              bool append = false);
}

#endif /* SETTINGS_H_ */
