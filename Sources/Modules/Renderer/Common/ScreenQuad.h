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
#ifndef RENDERER_SCREENQUAD_H_
#define RENDERER_SCREENQUAD_H_

#include <Core/Map.h>
#include <Gfx/IDevice.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
class LM_API_RDR ScreenQuad : public Core::Object, public IGPUResource
{
public:
    ScreenQuad(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib);
    ~ScreenQuad();
    bool initialise();
    void send(int32 width, int32 height,
              const Core::Vector4f & color = Core::Vector4f::ONE) const;
    void send(int32 x1, int32 y1, int32 x2, int32 y2,
              const Core::Vector4f & color = Core::Vector4f::ONE) const;
    void send(int32 x1, int32 y1, int32 x2, int32 y2,
              float u1, float v1, float u2, float v2,
              const Core::Vector4f & color = Core::Vector4f::ONE) const;

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:
    Ptr<Gfx::IDevice>       _pDevice;
    Ptr<ShaderLib>          _pShaderLib;
    Ptr<GPUResourceLib>     _pResLib;

    Ptr<Gfx::IVertexFormat> _pFormat;
    Ptr<Gfx::IVertexBuffer> _pVBuffer;
    Ptr<Gfx::IIndexBuffer>  _pIBuffer;
};

}

#endif /* SCREENQUAD_H_ */
