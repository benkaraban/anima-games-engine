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
#ifndef RENDERER_SHADOWRENDERER_H_
#define RENDERER_SHADOWRENDERER_H_

#include <Gfx/IDevice.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/ScreenQuad.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
class IShadowRenderer : public IGPUResource
{
public:
    virtual ~IShadowRenderer() {};
    virtual bool initialise() = 0;

    virtual void setContext(const RenderContext & context) = 0;
    virtual void startDynamicShadowMapRendering(int32 split) = 0;
    virtual void endDynamicShadowMapRendering() = 0;
    virtual Ptr<Gfx::ITexture2D> getDynamicShadowMap(int32 split) const = 0;

    virtual void startStaticShadowMapRendering() = 0;
    virtual void endStaticShadowMapRendering() = 0;
    virtual Ptr<Gfx::ITexture2D> getStaticShadowMap() const = 0;

    virtual void onDeviceLost() = 0;
    virtual void onDeviceReset() = 0;
};

class ShadowRendererDummy : public IShadowRenderer, public Core::Object
{
public:
    virtual bool initialise() { return true; };

    virtual void setContext(const RenderContext & context) {};
   virtual void startDynamicShadowMapRendering(int32 split) {};
    virtual void endDynamicShadowMapRendering() {};
    virtual Ptr<Gfx::ITexture2D> getDynamicShadowMap(int32 split) const { return null; };

    virtual void startStaticShadowMapRendering() {};
    virtual void endStaticShadowMapRendering() {};
    virtual Ptr<Gfx::ITexture2D> getStaticShadowMap() const { return null; };

    virtual void onDeviceLost() {};
    virtual void onDeviceReset() {};
};

class ShadowRenderer : public IShadowRenderer, public Core::Object
{
public:
    ShadowRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, 
                   const Ptr<ScreenQuad> & pScreenQuad, const Ptr<GPUResourceLib> & pResLib,
                   EShadowLevel shadowLevel);
    ~ShadowRenderer();
    virtual bool initialise();

    virtual void setContext(const RenderContext & context);
    virtual void startDynamicShadowMapRendering(int32 split);
    virtual void endDynamicShadowMapRendering();
    virtual Ptr<Gfx::ITexture2D> getDynamicShadowMap(int32 split) const;

    virtual void startStaticShadowMapRendering();
    virtual void endStaticShadowMapRendering();
    virtual Ptr<Gfx::ITexture2D> getStaticShadowMap() const;

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:
    Ptr<Gfx::IDevice>            _pDevice;
    Ptr<ShaderLib>               _pShaderLib;
    Ptr<ScreenQuad>              _pScreenQuad;
    Ptr<GPUResourceLib>          _pResLib;

    // Dynamic shadows ----------------------------------------------
    EShadowLevel                 _dynamicShadowLevel;
    int32                        _dynamicShadowMapSize;
    int32                        _dynamicBlurPassCount;

    Ptr<Gfx::ITexture2D>         _pDynamicShadowTexture0;
    Ptr<Gfx::ITexture2D>         _pDynamicShadowTexture1;
    Ptr<Gfx::ITexture2D>         _pDynamicShadowTexture2;

    Ptr<Gfx::IRenderTargetView>  _pDynamicShadowView0;
    Ptr<Gfx::IRenderTargetView>  _pDynamicShadowView1;
    Ptr<Gfx::IRenderTargetView>  _pDynamicShadowView2;
    Ptr<Gfx::IRenderTargetView>  _pDynamicCurView;

    Ptr<Gfx::ITexture2D>         _pDynamicShadowDepthTex;
    Ptr<Gfx::IDepthStencilView>  _pDynamicShadowDepthView;

    float                        _dynamicShadowWidthMul1;
    Ptr<Gfx::ITexture2D>         _pDynamicCurTexture;
    int32                        _dynamicCurSplit;

    // Static shadows -----------------------------------------------
    int32                        _staticShadowMapSize;

    Ptr<Gfx::ITexture2D>         _pStaticShadowTexture0;

    Ptr<Gfx::IRenderTargetView>  _pStaticShadowView0;
    Ptr<Gfx::IRenderTargetView>  _pStaticCurView;

    Ptr<Gfx::ITexture2D>         _pStaticShadowDepthTex;
    Ptr<Gfx::IDepthStencilView>  _pStaticShadowDepthView;

    float                        _staticShadowWidthMul;
    Ptr<Gfx::ITexture2D>         _pStaticCurTexture;
    int32                        _staticCurSplit;



    Gfx::GlobalRenderState       _stateBlur;

    Ptr<Gfx::IPixelShader>       _pPShaderBlur;
    int32                        _idDirectionScale;
    int32                        _idSamplerBlur;
};

}

#endif /* GLOWRENDERER_H_ */
