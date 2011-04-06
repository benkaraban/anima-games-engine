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
#include "GfxType.h"
#include <Core/Logger.h>
namespace Gfx
{
//-----------------------------------------------------------------------------
// TODO : à virer ?
//RSRasterizerDesc RSRasterizerDesc::_default;
//RSBlendDesc RSBlendDesc::_default;
//RSDepthStencilDesc RSDepthStencilDesc::_default;
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
RSRasterizerDesc::RSRasterizerDesc()
{
    cullMode = Gfx::CM_NOCM;
    frontFaceIsCounterClockwise = false;

    fillMode = FM_SOLID;

    depthBias = 0;
    depthBiasMax = 0;
    depthBiasScale = 0;
}
//-----------------------------------------------------------------------------
RSRasterizerDesc::RSRasterizerDesc(Invalid_t inv)
{
    cullMode = (ECullMode)-1;
    frontFaceIsCounterClockwise = false;

    fillMode = (EFillMode)-1;

    depthBias = -1;
    depthBiasMax = -1;
    depthBiasScale = -1;
}
//-----------------------------------------------------------------------------
RSRasterizerDesc::RSRasterizerDesc(ECullMode cullMode, bool frontFaceIsCCW, EFillMode fillMode) :
cullMode(cullMode),
frontFaceIsCounterClockwise(frontFaceIsCCW),
fillMode(fillMode),
depthBias(0),
depthBiasMax(0.0),
depthBiasScale(0.0)
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
RSBlendDesc::RSBlendDesc()
{
    enabled = false;
    src = BM_ONE;
    dst = BM_ZERO;
    op = BO_ADD;
    srcAlpha = BM_ONE;
    dstAlpha = BM_ZERO;
    opAlpha = BO_ADD;

    colorWriteEnabled = true;

    alphaTestEnabled = false;
    alphaTestFunc = COMPARISON_GREATER_EQUAL;
    alphaTestRef = 1;

    alphaCoverageEnabled = false;
    sRGBWriteEnabled = false;
}
//-----------------------------------------------------------------------------
RSBlendDesc::RSBlendDesc(Invalid_t)
{
    enabled = true;
    src = (EBlendMode) -1;
    dst = (EBlendMode) -1;
    op = (EBlendOperation) -1;
    srcAlpha = (EBlendMode) -1;
    dstAlpha = (EBlendMode) -1;
    opAlpha = (EBlendOperation) -1;

    colorWriteEnabled = false;

    alphaTestEnabled = true;
    alphaTestFunc = (EComparisonFunc)-1;
    alphaTestRef = (uint32)-1;

    alphaCoverageEnabled = true;
    sRGBWriteEnabled = true;
}
//-----------------------------------------------------------------------------
RSBlendDesc::RSBlendDesc(EBlendMode src, EBlendOperation op, EBlendMode dst) :
enabled(true),
src(src), dst(dst), op(op),
srcAlpha(src), dstAlpha(dst), opAlpha(op),
colorWriteEnabled(true),
alphaTestEnabled(false),
alphaTestFunc(COMPARISON_GREATER_EQUAL),
alphaTestRef(1),
alphaCoverageEnabled(false),
sRGBWriteEnabled(false)
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
RSDepthStencilDesc::RSDepthStencilDesc()
{
    //depth
    isDepthTestEnabled = false;
    isDepthWriteEnabled = false;
    depthFunction = COMPARISON_LESS;

    //stencil
    isStencilEnabled = false;
    stencilReadMask = DEFAULT_STENCIL_READ_MASK;
    stencilWriteMask = DEFAULT_STENCIL_WRITE_MASK;
    stencilRef=0;

    opStencil.opStencilNOK=KEEP;
    opStencil.opStencilOKDepthNOK=KEEP;
    opStencil.opStencilOKDepthOK=KEEP;
    opStencil.function=COMPARISON_ALWAYS;
}
//-----------------------------------------------------------------------------
RSDepthStencilDesc::RSDepthStencilDesc(Invalid_t)
{
    //depth
    isDepthTestEnabled = true;
    isDepthWriteEnabled = true;
    depthFunction = (EComparisonFunc) -1;

    //stencil
    isStencilEnabled = true;
    stencilReadMask = 0;
    stencilWriteMask = 0;
    stencilRef=-1;

    opStencil.opStencilNOK=(EStencilOperation) -1;
    opStencil.opStencilOKDepthNOK=(EStencilOperation) -1;
    opStencil.opStencilOKDepthOK=(EStencilOperation) -1;
    opStencil.function=(EComparisonFunc) -1;
}
//-----------------------------------------------------------------------------
RSDepthStencilDesc::RSDepthStencilDesc(bool depthTestOn, bool depthWriteOn, EComparisonFunc depthFunc) :
isDepthTestEnabled(depthTestOn),
isDepthWriteEnabled(depthWriteOn),
depthFunction(depthFunc),
isStencilEnabled(false),
stencilReadMask(DEFAULT_STENCIL_READ_MASK),
stencilWriteMask(DEFAULT_STENCIL_WRITE_MASK),
stencilRef(0)
{
    opStencil.opStencilNOK=KEEP;
    opStencil.opStencilOKDepthNOK=KEEP;
    opStencil.opStencilOKDepthOK=KEEP;
    opStencil.function=COMPARISON_ALWAYS;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
RSSamplerDesc::RSSamplerDesc(EFilterType filter, ETextureAddressMode addrU, ETextureAddressMode addrV, int32 maxAniso, bool isSRGB) :
filter(filter),
addrModeU(addrU),
addrModeV(addrV),
addrModeW(TEXTURE_ADDRESS_CLAMP),
maxAnisotropy(maxAniso),
borderColor(0.0f, 0.0f, 0.0f, 0.0f),
isSRGB(isSRGB)
{
}
//-----------------------------------------------------------------------------
RSSamplerDesc::RSSamplerDesc(ETextureAddressMode addr, bool isSRGB) :
    filter(FILTER_MIN_MAG_MIP_POINT),
    addrModeU(addr),
    addrModeV(addr),
    addrModeW(TEXTURE_ADDRESS_CLAMP),
    maxAnisotropy(0),
    borderColor(0.0f, 0.0f, 0.0f, 0.0f),
    isSRGB(isSRGB)
{
}
//-----------------------------------------------------------------------------
RSSamplerDesc::RSSamplerDesc()
{
    filter = FILTER_MIN_MAG_MIP_POINT;
    addrModeU = TEXTURE_ADDRESS_CLAMP;
    addrModeV = TEXTURE_ADDRESS_CLAMP;
    addrModeW = TEXTURE_ADDRESS_CLAMP;
    maxAnisotropy = 4;
    borderColor[0] = 0.f;
    borderColor[1] = 0.f;
    borderColor[2] = 0.f;
    borderColor[3] = 0.f;
    isSRGB = false;
}
//-----------------------------------------------------------------------------
RSSamplerDesc::RSSamplerDesc(Invalid_t)
{
    filter = (EFilterType) -1;
    addrModeU = (ETextureAddressMode) -1;
    addrModeV = (ETextureAddressMode) -1;
    addrModeW = (ETextureAddressMode) -1;
    maxAnisotropy = -1;
    borderColor[0] = 2.f;
    borderColor[1] = 2.f;
    borderColor[2] = 2.f;
    borderColor[3] = 2.f;
    isSRGB = true;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
int32 getSizeOf(EVertexAttributType type)
{
    int32 ret = 0;

    switch(type)
    {
        case VAT_FLOAT1: ret = 1 * sizeof(float); break;
        case VAT_FLOAT2: ret = 2 * sizeof(float); break;
        case VAT_FLOAT3: ret = 3 * sizeof(float); break;
        case VAT_FLOAT4: ret = 4 * sizeof(float); break;
        case VAT_COLOR: ret = 4 * sizeof(byte); break;
        case VAT_UBYTE4: ret = 4 * sizeof(byte); break;
        case VAT_UBYTE4N: ret = 4 * sizeof(byte); break;
        case VAT_SHORT2N: ret = 2 * sizeof(uint16); break;
        case VAT_SHORT4N: ret = 4 * sizeof(uint16); break;
        case VAT_DEC3N: ret = 1 * sizeof(uint32); break;
        case VAT_FLOAT16_2: ret = 2 * sizeof(uint16); break;
        case VAT_FLOAT16_4: ret = 4 * sizeof(uint16); break;
    }
    return ret;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
bool VertexElement::operator < (const VertexElement & ve) const
{
    if(streamId != ve.streamId)
        return streamId < ve.streamId;
    else if(offset != ve.offset)
        return offset < ve.offset;
    else if(type != ve.type)
        return int32(type) < int32(ve.type);
    else if(usage != ve.usage)
        return int32(usage) < int32(ve.usage);
    else
        return usageIndex < ve.usageIndex;
}
//-----------------------------------------------------------------------------
bool VertexElement::operator != (const VertexElement & ve) const
{
    return streamId != ve.streamId ||
        offset != ve.offset ||
        type != ve.type ||
        usage != ve.usage ||
        usageIndex != ve.usageIndex;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
void VertexFormatDesc::addAttribut(int32 streamId, EVertexAttributType type, EVertexAttributUsage usage, int32 usageIndex)
{
    VertexElement va;

    va.streamId = streamId;
    va.type = type;
    va.usage = usage;
    va.usageIndex = usageIndex;

    va.offset = 0;
    for (int ii=0; ii < getNbAttribut(); ii++)
        if(_attributs[ii].streamId == streamId)
            va.offset += getSizeOf(_attributs[ii].type);

    _attributs.push_back(va);
}
//-----------------------------------------------------------------------------
int32 VertexFormatDesc::getNbAttribut() const
{
    return _attributs.size();
}
//-----------------------------------------------------------------------------
int32 VertexFormatDesc::getVertexSize(int32 streamId) const
{
    int32 nbByte = 0;
    for (int ii=0; ii<getNbAttribut(); ii++)
    if(_attributs[ii].streamId == streamId)
    nbByte += getSizeOf(_attributs[ii].type);

    return nbByte;
}
//-----------------------------------------------------------------------------
void VertexFormatDesc::clear()
{
    _attributs.clear();
}
//-----------------------------------------------------------------------------
bool VertexFormatDesc::operator < (const VertexFormatDesc & vf) const
{
    for(int32 ii=0; ii < std::min(_attributs.size(), vf._attributs.size()); ii++)
    {
        if(_attributs[ii] != vf._attributs[ii])
            return _attributs[ii] < vf._attributs[ii];
    }

    return _attributs.size() < vf._attributs.size();
}
//-----------------------------------------------------------------------------
const VertexElement& VertexFormatDesc::getAttribut(const int32 indice) const
{
    return _attributs[indice];
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Texture1DDesc::Texture1DDesc() :
    width(1),
    mipLevels(1),
    format(TF_X8R8G8B8),
    usage(BUFFER_USAGE_IMMUTABLE),
    cpuAccess(BCA_ONLY_WRITE),
    bind(TB_BUFFER_DEFAULT),
    autoGenMipmap(false)
{}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Texture2DDesc::Texture2DDesc() :
    width(1),
    height(1),
    mipLevels(1),
    multiSampleType(MSAA_NONE),
    format(TF_X8R8G8B8),
    usage(BUFFER_USAGE_IMMUTABLE),
    cpuAccess(BCA_ONLY_WRITE),
    bind(TB_BUFFER_DEFAULT),
    autoGenMipmap(false)
{}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Texture3DDesc::Texture3DDesc() :
    width(1),
    height(1),
    nbSlice(1),
    mipLevels(1),
    format(TF_X8R8G8B8),
    usage(BUFFER_USAGE_IMMUTABLE),
    cpuAccess(BCA_ONLY_WRITE),
    bind(TB_BUFFER_DEFAULT),
    autoGenMipmap(false)
{}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TextureCubeDesc::TextureCubeDesc() :
    edgeLength(1),
    mipLevels(1),
    format(TF_X8R8G8B8),
    usage(BUFFER_USAGE_IMMUTABLE),
    cpuAccess(BCA_ONLY_WRITE),
    bind(TB_BUFFER_DEFAULT),
    autoGenMipmap(false)
{}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Viewport::Viewport(int32 x, int32 y, int32 w, int32 h, float minDepth, float maxDepth)
: topLeftX(x),
topLeftY(y),
width(w),
height(h),
minDepth(minDepth),
maxDepth(maxDepth)
{
}
//-----------------------------------------------------------------------------

} // namespace Gfx
