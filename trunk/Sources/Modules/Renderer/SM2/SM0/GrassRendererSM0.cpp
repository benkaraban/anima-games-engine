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
#include <Core/logger.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/SM2/SM0/GrassRendererSM0.h>

namespace Renderer
{

const int32 MAX_GRASS_COUNT = 500;
const int32 MAX_VERTEX_COUNT = MAX_GRASS_COUNT * VERTEX_BY_GRASS;
const int32 VERTEX_BUFFER_SIZE = MAX_VERTEX_COUNT * sizeof(GrassVertex);

const int32 INDEX_COUNT = MAX_GRASS_COUNT * 4 * 6; // 4 quads de 3 triangles (6 indices)
const int32 INDEX_BUFFER_SIZE = INDEX_COUNT * sizeof(uint16);

GrassRendererSM0::GrassRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                             const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
GrassRendererSM0::~GrassRendererSM0()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool GrassRendererSM0::initialise()
{
    bool result = true;

    try
    {
        // Shaders

        Core::List<Gfx::ShaderMacro> macros;

        _pVShader = _pShaderLib->getVShader(L"grass-fixed.vsh", Gfx::VS_V1_1, L"vs_main", macros);

        _pVConst = _pVShader->getConstantTable();

        _idWorldViewProj = _pVConst->getConstantIndex(L"WorldViewProj");
        _idWorldView = _pVConst->getConstantIndex(L"WorldView");
        _idFogRange = _pVConst->getConstantIndex(L"FogRange");

        _idWindDirU = _pVConst->getConstantIndex(L"WindDirU");
        _idWindDirV = _pVConst->getConstantIndex(L"WindDirV");
        _idWindWaveLength = _pVConst->getConstantIndex(L"WindWaveLength");
        _idWindPhase = _pVConst->getConstantIndex(L"WindPhase");
        _idWindIntensity = _pVConst->getConstantIndex(L"WindIntensity");

        // Format

        Gfx::VertexFormatDesc  formatDesc;
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT3,  Gfx::VAU_POSITION);
        formatDesc.addAttribut(0, Gfx::VAT_UBYTE4N, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT3,  Gfx::VAU_TEXTURE_COORD, 1); // center position

        _pFormat = _pDevice->createVertexFormat(formatDesc, _pVShader);

        // State

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(true, true, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
        Gfx::RSSamplerDesc      sampler(Gfx::TEXTURE_ADDRESS_CLAMP);
        setSampler(sampler, _settings.filterLevel);

        sampler.isSRGB = true;

        blend.sRGBWriteEnabled = true;
        blend.alphaTestEnabled = true;
        blend.alphaTestFunc = Gfx::COMPARISON_GREATER_EQUAL;
        blend.alphaTestRef = 64;

        _state = _pDevice->createState(raster, depth, blend);
        _state.ptSampler[0] = _pDevice->createState(sampler);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing GrassRendererSM0 : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void GrassRendererSM0::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void GrassRendererSM0::onDeviceReset()
{
    // Buffers

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, VERTEX_BUFFER_SIZE);
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, INDEX_BUFFER_SIZE);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 ii=0, jj=0; ii < INDEX_COUNT; ii += 6, jj += 4)
    {
        pIndices[0] = jj + 0; pIndices[1] = jj + 1; pIndices[2] = jj + 2;
        pIndices[3] = jj + 0; pIndices[4] = jj + 2; pIndices[5] = jj + 3;

        pIndices += 6;
    }

    _pIBuffer->unlock();
}
//-----------------------------------------------------------------------------
void GrassRendererSM0::startContext(const RenderContext & context, ERenderPass pass)
{
    _view = context.getView();
    _viewProj = context.getViewProj();
    _fogSettings = context.getFog();

    for(int32 ii=0; ii < WIND_COUNT; ii++)
        _windSettings[ii] = context.getWindSettings(ii);

    _time = context.getTime();
    _commands.clear();

    if(pass == PASS_LIGHTING || pass == PASS_REFLECTION)
    {
        const Core::List<GrassCluster *> & clusters = context.getGrassClusters();

        Command command;
        command.pass      = pass;
        command.mode      = CMD_TRANS;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < clusters.size(); ii++)
        {
            GrassCluster & cluster = *clusters[ii];

            command.camDist   = Core::dot((cluster.getBoundingBox().getCenter() - context.getEye()), context.getEyeDir());
            command.pExecData = (void*)&cluster;

            _commands.push_back(command);
        }
    }
}
//-----------------------------------------------------------------------------
void GrassRendererSM0::endContext()
{
}
//-----------------------------------------------------------------------------
void GrassRendererSM0::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void GrassRendererSM0::exec(Command * pStart, Command * pEnd)
{
    Core::Vector4f windDirU(
            _windSettings[0].direction.x,
            _windSettings[1].direction.x,
            _windSettings[2].direction.x,
            _windSettings[3].direction.x
    );

    Core::Vector4f windDirV(
            _windSettings[0].direction.z,
            _windSettings[1].direction.z,
            _windSettings[2].direction.z,
            _windSettings[3].direction.z
    );

    Core::Vector4f windWaveLength(
            _windSettings[0].waveLength / f_PI_MUL_2,
            _windSettings[1].waveLength / f_PI_MUL_2,
            _windSettings[2].waveLength / f_PI_MUL_2,
            _windSettings[3].waveLength / f_PI_MUL_2
    );

    Core::Vector4f windIntensity(
            _windSettings[0].intensity,
            _windSettings[1].intensity,
            _windSettings[2].intensity,
            _windSettings[3].intensity
    );

    Core::Vector4f windPhase(
            _windSettings[0].phase + f_PI_MUL_2 * Core::L_frac(_time / _windSettings[0].period),
            _windSettings[1].phase + f_PI_MUL_2 * Core::L_frac(_time / _windSettings[1].period),
            _windSettings[2].phase + f_PI_MUL_2 * Core::L_frac(_time / _windSettings[2].period),
            _windSettings[3].phase + f_PI_MUL_2 * Core::L_frac(_time / _windSettings[3].period)
    );

    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);

    _pVConst->setConstant(_idWindDirU, windDirU);
    _pVConst->setConstant(_idWindDirV, windDirV);
    _pVConst->setConstant(_idWindWaveLength, windWaveLength);
    _pVConst->setConstant(_idWindIntensity, windIntensity);
    _pVConst->setConstant(_idWindPhase, windPhase);

    _pVConst->setConstant(_idFogRange, fogRange);

    _pVConst->setConstant(_idWorldViewProj, _viewProj);
    _pVConst->setConstant(_idWorldView, _view);

    while(pStart != pEnd)
    {
        GrassCluster & cluster = *(GrassCluster*)pStart->pExecData;       
        Ptr<ITextureMap> pTexture = cluster.getTexture();

        _pDevice->setState(_state);

        _pDevice->setVertexFormat(_pFormat);
        _pDevice->setVertexShader(_pVShader);
        _pDevice->setPixelShader(null);

        Gfx::RSFixedFunc ff;
        ff.pTex0 = (LM_DEBUG_PTR_CAST<TextureMap>(pTexture))->getResource();
        ff.fogEnabled = true;
        ff.fogColor = _fogSettings.getColor();

        _pDevice->setFixedFuncPipe(ff);

        cluster.sendData(_pDevice);

        pStart++;
    }
}
//-----------------------------------------------------------------------------
}
