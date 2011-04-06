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
#ifndef RENDERER_STANDARDMATRENDERER_H_
#define RENDERER_STANDARDMATRENDERER_H_

#include <Core/List.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/PackedVertices.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/SM2/ICommandExecuter.h>
#include <Renderer/SM2/MeshSM2.h>
#include <Renderer/SM2/InternalSettings.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/IScene.h>

namespace Renderer
{
    class IStandardMatRenderer : public ICommandExecuter
{
public:
    virtual ~IStandardMatRenderer() {};

    virtual bool initialise() = 0;

    virtual void setStaticShadowMap(const Ptr<Gfx::ITexture2D> & pShadowMap) = 0;
    virtual void setDynamicShadowMap(int32 split, const Ptr<Gfx::ITexture2D> & pShadowMap) = 0;
    virtual void setDebugMode(const DebugSettings & mode) = 0;
    virtual void compileAllShaders() {};
};

LM_ENUM_2(ESkinLightingGroup,
          LGROUP_SKIN,
          LGROUP_SKIN_TSN);

LM_ENUM_4(EMeshLightingGroup,
          LGROUP_STATIC,
          LGROUP_STATIC_TSN,
          LGROUP_DYNAMIC,
          LGROUP_DYNAMIC_TSN);

const int32 MAX_POINT_LIGHTS_COUNT = 3;

// Lighting flags -------------------------------------------------------------

const int32 POINT_LIGHT_1_FLAG       = 0x00000001;
const int32 POINT_LIGHT_2_FLAG       = 0x00000002;
const int32 POINT_LIGHT_3_FLAG       = 0x00000003;
const int32 REFLEXIONS_MAP_FLAG      = 0x00000004;
const int32 SHADOW_FLAG              = 0x00000008;
const int32 CLAMP_TEX_BORDER_FLAG    = 0x00000010;
const int32 INTERNAL_TEX_BORDER_FLAG = 0x00000020;
const int32 NORMAL_MAP_DXT5_FLAG     = 0x00000040;
const int32 SPECULAR_FLAG            = 0x00000080;
const int32 ADV_SPECULAR_FLAG        = 0x00000100;
const int32 INVERTED_CULLING_FLAG    = 0x00000200;
const int32 HQ_PREVIEW_FLAG          = 0x00000400;

const int32 POINT_LIGHT_COUNT_MASK = 0x00000003;
const int32 ALL_LIGHTING_FLAG      = (POINT_LIGHT_COUNT_MASK | REFLEXIONS_MAP_FLAG | SHADOW_FLAG | CLAMP_TEX_BORDER_FLAG | HQ_PREVIEW_FLAG |
                                      INTERNAL_TEX_BORDER_FLAG | NORMAL_MAP_DXT5_FLAG | SPECULAR_FLAG | ADV_SPECULAR_FLAG | INVERTED_CULLING_FLAG);

// ----------------------------------------------------------------------------


LM_ENUM_4(EParamMode,
          MODE_SOLID,
          MODE_REFRAC,
          MODE_TRANS_BACK,
          MODE_TRANS_FRONT);

LM_ENUM_2(ERadiosityMode,
          MODE_FRONT,
          MODE_BACK);

class StandardMatRenderer : public IStandardMatRenderer
{
public:
    StandardMatRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib,
                        const Ptr<PackedVerticesFormat> & pFormat,
                        const Ptr<TextureMap> & pDefaultTexWhite,
                        const Ptr<TextureMap> & pDefaultTexBlack,
                        const Ptr<TextureMap> & pDebugTexMip,
                        const RendererSettings & settings);

    virtual bool initialise();

    virtual void setStaticShadowMap(const Ptr<Gfx::ITexture2D> & pShadowMap);
    virtual void setDynamicShadowMap(int32 split, const Ptr<Gfx::ITexture2D> & pShadowMap);
    virtual void setDebugMode(const DebugSettings & mode);

    virtual void startContext(const RenderContext & context, ERenderPass pass);
    virtual void endContext();
    
    virtual void enqueueCommands(Core::List<Command> & commands);
    virtual void exec(Command * pStart, Command * pEnd);

    virtual void compileAllShaders();

protected:
    struct ShadersParams
    {
        ShadersParams();

        EStaticVertexStreamData    staticStreamData;
        ESkinnedVertexStreamData   skinnedStreamData;
        Gfx::GlobalRenderState     state;
        Ptr<Gfx::IVertexFormat>    pFormat;
        Ptr<Gfx::IVertexShader>    pVShader;
        Ptr<Gfx::IPixelShader>     pPShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;
        Ptr<Gfx::ShaderConstantTable> pPConst;

        // Vertex shader

        int32    idWorldViewProj;
        int32    idWorld;
        int32    idWorldInvTrans;
        int32    idViewProj;
        int32    idStaticLightViewProj;
        int32    idDynamicLightViewProj0;
        int32    idDynamicLightViewProj1;
        int32    idWorldBone;
        int32    idEyePosition;
        int32    idFogRange;
        int32    idGlowExtent;
        int32    idDiffuseMapSize;
        int32    idLightingMapSize;
        int32    idZBias;
        int32    idViewportSize;

        int32    idIsTextureBorderOn;


        // Pixel shader

        int32    idView;

        // { scene light
        int32    idMainLightDir;
        int32    idMainLightAmbient;
        int32    idMainLightDiffuse0;
        int32    idMainLightDiffuse1;
        int32    idMainLightDiffuse2;
        int32    idMainLightSpecular;

        int32    idOcclusion;

        int32    idStaticShadowDiffusion;

        int32    idDynamicShadowStart;
        int32    idDynamicShadowRange;
        int32    idDynamicShadowOpacity;
        // }

        int32    idLightMapReflexion;
        int32    idFogColor;
        int32    idWaterLevel;

        int32    idMatEmissive;
        int32    idMatDiffuse;
        int32    idMatSpecular;
        int32    idMatShininess;
        int32    idMatReflexions;
        int32    idMatRefraction;
        int32    idMatRefrScale;
        int32    idMatRefrIndex;
        int32    idMatGlow;

        int32    idSamplerDiffuse;
        int32    idSamplerSpecular;
        int32    idSamplerNormal;
        int32    idSamplerLight;
        int32    idSamplerOcclusion;
        int32    idSamplerStaticShadow;
        int32    idSamplerDynamicShadow0;
        int32    idSamplerDynamicShadow1;
        int32    idSamplerReflexions;
        int32    idSamplerRefraction;
        int32    idSamplerDebugMIP;

        int32     idIsNormalMapDXT5;
        int32     idIsDebugNormal;
        int32     idIsDebugUV;
        int32     idIsDebugT;
        int32     idIsDebugB;
        int32     idIsDebugN;
        int32     idIsDebugMIP;

        struct PointLightParams
        {
            int32    idPosition;
            int32    idAmbient;
            int32    idDiffuse;
            int32    idSpecular;
            int32    idInvRadius;
        };

        PointLightParams   pointLights[MAX_POINT_LIGHTS_COUNT];

        bool    initTried;
        bool    initOK;
    };

    struct SceneInfos
    {
        Core::Matrix4f world;
        Core::Matrix4f worldViewProj;
        Core::Matrix4f worldInvTrans;
        Core::Matrix4f viewProj;
        Core::Matrix4f staticLightViewProj;
        Core::Matrix4f dynamicLightViewProj0;
        Core::Matrix4f dynamicLightViewProj1;

        Core::Vector3f eye;
    };

    ESkinLightingGroup getSkinLightingGroup(const Material & mat, const MaterialData & data, const LightingData & lighting) const;
    EMeshLightingGroup getMeshLightingGroup(const Material & mat, const MaterialData & data, const LightingData & lighting) const;
    int32              getLightingFlags(const Material & mat, const MaterialData & data, const LightingData & lighting) const;

    bool isTransparent(const Material & mat) const;

    ShadersParams & getSkinParams(EParamMode mode, ERenderPass pass, ESkinLightingGroup group, int32 flags);
    ShadersParams & getMeshParams(EParamMode mode, ERenderPass pass, EMeshLightingGroup group, int32 flags);

    void getParams(ShadersParams & params, ERenderPass pass, EParamMode mode, int32 iGroup, bool skin, int32 flags);
    bool bindParams(ShadersParams & params, ERenderPass pass, EParamMode mode, ESkinLightingGroup group, int32 flags, const SceneInfos & infos);
    bool bindParams(ShadersParams & params, ERenderPass pass, EParamMode mode, EMeshLightingGroup group, int32 flags, const SceneInfos & infos);

     
    void setSceneParams(ShadersParams & params, const SceneInfos & infos);
    void setObjectParams(ShadersParams & params, const SceneInfos & infos);
    void setSkeltonParams(ShadersParams & params, const Assets::SkeletonPose & pose, const Core::List<int32> & palette);

    void setMainLightParams(ShadersParams & params, const SceneInfos & infos);
    void setMatParams(ShadersParams & params, const Renderer::Material & mat, const Renderer::MaterialData & data);
    void setLightingParams(ShadersParams & params, const LightingData & lighting);

    void renderMesh(ShadersParams & params, const SceneInfos & sceneInfos, const MeshInstanceSM2 & mesh);
    void renderSkinMesh(ShadersParams & params, const SceneInfos & sceneInfos, const SkinMeshInstanceSM2 & mesh);

    void internalStartContext(const RenderContext & context, ERenderPass pass);

    InternalFogSettings        _fogSettings;
    InternalMainLightSettings  _mainLightSettings;
    float                      _waterLevel;
    bool                       _isDynamicShadowOn;

    Core::List<Command>        _commands;

    Ptr<Gfx::IDevice>          _pDevice;
    Ptr<ShaderLib>             _pShaderLib;
    Ptr<PackedVerticesFormat>  _pFormat;
    Ptr<TextureMap>            _pDefaultTexWhite;
    Ptr<TextureMap>            _pDefaultTexBlack;
    Ptr<TextureMap>            _pDebugTexMip;
    Ptr<Gfx::ITexture2D>       _pStaticShadowMap;
    Ptr<Gfx::ITexture2D>       _pDynamicShadowMap0;
    Ptr<Gfx::ITexture2D>       _pDynamicShadowMap1;
    IRenderTarget *            _pRenderTarget;
    DebugSettings              _mode;
    RendererSettings           _settings;

    ShadersParams _paramsLightingSkin[EParamMode_COUNT][ESkinLightingGroup_COUNT][ALL_LIGHTING_FLAG + 1];
    ShadersParams _paramsLightingMesh[EParamMode_COUNT][EMeshLightingGroup_COUNT][ALL_LIGHTING_FLAG + 1];

    ShadersParams _paramsReflecSkin[EParamMode_COUNT][ESkinLightingGroup_COUNT][ALL_LIGHTING_FLAG + 1];
    ShadersParams _paramsReflecMesh[EParamMode_COUNT][EMeshLightingGroup_COUNT][ALL_LIGHTING_FLAG + 1];

    ShadersParams _paramsDebugSkin[ESkinLightingGroup_COUNT][ALL_LIGHTING_FLAG + 1];
    ShadersParams _paramsDebugMesh[EMeshLightingGroup_COUNT][ALL_LIGHTING_FLAG + 1];

    ShadersParams _paramsWireframeSkin;
    ShadersParams _paramsWireframeMesh;

    ShadersParams _paramsGlowSkin;
    ShadersParams _paramsGlowMesh;

    ShadersParams _paramsStaticShadowMesh;

    ShadersParams _paramsLightMapMesh[EMeshLightingGroup_COUNT][ALL_LIGHTING_FLAG + 1];

    ShadersParams _paramsDynamicShadowSkin;
    ShadersParams _paramsDynamicShadowMesh;

    ShadersParams * _pLastParams;

    Core::Matrix4f   _view;
    Core::Matrix4f   _proj;
    Core::Matrix4f   _staticLightProj;
    Core::Matrix4f   _staticLightViewProj;
    Core::Matrix4f   _dynamicLightViewProj0;
    Core::Matrix4f   _dynamicLightViewProj1;
    Core::Vector3f   _eye;
    Core::Vector2f   _viewportSize;
};
}
#endif /* STANDARDMATRENDERER_H_ */
