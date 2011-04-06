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
#ifndef RENDERER_RENDERVIEW_
#define RENDERER_RENDERVIEW_

#include <Gfx/IDevice.h>
#include <Renderer/IRenderView.h>
#include <Renderer/Settings.h>
#include <Renderer/Common/GPUResource.h>
#include <Renderer/SM2/IRenderTarget.h>

namespace Renderer
{
class LM_API_SM2 RenderView : public Core::Object, public IRenderView, public IRenderTarget, public IGPUResource
{
public:
    RenderView(
        const Ptr<Gfx::IDevice> & pDevice,
        const Gfx::SwapChainDesc & sc,
        const Ptr<Gfx::ISwapChain> & pColorSwapChain,
        const Ptr<GPUResourceLib> & pResLib,
        const RendererSettings & settings
        );
    ~RenderView();

    virtual void changeLightBuffer(const Ptr<Gfx::IRenderTargetView> & pColorView);
    virtual void present();


    // Implémentation IRenderView

    virtual void resize(int32 width, int32 height);
    virtual int32 getWidth() const { return _sc.width; };
    virtual int32 getHeight() const { return _sc.height; };
    virtual Ptr<Core::Bitmap> getScreenShot() const;
    virtual void getDebugScreenShots(Screenshots & shots) const;


    // Implémentation IRenderTarget

    virtual void begin();
    virtual void end();
    virtual void updateRefractionBuffer(bool forceUpdate = false);
    virtual String getStats();

    virtual void bind(ERenderTargetBuffer buffer);
    virtual Ptr<Gfx::IShaderResourceView> getShaderTextureView(ERenderTargetBuffer buffer);
    virtual int32 getViewportWidth(ERenderTargetBuffer buffer) const;
    virtual int32 getViewportHeight(ERenderTargetBuffer buffer) const;

    virtual void onDeviceLost();
    virtual void onDeviceReset();
    virtual void onFullscreenChange(bool fullscreen);

protected:
    Ptr<Gfx::IDevice>            _pDevice;
    Ptr<GPUResourceLib>          _pResLib;
    Gfx::SwapChainDesc           _sc;
    RendererSettings             _settings;

    Core::List<ERenderTargetBuffer> _history;
    int32                        _forcedRefracLayer;
    int32                        _refracLayer;
    int32                        _maxRefracLayer;
    String                       _stats;
    ERenderTargetBuffer          _curBuffer;
    bool                         _initialCreation;

    Ptr<Gfx::ISwapChain>         _pColorSwapChain;
    Ptr<Gfx::IRenderTargetView>  _pColorView;

    Ptr<Gfx::ITexture2D>         _pDepthTex;
    Ptr<Gfx::IDepthStencilView>  _pDepthView;

    Ptr<Gfx::ITexture2D>            _pReflecColorTex;
    Ptr<Gfx::IRenderTargetView>     _pReflecColorView;

    Ptr<Gfx::ITexture2D>            _pRefracColorTex;
    Ptr<Gfx::IRenderTargetView>     _pRefracColorView;

    Ptr<Gfx::ITexture2D>            _pGlowColorTex0;
    Ptr<Gfx::IRenderTargetView>     _pGlowColorView0;

    Ptr<Gfx::ITexture2D>            _pGlowHalfColorTex;
    Ptr<Gfx::IRenderTargetView>     _pGlowHalfColorView;

    Ptr<Gfx::ITexture2D>            _pGlowColorTex1;
    Ptr<Gfx::IRenderTargetView>     _pGlowColorView1;

    Ptr<Gfx::ITexture2D>            _pGlowColorTex2;
    Ptr<Gfx::IRenderTargetView>     _pGlowColorView2;

    virtual void createBuffers();
    virtual void releaseBuffers();
};

}
#endif
