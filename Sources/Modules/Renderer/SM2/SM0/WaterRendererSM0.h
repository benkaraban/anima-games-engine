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
#ifndef RENDERER_WATERRENDERERSM0_H_
#define RENDERER_WATERRENDERERSM0_H_

#include <Renderer/SM2/WaterRenderer.h>

namespace Renderer
{
class WaterRendererSM0 : public Core::Object, public IWaterRenderer
{
public:
    WaterRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, 
                  const Ptr<TextureMap> & pDefaultTexture, const Ptr<GPUResourceLib> & pResLib,
                  const RendererSettings & settings);
    ~WaterRendererSM0();
    virtual bool initialise();

    virtual void startContext(const RenderContext & context, ERenderPass pass);
    virtual void endContext();
    
    virtual void enqueueCommands(Core::List<Command> & commands);
    virtual void exec(Command * pStart, Command * pEnd);

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:
    Ptr<Gfx::IDevice>         _pDevice;
    Ptr<ShaderLib>            _pShaderLib;
    Ptr<GPUResourceLib>       _pResLib;

    Ptr<TextureMap>           _pDefaultTexture;

    RendererSettings          _settings;

    Ptr<Gfx::IVertexBuffer>   _pVBuffer;
    Ptr<Gfx::IIndexBuffer>    _pIBuffer;

    struct ShaderParams
    {
        Ptr<Gfx::IVertexFormat>       pFormat;

        Ptr<Gfx::IVertexShader>       pVShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;

        Gfx::GlobalRenderState        state;

        int32     idWorldViewProj;
        int32     idWorld;
        int32     idEyePosition;
        int32     idFogOpacity;
        int32     idWaterDiffuse;
    };

    ShaderParams              _params[WATER_CONFIG_COUNT];
    Core::List<WaterPatch *>  _patches[WATER_CONFIG_COUNT];

    InternalFogSettings        _fogSettings;
    InternalMainLightSettings  _mainLightSettings;
    Core::Matrix4f             _view;
    Core::Matrix4f             _proj;
    Core::Matrix4f             _viewProj;
    Core::Vector3f             _eye;
    double                     _time;
    float                      _waterLevel;

    int32 _vcount;
    int32 _tcount;

    IRenderTarget *            _pRenderTarget;

    void initialise(ShaderParams & params, int32 flags);
    void render(const ShaderParams & params, const Core::List<WaterPatch *> patches);

    Ptr<Gfx::IShaderResourceView> getResource(const Ptr<ITextureMap> & pTexture) const;
};

}

#endif /* WaterRendererSM0_H_ */
