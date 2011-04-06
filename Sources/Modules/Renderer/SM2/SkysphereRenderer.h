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
#ifndef RENDERER_SKYSPHERERENDERER_H_
#define RENDERER_SKYSPHERERENDERER_H_

#include <Gfx/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/Skysphere.h>
#include <Renderer/SM2/InternalSettings.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/SM2/ICommandExecuter.h>

namespace Renderer
{
class LM_API_SM2 ISkysphereRenderer : public ICommandExecuter
{
public:
    virtual ~ISkysphereRenderer() {};
    virtual bool initialise() = 0;
    virtual void generateSkyBox(const SkysphereSettings & settings, SkyboxInfos & skybox) = 0;
};

class LM_API_SM2 SkysphereRenderer : public Core::Object, public ISkysphereRenderer
{
public:
    SkysphereRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib);
    virtual bool initialise();

    virtual void startContext(const RenderContext & context, ERenderPass pass);
    virtual void endContext();
    
    virtual void enqueueCommands(Core::List<Command> & commands);
    virtual void exec(Command * pStart, Command * pEnd);

    virtual void generateSkyBox(const SkysphereSettings & settings, SkyboxInfos & skybox);

protected:
    struct ShaderParams
    {
        Ptr<Gfx::IVertexShader>       pVShader;
        Ptr<Gfx::IPixelShader>        pPShader;
        Ptr<Gfx::ShaderConstantTable> pVConst;
        Ptr<Gfx::ShaderConstantTable> pPConst;
        Ptr<Gfx::IVertexFormat>       pFormat;

        Gfx::GlobalRenderState     state;

        int32     idWorldViewProj;
        int32     idColor;
        int32     idSunColor;
        int32     idSkyColor;
        int32     idFogColor;
        int32     idFogOpacity;
        int32     idMainLightDir;
        int32     idMainLightDiffuse;
        int32     idSampler;
    };

    void initialise(ShaderParams & params, ERenderPass pass, ELightingModel model, bool isProcedural);

    Ptr<Gfx::IDevice>          _pDevice;
    Ptr<ShaderLib>             _pShaderLib;

    ShaderParams               _paramsTexture[ERenderPass_COUNT][ELightingModel_COUNT];
    ShaderParams               _paramsProcedural[ERenderPass_COUNT][ELightingModel_COUNT];

    InternalFogSettings        _fogSettings;
    InternalMainLightSettings  _lightSettings;

    ERenderPass                 _pass;
    Core::List<Command>         _commands;

    Core::Matrix4f _view;
    Core::Matrix4f _proj;
};
}

#endif /* SKYSPHERERENDERER_H_ */
