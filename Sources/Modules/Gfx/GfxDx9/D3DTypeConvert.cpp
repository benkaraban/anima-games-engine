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
#include <Gfx/GfxDx9/D3DTypeConvert.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
D3DTEXTUREADDRESS D3DTypeConvert(const ETextureAddressMode adressMode, const D3DCAPS9& caps)
{
    D3DTEXTUREADDRESS ret = D3DTADDRESS_WRAP;

    switch (adressMode)
    {
    case TEXTURE_ADDRESS_WRAP:
        ret = D3DTADDRESS_WRAP;
        break;
    case TEXTURE_ADDRESS_MIRROR:
        if (caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR)
            ret = D3DTADDRESS_MIRROR;
        else
            ret = D3DTADDRESS_WRAP;
        break;
    case TEXTURE_ADDRESS_CLAMP:
        ret = D3DTADDRESS_CLAMP;
        break;
    case TEXTURE_ADDRESS_BORDER:
        if (caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
            ret = D3DTADDRESS_BORDER;
        else
            ret = D3DTADDRESS_CLAMP;
        break;
    case TEXTURE_ADDRESS_MIRROR_ONCE:
        ret = D3DTADDRESS_MIRROR;
        break;
    }
    return ret;
}
//-----------------------------------------------------------------------------
D3DSTENCILOP D3DTypeConvert(const EStencilOperation stencilOp)
{
    D3DSTENCILOP ret = D3DSTENCILOP_KEEP;

    switch (stencilOp)
    {
    case KEEP:
        ret = D3DSTENCILOP_KEEP;
        break;
    case ZERO:
        ret = D3DSTENCILOP_ZERO;
        break;
    case REPLACE:
        ret = D3DSTENCILOP_REPLACE;
        break;
    case INCREMENT_SAT:
        ret = D3DSTENCILOP_INCRSAT;
        break;
    case DECREMENT_SAT:
        ret = D3DSTENCILOP_DECRSAT;
        break;
    case INVERT:
        ret = D3DSTENCILOP_INVERT;
        break;
    case INCREMENT:
        ret = D3DSTENCILOP_INCR;
        break;
    case DECREMENT:
        ret = D3DSTENCILOP_DECR;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
D3DCMPFUNC D3DTypeConvert(const EComparisonFunc func)
{
    D3DCMPFUNC ret = D3DCMP_ALWAYS;

    switch (func)
    {
    case COMPARISON_NEVER:
        ret = D3DCMP_NEVER;
        break;
    case COMPARISON_LESS:
        ret = D3DCMP_LESS;
        break;
    case COMPARISON_EQUAL:
        ret = D3DCMP_EQUAL;
        break;
    case COMPARISON_LESS_EQUAL:
        ret = D3DCMP_LESSEQUAL;
        break;
    case COMPARISON_GREATER:
        ret = D3DCMP_GREATER;
        break;
    case COMPARISON_NOT_EQUAL:
        ret = D3DCMP_NOTEQUAL;
        break;
    case COMPARISON_GREATER_EQUAL:
        ret = D3DCMP_GREATEREQUAL;
        break;
    case COMPARISON_ALWAYS:
        ret = D3DCMP_ALWAYS;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
D3DBLENDOP D3DTypeConvert(const EBlendOperation blendOp)
{
    D3DBLENDOP ret = D3DBLENDOP_ADD;

    switch (blendOp)
    {
    case BO_ADD:
        ret = D3DBLENDOP_ADD;
        break;
    case BO_SUBTRACT:
        ret = D3DBLENDOP_SUBTRACT;
        break;
    case BO_REV_SUBTRACT:
        ret = D3DBLENDOP_REVSUBTRACT;
        break;
    case BO_MIN:
        ret = D3DBLENDOP_MIN;
        break;
    case BO_MAX:
        ret = D3DBLENDOP_MAX;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
D3DBLEND D3DTypeConvert(const EBlendMode blendMode)
{
    D3DBLEND ret = D3DBLEND_ONE;

    switch (blendMode)
    {
    case BM_ZERO:
        ret = D3DBLEND_ZERO;
        break;
    case BM_ONE:
        ret = D3DBLEND_ONE;
        break;
    case BM_SRC_COLOR:
        ret = D3DBLEND_SRCCOLOR;
        break;
    case BM_INVERT_SRC_COLOR:
        ret = D3DBLEND_INVSRCCOLOR;
        break;
    case BM_SRC_ALPHA:
        ret = D3DBLEND_SRCALPHA;
        break;
    case BM_INVERT_SRC_ALPHA:
        ret = D3DBLEND_INVSRCALPHA;
        break;
    case BM_DEST_ALPHA:
        ret = D3DBLEND_DESTALPHA;
        break;
    case BM_INVERT_DEST_ALPHA:
        ret = D3DBLEND_INVDESTALPHA;
        break;
    case BM_DEST_COLOR:
        ret = D3DBLEND_DESTCOLOR;
        break;
    case BM_INVERT_DEST_COLOR:
        ret = D3DBLEND_INVDESTCOLOR;
        break;
    case BM_SRC_ALPHA_SAT:
        ret = D3DBLEND_SRCALPHASAT;
        break;
    case BM_BLEND_FACTOR:
        ret = D3DBLEND_BLENDFACTOR;
        break;
    case BM_INVERT_BLEND_FACTOR:
        ret = D3DBLEND_INVBLENDFACTOR;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
D3DCULL D3DTypeConvert(const ECullMode cullMode, const bool frontFaceIsCounterClockwise)
{
    D3DCULL ret = D3DCULL_NONE;
    switch (cullMode)
    {
    case CM_NONE:
        ret = D3DCULL_NONE;
        break;
    case CM_FRONT:
        if (frontFaceIsCounterClockwise == true)
            ret = D3DCULL_CW;
        else
            ret = D3DCULL_CCW;
        break;
    case CM_BACK:
        if (frontFaceIsCounterClockwise == true)
            ret = D3DCULL_CCW;
        else
            ret = D3DCULL_CW;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
D3DFILLMODE D3DTypeConvert(const EFillMode fillMode)
{
    D3DFILLMODE ret = D3DFILL_SOLID;

    switch (fillMode)
    {
    case FM_WIREFRAME:
        ret = D3DFILL_WIREFRAME;
        break;

    case FM_SOLID:
        ret = D3DFILL_SOLID;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
D3DPRIMITIVETYPE D3DTypeConvert(const EPrimitiveType pt)
{
    D3DPRIMITIVETYPE prim = D3DPT_TRIANGLELIST;

    switch (pt)
    {
    case PT_POINTLIST:
        prim = D3DPT_POINTLIST;
        break;
    case PT_LINELIST:
        prim = D3DPT_LINELIST;
        break;
    case PT_LINESTRIP:
        prim = D3DPT_LINESTRIP;
        break;
    case PT_TRIANGLE_LIST:
        prim = D3DPT_TRIANGLELIST;
        break;
    case PT_TRIANGLE_STRIP:
        prim = D3DPT_TRIANGLESTRIP;
        break;
    case PT_TRIANGLE_FAN:
        prim = D3DPT_TRIANGLEFAN;
        break;
    }

    return prim;
}
//-----------------------------------------------------------------------------
D3DDECLTYPE D3DTypeConvert(const EVertexAttributType vat)
{
    D3DDECLTYPE dt;

    switch (vat)
    {
    case VAT_FLOAT1:
        dt = D3DDECLTYPE_FLOAT1;
        break;
    case VAT_FLOAT2:
        dt = D3DDECLTYPE_FLOAT2;
        break;
    case VAT_FLOAT3:
        dt = D3DDECLTYPE_FLOAT3;
        break;
    case VAT_FLOAT4:
        dt = D3DDECLTYPE_FLOAT4;
        break;
    case VAT_COLOR:
        dt = D3DDECLTYPE_D3DCOLOR;
        break;
    case VAT_UBYTE4:
        dt = D3DDECLTYPE_UBYTE4;
        break;
    case VAT_UBYTE4N:
        dt = D3DDECLTYPE_UBYTE4N;
        break;
    case VAT_SHORT2N:
        dt = D3DDECLTYPE_SHORT2N;
        break;
    case VAT_SHORT4N:
        dt = D3DDECLTYPE_SHORT4N;
        break;
    case VAT_DEC3N:
        dt = D3DDECLTYPE_DEC3N;
        break;
    case VAT_FLOAT16_2:
        dt = D3DDECLTYPE_FLOAT16_2;
        break;
    case VAT_FLOAT16_4:
        dt = D3DDECLTYPE_FLOAT16_4;
        break;
    }
    return dt;
}
//-----------------------------------------------------------------------------
D3DDECLUSAGE D3DTypeConvert(const EVertexAttributUsage vau)
{
    D3DDECLUSAGE du;

    switch (vau)
    {
    case VAU_POSITION:
        du = D3DDECLUSAGE_POSITION;
        break;
    case VAU_POSITIONT:
        du = D3DDECLUSAGE_POSITIONT;
        break;
    case VAU_NORMAL:
        du = D3DDECLUSAGE_NORMAL;
        break;
    case VAU_BINORMAL:
        du = D3DDECLUSAGE_BINORMAL;
        break;
    case VAU_TANGENT:
        du = D3DDECLUSAGE_TANGENT;
        break;
    case VAU_COLOR:
        du = D3DDECLUSAGE_COLOR;
        break;
    case VAU_TEXTURE_COORD:
        du = D3DDECLUSAGE_TEXCOORD;
        break;
    case VAU_BLEND_WEIGHTS:
        du = D3DDECLUSAGE_BLENDWEIGHT;
        break;
    case VAU_BLEND_INDICES:
        du = D3DDECLUSAGE_BLENDINDICES;
        break;
    case VAU_UNUSED:
        LM_ASSERT(false);
        break;
    }
    return du;
}
//-----------------------------------------------------------------------------
Core::List<D3DVERTEXELEMENT9> D3DTypeConvert(const VertexFormatDesc desc)
{
    Core::List<D3DVERTEXELEMENT9> vertexElt;
    D3DVERTEXELEMENT9 elt;

    for (int32 ii = 0; ii < desc.getNbAttribut(); ii++)
    {
        const VertexElement& ve = desc.getAttribut(ii);

        if(ve.usage != VAU_UNUSED)
        {
            elt.Stream = ve.streamId;
            elt.Offset = ve.offset;
            elt.Type = D3DTypeConvert(ve.type);
            elt.Method = D3DDECLMETHOD_DEFAULT;
            elt.Usage = D3DTypeConvert(ve.usage);
            elt.UsageIndex = ve.usageIndex;
            vertexElt.push_back(elt);
        }
    }

    D3DVERTEXELEMENT9 elt2 = D3DDECL_END();
    vertexElt.push_back(elt2);

    return vertexElt;
}

//-----------------------------------------------------------------------------
D3DFORMAT D3DTypeConvert(const EIndexBufferFormat ibf)
{
    D3DFORMAT ret = D3DFMT_INDEX32;
    switch (ibf)
    {
    case INDEX_BUFFFER_FMT_16:
        ret = D3DFMT_INDEX16;
        break;
    case INDEX_BUFFFER_FMT_32:
        ret = D3DFMT_INDEX32;
        break;
    }
    return ret;
}
//-----------------------------------------------------------------------------
D3DFORMAT D3DTypeConvert(EDisplayPixelFormat format)
{
    D3DFORMAT result = D3DFMT_X8R8G8B8;
    switch (format)
    {
    case DPF_A2R10G10B10:
        result = D3DFMT_A2R10G10B10;
        break;
    case DPF_A8R8G8B8:
        result = D3DFMT_A8R8G8B8;
        break;
    case DPF_X8R8G8B8:
        result = D3DFMT_X8R8G8B8;
        break;
    case DPF_A1R5G5B5:
        result = D3DFMT_X8R8G8B8;
        break;
    case DPF_X1R5G5B5:
        result = D3DFMT_X1R5G5B5;
        break;
    case DPF_R5G6B5:
        result = D3DFMT_R5G6B5;
        break;
    }
    return result;
}
//-----------------------------------------------------------------------------
DWORD D3DTypeConvert(const ELockType lt, const EBufferUsage& vbu)
{
    DWORD ret = 0;

    switch (lt)
    {
    case LOCK_READ:
        ret += D3DLOCK_READONLY;
        break;
    case LOCK_WRITE:
        ret += 0;
        break;
    case LOCK_READ_WRITE:
        ret += 0;
        break;
    case LOCK_WRITE_DISCARD:
        ret += D3DLOCK_DISCARD;
        break;
    case LOCK_WRITE_NO_OVERWRITE:
        ret += D3DLOCK_NOOVERWRITE;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
D3DVIEWPORT9 D3DTypeConvert(const Viewport& vp)
{
    D3DVIEWPORT9 D3Dvp;

    D3Dvp.X = vp.topLeftX;
    D3Dvp.Y = vp.topLeftY;
    D3Dvp.Width = vp.width;
    D3Dvp.Height = vp.height;
    D3Dvp.MinZ = vp.minDepth;
    D3Dvp.MaxZ = vp.maxDepth;

    return D3Dvp;
}
//-----------------------------------------------------------------------------
D3DFORMAT D3DTypeConvert(const ETextureFormat& tf)
{
    D3DFORMAT D3Dformat;

    switch (tf)
    {
    case TF_R8G8B8A8_UNORM:
        D3Dformat = D3DFMT_A8B8G8R8;
        break;
    case TF_R5G6B5:
        D3Dformat = D3DFMT_R5G6B5;
        break;
    case TF_R16:
        D3Dformat = D3DFMT_L16;
        break;
    case TF_G16R16:
        D3Dformat = D3DFMT_G16R16;
        break;
    case TF_A16B16G16R16:
        D3Dformat = D3DFMT_A16B16G16R16;
        break;
    case TF_R32F:
        D3Dformat = D3DFMT_R32F;
        break;
    case TF_G32R32F:
        D3Dformat = D3DFMT_G32R32F;
        break;
    case TF_A32B32G32R32F:
        D3Dformat = D3DFMT_A32B32G32R32F;
        break;
    case TF_R16F:
        D3Dformat = D3DFMT_R16F;
        break;
    case TF_G16R16F:
        D3Dformat = D3DFMT_G16R16F;
        break;
    case TF_A16B16G16R16F:
        D3Dformat = D3DFMT_A16B16G16R16F;
        break;
    case TF_X8R8G8B8:
        D3Dformat = D3DFMT_X8R8G8B8;
        break;
    case TF_A8R8G8B8:
        D3Dformat = D3DFMT_A8R8G8B8;
        break;
    case TF_D24S8:
        D3Dformat = D3DFMT_D24S8;
        break;
    case TF_DXT1:
        D3Dformat = D3DFMT_DXT1;
        break;
    case TF_DXT2:
        D3Dformat = D3DFMT_DXT2;
        break;
    case TF_DXT3:
        D3Dformat = D3DFMT_DXT3;
        break;
    case TF_DXT4:
        D3Dformat = D3DFMT_DXT4;
        break;
    case TF_DXT5:
        D3Dformat = D3DFMT_DXT5;
        break;
    }

    return D3Dformat;
}


//ATI : http://developer.amd.com/media/gpu_assets/Order_Matters_in_Resource_Creation.pdf
//ATI : http://ati.amd.com/developer/dx9/ATI-DX9_Optimization.pdf
//NVIDIA : http://www.nvidia.co.jp/object/General_FAQ.html
//NVIDIA : http://http.download.nvidia.com/developer/presentations/GDC_2004/GDC2004_PracticalPerformanceAnalysis.pdf

//pour index buffer et vertex buffer
void D3DTypeConvert(const EBufferUsage usage, EBufferCpuAccess cpuAccess, D3DPOOL& D3Dpool, DWORD& D3DUsage)
{
    switch (usage)
    {
    case BUFFER_USAGE_IMMUTABLE:  //cette zone memoire est prevu pour etre ecrite une fois et n'etre jamais lu. le flag EBufferCpuAccess n'est pas pris en compte
        D3Dpool = D3DPOOL_DEFAULT;
        D3DUsage = D3DUSAGE_WRITEONLY;
        break;

    case BUFFER_USAGE_DEFAULT:
        switch (cpuAccess)
        {
        case BCA_NO_ACCESS:
            D3Dpool = D3DPOOL_DEFAULT;
            D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
        break;
        case BCA_ONLY_WRITE:
            D3Dpool = D3DPOOL_DEFAULT;
            D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
        break;
        case BCA_ONLY_READ:
            D3Dpool = D3DPOOL_DEFAULT;
            D3DUsage = D3DUSAGE_DYNAMIC;
        break;
        case BCA_READ_AND_WRITE:
            D3Dpool = D3DPOOL_DEFAULT;
            D3DUsage = D3DUSAGE_DYNAMIC;
        break;
        }
        break;

    case BUFFER_USAGE_DYNAMIC:
        switch (cpuAccess)
        {
        case BCA_NO_ACCESS:
            D3Dpool = D3DPOOL_MANAGED;
            D3DUsage = D3DUSAGE_WRITEONLY;
        break;
        case BCA_ONLY_WRITE:
            D3Dpool = D3DPOOL_MANAGED;
            D3DUsage = D3DUSAGE_WRITEONLY;
        break;
        case BCA_ONLY_READ:
            D3Dpool = D3DPOOL_MANAGED;
            D3DUsage = 0;
        break;
        case BCA_READ_AND_WRITE:
            D3Dpool = D3DPOOL_MANAGED;
            D3DUsage = 0;
        break;
        }
        break;
    }
}
//-----------------------------------------------------------------------------
//pour texture
void D3DTypeConvert(const ETextureBind& bind, const EBufferUsage& usage, EBufferCpuAccess cpuAccess, D3DPOOL& D3Dpool, DWORD& D3DUsage)
{
    switch (bind)
    {
    case TB_RENDER_TARGET:
        D3Dpool = D3DPOOL_DEFAULT;
        D3DUsage = D3DUSAGE_RENDERTARGET;
        break;
    case TB_DEPTH_STENCIL_SURFACE:
        D3Dpool = D3DPOOL_DEFAULT;
        D3DUsage = D3DUSAGE_DEPTHSTENCIL;
        break;
    case TB_DEPTH_STENCIL_TEXTURE:
        D3Dpool = D3DPOOL_DEFAULT;
        D3DUsage = D3DUSAGE_DEPTHSTENCIL;
        break;
    case TB_BUFFER_DEFAULT:
        switch (usage)
        {
        //----------------------------
        case BUFFER_USAGE_IMMUTABLE:
            switch (cpuAccess)
            {
            case BCA_NO_ACCESS:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = 0;
            break;
            case BCA_ONLY_WRITE:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = 0;
            break;
            case BCA_ONLY_READ:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = 0;
            break;
            case BCA_READ_AND_WRITE:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = 0;
            break;
            }
            break;
        //------------------------------
        case BUFFER_USAGE_DEFAULT:
            switch (cpuAccess)
            {
            case BCA_NO_ACCESS:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = D3DUSAGE_DYNAMIC;
            break;
            case BCA_ONLY_WRITE:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = D3DUSAGE_DYNAMIC;
            break;
            case BCA_ONLY_READ:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = D3DUSAGE_DYNAMIC;
            break;
            case BCA_READ_AND_WRITE:
                D3Dpool = D3DPOOL_DEFAULT;
                D3DUsage = D3DUSAGE_DYNAMIC;
            break;
            }
            break;
        //------------------------------
        case BUFFER_USAGE_DYNAMIC:
            switch (cpuAccess)
            {
            case BCA_NO_ACCESS:
                D3Dpool = D3DPOOL_MANAGED;
                D3DUsage = 0;
            break;
            case BCA_ONLY_WRITE:
                D3Dpool = D3DPOOL_MANAGED;
                D3DUsage = 0;
            break;
            case BCA_ONLY_READ:
                D3Dpool = D3DPOOL_MANAGED;
                D3DUsage = 0;
            break;
            case BCA_READ_AND_WRITE:
                D3Dpool = D3DPOOL_SYSTEMMEM;
                D3DUsage = 0;
            break;
            }
            break;
        }
        break;
    }
}

//-----------------------------------------------------------------------------
//============================================================================
Core::EBitmapFormat D3DTypeConvert(const D3DFORMAT D3Dformat)
{
    switch (D3Dformat)
    {
    case D3DFMT_R8G8B8:     return Core::RGB_U8;
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:   return Core::ARGB_U8;
    case D3DFMT_A8:         return Core::GRAY_U8;
    case D3DFMT_R32F:       return Core::GRAY_F32;
    case D3DFMT_A32B32G32R32F: return Core::ARGB_F32;

    case D3DFMT_UNKNOWN: 
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_R3G3B2:
    case D3DFMT_A8R3G3B2:
    case D3DFMT_X4R4G4B4:
    case D3DFMT_A2B10G10R10:
    case D3DFMT_A8B8G8R8:
    case D3DFMT_X8B8G8R8:
    case D3DFMT_G16R16:
    case D3DFMT_A2R10G10B10:
    case D3DFMT_A16B16G16R16:
    case D3DFMT_A8P8:
    case D3DFMT_P8:
    case D3DFMT_L8:
    case D3DFMT_A8L8:
    case D3DFMT_A4L4:
    case D3DFMT_V8U8:
    case D3DFMT_L6V5U5:
    case D3DFMT_X8L8V8U8:
    case D3DFMT_Q8W8V8U8:
    case D3DFMT_V16U16:
    case D3DFMT_A2W10V10U10:
    case D3DFMT_UYVY:
    case D3DFMT_R8G8_B8G8:
    case D3DFMT_YUY2:
    case D3DFMT_G8R8_G8B8:
    case D3DFMT_DXT1:
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D32:
    case D3DFMT_D15S1:
    case D3DFMT_D24S8:
    case D3DFMT_D24X8:
    case D3DFMT_D24X4S4:
    case D3DFMT_D16:
    case D3DFMT_D32F_LOCKABLE:
    case D3DFMT_D24FS8:
    case D3DFMT_L16:
    case D3DFMT_VERTEXDATA:
    case D3DFMT_INDEX16:
    case D3DFMT_INDEX32:
    case D3DFMT_Q16W16V16U16:
    case D3DFMT_MULTI2_ARGB8:
    case D3DFMT_R16F:
    case D3DFMT_G16R16F:
    case D3DFMT_A16B16G16R16F:
    case D3DFMT_G32R32F:
    case D3DFMT_CxV8U8:
    case D3DFMT_FORCE_DWORD:
    case D3DFMT_D32_LOCKABLE:
    case D3DFMT_S8_LOCKABLE:
    case D3DFMT_A1:
    case D3DFMT_BINARYBUFFER:
        throw Core::Exception(L"Unsupported format cvt ");
        break;
    }

    return Core::ARGB_U8;
}
//-----------------------------------------------------------------------------
D3DCUBEMAP_FACES D3DTypeConvert(const ECubeMapFace& cubeMapFace)
{
    D3DCUBEMAP_FACES D3DCubeMapFace;

    switch (cubeMapFace)
    {
         case CUBE_FACE_POSITIVE_X:
            D3DCubeMapFace = D3DCUBEMAP_FACE_POSITIVE_X;
         break;
         case CUBE_FACE_NEGATIVE_X:
            D3DCubeMapFace = D3DCUBEMAP_FACE_NEGATIVE_X;
         break;
         case CUBE_FACE_POSITIVE_Y:
            D3DCubeMapFace = D3DCUBEMAP_FACE_POSITIVE_Y;
         break;
         case CUBE_FACE_NEGATIVE_Y:
            D3DCubeMapFace = D3DCUBEMAP_FACE_NEGATIVE_Y;
         break;
         case CUBE_FACE_POSITIVE_Z:
            D3DCubeMapFace = D3DCUBEMAP_FACE_POSITIVE_Z;
         break;
         case CUBE_FACE_NEGATIVE_Z:
            D3DCubeMapFace = D3DCUBEMAP_FACE_NEGATIVE_Z;
         break;
    }

    return D3DCubeMapFace;
}
//-----------------------------------------------------------------------------
D3DMULTISAMPLE_TYPE D3DTypeConvert(EMultiSamplingType ms)
{
    D3DMULTISAMPLE_TYPE result = D3DMULTISAMPLE_NONE;

    switch(ms)
    {
    case MSAA_NONE: result = D3DMULTISAMPLE_NONE; break;
    case MSAA_2X:   result = D3DMULTISAMPLE_2_SAMPLES; break;
    case MSAA_4X:   result = D3DMULTISAMPLE_4_SAMPLES; break;
    case MSAA_8X:   result = D3DMULTISAMPLE_8_SAMPLES; break;
    }

    return result;
}
//-----------------------------------------------------------------------------
void D3DTypeConvert(D3DPRESENT_PARAMETERS & presentParam, const SwapChainDesc & desc)
{
    ZeroMemory(&presentParam, sizeof(D3DPRESENT_PARAMETERS));
    presentParam.BackBufferWidth = desc.width,
    presentParam.BackBufferHeight = desc.height;
    presentParam.BackBufferCount = desc.bufferCount - 1;
    presentParam.hDeviceWindow = static_cast<HWND>(desc.handleWindow);
    presentParam.PresentationInterval = desc.vsync ? 1 : D3DPRESENT_INTERVAL_IMMEDIATE;
    presentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParam.EnableAutoDepthStencil = TRUE;
    presentParam.AutoDepthStencilFormat = D3DFMT_D24S8;
    presentParam.Flags = 0;
    presentParam.MultiSampleType = D3DTypeConvert(desc.multiSamplingType);
    presentParam.MultiSampleQuality = desc.multiSamplingQuality;

    switch(desc.screenMode)
    {
    case FULLSCREEN_MODE:
        presentParam.Windowed = FALSE;
        presentParam.BackBufferFormat = D3DTypeConvert(desc.format);
        break;
    case WINDOWED_MODE:
        presentParam.Windowed = TRUE;
        presentParam.BackBufferFormat = D3DFMT_UNKNOWN;
        break;
    }
}
//-----------------------------------------------------------------------------
} // namespace Gfx
