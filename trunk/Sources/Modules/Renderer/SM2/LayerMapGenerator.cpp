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
#include <Core/Logger.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Camera.h>
#include <Renderer/SM2/LayerMapGenerator.h>
#include <Renderer/SM2/SceneSM2.h>

namespace Renderer
{
const float MAP_TYPE_DIFFUSE      = 0.f;
const float MAP_TYPE_NORMAL       = 1.f;
const float MAP_TYPE_DISPLACEMENT = 2.f;
//-----------------------------------------------------------------------------
LayerMapGenerator::LayerMapGenerator(const Ptr<Gfx::IDevice> & pDevice, 
                                     const Ptr<ShaderLib> & pShaderLib, 
                                     const Ptr<PackedVerticesFormat> & pFormat,
                                     const Ptr<TextureMap> & pDefaultWhiteTexture,
                                     const Ptr<TextureMap> & pDefaultBumpTexture) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pFormat(pFormat),
    _pDefaultWhiteTexture(pDefaultWhiteTexture),
    _pDefaultBumpTexture(pDefaultBumpTexture)
{
}
//-----------------------------------------------------------------------------
bool LayerMapGenerator::initialise()
{
    bool result = true;

    try
    {
        Core::List<Gfx::ShaderMacro> macros;

        if(_pFormat->packedNormals())
            macros.push_back(Gfx::ShaderMacro(L"PACKED_NORMAL_FLAG", L"1"));

        _pVShaderGen = _pShaderLib->getVShader(L"generate-layermap.vsh", Gfx::VS_V3_0, L"vs_main", macros);
        _pPShaderGen = _pShaderLib->getPShader(L"generate-layermap.psh", Gfx::PS_V3_0, L"ps_main", macros);

        Ptr<Gfx::ShaderConstantTable> pVConstTable = _pVShaderGen->getConstantTable();
        Ptr<Gfx::ShaderConstantTable> pPConstTable = _pPShaderGen->getConstantTable();

        _idWorldGen             = _pVShaderGen->getConstantTable()->getConstantIndex(L"World");
        _idTextureSizeGen       = _pVShaderGen->getConstantTable()->getConstantIndex(L"TextureSize");
        _idIsTextureBorderOnGen = _pVShaderGen->getConstantTable()->getConstantIndex(L"IsTextureBorderOn");

        _idMinPos               = pPConstTable->getConstantIndex(L"MinPos");
        _idRangePos             = pPConstTable->getConstantIndex(L"RangePos");
        _idIsNormalMap          = pPConstTable->getConstantIndex(L"IsNormalMap");
        _idIsChain              = pPConstTable->getConstantIndex(L"IsChain");
        _idIsLayer1DetailLayer  = pPConstTable->getConstantIndex(L"IsLayer1DetailLayer");
        _idMatRotationLayer     = pPConstTable->getConstantIndex(L"MatRotationLayer");
        for(int32 ii=0; ii < LAYER_COUNT; ii++)
        {
            _idIsDXT5Norm[ii]      = pPConstTable->getConstantIndexIfExists(L"IsDXT5Norm" + Core::toString(ii));
            _idGenerateU[ii]       = pPConstTable->getConstantIndexIfExists(L"GenerateU" + Core::toString(ii));
            _idGenerateV[ii]       = pPConstTable->getConstantIndexIfExists(L"GenerateV" + Core::toString(ii));
            _idColor[ii]           = pPConstTable->getConstantIndex(L"Color" + Core::toString(ii));
            _idSamplerSource[ii]   = pPConstTable->getConstantIndexIfExists(L"SamplerSource" + Core::toString(ii));
            _idSamplerMask[ii]     = pPConstTable->getConstantIndexIfExists(L"SamplerMask" + Core::toString(ii));
            _idMaskContrast[ii]    = pPConstTable->getConstantIndexIfExists(L"MaskContrast" + Core::toString(ii));
            _idNormalStrength[ii]  = pPConstTable->getConstantIndexIfExists(L"NormalStrength" + Core::toString(ii));
            _idMapType[ii]         = pPConstTable->getConstantIndexIfExists(L"MapType" + Core::toString(ii));
            _idSamplerSize[ii]     = pPConstTable->getConstantIndexIfExists(L"SamplerSize" + Core::toString(ii));
        }

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend;

        _stateGen = _pDevice->createState(raster, depth, blend);
        _pFormatGen = _pFormat->getVertexFormat(_pDevice, _pVShaderGen, VD_STATIC_G_B);

        Gfx::RSSamplerDesc samplerRGB;
        samplerRGB.filter = Gfx::FILTER_ANISOTROPIC;
        samplerRGB.addrModeU = Gfx::TEXTURE_ADDRESS_WRAP;
        samplerRGB.addrModeV = Gfx::TEXTURE_ADDRESS_WRAP;
        samplerRGB.maxAnisotropy = 8;

        Gfx::RSSamplerDesc samplerMask;
        samplerMask.filter = Gfx::FILTER_ANISOTROPIC;
        samplerMask.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
        samplerMask.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
        samplerMask.maxAnisotropy = 8;

        for(int32 ii=0; ii < LAYER_COUNT; ii++)
        {
            if(_idSamplerSource[ii] != Gfx::UNDEFINED_SHADER_CONST)
                _stateGen.ptSampler[pPConstTable->getSamplerId(_idSamplerSource[ii])] = _pDevice->createState(samplerRGB);

            if(_idSamplerMask[ii] != Gfx::UNDEFINED_SHADER_CONST)
                _stateGen.ptSampler[pPConstTable->getSamplerId(_idSamplerMask[ii])] = _pDevice->createState(samplerMask);
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing Layermap generator : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
Ptr<Core::Bitmap> LayerMapGenerator::generateFromModel(
        const LayerMapInfos & lm, 
        const Ptr<IMeshInstance> & pMesh)
{
    LayerMapInfos lm2;
    lm2 = lm;
    lm2.layers.resize(LAYER_COUNT);

    // Surfaces
   
    Gfx::Texture2DDesc texDesc;
    texDesc.width = lm.width;
    texDesc.height = lm.height;
    texDesc.mipLevels = 1;
    texDesc.format = Gfx::TF_X8R8G8B8;
    texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
    texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
    texDesc.bind = Gfx::TB_RENDER_TARGET;
    texDesc.autoGenMipmap = false;

    Gfx::RenderTargetViewDesc rtDesc;
    rtDesc.texture2D.mipSlice = 0;
    Ptr<Gfx::ITexture2D> pTexture1 = _pDevice->createTexture2D(texDesc);
    Ptr<Gfx::IRenderTargetView> pView1 = _pDevice->createRenderTargetView(pTexture1, rtDesc);
    Ptr<ITextureMap> pTexMap1(new TextureMap(pTexture1));

    // GÈnÈration

    internalGenerateFromModel(lm2, pMesh, pView1, false, false);

    if(lm.layers.size() > LAYER_COUNT)
    {
        Ptr<Gfx::ITexture2D> pTexture2 = _pDevice->createTexture2D(texDesc);
        Ptr<Gfx::IRenderTargetView> pView2 = _pDevice->createRenderTargetView(pTexture2, rtDesc);
        Ptr<ITextureMap> pTexMap2(new TextureMap(pTexture2));

        bool flag = true;
        int32 i1 = LAYER_COUNT;

        do
        {
            int32 i2 = std::min(lm.layers.size(), i1 + (LAYER_COUNT - 1));

            LayerMapInfos::LayerInfo layer;
            layer.pSourceTexture = (flag ? pTexMap1 : pTexMap2);

            lm2.layers.clear();
            lm2.layers.push_back(layer);
            lm2.layers.insert(lm2.layers.end(), &lm.layers[i1], &lm.layers[i1] + (i2 - i1));
            lm2.layers.resize(LAYER_COUNT);

            internalGenerateFromModel(lm2, pMesh, (flag ? pView2 : pView1), true, false);

            flag = !flag;
            i1 = i2;

        }while(i1 < lm.layers.size());

        if(!flag)
        {
            pView1 = pView2;
            pTexMap1 = pTexMap2;
        }

        pView2 = null;
        pTexMap2 = null;
    }

    if(lm.useDetailLayer && lm.isNormalMap)
    {
        LayerMapInfos::LayerInfo layer;
        layer.pSourceTexture = pTexMap1;

        lm2.layers.clear();
        lm2.layers.resize(LAYER_COUNT);
        lm2.layers[0] = layer;
        lm2.layers[1] = lm.detailLayer;

        if(lm.layers.empty())
            lm2.layers[1].color.a = 1.0f;

        Ptr<Gfx::ITexture2D> pTexture2 = _pDevice->createTexture2D(texDesc);
        Ptr<Gfx::IRenderTargetView> pView2 = _pDevice->createRenderTargetView(pTexture2, rtDesc);
        Ptr<ITextureMap> pTexMap2(new TextureMap(pTexture2));

        internalGenerateFromModel(lm2, pMesh, pView2, true, true);

        pView1 = pView2;
        pTexMap1 = pTexMap2;
    }

    Ptr<Core::Bitmap> pBitmap(_pDevice->toBitmap(pView1));

    pView1 = null;
    pTexMap1 = null;

    return pBitmap;
}
//-----------------------------------------------------------------------------
void LayerMapGenerator::internalGenerateFromModel(
        const LayerMapInfos & lm, 
        const Ptr<IMeshInstance> & pMesh,
        const Ptr<Gfx::IRenderTargetView> & pView,
        bool isChain,
        bool isLayer1DetailLayer)
{
    _pDevice->setState(_stateGen);
    _pDevice->setVertexFormat(_pFormatGen);
    _pDevice->setVertexShader(_pVShaderGen);
    _pDevice->setPixelShader(_pPShaderGen);

    Ptr<Gfx::ShaderConstantTable> pVConstTable = _pVShaderGen->getConstantTable();
    Ptr<Gfx::ShaderConstantTable> pPConstTable = _pPShaderGen->getConstantTable();

    pVConstTable->setConstant(_idWorldGen, pMesh->getWorldMatrix());
    pVConstTable->setConstant(_idTextureSizeGen, Core::Vector2f(float(lm.width), float(lm.height)));
    pVConstTable->setConstant(_idIsTextureBorderOnGen, true); // Pour l'instant, toujours ‡ true

    Core::AABoxf box(lm.box);
    Core::Vector3f range(box.getSize());
    range.x = 1.0f / range.x;
    range.y = 1.0f / range.y;
    range.z = 1.0f / range.z;

    pPConstTable->setConstant(_idMinPos, box.min);
    pPConstTable->setConstant(_idRangePos, range);
    pPConstTable->setConstant(_idIsNormalMap, lm.isNormalMap);
    pPConstTable->setConstant(_idIsChain, isChain);
    pPConstTable->setConstant(_idIsLayer1DetailLayer, isLayer1DetailLayer);
    
    LM_ASSERT(lm.layers.size() == LAYER_COUNT);

    for(int32 ii=0; ii < LAYER_COUNT; ii++)
    {
        Core::Matrix4f matrixAngleLayer (Core::IDENTITY);
        if (lm.layers[ii].angle != 0.0)
            matrixAngleLayer = Core::Matrix4f  (Core::ROTATION, -lm.layers[ii].angle, Core::ROT_Y_YAW);

        pPConstTable->setConstant(_idMatRotationLayer, matrixAngleLayer);

        pPConstTable->setConstantSafe(_idGenerateU[ii], lm.layers[ii].generateU);
        pPConstTable->setConstantSafe(_idGenerateV[ii], lm.layers[ii].generateV);
        pPConstTable->setConstantSafe(_idColor[ii], lm.layers[ii].color);
        
        if(_idMaskContrast[ii] != Gfx::UNDEFINED_SHADER_CONST)
            pPConstTable->setConstantSafe(_idMaskContrast[ii], lm.layers[ii].maskContrast);

        if(_idNormalStrength[ii] != Gfx::UNDEFINED_SHADER_CONST)
            pPConstTable->setConstantSafe(_idNormalStrength[ii], lm.layers[ii].normalStrength);

        if(_idSamplerSize[ii] != Gfx::UNDEFINED_SHADER_CONST)
        {
            Ptr<Renderer::ITextureMap> pTex;
            if (lm.layers[ii].pSourceTexture == null)   pTex = getDefaultTexture(lm.isNormalMap);
            else                                        pTex = lm.layers[ii].pSourceTexture;

            int32 texWidth  = pTex->getSourceTexture()->getWidth();
            int32 texHeight = pTex->getSourceTexture()->getHeight();
            if (texWidth!=texHeight)    ERR << L"LayerMapGenerator::internalGenerateFromModel : not a square texture for generate Terrain";

            float samplerSize = 1.f/(float)texWidth;
            pPConstTable->setConstantSafe(_idSamplerSize[ii], samplerSize);
        }

        if(_idMapType[ii] != Gfx::UNDEFINED_SHADER_CONST)
        {
            float mapType = MAP_TYPE_DIFFUSE;
            if (lm.isNormalMap)
            {
                if (lm.layers[ii].useDisplacementMap==false)    mapType = MAP_TYPE_NORMAL;
                else                                            mapType = MAP_TYPE_DISPLACEMENT;
            }
            pPConstTable->setConstantSafe(_idMapType[ii], mapType);
        }

        if(_idSamplerSource[ii] != Gfx::UNDEFINED_SHADER_CONST)
        {
            if(lm.layers[ii].pSourceTexture == null)
            {
                pPConstTable->setSampler2D(_idSamplerSource[ii], getDefaultTexture(lm.isNormalMap)->getResource());
                pPConstTable->setConstant(_idIsDXT5Norm[ii], false);
            }
            else
            {
                bool isDXT5Norm = (lm.isNormalMap && lm.layers[ii].pSourceTexture->getSourceTexture() != null &&
                    lm.layers[ii].pSourceTexture->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5);

                pPConstTable->setConstant(_idIsDXT5Norm[ii], isDXT5Norm);
                pPConstTable->setSampler2D(_idSamplerSource[ii], LM_DEBUG_PTR_CAST<TextureMap>(lm.layers[ii].pSourceTexture)->getResource());
            }
        }

        if(_idSamplerMask[ii] != Gfx::UNDEFINED_SHADER_CONST)
        {
            if(lm.layers[ii].pMaskTexture == null)
                pPConstTable->setSampler2D(_idSamplerMask[ii], getDefaultTexture(lm.isNormalMap)->getResource());
            else
                pPConstTable->setSampler2D(_idSamplerMask[ii], LM_DEBUG_PTR_CAST<TextureMap>(lm.layers[ii].pMaskTexture)->getResource());
        }
    }

    _pDevice->setRenderTarget(pView, null);
    _pDevice->clearCurrentRenderTargetView(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0, Gfx::CLEAR_COLOR_BUFFER);

    Ptr<Mesh> pInstance = LM_DEBUG_PTR_CAST<Mesh>(pMesh->getMesh());
    const IPackedStaticVertices & vertices = pInstance->getVertices();

    _pDevice->beginDraw();

    vertices.bindData(_pDevice, VD_STATIC_G_B);
    vertices.sendData(_pDevice, VD_STATIC_G_B);

    _pDevice->endDraw();
}
//-----------------------------------------------------------------------------
Ptr<TextureMap> LayerMapGenerator::getDefaultTexture(bool isNormal)
{
    if(isNormal)
        return _pDefaultBumpTexture;
    else
        return _pDefaultWhiteTexture;
}
//-----------------------------------------------------------------------------
}
