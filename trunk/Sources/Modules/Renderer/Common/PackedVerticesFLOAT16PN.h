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
#ifndef RENDERER_COMMON_PACKEDVERTICESSETFLOAT16PN_H_
#define RENDERER_COMMON_PACKEDVERTICESSETFLOAT16PN_H_

#include <Core/Standard.h>
#include <Assets/VerticeSet.h>
#include <Assets/Vertex.h>
#include <Gfx/IDevice.h>
#include <Renderer/Common/PackedVertices.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
class LM_API_RDR PackedStaticVerticesFLOAT16PN : public Core::Object, public IPackedStaticVertices, public IGPUResource
{
public:
    PackedStaticVerticesFLOAT16PN(const Ptr<Gfx::IDevice> & pDevice, 
                                  const Ptr<GPUResourceLib> & pResLib,
                                  const Ptr<Assets::VerticeSet> & pVertices);
    virtual ~PackedStaticVerticesFLOAT16PN();

    virtual void bindData(const Ptr<Gfx::IDevice> & pDevice, EStaticVertexStreamData data) const;
    virtual void sendData(const Ptr<Gfx::IDevice> & pDevice, EStaticVertexStreamData data) const;

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:
    Ptr<Gfx::IDevice>        _pDevice;
    Ptr<GPUResourceLib>      _pResLib;
    Ptr<Assets::VerticeSet>  _pVertices;
    Ptr<Gfx::IVertexBuffer>  _pBufferGB;
    Ptr<Gfx::IIndexBuffer>   _pBufferIndex;
    int32                    _vertexCount;
    int32                    _triangleCount;
};
//-----------------------------------------------------------------------------
class LM_API_RDR PackedSkinnedVerticesFLOAT16PN : public Core::Object, public IPackedSkinnedVertices, public IGPUResource
{
public:
    PackedSkinnedVerticesFLOAT16PN(const Ptr<Gfx::IDevice> & pDevice, 
                                   const Ptr<GPUResourceLib> & pResLib,
                                   const Ptr<Assets::VerticeSet> & pVertices);
    virtual ~PackedSkinnedVerticesFLOAT16PN();

    virtual int32 getBlendingGroupCount() const;
    virtual const Core::List<int32> getPalette(int32 groupID) const;

    virtual void bindData(const Ptr<Gfx::IDevice> & pDevice, ESkinnedVertexStreamData data) const;
    virtual void sendData(const Ptr<Gfx::IDevice> & pDevice, ESkinnedVertexStreamData data, int32 groupID) const;

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:
    Ptr<Gfx::IDevice>        _pDevice;
    Ptr<GPUResourceLib>      _pResLib;
    Ptr<Assets::VerticeSet>  _pVertices;
    Core::List<Assets::BlendingGroup> _groups;
    Ptr<Gfx::IVertexBuffer>  _pBufferGB;
    Ptr<Gfx::IIndexBuffer>   _pBufferIndex;
    int32                    _vertexCount;
    int32                    _triangleCount;
};
//-----------------------------------------------------------------------------
class LM_API_RDR PackedVerticesFormatFLOAT16PN : public Core::Object, public PackedVerticesFormat
{
public:
    PackedVerticesFormatFLOAT16PN(const Ptr<GPUResourceLib> & pResLib);
    virtual ~PackedVerticesFormatFLOAT16PN() {};

    virtual bool initialise(const Ptr<Gfx::IDevice> & pDevice);

    virtual Ptr<Gfx::IVertexFormat> getVertexFormat(
            const Ptr<Gfx::IDevice> & pDevice,
            const Ptr<Gfx::IVertexShader> & pShader,
            EStaticVertexStreamData data);

    virtual Ptr<Gfx::IVertexFormat> getVertexFormat(
            const Ptr<Gfx::IDevice> & pDevice,
            const Ptr<Gfx::IVertexShader> & pShader,
            ESkinnedVertexStreamData data);

    virtual Ptr<IPackedStaticVertices> packStaticVerts(const Ptr<Gfx::IDevice> & pDevice, const Ptr<Assets::VerticeSet> & pVertices);
    virtual Ptr<IPackedSkinnedVertices> packSkinnedVerts(const Ptr<Gfx::IDevice> & pDevice, const Ptr<Assets::VerticeSet> & pVertices);
    virtual bool packedNormals() const;

    struct VertexSkinGB
    {
        Core::Vector3f   position;       // 12 - 12
        uint8            normal[4];      //  4 - 16
        Assets::float16  texcoord[2];    //  4 - 20
        uint8            tangent[4];     //  4 - 24
        uint8            bonesId[4];     //  4 - 28
        uint8            bonesWeight[4]; //  4 - 32
    };

    struct VertexStaticGB
    {
        Core::Vector3f   position;       // 12 - 12
        uint8            normal[4];      //  4 - 16
        Assets::float16  texcoord[2];    //  4 - 20
        uint8            tangent[4];     //  4 - 24
        byte             padding[8];     //  8 - 32
    };
protected:
    Ptr<GPUResourceLib>    _pResLib;

    Gfx::VertexFormatDesc  _descSkinG;
    Gfx::VertexFormatDesc  _descSkinGB;

    Gfx::VertexFormatDesc  _descStaticG;
    Gfx::VertexFormatDesc  _descStaticGB;
};
//-----------------------------------------------------------------------------
}

#endif /* PACKEDVERTICESSET_H_ */
