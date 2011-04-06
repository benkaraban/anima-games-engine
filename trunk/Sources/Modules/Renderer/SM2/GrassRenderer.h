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
#ifndef RENDERER_GRASSRENDERER_H_
#define RENDERER_GRASSRENDERER_H_

#include <Gfx/IDevice.h>
#include <Renderer/SM2/ICommandExecuter.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{

class IGrassRenderer : public IGPUResource, public ICommandExecuter
{
public:
    virtual ~IGrassRenderer() {};

    virtual bool initialise() = 0;

    virtual void onDeviceLost() = 0;
    virtual void onDeviceReset() = 0;
};

class GrassRenderer : public Core::Object, public IGrassRenderer
{
public:
    GrassRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                  const RendererSettings & settings);
    ~GrassRenderer();

    virtual bool initialise();

    virtual void startContext(const RenderContext & context, ERenderPass pass);
    virtual void endContext();
    
    virtual void enqueueCommands(Core::List<Command> & commands);
    virtual void exec(Command * pStart, Command * pEnd);

    virtual void onDeviceLost();
    virtual void onDeviceReset();


protected:
    Ptr<Gfx::IDevice>             _pDevice;
    Ptr<ShaderLib>                _pShaderLib;
    Ptr<GPUResourceLib>           _pResLib;
    RendererSettings              _settings;

    Ptr<Gfx::IVertexShader>       _pVShader;
    Ptr<Gfx::IPixelShader>        _pPShader;
    Ptr<Gfx::ShaderConstantTable> _pVConst;
    Ptr<Gfx::ShaderConstantTable> _pPConst;
    Ptr<Gfx::IVertexFormat>       _pFormat;

    Ptr<Gfx::IVertexBuffer>       _pVBuffer;
    Ptr<Gfx::IIndexBuffer>        _pIBuffer;

    Gfx::GlobalRenderState     _state;

    int32     _idWorldViewProj;
    int32     _idWorldView;

    int32     _idWindDirU;
    int32     _idWindDirV;
    int32     _idWindWaveLength;
    int32     _idWindPhase;
    int32     _idWindIntensity;

    /*
    int32     _idWindDir;
    int32     _idWindPeriod;
    int32     _idWindWavePeriod;
    int32     _idWindStrength;
    int32     _idTime;
    */
    int32     _idSampler;
    int32     _idFogRange;
    int32     _idFogColor;

    Core::Matrix4f      _viewProj;
    Core::Matrix4f      _view;
    InternalFogSettings _fogSettings;
    WindSettings        _windSettings[WIND_COUNT];

    float               _time;

    ERenderPass                 _pass;
    Core::List<Command>         _commands;
};

}

#endif /* GRASSRENDERER_H_ */
