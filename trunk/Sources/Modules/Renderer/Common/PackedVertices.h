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
#ifndef RENDERER_COMMON_PACKEDVERTICESSET_H_
#define RENDERER_COMMON_PACKEDVERTICESSET_H_

#include <Core/Standard.h>
#include <Assets/VerticeSet.h>
#include <Assets/Vertex.h>
#include <Gfx/IDevice.h>

namespace Renderer
{
/**
 * TEXCOORD0 : texcoord
 * TEXCOORD1 : texcoord2
 * TEXCOORD2 :
 * TEXCOORD3 :
 * TEXCOORD5 : tangent (w : signe bitangente)
 * TEXCOORD6 : bonesId
 * TEXCOORD7 : bonesWeight
 */

/**
 * G : Géometrie (position, normale, texture coords, bones id, bones weight)
 * B : Bump      (tangente, bitangente)
 */
LM_ENUM_2(ESkinnedVertexStreamData,
    VD_SKIN_G,
    VD_SKIN_G_B);

/**
 * G : Géometrie (position, normale, texture coords)
 * B : Bump      (tangente, bitangente)
 */
LM_ENUM_2(EStaticVertexStreamData,
    VD_STATIC_G,
    VD_STATIC_G_B);
//-----------------------------------------------------------------------------
class LM_API_RDR IPackedStaticVertices
{
public:
    virtual ~IPackedStaticVertices() {};
    virtual void bindData(const Ptr<Gfx::IDevice> & pDevice, EStaticVertexStreamData data) const = 0;
    virtual void sendData(const Ptr<Gfx::IDevice> & pDevice, EStaticVertexStreamData data) const = 0;
};
//-----------------------------------------------------------------------------
class LM_API_RDR IPackedSkinnedVertices
{
public:
    virtual ~IPackedSkinnedVertices() {};
    virtual int32 getBlendingGroupCount() const = 0;
    virtual const Core::List<int32> getPalette(int32 groupID) const = 0;
    virtual void bindData(const Ptr<Gfx::IDevice> & pDevice, ESkinnedVertexStreamData data) const = 0;
    virtual void sendData(const Ptr<Gfx::IDevice> & pDevice, ESkinnedVertexStreamData data, int32 groupID) const = 0;
};
//-----------------------------------------------------------------------------
class LM_API_RDR PackedVerticesFormat
{
public:
    virtual ~PackedVerticesFormat() {};

    virtual bool initialise(const Ptr<Gfx::IDevice> & pDevice) = 0;

    virtual Ptr<Gfx::IVertexFormat> getVertexFormat(
            const Ptr<Gfx::IDevice> & pDevice,
            const Ptr<Gfx::IVertexShader> & pShader,
            EStaticVertexStreamData data) = 0;

    virtual Ptr<Gfx::IVertexFormat> getVertexFormat(
            const Ptr<Gfx::IDevice> & pDevice,
            const Ptr<Gfx::IVertexShader> & pShader,
            ESkinnedVertexStreamData data) = 0;

    virtual Ptr<IPackedStaticVertices> packStaticVerts(const Ptr<Gfx::IDevice> & pDevice, const Ptr<Assets::VerticeSet> & pVertices) = 0;
    virtual Ptr<IPackedSkinnedVertices> packSkinnedVerts(const Ptr<Gfx::IDevice> & pDevice, const Ptr<Assets::VerticeSet> & pVertices) = 0;
    virtual bool packedNormals() const = 0;
};
//-----------------------------------------------------------------------------
}

#endif /* PACKEDVERTICESSET_H_ */
