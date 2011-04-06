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
#include "IDevice.h"

namespace Gfx
{
//-----------------------------------------------------------------------------
IDevice::IDevice()
{
}
//-----------------------------------------------------------------------------
IPixelShaderBinObjectPtr  IDevice::compilePShader(const String& code, EPixelShaderVersion version, const String& entryPoint, const Ptr<IShaderIncluder> & pIncluder)
{
    Core::List<ShaderMacro> macros;
    return compilePShader(code, version, entryPoint, macros, pIncluder);
}
//-----------------------------------------------------------------------------
IVertexShaderBinObjectPtr IDevice::compileVShader(const String& code, EVertexShaderVersion version, const String& entryPoint, const Ptr<IShaderIncluder> & pIncluder)
{
    Core::List<ShaderMacro> macros;
    return compileVShader(code, version, entryPoint, macros, pIncluder);
}
//-----------------------------------------------------------------------------
GlobalRenderState IDevice::createState(const RSRasterizerDesc & raster,
                                       const RSDepthStencilDesc& depth,
                                       const RSBlendDesc& blend)
{
    GlobalRenderState result;

    result.pRasterizer = createState(raster);
    result.pDepthStencil = createState(depth);
    result.pBlend = createState(blend);

    for(int32 ii=0; ii < MAX_RS_SAMPLER; ii++)
        result.ptSampler[ii] = null;

    return result;
}
//-----------------------------------------------------------------------------
void IDevice::setState(const GlobalRenderState & state)
{
    setState(state.pRasterizer);
    setState(state.pDepthStencil);
    setState(state.pBlend);
    for(int32 ii=0; ii < MAX_RS_SAMPLER; ii++)
        if(state.ptSampler[ii] != null)
            setState(ii, state.ptSampler[ii]);
}
//-----------------------------------------------------------------------------
IDevice::~IDevice()
{
}
//-----------------------------------------------------------------------------
}//namespace Gfx
