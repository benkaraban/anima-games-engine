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
#ifndef DEVICEDX9_H_
#define DEVICEDX9_H_


#include <Gfx/GfxDx9/VertexBufferDx9.h>
#include <Gfx/GfxDx9/IndexBufferDx9.h>
#include <Gfx/GfxDx9/SwapChainDx9.h>
#include <Gfx/GfxDx9/RenderStateDx9.h>
#include <Gfx/GfxDx9/RenderTargetDx9.h>
#include <Gfx/GfxDx9/ShaderDx9.h>
#include <Gfx/IDevice.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
class LM_API_GX9 DeviceDx9: public IDevice
{
public:
    DeviceDx9(LPDIRECT3DDEVICE9 pDevice, const D3DCAPS9 & dcDx9, const GraphicCapabilities & caps, int32 frameBufferMem);
    virtual ~DeviceDx9();

    ////////////////////////////////
    //  Render State
    ////////////////////////////////
    virtual IRSRasterizerPtr createState(const RSRasterizerDesc& desc);
    virtual IRSBlendPtr createState(const RSBlendDesc& desc);
    virtual IRSDepthStencilPtr createState(const RSDepthStencilDesc& desc);
    virtual IRSSamplerPtr createState(const RSSamplerDesc& desc);

    virtual void setState(const IRSRasterizerPtr& newS);
    virtual void setState(const IRSBlendPtr& newS);
    virtual void setState(const IRSDepthStencilPtr& newS);
    virtual void setState(int32 id, const IRSSamplerPtr& newS);

    ////////////////////////////////
    //  Geometrie
    ////////////////////////////////
    virtual IVertexFormatPtr createVertexFormat(const VertexFormatDesc & desc, const IVertexShaderPtr & pShader);
    //Note pInitData est utilisé pour initialiser les buffer (surtout utile si IMMUATABLE)
    virtual IVertexBufferPtr createVertexBuffer(const VertexBufferDesc& desc, const SubresourceData* pInitData = NULL);
    virtual IIndexBufferPtr createIndexBuffer(const IndexBufferDesc& desc, const SubresourceData* pInitData = NULL);

    virtual void setVertexFormat(const IVertexFormatPtr & pFormat);
    virtual void setVertexBuffer(int32 stream, const IVertexBufferPtr & pBuffer, int32 stride, int32 offset);
    virtual void setVertexBuffers(int32 nbBuffers, IVertexBufferPtr* tabVB, int32* tabStrides, int32* tabOffset);
    virtual void setIndexBuffer(const IIndexBufferPtr & pBuffer);

    ////////////////////////////////
    //  Shader
    ////////////////////////////////
    virtual IPixelShaderBinObjectPtr compilePShader(const String& code, EPixelShaderVersion version,
                                                    const String& entryPoint, const Core::List<ShaderMacro> & macros,
                                                    const Ptr<IShaderIncluder> & pIncluder);

    virtual IVertexShaderBinObjectPtr compileVShader(const String& code, EVertexShaderVersion version,
                                                     const String& entryPoint, const Core::List<ShaderMacro> & macros,
                                                     const Ptr<IShaderIncluder> & pIncluder);

    virtual IVertexShaderBinObjectPtr readPrecompiledVShader(Core::InputStream & inputStream);
    virtual IPixelShaderBinObjectPtr readPrecompiledPShader(Core::InputStream & inputStream);

    virtual void writePrecompiledVShader(Core::OutputStream & outputStream, const IVertexShaderBinObjectPtr & pBinObject);
    virtual void writePrecompiledPShader(Core::OutputStream & outputStream, const IPixelShaderBinObjectPtr & pBinObject);

    virtual IVertexShaderPtr createVertexShader(const IVertexShaderBinObjectPtr & pShaderBinObject);
    virtual IPixelShaderPtr createPixelShader(const IPixelShaderBinObjectPtr & pShaderBinObject);

    virtual void setVertexShader(const IVertexShaderPtr & pShader);
    virtual void setPixelShader(const IPixelShaderPtr & pShader);

    virtual void PSSetShaderResources(const int32 startSlot, const int32& nbViews,
                                      IShaderResourceViewPtr* pShaderResourceViews);
    virtual void VSSetShaderResources(const int32 startSlot, const int32& nbViews,
                                      IShaderResourceViewPtr* pShaderResourceViews);

    virtual void setFixedFuncPipe(const RSFixedFunc & fixed);

    ////////////////////////////////
    //  RenderWindow
    ////////////////////////////////
    virtual IRenderTargetViewPtr createRenderTargetView(ISwapChainPtr pSwapChain, const RenderTargetViewDesc& desc);
    virtual IRenderTargetViewPtr createRenderTargetView(ITexture2DPtr tex2D, const RenderTargetViewDesc& desc);
    virtual IRenderTargetViewPtr createRenderTargetView(ITextureCubePtr texCube, const RenderTargetViewDesc& desc);
    virtual IRenderTargetViewPtr createRenderTargetView(const RenderTargetViewDesc& desc);

    virtual EDeviceState getDeviceState();

    virtual ISwapChainPtr createSwapChain(const SwapChainDesc& desc);
    virtual ISwapChainPtr reset(const SwapChainDesc & desc);

    ////////////////////////////////
    //  RenderTarget
    ////////////////////////////////
    virtual void setRenderTarget(IRenderTargetViewPtr rtv, IDepthStencilViewPtr dsv);
    virtual void setRenderTargets(const int32 nbRtv, IRenderTargetViewPtr* tabRtv, IDepthStencilViewPtr dsv);
    virtual void getRenderTarget(IRenderTargetViewPtr& rtv, IDepthStencilViewPtr& dsv);
    virtual void clearCurrentRenderTargetView(const RGBA& color, float depth = 1.0f, int32 stencil = 0, int32 flags = CLEAR_ALL_BUFFERS);

    virtual void copyRenderTargets(const Ptr<IRenderTargetView> & pSrc, const Ptr<IRenderTargetView> & pDst);

    ////////////////////////////////
    //  DepthStencil
    ////////////////////////////////
    virtual IDepthStencilViewPtr createDepthStencilView(ITexture2DPtr tex2D, const DepthStencilViewDesc& desc);

    ////////////////////////////////
    //  Texture
    ////////////////////////////////
    //Note pInitData est utilisé pour initialiser les texture (surtout utile si IMMUATABLE)
    virtual ITexture1DPtr createTexture1D(const Texture1DDesc& desc, const SubresourceData* pInitData = NULL);
    virtual ITexture2DPtr createTexture2D(const Texture2DDesc& desc, const SubresourceData* pInitData = NULL);
    virtual ITexture3DPtr createTexture3D(const Texture3DDesc& desc, const SubresourceData* pInitData = NULL);
    virtual ITextureCubePtr createTextureCube(const TextureCubeDesc& desc, const SubresourceData* pInitData [6] = NULL);

    virtual void updateTexture(const Ptr<ITexture1D> & pSrc, const Ptr<ITexture1D> & pDst);
    virtual void updateTexture(const Ptr<ITexture2D> & pSrc, const Ptr<ITexture2D> & pDst);
    virtual void updateTexture(const Ptr<ITexture3D> & pSrc, const Ptr<ITexture3D> & pDst);
    virtual void updateTexture(const Ptr<ITextureCube> & pSrc, const Ptr<ITextureCube> & pDst);

    ////////////////////////////////
    //  Draw
    ////////////////////////////////
    virtual void beginDraw();
    virtual void endDraw();

    virtual void drawIndexed(EPrimitiveType primType, int32 usedVertices, int32 startIndex, int32 primCount);
    virtual void drawIndexed(EPrimitiveType primType, int32 baseVertexIndex, int32 MinIndex, int32 usedVertices, int32 startIndex, int32 primCount);

    virtual void getCaps(GraphicCapabilities & caps);
    virtual void setViewport(const Viewport& vp);

    virtual Ptr<Core::Bitmap> toBitmap(const IRenderTargetViewPtr& pRtv);
    virtual Ptr<Core::Bitmap> toBitmap(const IDepthStencilViewPtr& pDtv);

    ////////////////////////////////
    //  Infos
    ////////////////////////////////

    virtual int64 getTotalVideoMem();
    virtual int64 getAvailableVideoMem();

    virtual void startStats();
    virtual void endStats();
    virtual const GfxStats & getStats() const;

    ////////////////////////////////
    //  HUD
    ////////////////////////////////

    virtual Ptr<IFont> createFont(const FontDesc & desc);

    ////////////////////////////////
    //  Internal
    ////////////////////////////////

    virtual void tagTextureUnitAsUsed(int32 id);

protected:
    void compileShader(const String & code, const String8 & str8Version, const String & entryPoint,
                       LPD3DXCONSTANTTABLE & pConstTableD3D, LPD3DXBUFFER & pShaderData,
                       const Core::List<ShaderMacro> & macros, const Ptr<IShaderIncluder> & pIncluder);

    void internalSetFilter(int32 sampler, D3DTEXTUREFILTERTYPE min, D3DTEXTUREFILTERTYPE mag, D3DTEXTUREFILTERTYPE mip);
    void setDefaultState();

protected:
    LPDIRECT3DDEVICE9 _pDevice;

    D3DCAPS9 _capsD3D;
    GraphicCapabilities _caps;

    GfxStats    _stats;

    GlobalRenderState _currentState;
    bool _separateAlphaBlendEnabled;
    bool _deviseIsLost;

    int32   _renderTargetsCount;
    int64   _totalVideoMem;

    int32  _minFilter[MAX_SAMPLERS];
    int32  _magFilter[MAX_SAMPLERS];
    int32  _mipFilter[MAX_SAMPLERS];
    int32  _maxAniso[MAX_SAMPLERS];
    bool   _samplerUsed[MAX_SAMPLERS];

    RSFixedFunc _fixed;
    bool        _fixedFuncInit;

    Core::Map<VertexFormatDesc, IVertexFormatPtr>   _vtxFormats;

    Ptr<IVertexFormat>      _pCurVertexFormat;
    Ptr<IVertexShader>      _pCurVertexShader;
    Ptr<IPixelShader>       _pCurPixelShader;

    bool   _clipPlaneEnabled[D3DMAXUSERCLIPPLANES];
};
//-----------------------------------------------------------------------------
} // namespace Gfx

#endif /* DEVICEDX9_H_ */
