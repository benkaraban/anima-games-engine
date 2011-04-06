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
#ifndef RENDERER_WATERRENDERER_H_
#define RENDERER_WATERRENDERER_H_

#include <Gfx/IDevice.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/WaterPatch.h>
#include <Renderer/SM2/ICommandExecuter.h>
#include <Renderer/SM2/InternalSettings.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
class TextureMap;

class IWaterRenderer : public IGPUResource, public ICommandExecuter
{
public:
    virtual ~IWaterRenderer() {};
    virtual bool initialise() = 0;

    virtual void onDeviceLost() = 0;
    virtual void onDeviceReset() = 0;
};

const int32 WATER_REFRAC_FLAG      = 0x00000001;
const int32 WATER_REFLEC_FLAG      = 0x00000002;
const int32 WATER_IS_NORMAL_DXT5_0 = 0x00000004;
const int32 WATER_IS_NORMAL_DXT5_1 = 0x00000008;
const int32 WATER_IS_NORMAL_DXT5_2 = 0x00000010;
const int32 WATER_IS_NORMAL_DXT5_3 = 0x00000020;

const int32 WATER_CONFIG_COUNT = (WATER_REFRAC_FLAG | WATER_REFLEC_FLAG | 
                                  WATER_IS_NORMAL_DXT5_0 | WATER_IS_NORMAL_DXT5_1 | WATER_IS_NORMAL_DXT5_2 | WATER_IS_NORMAL_DXT5_3) + 1;

class WaterRenderer : public Core::Object, public IWaterRenderer
{
public:
    WaterRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, 
                  const Ptr<TextureMap> & pDefaultTexture, const Ptr<GPUResourceLib> & pResLib,
                  const RendererSettings & settings);
    ~WaterRenderer();
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
        Ptr<Gfx::IPixelShader>        pPShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;
        Ptr<Gfx::ShaderConstantTable> pPConst;

        Gfx::GlobalRenderState        state;

        int32     idWorldViewProj;
        int32     idWorld;
        int32     idEyePosition;
        int32     idFogOpacity;

        int32     idFogColor;
        int32     idWaveU;
        int32     idWaveV;
        int32     idWaveScaleU;
        int32     idWaveScaleV;
        int32     idWaveAmplitude;
        int32     idWaterDiffuse;
        int32     idWaterSpecular;
        int32     idWaterShininess;
        int32     idPixelAlign;
        int32     idReflScale;
        int32     idRefrScale;
        int32     idFresnelClamp;
        int32     idFresnelPower;
        int32     idSamplerWave0;
        int32     idSamplerWave1;
        int32     idSamplerWave2;
        int32     idSamplerWave3;
        int32     idSamplerReflection;
        int32     idSamplerRefraction;
        int32     idSamplerDiffuse;
        int32     idMainLightDir;
        int32     idMainLightAmbient;
        int32     idMainLightDiffuse;
        int32     idMainLightSpecular;
        int32     idIsNormalMap0DXT5;
        int32     idIsNormalMap1DXT5;
        int32     idIsNormalMap2DXT5;
        int32     idIsNormalMap3DXT5;
    };

    Ptr<Gfx::IVertexFormat>       _pFormatMask;
    Ptr<Gfx::IVertexShader>       _pVShaderMask;
    Gfx::GlobalRenderState        _stateMask;
    int32                         _idWorldViewProjMask;

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
    int32                      _patchCount;

    int32 _vcount;
    int32 _tcount;

    IRenderTarget *     _pRenderTarget;
    ERenderPass         _pass;

    void initialise(ShaderParams & params, int32 flags);
    void renderMask(const Core::List<WaterPatch *> patches);
    void render(const ShaderParams & params, const Core::List<WaterPatch *> patches);

    Ptr<Gfx::IShaderResourceView> getResource(const Ptr<ITextureMap> & pTexture) const;
};

}

#endif /* WATERRENDERER_H_ */
