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
#ifndef RENDERERSM2_BILLBOARDRENDERERSM0_H_
#define RENDERERSM2_BILLBOARDRENDERERSM0_H_

#include <Renderer/SM2/BillboardRenderer.h>

namespace Renderer
{
class LM_API_SM2 BillboardRendererSM0 : public IBillboardRenderer
{
public:
    BillboardRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                      const RendererSettings & settings);
    ~BillboardRendererSM0();

    virtual bool initialise();

    virtual void startContext(const RenderContext & context, ERenderPass pass);
    virtual void endContext();
    
    virtual void enqueueCommands(Core::List<Command> & commands);
    virtual void exec(Command * pStart, Command * pEnd);

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:

    Ptr<Gfx::IDevice>          _pDevice;
    Ptr<ShaderLib>             _pShaderLib;
    Ptr<GPUResourceLib>        _pResLib;
    RendererSettings           _settings;

    struct ShaderParams
    {
        Ptr<Gfx::IVertexShader>       pVShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;
        Ptr<Gfx::IVertexFormat>       pFormat;

        Gfx::GlobalRenderState        state;

        int32     idWorldView;
        int32     idProj;
        int32     idEyePosition;
        int32     idFogRange;
        int32     idSpriteCount;
        int32     idSpriteCountU;
        int32     idSpriteUVSize;
        int32     idFactor;
    };

    void initialise(ShaderParams & params, EBillboardMode mode, int32 flags);
    void send(const ShaderParams & params, BillboardGroup & bill, ERenderPass pass);
    int32 getFlags(const BillboardGroup & bill) const;

    static const int32 SPRITE_FLAG          = 0x00000001;
    static const int32 FLAG_COUNT           = (SPRITE_FLAG) + 1;

    ShaderParams    _params[EBillboardMode_COUNT][FLAG_COUNT];

    Ptr<Gfx::IIndexBuffer>      _pIBuffer;

    ERenderPass                 _pass;
    Core::List<Command>         _commands;

    IRenderTarget *     _pRenderTarget;
    InternalFogSettings _fogSettings;
    Core::Matrix4f   _view;
    Core::Matrix4f   _proj;
    Core::Vector3f   _eye;
};

}

#endif /* DECALRENDERER_H_ */
