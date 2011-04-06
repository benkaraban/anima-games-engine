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
#ifndef RENDERER_CUSTOMMATRENDERER_H_
#define RENDERER_CUSTOMMATRENDERER_H_

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
class ICustomMatRenderer : public ICommandExecuter
{
public:
    virtual ~ICustomMatRenderer() {};

    virtual bool initialise() = 0;

    //virtual void setShadowMap(int32 split, const Ptr<Gfx::ITexture2D> & pShadowMap) = 0;
    //virtual void setDebugMode(const DebugSettings & mode) = 0;
};

class CustomMatRenderer : public ICustomMatRenderer
{
public:
    CustomMatRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib,
                      const Ptr<PackedVerticesFormat> & pFormat,
                      const Ptr<TextureMap> & pDefaultTexWhite,
                      const Ptr<TextureMap> & pDefaultTexBump,
                      const RendererSettings & settings);

    virtual bool initialise();

    //virtual void setShadowMap(int32 split, const Ptr<Gfx::ITexture2D> & pShadowMap);
    //virtual void setDebugMode(const DebugSettings & mode);

    virtual void startContext(const RenderContext & context, ERenderPass pass);
    virtual void endContext();
    
    virtual void enqueueCommands(Core::List<Command> & commands);
    virtual void exec(Command * pStart, Command * pEnd);

protected:
    struct ShadersParams
    {
        //ShadersParams();

        EStaticVertexStreamData    staticStreamData;
        ESkinnedVertexStreamData   skinnedStreamData;
        Gfx::GlobalRenderState     state;
        Ptr<Gfx::IVertexFormat>    pFormat;
        Ptr<Gfx::IVertexShader>    pVShader;
        Ptr<Gfx::IPixelShader>     pPShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;
        Ptr<Gfx::ShaderConstantTable> pPConst;
        bool                          isNormal;

        // Vertex shader

        int32       idWorldViewProj;
        int32       idWorldView;
        int32       idFogRange;
        int32       idDUV0;
        int32       idDUV1;
        int32       idDUV2;

        // Pixel shader

        int32    idCustomColor;
        int32    idRefrScale;

        int32    idSamplerTexture0;
        int32    idSamplerTexture1;
        int32    idSamplerTexture2;
        int32    idSamplerRefraction;
    };

    ERenderPass                 _pass;
    InternalFogSettings         _fogSettings;
    //InternalMainLightSettings  _mainLightSettings;
    //float                      _waterLevel;
    //bool                       _shadowOn;
    Core::Matrix4f      _viewProj;
    Core::Matrix4f      _view;
    Core::Matrix4f      _proj;
    double              _time;

    Core::List<Command>        _commands;

    Ptr<Gfx::IDevice>          _pDevice;
    Ptr<ShaderLib>             _pShaderLib;
    Ptr<PackedVerticesFormat>  _pFormat;
    Ptr<TextureMap>            _pDefaultTexWhite;
    Ptr<TextureMap>            _pDefaultTexBump;
    IRenderTarget *            _pRenderTarget;
    //DebugSettings              _mode;
    RendererSettings           _settings;

    static const int32 FLAG_ADD_12 = 0x00000001;
    static const int32 FLAG_ADD_01 = 0x00000002;
    static const int32 FLAG_COLOR_COUNT  = FLAG_ADD_01 + FLAG_ADD_12 + 1;

    ShadersParams   _paramsAdd[FLAG_COLOR_COUNT];
    ShadersParams   _paramsLerp[FLAG_COLOR_COUNT];
    ShadersParams   _paramsAddLerp[FLAG_COLOR_COUNT];

    static const int32 FLAG_IS_NORMAL_DXT5_0 = 0x00000001;
    static const int32 FLAG_IS_NORMAL_DXT5_1 = 0x00000002;
    static const int32 FLAG_IS_NORMAL_DXT5_2 = 0x00000004;
    static const int32 FLAG_REFRAC_COUNT  = FLAG_IS_NORMAL_DXT5_0 + FLAG_IS_NORMAL_DXT5_1 + FLAG_IS_NORMAL_DXT5_2 + 1;

    ShadersParams   _paramsRefrac[FLAG_REFRAC_COUNT];

    void initialise(ShadersParams & params, Assets::ECustomMode mode, bool skin, int32 flags);
    
    void send(ShadersParams & params, const MeshInstanceSM2 & mesh, const Material & mat, ERenderPass pass);
    void send(const MeshInstanceSM2 & mesh, ERenderPass pass);
    void setTexture(const Ptr<Gfx::ShaderConstantTable> & pVConst, int32 idSampler, const Ptr<ITextureMap> & pTex, bool isNormal);
};
}
#endif /* STANDARDMATRENDERER_H_ */
