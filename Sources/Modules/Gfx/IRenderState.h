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
#ifndef IRENDERSTATE_H_
#define IRENDERSTATE_H_

#include <Core/Standard.h>
#include <Core/List.h>

#include <Gfx/GfxType.h>
#include <Gfx/IShader.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
class LM_API_GFX IVertexFormat
{
public:
    virtual ~IVertexFormat();
    virtual VertexFormatDesc getDesc() const = 0;
};
typedef Core::Ptr<IVertexFormat> IVertexFormatPtr;
//-----------------------------------------------------------------------------
class LM_API_GFX IRSRasterizer
{
public:
    IRSRasterizer(const RSRasterizerDesc& otherDesc);
    virtual ~IRSRasterizer();

    RSRasterizerDesc getDesc() const;

protected:
    RSRasterizerDesc _desc;
};
typedef Core::Ptr<IRSRasterizer > IRSRasterizerPtr;
//-----------------------------------------------------------------------------
class LM_API_GFX IRSBlend
{
public:
    IRSBlend(const RSBlendDesc& otherDesc);
    virtual ~IRSBlend();

    RSBlendDesc getDesc() const;

protected:
    RSBlendDesc _desc;
};
typedef Core::Ptr<IRSBlend > IRSBlendPtr;
//-----------------------------------------------------------------------------
class LM_API_GFX IRSDepthStencil
{
public:
    IRSDepthStencil(const RSDepthStencilDesc& otherDesc);
    virtual ~IRSDepthStencil();

    RSDepthStencilDesc getDesc() const;

protected:
    RSDepthStencilDesc _desc;
};
typedef Core::Ptr<IRSDepthStencil > IRSDepthStencilPtr;
//-----------------------------------------------------------------------------
class LM_API_GFX IRSSampler
{
public:
    IRSSampler(const RSSamplerDesc& otherDesc);
    virtual ~IRSSampler();

    RSSamplerDesc getDesc() const;

protected:
    RSSamplerDesc _desc;
};
typedef Core::Ptr<IRSSampler > IRSSamplerPtr;
//-----------------------------------------------------------------------------
const int32 MAX_RS_SAMPLER = 16;
struct GlobalRenderState
{
    IRSRasterizerPtr    pRasterizer;
    IRSDepthStencilPtr  pDepthStencil;
    IRSBlendPtr         pBlend;
    IRSSamplerPtr       ptSampler[MAX_RS_SAMPLER];
};
//-----------------------------------------------------------------------------
struct LM_API_GFX RSFixedFunc
{
    RSFixedFunc();

    Core::Matrix4f  world;
    Core::Matrix4f  view;
    Core::Matrix4f  proj;

    bool            fogEnabled;
    Core::Vector4f  fogColor;

    Ptr<IShaderResourceView>    pTex0;
    Ptr<IShaderResourceView>    pTex1;
};
} //namepsace Gfx

#endif /* IRENDERSTATE_H_ */
