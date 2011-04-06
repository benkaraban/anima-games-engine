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
#ifndef RENDERER_STANDARDMATRENDERERSM0_H_
#define RENDERER_STANDARDMATRENDERERSM0_H_

#include <Renderer/SM2/StandardMatRenderer.h>

namespace Renderer
{
// ----------------------------------------------------------------------------

class StandardMatRendererSM0 : public IStandardMatRenderer
{
public:
    StandardMatRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib,
                                 const Ptr<PackedVerticesFormat> & pFormat,
                                 const Ptr<TextureMap> & pDefaultTexWhite,
                                 const Ptr<TextureMap> & pDefaultTexBlack,
                                 const RendererSettings & settings);

    virtual bool initialise();

    virtual void setStaticShadowMap(const Ptr<Gfx::ITexture2D> & pShadowMap);
    virtual void setDynamicShadowMap(int32 split, const Ptr<Gfx::ITexture2D> & pShadowMap);
    virtual void setDebugMode(const DebugSettings & mode);

    virtual void startContext(const RenderContext & context, ERenderPass pass);
    virtual void endContext();

    virtual void enqueueCommands(Core::List<Command> & commands);
    virtual void exec(Command * pStart, Command * pEnd);

protected:
    struct ShadersParams
    {
        ShadersParams();

        EStaticVertexStreamData    staticStreamData;
        ESkinnedVertexStreamData   skinnedStreamData;
        Gfx::GlobalRenderState     state;
        Ptr<Gfx::IVertexFormat>    pFormat;
        Ptr<Gfx::IVertexShader>    pVShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;

        // Vertex shader

        int32    idWorldViewProj;
        int32    idWorld;
        int32    idWorldInvTrans;
        int32    idViewProj;
        int32    idWorldBone;
        int32    idEyePosition;
        int32    idFogRange;
        int32    idDiffuseMapSize;
        int32    idLightingMapSize;
        int32    idZBias;

        int32    idView;

        int32    idMainLightDir;
        int32    idMainLightAmbient;
        int32    idMainLightDiffuse;
        int32    idMainLightSpecular;
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

        bool    initTried;
        bool    initOK;
    };

    struct SceneInfos
    {
        Core::Matrix4f world;
        Core::Matrix4f worldViewProj;
        Core::Matrix4f worldInvTrans;
        Core::Matrix4f viewProj;

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
    void setSkeltonParams(ShadersParams & params, const Assets::SkeletonPose & pose, const Core::List<int32> & palette);

    void setMainLightParams(ShadersParams & params, const SceneInfos & infos);
    void setMatParams(ShadersParams & params, const Renderer::Material & mat, const Renderer::MaterialData & data, const SceneInfos & infos);

    void renderMesh(ShadersParams & params, const SceneInfos & sceneInfos, const MeshInstanceSM2 & mesh);
    void renderSkinMesh(ShadersParams & params, const SceneInfos & sceneInfos, const SkinMeshInstanceSM2 & mesh);

    InternalFogSettings        _fogSettings;
    InternalMainLightSettings  _mainLightSettings;

    Ptr<Gfx::IDevice>          _pDevice;
    Ptr<ShaderLib>             _pShaderLib;
    Ptr<PackedVerticesFormat>  _pFormat;
    Ptr<TextureMap>            _pDefaultTexWhite;
    Ptr<TextureMap>            _pDefaultTexBlack;
    Ptr<Gfx::ITexture2D>       _pShadowMap0;
    Ptr<Gfx::ITexture2D>       _pShadowMap1;
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

    ShadersParams _paramsShadowSkin;
    ShadersParams _paramsShadowMesh;

    Core::Matrix4f   _view;
    Core::Matrix4f   _proj;
    Core::Vector3f   _eye;
};
}
#endif /* STANDARDMATRENDERER_H_ */
