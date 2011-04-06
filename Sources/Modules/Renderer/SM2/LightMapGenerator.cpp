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
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Camera.h>
#include <Renderer/SM2/LightMapGenerator.h>
#include <Renderer/SM2/SceneSM2.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
LightMapGenerationInfos::PixelInfos::PixelInfos() : used(false)
{}
//-----------------------------------------------------------------------------
LightMapCoordsGenerator::LightMapCoordsGenerator(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<PackedVerticesFormat> & pFormat, const Ptr<ScreenQuad> & pScreenQuad) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pFormat(pFormat),
    _pScreenQuad(pScreenQuad)
{
}
//-----------------------------------------------------------------------------
bool LightMapCoordsGenerator::initialise()
{
    bool result = true;

    try
    {
        // Pixel shaders - Generate texcoords
        {
            Core::List<Gfx::ShaderMacro> macros;

            if(_pFormat->packedNormals())
                macros.push_back(Gfx::ShaderMacro(L"PACKED_NORMAL_FLAG", L"1"));

            _pVShaderGen = _pShaderLib->getVShader(L"generate-lightmap-coords.vsh", Gfx::VS_V3_0, L"vs_main", macros);
            _pPShaderGen = _pShaderLib->getPShader(L"generate-lightmap-coords.psh", Gfx::PS_V3_0, L"ps_main", macros);

            _idWorldGen = _pVShaderGen->getConstantTable()->getConstantIndex(L"World");
            _idTextureSizeGen = _pVShaderGen->getConstantTable()->getConstantIndex(L"TextureSize");
            _idIsTextureBorderOnGen = _pVShaderGen->getConstantTable()->getConstantIndex(L"IsTextureBorderOn");

            _idSamplerNormalGen = _pPShaderGen->getConstantTable()->getConstantIndex(L"SamplerNormal");
            _idIsNormalMapDXT5Gen = _pPShaderGen->getConstantTable()->getConstantIndex(L"IsNormalMapDXT5");
            _idIsNormalMapOnGen = _pPShaderGen->getConstantTable()->getConstantIndex(L"IsNormalMapOn");

            Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
            Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
            Gfx::RSBlendDesc        blend;

            _stateGen = _pDevice->createState(raster, depth, blend);

            Gfx::RSSamplerDesc samplerNormal;
            samplerNormal.filter = Gfx::FILTER_ANISOTROPIC;
            samplerNormal.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
            samplerNormal.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
            samplerNormal.maxAnisotropy = 8;
            samplerNormal.isSRGB = false;

            _stateGen.ptSampler[_pPShaderGen->getConstantTable()->getSamplerId(_idSamplerNormalGen)] = _pDevice->createState(samplerNormal);

            _pFormatGen = _pFormat->getVertexFormat(_pDevice, _pVShaderGen, VD_STATIC_G_B);
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
void LightMapCoordsGenerator::generateFromModel(LightMapGenerationInfos & lm, const Core::Matrix4f & world, const IPackedStaticVertices & vertices, int32 width, int32 height, bool superSample)
{
    // Surfaces
   
    int32 sizeMul   = (lm.internalTextureBorder || !superSample) ? 1 : std::max(1, std::min(2048 / width, 2048 / height));
    int32 srcWidth  = sizeMul * width;
    int32 srcHeight = sizeMul * height;

    Gfx::Texture2DDesc texDesc;
    texDesc.width = srcWidth;
    texDesc.height = srcHeight;
    texDesc.mipLevels = 1;
    texDesc.format = Gfx::TF_A32B32G32R32F;
    texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
    texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
    texDesc.bind = Gfx::TB_RENDER_TARGET;
    texDesc.autoGenMipmap = false;

    Gfx::RenderTargetViewDesc rtDesc;
    rtDesc.texture2D.mipSlice = 0;

    Ptr<Gfx::ITexture2D> pTexPosition = _pDevice->createTexture2D(texDesc);
    Ptr<Gfx::ITexture2D> pTexNormal = _pDevice->createTexture2D(texDesc);

    Ptr<Gfx::IRenderTargetView> pViewPosition = _pDevice->createRenderTargetView(pTexPosition, rtDesc);
    Ptr<Gfx::IRenderTargetView> pViewNormal = _pDevice->createRenderTargetView(pTexNormal, rtDesc);

    _pDevice->beginDraw();

    _pDevice->setState(_stateGen);
    _pDevice->setVertexFormat(_pFormatGen);
    _pDevice->setVertexShader(_pVShaderGen);
    _pDevice->setPixelShader(_pPShaderGen);

    _pVShaderGen->getConstantTable()->setConstant(_idWorldGen, world);
    _pVShaderGen->getConstantTable()->setConstant(_idTextureSizeGen, Core::Vector2f(float(srcWidth), float(srcHeight)));
    _pVShaderGen->getConstantTable()->setConstant(_idIsTextureBorderOnGen, lm.internalTextureBorder);

    if(lm.pNormalMap != null)
    {
        _pPShaderGen->getConstantTable()->setSampler2D(_idSamplerNormalGen, lm.pNormalMap->getResource());
        _pPShaderGen->getConstantTable()->setConstant(_idIsNormalMapOnGen, true);
        _pPShaderGen->getConstantTable()->setConstant(_idIsNormalMapDXT5Gen, (lm.pNormalMap->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5));
    }
    else
    {
        _pPShaderGen->getConstantTable()->setSampler2D(_idSamplerNormalGen, null);
        _pPShaderGen->getConstantTable()->setConstant(_idIsNormalMapOnGen, false);
        _pPShaderGen->getConstantTable()->setConstant(_idIsNormalMapDXT5Gen, false);
    }

    Gfx::IRenderTargetViewPtr targets[2];

    targets[0] = pViewPosition;
    targets[1] = pViewNormal;

    _pDevice->setRenderTargets(2, targets, null);
    _pDevice->clearCurrentRenderTargetView(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0, Gfx::CLEAR_COLOR_BUFFER);

    vertices.bindData(_pDevice, VD_STATIC_G_B);
    vertices.sendData(_pDevice, VD_STATIC_G_B);

    targets[0] = null;
    targets[1] = null;

    resize(pTexPosition, pViewPosition, srcWidth, srcHeight, width, height);
    resize(pTexNormal, pViewNormal, srcWidth, srcHeight, width, height);


    Ptr<Core::Bitmap> pBitmapPos  = _pDevice->toBitmap(pViewPosition);
    Ptr<Core::Bitmap> pBitmapNorm = _pDevice->toBitmap(pViewNormal);

    _pDevice->endDraw();

    if(lm.debugPositionBitmap)
        lm.pPositionBitmap = pBitmapPos;

    if(lm.debugNormalBitmap)
        lm.pNormalBitmap = pBitmapNorm;

    lm.pixels.resize(height);

    float * pPosition = reinterpret_cast<float *>(pBitmapPos->getData());
    float * pNormal = reinterpret_cast<float *>(pBitmapNorm->getData());

    for(int32 y=0; y < height; y++)
    {
        Core::List<LightMapGenerationInfos::PixelInfos> & line = lm.pixels[y];
        line.resize(width);

        for(int32 x=0; x < width; x++)
        {
            LightMapGenerationInfos::PixelInfos & pixel = line[x];

            pixel.used = (pPosition[3] > 0.0f);

            pixel.position.x = pPosition[0];
            pixel.position.y = pPosition[1];
            pixel.position.z = pPosition[2];

            pixel.normal.x = pNormal[0];
            pixel.normal.y = pNormal[1];
            pixel.normal.z = pNormal[2];


            pPosition += 4;
            pNormal += 4;
        }
    }

    // Bords
    for(int32 iBorder=0; iBorder < 1; iBorder++)
    {
        Core::List<Core::List<LightMapGenerationInfos::PixelInfos> > pixels = lm.pixels;

        for(int32 y1=0; y1 < height; y1++)
        {
            int32 y0 = std::max(0, y1 - 1);
            int32 y2 = std::min(height - 1, y1 + 1);

            Core::List<LightMapGenerationInfos::PixelInfos> & line = pixels[y1];

            const Core::List<LightMapGenerationInfos::PixelInfos> & line0 = lm.pixels[y0];
            const Core::List<LightMapGenerationInfos::PixelInfos> & line1 = lm.pixels[y1];
            const Core::List<LightMapGenerationInfos::PixelInfos> & line2 = lm.pixels[y2];

            for(int32 x1=0; x1 < width; x1++)
            {
                int32 x0 = std::max(0, x1 - 1);
                int32 x2 = std::min(width - 1, x1 + 1);

                LightMapGenerationInfos::PixelInfos & pix = line[x1];

                const LightMapGenerationInfos::PixelInfos & p00 = line0[x0];
                const LightMapGenerationInfos::PixelInfos & p10 = line0[x1];
                const LightMapGenerationInfos::PixelInfos & p20 = line0[x2];

                const LightMapGenerationInfos::PixelInfos & p01 = line1[x0];
                const LightMapGenerationInfos::PixelInfos & p11 = line1[x1];
                const LightMapGenerationInfos::PixelInfos & p21 = line1[x2];

                const LightMapGenerationInfos::PixelInfos & p02 = line2[x0];
                const LightMapGenerationInfos::PixelInfos & p12 = line2[x1];
                const LightMapGenerationInfos::PixelInfos & p22 = line2[x2];

                if(!pix.used)
                {
                    if     (p10.used) pix = p10; // up
                    else if(p12.used) pix = p12; // down
                    else if(p01.used) pix = p01; // left
                    else if(p21.used) pix = p21; // right
                    else if(p00.used) pix = p00; // up left
                    else if(p20.used) pix = p20; // up right
                    else if(p02.used) pix = p02; // down left
                    else if(p22.used) pix = p22; // down right
                }
            }
        }

        lm.pixels.swap(pixels);
    }

    targets[0] = null;
    targets[1] = null;
    pViewPosition = null;
    pViewNormal = null;
}
//-----------------------------------------------------------------------------
void LightMapCoordsGenerator::resize(Ptr<Gfx::ITexture2D> & pTex, Ptr<Gfx::IRenderTargetView> & pView, int32 srcWidth, int32 srcHeight, int32 dstWidth, int32 dstHeight)
{
    if(srcWidth == dstWidth && srcHeight == dstHeight)
        return;

    int32 halfWidth  = std::max(dstWidth,  srcWidth  / 2);
    int32 halfHeight = std::max(dstHeight, srcHeight / 2);

    Gfx::Texture2DDesc texDesc;
    texDesc.width = halfWidth;
    texDesc.height = halfHeight;
    texDesc.mipLevels = 1;
    texDesc.format = Gfx::TF_A32B32G32R32F;
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
