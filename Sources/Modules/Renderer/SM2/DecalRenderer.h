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
#ifndef RENDERERSM2_DECALRENDERER_H_
#define RENDERERSM2_DECALRENDERER_H_

#include <Core/List.h>
#include <Gfx/IDevice.h>
#include <Renderer/SM2/ICommandExecuter.h>
#include <Renderer/SM2/InternalSettings.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/Common/Decal.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
class LM_API_SM2 IDecalRenderer : public IGPUResource, public ICommandExecuter
{
public:
    virtual ~IDecalRenderer() {};

    virtual bool initialise() = 0;

    virtual void onDeviceLost() = 0;
    virtual void onDeviceReset() = 0;
};

class LM_API_SM2 DecalRenderer : public Core::Object, public IDecalRenderer
{
public:
    DecalRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                  const RendererSettings & settings);
    ~DecalRenderer();

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
        Ptr<Gfx::IPixelShader>        pPShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;
        Ptr<Gfx::ShaderConstantTable> pPConst;
        Ptr<Gfx::IVertexFormat>       pFormat;

        Gfx::GlobalRenderState     state;

        int32     idWorldViewProj;
        int32     idWorldView;
        int32     idFogRange;
        int32     idColor;
        int32     idSpriteCount;
        int32     idSpriteCountU;
        int32     idSpriteUVSize;

        int32     idSamplerColor;
        int32     idSamplerNormal;
        int32     idSamplerRefraction;
        int32     idRefrScale;
    };

    void initialise(ShaderParams & params, EDecalMode mode, int32 flags);
    void bind(const ShaderParams & params, const Decal & decal);
    int32 getFlags(const Decal & decal) const;

    static const int32 SPRITE_FLAG          = 0x00000001;
    static const int32 NORMAL_MAP_DXT5_FLAG = 0x00000002;
    static const int32 FLAG_COUNT           = (SPRITE_FLAG | NORMAL_MAP_DXT5_FLAG) + 1;

    ShaderParams    _params[EDecalMode_COUNT][FLAG_COUNT];

    Ptr<Gfx::IVertexBuffer>   _pVBuffer;
    Ptr<Gfx::IIndexBuffer>    _pIBuffer;

    ERenderPass               _pass;
    Core::List<Command>       _commands;

    IRenderTarget *     _pRenderTarget;
    InternalFogSettings _fogSettings;
    Core::Matrix4f   _viewProj;
    Core::Matrix4f   _view;
    Core::Matrix4f   _proj;
};

}

#endif /* DECALRENDERER_H_ */
