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
#ifndef IDEVICE_H_
#define IDEVICE_H_

#include <Core/Bitmap.h>
#include <Gfx/IRenderState.h>
#include <Gfx/IShader.h>
#include <Gfx/IVertexBuffer.h>
#include <Gfx/IIndexBuffer.h>
#include <Gfx/ISwapChain.h>
#include <Gfx/IRenderTarget.h>
#include <Gfx/IDepthStencil.h>
#include <Gfx/ITexture.h>
#include <Gfx/IFont.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
class LM_API_GFX IDevice
{
protected:
    IDevice();

public:
    virtual ~IDevice();

    ////////////////////////////////
    //  Render State
    ////////////////////////////////
    virtual IRSRasterizerPtr createState(const RSRasterizerDesc& desc) = 0;
    virtual IRSBlendPtr createState(const RSBlendDesc& desc) = 0;
    virtual IRSDepthStencilPtr createState(const RSDepthStencilDesc& desc) = 0;
    virtual IRSSamplerPtr createState(const RSSamplerDesc& desc) = 0;

    GlobalRenderState createState(const RSRasterizerDesc & raster, const RSDepthStencilDesc& depth, const RSBlendDesc& blend);

    virtual void setState(const IRSRasterizerPtr& pState) = 0;
    virtual void setState(const IRSBlendPtr& pState) = 0;
    virtual void setState(const IRSDepthStencilPtr& pState) = 0;
    virtual void setState(int32 id, const IRSSamplerPtr& pState) = 0;

    virtual void setState(const GlobalRenderState & state);

    ////////////////////////////////
    //  Geometrie
    ////////////////////////////////
    virtual IVertexFormatPtr createVertexFormat(const VertexFormatDesc & desc, const IVertexShaderPtr & pShader) = 0;
    virtual IVertexBufferPtr createVertexBuffer(const VertexBufferDesc& desc, const SubresourceData* pInitData = NULL) = 0;
    virtual IIndexBufferPtr createIndexBuffer(const IndexBufferDesc& desc, const SubresourceData* pInitData = NULL) = 0;

    virtual void setVertexFormat(const IVertexFormatPtr & pFormat) = 0;
    virtual void setVertexBuffer(int32 stream, const IVertexBufferPtr & pBuffer, int32 stride, int32 offset) = 0;
    virtual void setVertexBuffers(int32 nbBuffers, IVertexBufferPtr* tabVB, int32* tabStrides, int32* tabOffset) = 0;
    virtual void setIndexBuffer(const IIndexBufferPtr & pBuffer) = 0;

    ////////////////////////////////
    //  Shader
    ////////////////////////////////
    virtual IPixelShaderBinObjectPtr compilePShader(const String& code, EPixelShaderVersion version,
                                                    const String& entryPoint, const Ptr<IShaderIncluder> & pIncluder);
    virtual IPixelShaderBinObjectPtr compilePShader(const String& code, EPixelShaderVersion version,
                                                    const String& entryPoint, const Core::List<ShaderMacro> & macros,
                                                    const Ptr<IShaderIncluder> & pIncluder) = 0;

    virtual IVertexShaderBinObjectPtr compileVShader(const String& code, EVertexShaderVersion version,
                                                     const String& entryPoint, const Ptr<IShaderIncluder> & pIncluder);
    virtual IVertexShaderBinObjectPtr compileVShader(const String& code, EVertexShaderVersion version, const String& entryPoint,
                                                     const Core::List<ShaderMacro> & macros,
                                                     const Ptr<IShaderIncluder> & pIncluder) = 0;

    virtual IVertexShaderBinObjectPtr readPrecompiledVShader(Core::InputStream & inputStream) = 0;
    virtual IPixelShaderBinObjectPtr readPrecompiledPShader(Core::InputStream & inputStream) = 0;

    virtual void writePrecompiledVShader(Core::OutputStream & outputStream, const IVertexShaderBinObjectPtr & pBinObject) = 0;
    virtual void writePrecompiledPShader(Core::OutputStream & outputStream, const IPixelShaderBinObjectPtr & pBinObject) = 0;

    virtual IVertexShaderPtr createVertexShader(const IVertexShaderBinObjectPtr & shader) = 0;
    virtual IPixelShaderPtr createPixelShader(const IPixelShaderBinObjectPtr & shader) = 0;

    virtual void setPixelShader(const IPixelShaderPtr & shader) = 0;
    virtual void setVertexShader(const IVertexShaderPtr & shader) = 0;

    virtual void PSSetShaderResources(const int32 startSlot, const int32& nbViews,
                                      IShaderResourceViewPtr* pShaderResourceViews) = 0;
    virtual void VSSetShaderResources(const int32 startSlot, const int32& nbViews,
                                      IShaderResourceViewPtr* pShaderResourceViews) = 0;

    virtual void setFixedFuncPipe(const RSFixedFunc & fixed) = 0;

    ////////////////////////////////
    //  SwapChain
    ////////////////////////////////
    virtual EDeviceState getDeviceState() = 0;

    virtual ISwapChainPtr createSwapChain(const SwapChainDesc& desc) = 0;
    virtual ISwapChainPtr reset(const SwapChainDesc & desc) = 0;

    ////////////////////////////////
    //  RenderTarget
    ////////////////////////////////
    virtual IRenderTargetViewPtr createRenderTargetView(ISwapChainPtr pSwapChain, const RenderTargetViewDesc& desc) = 0;
    virtual IRenderTargetViewPtr createRenderTargetView(ITexture2DPtr tex2D, const RenderTargetViewDesc& desc) = 0;
    virtual IRenderTargetViewPtr createRenderTargetView(ITextureCubePtr texCube, const RenderTargetViewDesc& desc) = 0;
    virtual IRenderTargetViewPtr createRenderTargetView(const RenderTargetViewDesc& desc) = 0;

    virtual void setRenderTarget(IRenderTargetViewPtr rtv, IDepthStencilViewPtr dsv) = 0;
    virtual void setRenderTargets(const int32 nbRtv, IRenderTargetViewPtr* tabRtv, IDepthStencilViewPtr dsv) = 0;
    virtual void getRenderTarget(IRenderTargetViewPtr& rtv, IDepthStencilViewPtr& dsv) = 0;

    virtual void clearCurrentRenderTargetView(const RGBA& color, float depth = 1.0f, int32 stencil = 0, int32 flags = CLEAR_ALL_BUFFERS) = 0;

    virtual void copyRenderTargets(const Ptr<IRenderTargetView> & pSrc, const Ptr<IRenderTargetView> & pDst) = 0;

    ////////////////////////////////
    //  DepthStencil
    ////////////////////////////////
    virtual IDepthStencilViewPtr createDepthStencilView(ITexture2DPtr tex2D, const DepthStencilViewDesc& desc) = 0;

    ////////////////////////////////
    //  Texture
    ////////////////////////////////
    virtual ITexture1DPtr createTexture1D(const Texture1DDesc& desc, const SubresourceData* pInitData = NULL) = 0;
    virtual ITexture2DPtr createTexture2D(const Texture2DDesc& desc, const SubresourceData* pInitData = NULL) = 0;
    virtual ITexture3DPtr createTexture3D(const Texture3DDesc& desc, const SubresourceData* pInitData = NULL) = 0;
    virtual ITextureCubePtr createTextureCube(const TextureCubeDesc& desc, const SubresourceData* pInitData [6] = NULL) = 0;

    virtual void updateTexture(const Ptr<ITexture1D> & pSrc, const Ptr<ITexture1D> & pDst) = 0;
    virtual void updateTexture(const Ptr<ITexture2D> & pSrc, const Ptr<ITexture2D> & pDst) = 0;
    virtual void updateTexture(const Ptr<ITexture3D> & pSrc, const Ptr<ITexture3D> & pDst) = 0;
    virtual void updateTexture(const Ptr<ITextureCube> & pSrc, const Ptr<ITextureCube> & pDst) = 0;

    ////////////////////////////////
    //  Draw
    ////////////////////////////////
    virtual void beginDraw() = 0;
    virtual void endDraw() = 0;

    virtual void drawIndexed(EPrimitiveType primType, int32 usedVertices, int32 startIndex, int32 primCount) = 0;
    virtual void drawIndexed(EPrimitiveType primType, int32 baseVertexIndex, int32 MinIndex, int32 usedVertices, int32 startIndex, int32 primCount) = 0;


    virtual void getCaps(GraphicCapabilities & caps) = 0;
    virtual void setViewport(const Viewport& vp) = 0;

    virtual Ptr<Core::Bitmap> toBitmap(const IRenderTargetViewPtr& pRtv) = 0;
    virtual Ptr<Core::Bitmap> toBitmap(const IDepthStencilViewPtr& pDtv) = 0;

    ////////////////////////////////
    //  Infos
    ////////////////////////////////

    virtual int64 getTotalVideoMem() = 0;
    virtual int64 getAvailableVideoMem() = 0;

    virtual void startStats() = 0;
    virtual void endStats() = 0;
    virtual const GfxStats & getStats() const = 0;

    ////////////////////////////////
    //  HUD
    ////////////////////////////////

    virtual Ptr<IFont> createFont(const FontDesc & desc) = 0;


    ////////////////////////////////
    //  Internal
    ////////////////////////////////

    virtual void tagTextureUnitAsUsed(int32 id) = 0;

};
typedef Core::Ptr<IDevice> IDevicePtr;
//-----------------------------------------------------------------------------
}//namespace Gfx

#endif /* IDEVICE_H_ */
