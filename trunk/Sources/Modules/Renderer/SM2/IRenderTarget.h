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
#ifndef RENDERER_IRENDERTARGET_H_
#define RENDERER_IRENDERTARGET_H_

#include <Gfx/IShader.h>

namespace Renderer
{
LM_ENUM_7(ERenderTargetBuffer,
        RT_LIGHT_BUFFER,      // Buffer RGBA principal
        RT_REFLECTION_BUFFER,
        RT_REFRACTION_BUFFER, // Sert aussi pour le bloom
        RT_GLOW0_BUFFER,      // Buffers RGBA taille identique buffer principal
        RT_GLOW_HALF_BUFFER,
        RT_GLOW1_BUFFER,      // Buffers RGBA au 16ième de la taille du buffer principal (1/4 * 1/4)
        RT_GLOW2_BUFFER);

class LM_API_SM2 IRenderTarget
{
public:
    virtual ~IRenderTarget() {};
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual String getStats() = 0;
    virtual void updateRefractionBuffer(bool forceUpdate = false) = 0;
    virtual void bind(ERenderTargetBuffer buffer) = 0;
    virtual Ptr<Gfx::IShaderResourceView> getShaderTextureView(ERenderTargetBuffer buffer) = 0;
    virtual int32 getViewportWidth(ERenderTargetBuffer buffer) const = 0;
    virtual int32 getViewportHeight(ERenderTargetBuffer buffer) const = 0;
};
}

#endif /* IRENDERTARGET_H_ */
