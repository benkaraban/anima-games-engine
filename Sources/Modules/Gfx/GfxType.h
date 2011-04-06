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
#ifndef GFXTYPE_H_
#define GFXTYPE_H_

#include <Core/Standard.h>
#include <Core/List.h>

#include <Core/Math/Vector.h>

namespace Gfx
{

class LM_API_GFX GfxException: public Core::Exception
{
public:
    GfxException(const String & message) :
        Exception(message)
    {
    }
    ;
};

class LM_API_GFX OutOfVideoMemoryException : public GfxException
{
public:
    OutOfVideoMemoryException(const String & message) :
        GfxException(message)
    {
    };
};

typedef Core::Vector4f RGBA;

const int32 GFX_DISPLAYMODE_WIDTH_MIN = 800;
const int32 GFX_DISPLAYMODE_HEIGHT_MIN = 600;
const uint8 DEFAULT_STENCIL_READ_MASK = 0xff;
const uint8 DEFAULT_STENCIL_WRITE_MASK = 0xff;

const int32 MAX_SAMPLERS = 16;

const int32 CLEAR_COLOR_BUFFER     = 0x00000001;
const int32 CLEAR_DEPTH_BUFFER     = 0x00000002;
const int32 CLEAR_STENCIL_BUFFER   = 0x00000004;
const int32 CLEAR_ALL_BUFFERS      = 0xFFFFFFFF;

//-----------------------------------------------------------------------------
LM_ENUM_1(Invalid_t, INVALID);
//-----------------------------------------------------------------------------
LM_ENUM_6(EDisplayPixelFormat, DPF_A2R10G10B10, DPF_A8R8G8B8, DPF_X8R8G8B8, DPF_A1R5G5B5, DPF_X1R5G5B5, DPF_R5G6B5);
//-----------------------------------------------------------------------------
LM_ENUM_4(EMultiSamplingType, MSAA_NONE, MSAA_2X, MSAA_4X, MSAA_8X);
//-----------------------------------------------------------------------------
//decrit un mode d'affichage
struct LM_API_GFX DisplayMode
{
    int32 width;
    int32 height;
    int32 refreshRate;
    EDisplayPixelFormat format;

    DisplayMode(int32 w, int32 h, int32 r, EDisplayPixelFormat f) :
        width(w), height(h), refreshRate(r), format(f)
    {
    }
    ;

    bool operator <(const DisplayMode & mode) const
    {
        if(format != mode.format)
            return format < mode.format;

        if(height != mode.height)
            return height < mode.height;

        if(width != mode.width)
            return width < mode.width;

        return refreshRate < mode.refreshRate;
    }
    ;
};
//-----------------------------------------------------------------------------
//les capacites graphique disponible

LM_ENUM_4(EVendor, VENDOR_ATI, VENDOR_NVIDIA, VENDOR_INTEL, VENDOR_UNKOWN);

LM_ENUM_8(EPixelShaderVersion, PS_V0_0, PS_V1_1, PS_V1_2, PS_V1_3, PS_V1_4, PS_V2_0, PS_V2_a, PS_V3_0);

LM_ENUM_4(EVertexShaderVersion, VS_V0_0, VS_V1_1, VS_V2_0, VS_V3_0);

struct GraphicCapabilities
{
    // General ----------------------------------------

    EVendor     vendor;

    // Video memory -----------------------------------

    int64       videoMem; // Le résultat qu'on a pu obtenir, qq soit ma méthode

    bool        detailedVRAMInfosAvailable;
    int64       dedicatedVideoMem;
    int64       dedicatedSystemMem;
    int64       sharedSystemMem;

    bool        globalVRAMInfosAvailable;
    int64       globalVideoMem;

    // Shader caps  -----------------------------------

    EVertexShaderVersion vsVersion;
    EPixelShaderVersion psVersion;

    int32 maxVSConstCount;
    int32 maxPS2InstCount;

    // Texture caps -----------------------------------

    EMultiSamplingType  maxMSAA;
    int32               maxMSAAQuality;
    int32               maxAnisotropy;
    bool                supportAlphaCoverage;
    bool                supportFLOAT16Tex;
    bool                supportFLOAT16Blend;
    bool                supportSeparateAlphaBlend;
    bool                supportBlendingOps;

    // Vertex formats ---------------------------------

    bool supportUBYTE4;
    bool supportUBYTE4N;
    bool supportSHORT2N;
    bool supportSHORT4N;
    bool supportDEC3N;
    bool supportFLOAT16_2;
    bool supportFLOAT16_4;
};

LM_API_GFX Core::String toString(const GraphicCapabilities& gc);
//-----------------------------------------------------------------------------
//determine la maniere dont sera rendu un triangle
LM_ENUM_2(EFillMode,
        FM_WIREFRAME, //les sommets d'un même triangle sont connectés par une ligne
        FM_SOLID) //le triangle est remplie
//-----------------------------------------------------------------------------
//pour eviter de dessiner les triangle ayant une direction non souhaité
LM_ENUM_3(ECullMode,
        CM_NOCM, //tous les triangles sont dessines
        CM_FRONT, //les triangles faisant face ne sont pas dessines
        CM_BACK) //les triangles ayant une direction  de dos ne sont pas dessine
//-----------------------------------------------------------------------------
//permet de confirgurer l'étape de blending : qui permet de combiner un ou plusieur pixel pour obtenir le pixel final
LM_ENUM_13 (EBlendMode,
        BM_ZERO, //la donnee est une couleur, et sera noir
        BM_ONE, //la donnee est une couleur, et sera blanche
        BM_SRC_COLOR, //la donnee est une couleur, la couleur du pixel est donné par le pixel shader
        BM_INVERT_SRC_COLOR, //la donnee est une couleur, la couleur du pixel est l'inverse de la couleur retourne par le pixel  shader (1-R, 1-G, 1-B)
        BM_SRC_ALPHA, //la donnee est une valeur alpha issue du pixel shader
        BM_INVERT_SRC_ALPHA,//la donnee est une valeur alpha, et est égal à l'inverse de la valeur issue du pixel shader
        BM_DEST_ALPHA, //la donnee est une valeur alpha, issue d'un renderTarget
        BM_INVERT_DEST_ALPHA, //la donnee est une valeur alpha, et est égal à l'inverse de la valeur issue d'un renderTarget
        BM_DEST_COLOR, //la donnee est une couleur, issue d'un renderTarget
        BM_INVERT_DEST_COLOR, //la donnee est une couleur, et est égal à l'inverse de la valeur issue d'un renderTarget
        BM_SRC_ALPHA_SAT, //la donnee est une valeur alpha issue du pixel shader la valeur alpha est seuillé à 1 ou moins
        BM_BLEND_FACTOR, //la donnée est un blend factor qui à été spécifier par un setState (TODO a preciser)
        BM_INVERT_BLEND_FACTOR //la donnée est un blend factor, (TODO a preciser)
        /* BM_SRC1_COLOR,    //TODO non pris en compte dans directx 9 ? présent dans direct 9Ex ?
         BM_INVERT_SRC1_COLOR,
         BM_SRC1_ALPHA,
         BM_INVERT_SRC1_ALPHA*/)
//-----------------------------------------------------------------------------
//permet de preciser la maniere dont s'effectue le blending
LM_ENUM_5 (EBlendOperation,
        BO_ADD, // addition des deux pixel
        BO_SUBTRACT, //retire le pixel src1 du pixel src2
        BO_REV_SUBTRACT, //retire le pixel src2 du pixel src1
        BO_MIN, //garde le minimum entre src1 et src2
        BO_MAX) //garde le maximum entre src1 et src2
//-----------------------------------------------------------------------------
//decrit les operation qui peuvent avoir lieu durant le test du depth-stencil
LM_ENUM_8 (EStencilOperation,
        KEEP, //garde la valeur actuel du stencil
        ZERO, //place la valeur du stencil à zero
        REPLACE, //place la valeur par default du stencil (cette valeur est decrite dans RSDepthStencilDesc
        INCREMENT_SAT, //increment de 1 la valeur du stencil et clamp le resultat
        DECREMENT_SAT, //decrement de 1 la valeur du stencil et clamp le resultat
        INVERT, //inverse la valeur actuel du stencil
        INCREMENT, //incremente à 1 la valeur du stencil et wrap le resultat si necessaire
        DECREMENT) //decremente de 1 la valeur du stencil et wrap le result si necessaire
//-----------------------------------------------------------------------------
//decrit un mode de comparaison
LM_ENUM_8 (EComparisonFunc,
        COMPARISON_NEVER, //la comparaisn retourne false à chaque fois
        COMPARISON_LESS, //retourne src < dst
        COMPARISON_EQUAL, //src == dst
        COMPARISON_LESS_EQUAL, //src<=dst
        COMPARISON_GREATER, //src>dst
        COMPARISON_NOT_EQUAL, //src!=dst
        COMPARISON_GREATER_EQUAL,//src>=dst
        COMPARISON_ALWAYS)//la comparaison retourne true à chaque fois
//-----------------------------------------------------------------------------
//option de filtrage durant la phase de texture sampling
LM_ENUM_9 (EFilterType,
        FILTER_MIN_MAG_MIP_POINT,
        FILTER_MIN_MAG_POINT_MIP_LINEAR,
        FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
        FILTER_MIN_POINT_MAG_MIP_LINEAR,
        FILTER_MIN_LINEAR_MAG_MIP_POINT,
        FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        FILTER_MIN_MAG_LINEAR_MIP_POINT,
        FILTER_MIN_MAG_MIP_LINEAR,
        FILTER_ANISOTROPIC
        /*FILTER_COMPARISON_MIN_MAG_MIP_POINT,
         FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
         FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
         FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
         FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
         FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
         FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
         FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
         FILTER_COMPARISON_ANISOTROPIC,
         FILTER_TEXT_1BIT*/) //TODO dispo dans D3D9 ???
//-----------------------------------------------------------------------------
//gestion des bords de la texture : détermine les coordonnes lorsqu'elles ne sont pas comprise dans la texture
LM_ENUM_5 (ETextureAddressMode,
        TEXTURE_ADDRESS_WRAP, // repete la texture à l'identique
        TEXTURE_ADDRESS_MIRROR, // repete la texture en l'inverssant
        TEXTURE_ADDRESS_CLAMP, // clamp la coordonne à 0 ou 1
        TEXTURE_ADDRESS_BORDER, // utilise la couleur présiser dans RSSamplerDesc lorsque la coordonne n'est pas comprise dans [0,1]
        TEXTURE_ADDRESS_MIRROR_ONCE) //idem à clamp sauf que les coordonne peuvent etre comprise entre [-1,1]; entre [-1,0] la texture utilise est la symetrie de la texture
//-----------------------------------------------------------------------------
//L'enumere décrivant l'usage de chaque attribut d'un vertex
LM_ENUM_10 (EVertexAttributUsage,
        VAU_POSITION,
        VAU_POSITIONT,
        VAU_NORMAL,
        VAU_BINORMAL,
        VAU_TANGENT,
        VAU_COLOR,
        VAU_TEXTURE_COORD,
        VAU_BLEND_WEIGHTS,
        VAU_BLEND_INDICES,
        VAU_UNUSED)
//-----------------------------------------------------------------------------
//l'énuméré décrivant le type de chaque attribut d'un vertex
LM_ENUM_12 (EVertexAttributType,
        VAT_FLOAT1,
        VAT_FLOAT2,
        VAT_FLOAT3,
        VAT_FLOAT4,
        VAT_COLOR,
        VAT_UBYTE4,
        VAT_UBYTE4N,
        VAT_SHORT2N,
        VAT_SHORT4N,
        VAT_DEC3N,
        VAT_FLOAT16_2,
        VAT_FLOAT16_4);
//-----------------------------------------------------------------------------
//La structure décrivant un element d'un vertex
struct LM_API_GFX VertexElement
{
    int32 streamId;
    int32 offset; //l'offset pour atteindre l'attribut
    EVertexAttributType type;
    EVertexAttributUsage usage;
    int32 usageIndex;

    bool operator < (const VertexElement & ve) const;
    bool operator != (const VertexElement & ve) const;
};
//-----------------------------------------------------------------------------
//render state : input element -> declare le format des vertex contenue dans les vertexbuffer utilisé par le vertex shader
//exemple : pour un vertex ayant les attributs suivant : {position, binormal, tangent}
//
//RSInputElementDesc ie;
//ie.addAttribut(VAT_3_FLOAT, VAU_POSITION);
//ie.addAttribut(VAT_4_UINT8, VAU_BINORMAL);
//ie.addAttribut(VAT_4_UINT8, VAU_TANGENT);
struct LM_API_GFX VertexFormatDesc
{
public:
    void addAttribut(int32 streamId, EVertexAttributType type, EVertexAttributUsage usage, int32 usageIndex = 0);
    void clear();

    int32 getNbAttribut() const;
    const VertexElement& getAttribut(const int32 indice) const;
    int32 getVertexSize(int32 streamId) const;

    bool operator < (const VertexFormatDesc & vf) const;

protected:
    Core::List<VertexElement> _attributs;
};
//-----------------------------------------------------------------------------
//render state : rasterizer :
struct LM_API_GFX RSRasterizerDesc
{
    RSRasterizerDesc();
    RSRasterizerDesc(Invalid_t);
    RSRasterizerDesc(ECullMode cullMode, bool frontFaceIsCCW, EFillMode fillMode);

    ECullMode cullMode; //permet de ne pas prendre en compte les triangles suivant leur direction (front ou back)
    bool frontFaceIsCounterClockwise; //methode pour determiner les triangle front ou back

    EFillMode fillMode; //affiche les triangle plein ou en fil de fer

    //gestion du biais en cas de triangle coplanaire (z-fighting)
    int32 depthBias; //biais profondeur ajouter a  un pixel
    float depthBiasMax; //valeur maximal du biais donnee au pixel
    float depthBiasScale;//

    //TODO
    /*
     bool _depthClipIsEnable;    //active ou pas le clipping basé sur la distance
     bool _scissorIsEnable;        //active ou pas le scissor ! qui permet de ne pas prendre en compte les pixel en dehaor d'un rectangle prédéfini
     bool _multisambpleIsEnable;
     bool _antialiasedLineIsEnable;
     */
    //    static RSRasterizerDesc _default;
};
//-----------------------------------------------------------------------------
struct LM_API_GFX RSBlendDesc //TODO voir si on s'inspire de D3D10 ou de D3D10.1

{
    RSBlendDesc();
    RSBlendDesc(Invalid_t);
    RSBlendDesc(EBlendMode src, EBlendOperation op, EBlendMode dst);

    bool enabled;
    EBlendMode src; //determine le pixel source
    EBlendMode dst; //determine le second pixel source
    EBlendOperation op; //determine comment sont combinés les pixel
    EBlendMode srcAlpha; //determine la source alpha
    EBlendMode dstAlpha; //determine la seconde source alpha
    EBlendOperation opAlpha;

    bool            colorWriteEnabled;

    bool            alphaTestEnabled;
    EComparisonFunc alphaTestFunc;
    uint32          alphaTestRef;

    bool            alphaCoverageEnabled;
    bool            sRGBWriteEnabled;

    //bool _isEnable[NBMAX_RENDER_TARGETS]; //TODO a voir
    // uint8 _writeMask[NBMAX_RENDER_TARGETS]; //TODO a voir

    //    static RSBlendDesc _default;
};
//-----------------------------------------------------------------------------
struct LM_API_GFX StencilOpDesc
{
    EStencilOperation opStencilNOK;
    EStencilOperation opStencilOKDepthNOK;
    EStencilOperation opStencilOKDepthOK;
    EComparisonFunc function;
};
//-----------------------------------------------------------------------------
struct LM_API_GFX RSDepthStencilDesc
{
    RSDepthStencilDesc();
    RSDepthStencilDesc(Invalid_t);
    RSDepthStencilDesc(bool depthTestOn, bool depthWriteOn, EComparisonFunc depthFunc);

    //depth
    bool isDepthTestEnabled; //active ou pas le test de profondeur
    bool isDepthWriteEnabled;
    EComparisonFunc depthFunction; //determine comment le test de profondeur s'effectue

    //stencil
    bool isStencilEnabled; //active ou pas le test stencil
    uint8 stencilReadMask; //determine les bits significatifs des valeurs lu dans le stencil buffer lors du test
    uint8 stencilWriteMask; //determine les bits significatifs des valeurs ecrite dans le stencil buffer

    int32 stencilRef;

    StencilOpDesc opStencil; //configuration du stencil en fropnt et en back


    /*//TODO voir comment configurer le stencil avec D3D9 en front ou back ??
     DescriptionStencilOperation _opStencilForBackSurface; //determine comment utiliser le rÃ©sultat du test detph et stencil pour les pixel ayant une normal (Ã  la surface) dans le sens de la camÃ©ra
     DescriptionStencilOperation _opStencilForFrontSurface;//determine comment utiliser le rÃ©sultat du test detph et stencil pour les pixel ayant une normal (Ã  la surface) faisant face Ã  la camÃ©ra
     */
    //    static RSDepthStencilDesc _default;
};
//-----------------------------------------------------------------------------
struct LM_API_GFX RSSamplerDesc
{
    RSSamplerDesc();
    RSSamplerDesc(Invalid_t);
    RSSamplerDesc(ETextureAddressMode addr, bool isSRGB=false);
    RSSamplerDesc(EFilterType filter, ETextureAddressMode addrU, ETextureAddressMode addrV, int32 maxAniso=0, bool isSRGB=false);

    EFilterType filter; //la methode tutilise lors du sampling d'une texture
    ETextureAddressMode addrModeU; //gestion des bords sur la coordonnée u
    ETextureAddressMode addrModeV; //gestion des bords sur la coordonnée v
    ETextureAddressMode addrModeW; //gestion des bords sur la coordonnée w
    int32 maxAnisotropy; //valeur max utilise pour un filtrage anisotropique
    Core::Vector4f borderColor; //couleur utilise si TEXTURE_ADDRESS_BORDER fixe pour u,v, ou w
    bool           isSRGB;

    //float MipLODBias;    //TODO
    //ECOMPARAISON_FUNCTION _function; //TODO voir si present dans D3D9 ??
    //float MinLOD; //TODO voir si present dans D3D9 ??
    //float MaxLOD; //TODO voir si present dans D3D9 ??

    //    static RSSamplerDesc _default;
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//décrit l'usage du  buffer
LM_ENUM_3 (EBufferUsage,
        BUFFER_USAGE_IMMUTABLE,  //la ressource ne changera pas
        BUFFER_USAGE_DEFAULT, //pour les ressources qui seront modifié rarement
        BUFFER_USAGE_DYNAMIC) //l'application peut etre amené à acceder souvent au buffer    //D3D9 => dynamic
//-----------------------------------------------------------------------------
LM_ENUM_4 (EBufferCpuAccess,
        BCA_NO_ACCESS, //pas d'access CPU le buffer est utilisé que par le GPU
        BCA_ONLY_WRITE,
        BCA_ONLY_READ,
        BCA_READ_AND_WRITE)


//-----------------------------------------------------------------------------
//décrit le format des données contenu dans un indexbuffer
LM_ENUM_2 (EIndexBufferFormat,
        INDEX_BUFFFER_FMT_16, //chaque index est codé sur 16 bit
        INDEX_BUFFFER_FMT_32) //chaque index est codé sur 32 bit
//-----------------------------------------------------------------------------
struct LM_API_GFX VertexBufferDesc
{
    EBufferUsage usage;
    EBufferCpuAccess cpuAccess;

    int32 size; //en octets

    VertexBufferDesc() : usage(BUFFER_USAGE_IMMUTABLE), cpuAccess(BCA_ONLY_WRITE), size(0)
    {};

    VertexBufferDesc(EBufferUsage otherUsage, EBufferCpuAccess otherCpuAccess, int32 otherSize) : usage(otherUsage), cpuAccess(otherCpuAccess), size(otherSize)
    {};
};
//-----------------------------------------------------------------------------
struct LM_API_GFX IndexBufferDesc
{
    EBufferUsage usage;
    EBufferCpuAccess cpuAccess;

    EIndexBufferFormat format; //pour Dx9 on stocke le format du buffer dans le buffer
    int32 size;

    IndexBufferDesc() : usage(BUFFER_USAGE_IMMUTABLE), cpuAccess(BCA_ONLY_WRITE), format(INDEX_BUFFFER_FMT_16), size(0)
    {};

    IndexBufferDesc(EBufferUsage otherUage, EBufferCpuAccess otherCpuAccess, EIndexBufferFormat otherFormat, int32 otherSize)
    : usage(otherUage), cpuAccess(otherCpuAccess), format(otherFormat), size(otherSize)
    {};
};
//-----------------------------------------------------------------------------
LM_ENUM_5 (ELockType,
        LOCK_READ, //read only
        LOCK_WRITE, //write only
        LOCK_READ_WRITE, //read and write
        LOCK_WRITE_DISCARD, //write only, le contenu précédent du buffer est perdu, en utilisant ce flag l'application est sencé réécrire sur l'ensemble du buffer
        LOCK_WRITE_NO_OVERWRITE) //l'application garantie qu'elle ne va pas ecrire sur une partie du buffer envoyé au GPU
//-----------------------------------------------------------------------------
//retourne la taille en octec d'un attribut
LM_API_GFX int32 getSizeOf(EVertexAttributType type);
//-----------------------------------------------------------------------------
LM_ENUM_23 (EShaderConstantType,
        SHADER_PARAM_BOOL,
        SHADER_PARAM_FLOAT1,
        SHADER_PARAM_FLOAT2,
        SHADER_PARAM_FLOAT3,
        SHADER_PARAM_FLOAT4,
        SHADER_PARAM_MATRIX_2X2,
        SHADER_PARAM_MATRIX_2X3,
        SHADER_PARAM_MATRIX_2X4,
        SHADER_PARAM_MATRIX_3X2,
        SHADER_PARAM_MATRIX_3X3,
        SHADER_PARAM_MATRIX_3X4,
        SHADER_PARAM_MATRIX_4X2,
        SHADER_PARAM_MATRIX_4X3,
        SHADER_PARAM_MATRIX_4X4,
        SHADER_PARAM_INT1,
        SHADER_PARAM_INT2,
        SHADER_PARAM_INT3,
        SHADER_PARAM_INT4,
        SHADER_PARAM_SAMPLER1D,
        SHADER_PARAM_SAMPLER2D,
        SHADER_PARAM_SAMPLER3D,
        SHADER_PARAM_SAMPLERCUBE,
        SHADER_PARAM_UNKNOWN)
//TODO a voir
/*      SHADER_PARAM_SAMPLER1D,
 SHADER_PARAM_SAMPLER2D,
 SHADER_PARAM_SAMPLER3D,
 SHADER_PARAM_SAMPLERCUBE,
 SHADER_PARAM_SAMPLER1DSHADOW,
 SHADER_PARAM_SAMPLER2DSHADOW,*/
//-----------------------------------------------------------------------------
LM_ENUM_2 (EShaderType,
        VERTEX_SHADER,
        PIXEL_SHADER);
//-----------------------------------------------------------------------------
LM_ENUM_6 (EPrimitiveType,
        PT_POINTLIST,
        PT_LINELIST,
        PT_LINESTRIP,
        PT_TRIANGLE_LIST,
        PT_TRIANGLE_STRIP,
        PT_TRIANGLE_FAN)
//-----------------------------------------------------------------------------
LM_ENUM_19 (ETextureFormat,
        TF_X8R8G8B8,
        TF_A8R8G8B8,
        TF_R8G8B8A8_UNORM,
        TF_R5G6B5,
        TF_R16,
        TF_G16R16,
        TF_A16B16G16R16,
        TF_R32F,
        TF_G32R32F,
        TF_A32B32G32R32F,
        TF_R16F,
        TF_G16R16F,
        TF_A16B16G16R16F,
        TF_D24S8,
        TF_DXT1,
        TF_DXT2,
        TF_DXT3,
        TF_DXT4,
        TF_DXT5)
//-----------------------------------------------------------------------------
LM_ENUM_4 (ETextureBind, //TODO a faire
        TB_RENDER_TARGET,
        TB_DEPTH_STENCIL_SURFACE,
        TB_DEPTH_STENCIL_TEXTURE,
        TB_BUFFER_DEFAULT)
//-----------------------------------------------------------------------------
struct LM_API_GFX Texture1DDesc
{
    Texture1DDesc();

    int32 width;
    int32 mipLevels;

    ETextureFormat format;
    EBufferUsage usage;
    EBufferCpuAccess cpuAccess;

    ETextureBind bind;
    bool         autoGenMipmap;
    /*
    UINT ArraySize;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;*/

};
//-----------------------------------------------------------------------------
struct LM_API_GFX Texture2DDesc
{
    Texture2DDesc();

    int32               width;
    int32               height;
    int32               mipLevels;

    EMultiSamplingType  multiSampleType;

    ETextureFormat      format;
    EBufferUsage        usage;
    EBufferCpuAccess    cpuAccess;

    ETextureBind        bind;
    bool                autoGenMipmap;
};
//-----------------------------------------------------------------------------
struct LM_API_GFX Texture3DDesc
{
    Texture3DDesc();

    int32 width;
    int32 height;
    int32 nbSlice;

    int32 mipLevels;

    ETextureFormat format;
    EBufferUsage usage;
    EBufferCpuAccess cpuAccess;

    ETextureBind bind;
    bool         autoGenMipmap;
    //int32 ArraySize;  //TODO voir si utils
    //DXGI_SAMPLE_DESC SampleDesc; // TODO  TODO TODO !!
    //UINT BindFlags;   //TODO
    //UINT CPUAccessFlags;  //TODO
    //UINT MiscFlags;   //TODO
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct LM_API_GFX TextureCubeDesc
{
    TextureCubeDesc();

    int32 edgeLength;
    int32 mipLevels;
    ETextureFormat format;
    EBufferUsage usage;
    EBufferCpuAccess cpuAccess;
    ETextureBind bind;
    bool         autoGenMipmap;
};
//-----------------------------------------------------------------------------
LM_ENUM_6(ECubeMapFace,
         CUBE_FACE_POSITIVE_X,
         CUBE_FACE_NEGATIVE_X,
         CUBE_FACE_POSITIVE_Y,
         CUBE_FACE_NEGATIVE_Y,
         CUBE_FACE_POSITIVE_Z,
         CUBE_FACE_NEGATIVE_Z);

//-----------------------------------------------------------------------------
struct LM_API_GFX DSVSubresourceTex2D
{
    int32 mipSlice;
};
struct LM_API_GFX DepthStencilViewDesc
{
    union
    {
        DSVSubresourceTex2D texture2D;
    };
};
//-----------------------------------------------------------------------------
struct LM_API_GFX RTVSubresourceTex2D
{
    int32 mipSlice;
};

struct LM_API_GFX RTVSubresourceTexCube
{
    int32 mipSlice;
    ECubeMapFace cubeFace;
};

struct LM_API_GFX RTVSubresourceSwapChain
{
    int32 idBuffer;
};

struct LM_API_GFX RTVSubresourceSurface
{
    int32               width;
    int32               height;
    ETextureFormat      format;
    EMultiSamplingType  multiSampleType;
};

struct LM_API_GFX RenderTargetViewDesc
{
    union
    {
        RTVSubresourceTex2D     texture2D;
        RTVSubresourceTexCube   textureCube;
        RTVSubresourceSwapChain swapChain;
        RTVSubresourceSurface   surface;
    };
};
//-----------------------------------------------------------------------------
struct LM_API_GFX FontDesc
{
    String      name;
    int32       pixHeight;
    bool        bold;
    bool        italic;
};
//-----------------------------------------------------------------------------
struct LM_API_GFX SRVSubresourceTex1D
{
    int32 mostDetailedMip;
};

struct LM_API_GFX SRVSubresourceTex2D
{
    int32 mostDetailedMip;
};

struct LM_API_GFX SRVSubresourceTex3D
{
    int32 mostDetailedMip;
};

struct LM_API_GFX SRVSubresourceTexCube
{
    int32 empty;
};

struct LM_API_GFX ShaderResourceViewDesc
{
    union
    {
        SRVSubresourceTex1D texture1D;
        SRVSubresourceTex2D texture2D;
        SRVSubresourceTex2D texture3D;
        SRVSubresourceTexCube textureCube;
    };
};

struct LM_API_GFX SubresourceData
{
    const void* data;
    int32 rowStep;  //only for 2D and 3D texture data
    int32 sliceStep;    //only for 3D texture data
};

//-----------------------------------------------------------------------------
struct LM_API_GFX Viewport
{
    Viewport(int32 x, int32 y, int32 w, int32 h, float minDepth, float maxDepth);

    int32 topLeftX;
    int32 topLeftY;
    int32 width;
    int32 height;
    float minDepth;
    float maxDepth;
};

struct LM_API_GFX MappedTexture2D
{
    void* pData;
    int32 rowStep;
};

struct LM_API_GFX MappedTexture3D
{
    void* pData;
    int32 rowStep;
    int32 sliceStep;
};

//-----------------------------------------------------------------------------
LM_ENUM_4 (EDeviceState,
        DS_DEVICE_OK,
        DS_DEVICE_LOST,
        DS_DEVICE_NOT_RESET,
        DS_DEVICE_ERROR);

struct LM_API_GFX GfxStats
{
    int32       setVertexDeclCount;
    int32       vsSwitchCount;
    int32       psSwitchCount;
    int32       setPSConstCount;
    int32       setVSConstCount;
    int32       triCount;
};

}//namespace Gfx

namespace Core
{

inline String toString(const Gfx::VertexElement& ve)
{
    String str;
    return str;
}
} // namespace Core

#endif /* GFXTYPE_H_ */
