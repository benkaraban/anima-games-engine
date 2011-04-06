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
#include <Core/Logger.h>
#include <Core/TGA.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Camera.h>
#include <Renderer/SM2/OcclusionMapGenerator.h>
#include <Renderer/SM2/SceneSM2.h>

namespace Renderer
{

//-----------------------------------------------------------------------------
OcclusionMapGenerator::OcclusionMapGenerator(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<PackedVerticesFormat> & pFormat, const Ptr<ScreenQuad> & pScreenQuad) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pFormat(pFormat),
    _pScreenQuad(pScreenQuad)
{
}
//-----------------------------------------------------------------------------
bool OcclusionMapGenerator::initialise()
{
    bool result = true;

    try
    {
        // Pixel shaders - Generate texcoords
        {
            Core::List<Gfx::ShaderMacro> macros;

            if(_pFormat->packedNormals())
                macros.push_back(Gfx::ShaderMacro(L"PACKED_NORMAL_FLAG", L"1"));

            _pVShaderGen = _pShaderLib->getVShader(L"generate-occlusionmap.vsh", Gfx::VS_V3_0, L"vs_main", macros);
            _pPShaderGen = _pShaderLib->getPShader(L"generate-occlusionmap.psh", Gfx::PS_V3_0, L"ps_main", macros);

            _idTextureSizeGen = _pVShaderGen->getConstantTable()->getConstantIndex(L"TextureSize");
            _idIsTextureBorderOnGen = _pVShaderGen->getConstantTable()->getConstantIndex(L"IsTextureBorderOn");

            Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
            Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
            Gfx::RSBlendDesc        blend;

            _stateGen = _pDevice->createState(raster, depth, blend);
            _pFormatGen = _pFormat->getVertexFormat(_pDevice, _pVShaderGen, VD_SKIN_G_B);
        }

        // Pixel shaders - Build mipmaps
        {
            Core::List<Gfx::ShaderMacro> macros;

            _pPShaderMip = _pShaderLib->getPShader(L"build-mipmap.psh", Gfx::PS_V3_0, L"ps_main", macros);

            _idSourceSizeMip = _pPShaderMip->getConstantTable()->getConstantIndex(L"SourceSize");
            _idSamplerImageMip = _pPShaderMip->getConstantTable()->getConstantIndex(L"SamplerImage");
            _idIsAlphaWeightedMip = _pPShaderMip->getConstantTable()->getConstantIndex(L"IsAlphaWeighted");

            Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
            Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
            Gfx::RSBlendDesc        blend;

            _stateMip = _pDevice->createState(raster, depth, blend);
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing Occlusion generator : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
Ptr<Core::Bitmap> OcclusionMapGenerator::generateOcclusionMap(const OcclusionMapInfos & om, const Ptr<Assets::VerticeSet> & pVSet)
{
    Ptr<IPackedSkinnedVertices> pPackedVertices = _pFormat->packSkinnedVerts(_pDevice, pVSet);

    // Surfaces
   
    int32 sizeMul   = (om.internalTextureBorder || !om.superSampleUV) ? 1 : std::max(1, std::min(2048 / om.width, 2048 / om.height));
    int32 srcWidth  = sizeMul * om.width;
    int32 srcHeight = sizeMul * om.height;

    Gfx::Texture2DDesc texDesc;
    texDesc.width = srcWidth;
    texDesc.height = srcHeight;
    texDesc.mipLevels = 1;
    texDesc.format = Gfx::TF_A8R8G8B8;
    texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
    texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
    texDesc.bind = Gfx::TB_RENDER_TARGET;
    texDesc.autoGenMipmap = false;

    Gfx::RenderTargetViewDesc rtDesc;
    rtDesc.texture2D.mipSlice = 0;

    Ptr<Gfx::ITexture2D> pTexOcclusion = _pDevice->createTexture2D(texDesc);
    Ptr<Gfx::IRenderTargetView> pViewOcclusion = _pDevice->createRenderTargetView(pTexOcclusion, rtDesc);

    _pDevice->beginDraw();

    _pDevice->setState(_stateGen);
    _pDevice->setVertexFormat(_pFormatGen);
    _pDevice->setVertexShader(_pVShaderGen);
    _pDevice->setPixelShader(_pPShaderGen);

    _pVShaderGen->getConstantTable()->setConstant(_idTextureSizeGen, Core::Vector2f(float(srcWidth), float(srcHeight)));
    _pVShaderGen->getConstantTable()->setConstant(_idIsTextureBorderOnGen, om.internalTextureBorder);

    Gfx::IRenderTargetViewPtr targets[2];

    _pDevice->setRenderTarget(pViewOcclusion, null);
    _pDevice->clearCurrentRenderTargetView(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f), 0.0f, 0, Gfx::CLEAR_COLOR_BUFFER);

    pPackedVertices->bindData(_pDevice, VD_SKIN_G_B);
    pPackedVertices->sendData(_pDevice, VD_SKIN_G_B, 0);

    targets[0] = null;
    targets[1] = null;

    resize(pTexOcclusion, pViewOcclusion, srcWidth, srcHeight, om.width, om.height);

    Ptr<Core::Bitmap> pBitmapOcclusion = _pDevice->toBitmap(pViewOcclusion);

    _pDevice->endDraw();

    pViewOcclusion = null;

    for(int32 ii=0; ii < 4; ii++)
        pBitmapOcclusion = borderDiffuse(pBitmapOcclusion);

    return pBitmapOcclusion;
}
//-----------------------------------------------------------------------------
void OcclusionMapGenerator::resize(Ptr<Gfx::ITexture2D> & pTex, Ptr<Gfx::IRenderTargetView> & pView, int32 srcWidth, int32 srcHeight, int32 dstWidth, int32 dstHeight)
{
    if(srcWidth == dstWidth && srcHeight == dstHeight)
        return;

    int32 halfWidth  = std::max(dstWidth,  srcWidth  / 2);
    int32 halfHeight = std::max(dstHeight, srcHeight / 2);

    Gfx::Texture2DDesc texDesc;
    texDesc.width = halfWidth;
    texDesc.height = halfHeight;
    texDesc.mipLevels = 1;
    texDesc.format = Gfx::TF_A8R8G8B8;
    texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
    texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
    texDesc.bind = Gfx::TB_RENDER_TARGET;
    texDesc.autoGenMipmap = false;

    Gfx::RenderTargetViewDesc rtDesc;
    rtDesc.texture2D.mipSlice = 0;

    Ptr<Gfx::ITexture2D> pTexHalf = _pDevice->createTexture2D(texDesc);
    Ptr<Gfx::IRenderTargetView> pViewHalf = _pDevice->createRenderTargetView(pTexHalf, rtDesc);

    _pDevice->setState(_stateMip);
    _pDevice->setPixelShader(_pPShaderMip);

    _pPShaderMip->getConstantTable()->setConstant(_idSourceSizeMip, Core::Vector2f(float(srcWidth), float(srcHeight)));
    _pPShaderMip->getConstantTable()->setConstant(_idIsAlphaWeightedMip, true);
    _pPShaderMip->getConstantTable()->setSampler2D(_idSamplerImageMip, pTex->getShaderResourceView());

    _pDevice->setRenderTarget(pViewHalf, null);
    _pDevice->clearCurrentRenderTargetView(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0, Gfx::CLEAR_COLOR_BUFFER);

    _pScreenQuad->send(halfWidth, halfHeight);

    if(halfWidth != dstWidth || halfHeight != dstHeight)
    {
        resize(pTexHalf, pViewHalf, halfWidth, halfHeight, dstWidth, dstHeight);
    }

    std::swap(pTex, pTexHalf);
    std::swap(pView, pViewHalf);

    _pDevice->setRenderTarget(pView, null);
    _pPShaderMip->getConstantTable()->setSampler2D(_idSamplerImageMip, null);

    pViewHalf = null;
    pTexHalf = null;
}
//-----------------------------------------------------------------------------
}
