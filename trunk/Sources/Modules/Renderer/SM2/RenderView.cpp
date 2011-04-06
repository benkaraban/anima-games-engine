/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include <Renderer/SM2/RenderView.h>

#include <Core/Logger.h>
#include <Core/Debug/PerfLogger.h>

namespace Renderer
{
//------------------------------------------------------------------------------
RenderView::RenderView(
        const Ptr<Gfx::IDevice> & pDevice,
        const Gfx::SwapChainDesc & sc,
        const Ptr<Gfx::ISwapChain> & pColorSwapChain,
        const Ptr<GPUResourceLib> & pResLib,
        const RendererSettings & settings
        ) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _sc(sc),
    _settings(settings),
    _curBuffer(ERenderTargetBuffer(-1)),
    _initialCreation(true),
    _pColorSwapChain(pColorSwapChain)
{
    _pResLib->registerResource(this);
}
//------------------------------------------------------------------------------
RenderView::~RenderView()
{
    _pResLib->unregisterResource(this);
}
//------------------------------------------------------------------------------
void RenderView::resize(int32 width, int32 height)
{
    if((_sc.width != width || _sc.height != height) &&
        (width != 0 && height != 0) &&
        (_sc.screenMode == Gfx::WINDOWED_MODE))
    {
        LM_ASSERT(_sc.screenMode == Gfx::WINDOWED_MODE);
        releaseBuffers();
        _sc.width = width;
        _sc.height = height;
        createBuffers();
    }
}
//------------------------------------------------------------------------------
void RenderView::createBuffers()
{
  //  INF << L"Used VMEM before create view : " << Core::toString(_pDevice->getTotalVideoMem() - _pDevice->getAvailableVideoMem()) << L"\n";

    // Light ------------------------------------------------------------------

    // Si le handleWindow est ‡ NULL il s'agit d'une renderview "trick" pour les reflexions, la
    // color view sera passÈe avec changeColorView

    if(_sc.handleWindow != NULL)
    {
        if(_sc.screenMode == Gfx::WINDOWED_MODE)
        {
            LM_ASSERT(_pColorSwapChain == null);
            _pColorSwapChain = _pDevice->createSwapChain(_sc);
        }
        else
        {
            // Le color buffer a dÈj‡ ÈtÈ crÈÈ en mÍme temps que le device
            // et a ÈtÈ passÈ en argument du constructeur
            LM_ASSERT(_pColorSwapChain != null);
        }

        if(_pColorSwapChain != null)
        {
            Gfx::RenderTargetViewDesc rtDesc;
            rtDesc.swapChain.idBuffer = 0;

            _pColorView = _pDevice->createRenderTargetView(_pColorSwapChain, rtDesc);
        }
    }

    // Z buffer
    {
        Gfx::Texture2DDesc tex2Desc;
        tex2Desc.width = _sc.width;
        tex2Desc.height = _sc.height;
        tex2Desc.mipLevels = 0;
        tex2Desc.format = Gfx::TF_D24S8;
        tex2Desc.bind = Gfx::TB_DEPTH_STENCIL_SURFACE;
        tex2Desc.multiSampleType = _sc.multiSamplingType;

        Gfx::DepthStencilViewDesc dsvDesc;
        dsvDesc.texture2D.mipSlice = 0;

        _pDepthTex = _pDevice->createTexture2D(tex2Desc);
        _pDepthView = _pDevice->createDepthStencilView(_pDepthTex, dsvDesc);
    }

    // Reflection -------------------------------------------------------------
    {
        Gfx::DepthStencilViewDesc dsDesc;
        dsDesc.texture2D.mipSlice = 0;

        Gfx::Texture2DDesc desc;

        switch(_settings.reflecLevel)
        {
        case REFLEC_NONE:
            desc.width = 1;
            desc.height = 1;
            break;
        case REFLEC_LOW:
            desc.width = std::max(1, _sc.width / 4);
            desc.height = std::max(1, _sc.height / 4);
            break;
        case REFLEC_MEDIUM:
            desc.width = std::max(1, _sc.width / 2);
            desc.height = std::max(1, _sc.height / 2);
            break;
        case REFLEC_HIGH:
            desc.width = std::max(1, _sc.width / 1);
            desc.height = std::max(1, _sc.height / 1);
            break;
        }

        desc.mipLevels = 1;
        desc.format = Gfx::TF_A8R8G8B8;
        desc.usage = Gfx::BUFFER_USAGE_DEFAULT;
        desc.cpuAccess = Gfx::BCA_NO_ACCESS;
        desc.bind = Gfx::TB_RENDER_TARGET;

        _pReflecColorTex = _pDevice->createTexture2D(desc);

        Gfx::RenderTargetViewDesc rtDesc;
        rtDesc.texture2D.mipSlice = 0;

        _pReflecColorView = _pDevice->createRenderTargetView(_pReflecColorTex, rtDesc);
    }

    // Refraction -------------------------------------------------------------
    {
        Gfx::DepthStencilViewDesc dsDesc;
        dsDesc.texture2D.mipSlice = 0;

        Gfx::Texture2DDesc desc;
        desc.width = _sc.width;
        desc.height = _sc.height;
        desc.mipLevels = 1;
        desc.format = Gfx::TF_A8R8G8B8;
        desc.usage = Gfx::BUFFER_USAGE_DEFAULT;
        desc.cpuAccess = Gfx::BCA_NO_ACCESS;
        desc.bind = Gfx::TB_RENDER_TARGET;

        _pRefracColorTex = _pDevice->createTexture2D(desc);

        Gfx::RenderTargetViewDesc rtDesc;
        rtDesc.texture2D.mipSlice = 0;

        _pRefracColorView = _pDevice->createRenderTargetView(_pRefracColorTex, rtDesc);
    }

    // Glow -------------------------------------------------------------------

    {
        int32 glowWidth = std::max(1, _sc.width / 4);
        int32 glowHeight = std::max(1, _sc.height / 4);

        if(!_settings.glowOn && !_settings.postFXOn)
        {
            glowWidth = 1;
            glowHeight = 1;
        }

        Gfx::Texture2DDesc desc;
        desc.width = glowWidth;
        desc.height = glowHeight;
        desc.mipLevels = 1;
        desc.usage = Gfx::BUFFER_USAGE_DEFAULT;
        desc.cpuAccess = Gfx::BCA_NO_ACCESS;

        Gfx::RenderTargetViewDesc rtDesc;
        rtDesc.texture2D.mipSlice = 0;

        desc.format = Gfx::TF_A8R8G8B8;
        desc.usage = Gfx::BUFFER_USAGE_DEFAULT;
        desc.cpuAccess = Gfx::BCA_NO_ACCESS;
        desc.bind = Gfx::TB_RENDER_TARGET;

        _pGlowColorTex1 = _pDevice->createTexture2D(desc);
        _pGlowColorTex2 = _pDevice->createTexture2D(desc);

        _pGlowColorView1 = _pDevice->createRenderTargetView(_pGlowColorTex1, rtDesc);
        _pGlowColorView2 = _pDevice->createRenderTargetView(_pGlowColorTex2, rtDesc);

        desc.width = _sc.width;
        desc.height = _sc.height;

        if(!_settings.glowOn)
        {
            desc.width = 1;
            desc.height = 1;
        }

        _pGlowColorTex0 = _pDevice->createTexture2D(desc);
        _pGlowColorView0 = _pDevice->createRenderTargetView(_pGlowColorTex0, rtDesc);

        desc.width = std::max(1, _sc.width / 2);
        desc.height = std::max(1, _sc.height / 2);
        desc.multiSampleType = Gfx::MSAA_NONE;;

        _pGlowHalfColorTex = _pDevice->createTexture2D(desc);
        _pGlowHalfColorView = _pDevice->createRenderTargetView(_pGlowHalfColorTex, rtDesc);
    }

   // INF << L"Used VMEM after  create view : " << Core::toString(_pDevice->getTotalVideoMem() - _pDevice->getAvailableVideoMem()) << L"\n";
}
//------------------------------------------------------------------------------
void RenderView::begin()
{
    _history.clear();
    _curBuffer = ERenderTargetBuffer(-1);
    _refracLayer = 0;
    _forcedRefracLayer = 0;

    switch(_settings.refracLevel)
    {
    case REFRAC_NONE:   _maxRefracLayer = 0; break;
    case REFRAC_LOW:    _maxRefracLayer = 1; break;
    case REFRAC_MEDIUM: _maxRefracLayer = 2; break;
    case REFRAC_HIGH:   _maxRefracLayer = 3; break;
    }
}
//------------------------------------------------------------------------------
void RenderView::end()
{
    _stats = L"views{refrac:";
    _stats << Core::toString(_refracLayer) << L"/" << Core::toString(_maxRefracLayer);
    _stats << L" (";
    _stats << Core::toString(_forcedRefracLayer);
    _stats << L") ";

    for(int32 ii=0; ii < _history.size(); ii++)
    {
        String str(Renderer::toString(_history[ii]));
        if(ii > 0)
            _stats << L" ";
        _stats << str.slice(3, str.size() - 8);
    }

    _stats << L"}\n";
}
//------------------------------------------------------------------------------
String RenderView::getStats()
{
    return _stats;
}
//------------------------------------------------------------------------------
void RenderView::updateRefractionBuffer(bool forceUpdate)
{
    if(forceUpdate)
    {
        _pDevice->copyRenderTargets(_pColorView, _pRefracColorView);
        _forcedRefracLayer ++;
    }
    else
    {
        if(_refracLayer < _maxRefracLayer)
        {
            _pDevice->copyRenderTargets(_pColorView, _pRefracColorView);
            _refracLayer ++;
        }
    }
}
//------------------------------------------------------------------------------
void RenderView::bind(ERenderTargetBuffer buffer)
{
    if(buffer == _curBuffer)
        return;

    _history.push_back(buffer);

    if(_curBuffer == RT_GLOW0_BUFFER && _sc.multiSamplingType != Gfx::MSAA_NONE)
    {
        _pDevice->copyRenderTargets(_pColorView, _pGlowColorView0);
    }

    switch(buffer)
    {
    case RT_REFRACTION_BUFFER:
        LM_ASSERT(false);
    case RT_GLOW0_BUFFER:
        if(_sc.multiSamplingType == Gfx::MSAA_NONE)
            _pDevice->setRenderTarget(_pGlowColorView0, _pDepthView);
        else
            _pDevice->setRenderTarget(_pColorView, _pDepthView);
        break;
    case RT_LIGHT_BUFFER:
        _pDevice->setRenderTarget(_pColorView, _pDepthView);
        break;
    case RT_REFLECTION_BUFFER:
        _pDevice->setRenderTarget(_pReflecColorView, _pDepthView);
        break;
    case RT_GLOW_HALF_BUFFER:
        _pDevice->setRenderTarget(_pGlowHalfColorView, null);
        break;
    case RT_GLOW1_BUFFER:
        _pDevice->setRenderTarget(_pGlowColorView1, null);
        break;
    case RT_GLOW2_BUFFER:
        _pDevice->setRenderTarget(_pGlowColorView2, null);
        break;
    }

    _curBuffer = buffer;
}
//------------------------------------------------------------------------------
void RenderView::present()
{
    LM_ASSERT(_pColorSwapChain != null);
    LM_START_PERF_LOG(L"flip");
    _pColorSwapChain->present();
    LM_END_PERF_LOG(L"flip");
}
//-----------------------------------------------------------------------------
int32 RenderView::getViewportWidth(ERenderTargetBuffer buffer) const
{
    int32 result = 0;

    switch(buffer)
    {
    case RT_REFLECTION_BUFFER: result = _pReflecColorView->getWidth(); break;
    case RT_REFRACTION_BUFFER:
    case RT_GLOW0_BUFFER:
    case RT_LIGHT_BUFFER: result = _sc.width; break;
    case RT_GLOW_HALF_BUFFER: result = _pGlowHalfColorView->getWidth(); break;
    case RT_GLOW1_BUFFER: result = _pGlowColorView1->getWidth(); break;
    case RT_GLOW2_BUFFER: result = _pGlowColorView2->getWidth(); break;
    }

    return result;
}
//-----------------------------------------------------------------------------
int32 RenderView::getViewportHeight(ERenderTargetBuffer buffer) const
{
    int32 result = 0;

    switch(buffer)
    {
    case RT_REFLECTION_BUFFER: result = _pReflecColorView->getHeight(); break;
    case RT_REFRACTION_BUFFER:
    case RT_GLOW0_BUFFER:
    case RT_LIGHT_BUFFER: result = _sc.height; break;
    case RT_GLOW_HALF_BUFFER: result = _pGlowHalfColorView->getHeight(); break;
    case RT_GLOW1_BUFFER: result = _pGlowColorView1->getHeight(); break;
    case RT_GLOW2_BUFFER: result = _pGlowColorView2->getHeight(); break;
    }

    return result;
}
//------------------------------------------------------------------------------
void RenderView::changeLightBuffer(const Ptr<Gfx::IRenderTargetView> & pColorView)
{
    _pColorView = pColorView;
}
//------------------------------------------------------------------------------
Ptr<Gfx::IShaderResourceView> RenderView::getShaderTextureView(ERenderTargetBuffer buffer)
{
    Ptr<Gfx::IShaderResourceView> pResult;

    switch(buffer)
    {
    case RT_LIGHT_BUFFER:      pResult = null; break;
    case RT_REFLECTION_BUFFER: pResult = _pReflecColorTex->getShaderResourceView(); break;
    case RT_REFRACTION_BUFFER: pResult = _pRefracColorTex->getShaderResourceView(); break;
    case RT_GLOW0_BUFFER:      pResult = _pGlowColorTex0->getShaderResourceView(); break;
    case RT_GLOW_HALF_BUFFER:  pResult = _pGlowHalfColorTex->getShaderResourceView(); break;
    case RT_GLOW1_BUFFER:      pResult = _pGlowColorTex1->getShaderResourceView(); break;
    case RT_GLOW2_BUFFER:      pResult = _pGlowColorTex2->getShaderResourceView(); break;
    }

    return pResult;
}
//------------------------------------------------------------------------------
Ptr<Core::Bitmap> RenderView::getScreenShot() const
{
    return _pDevice->toBitmap(_pColorView);
}
//------------------------------------------------------------------------------
void RenderView::getDebugScreenShots(Screenshots & shots) const
{
    shots.pColor = _pDevice->toBitmap(_pColorView);
    shots.pReflec = _pDevice->toBitmap(_pReflecColorView);
    shots.pRefrac = _pDevice->toBitmap(_pRefracColorView);
    shots.pGlow0 = _pDevice->toBitmap(_pGlowColorView0);
    shots.pGlow1 = _pDevice->toBitmap(_pGlowColorView1);
    shots.pGlow2 = _pDevice->toBitmap(_pGlowColorView2);
    shots.pGlowHalf = _pDevice->toBitmap(_pGlowHalfColorView);
}
//------------------------------------------------------------------------------
void RenderView::releaseBuffers()
{
  //  INF << L"Used VMEM before delete view : " << Core::toString(_pDevice->getTotalVideoMem() - _pDevice->getAvailableVideoMem()) << L"\n";

    _pReflecColorView = null;
    _pReflecColorTex = null;

    _pRefracColorView = null;
    _pRefracColorTex = null;

    _pGlowColorView0 = null;
    _pGlowColorTex0 = null;

    _pGlowHalfColorView = null;
    _pGlowHalfColorTex = null;

    _pGlowColorView1 = null;
    _pGlowColorTex1 = null;

    _pGlowColorView2 = null;
    _pGlowColorTex2 = null;

    _pDepthView = null;
    _pDepthTex = null;

    _pColorView = null;
    _pColorSwapChain = null;

    //INF << L"Used VMEM after  delete view : " << Core::toString(_pDevice->getTotalVideoMem() - _pDevice->getAvailableVideoMem()) << L"\n";
}
//------------------------------------------------------------------------------
void RenderView::onDeviceLost()
{
    releaseBuffers();
}
//------------------------------------------------------------------------------
void RenderView::onDeviceReset()
{
    if(_sc.screenMode == Gfx::WINDOWED_MODE || _initialCreation)
    {
        createBuffers();
        _initialCreation = false;
    }
    else
    {
        // La RenderView va Ítre recrÈÈe ‡ partir de la swap chain fournie au reset du device
    }
}
//------------------------------------------------------------------------------
void RenderView::onFullscreenChange(bool fullscreen)
{
    _sc.screenMode = fullscreen ? Gfx::FULLSCREEN_MODE : Gfx::WINDOWED_MODE;
}
//------------------------------------------------------------------------------
}
