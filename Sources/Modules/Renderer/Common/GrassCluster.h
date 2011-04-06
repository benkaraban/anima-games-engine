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
#ifndef RENDERER_GRASSCLUSTER_H_
#define RENDERER_GRASSCLUSTER_H_

#include <Core/List.h>
#include <Gfx/IDevice.h>
#include <Renderer/ITexture.h>
#include <Renderer/IGrassCluster.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
const int32 VERTEX_BY_GRASS = 3 * 4; // 3 quads en étoiles

struct GrassVertex
{
    Core::Vector3f   position;
    union
    {
        byte             texCoords[4];
        uint32           texCoordsUint;
    };
    Core::Vector3f   centerPos;
    byte             padding[4];
};

class LM_API_RDR GrassCluster : public Core::Object, public IGrassCluster, public IGPUResource
{
public:
    GrassCluster(const Ptr<Gfx::IDevice> & pDevice,
                 const Core::List<GrassPatch> & patches,
                 const Ptr<ITextureMap> & pTexture, 
                 const Ptr<GPUResourceLib> & pResLib);
    ~GrassCluster();

    virtual const Core::Matrix4f & getWorldMatrix() const { return _worldMatrix; };
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);

    const Ptr<ITextureMap> getTexture() const { return _pTexture; };
    const Core::List<GrassPatch> & getPatches() const { return _patches; };
    const Core::List<GrassVertex> & getVertices() const { return _vertices; };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    virtual void onDeviceLost();
    virtual void onDeviceReset();

    virtual void sendData(const Ptr<Gfx::IDevice> & pDevice);

protected:
    void updateVertices();

    Ptr<Gfx::IDevice>              _pDevice;
    Ptr<GPUResourceLib>            _pResLib;

    Core::List<GrassPatch>         _patches;
    Core::List<GrassVertex>        _vertices;

    Ptr<Gfx::IVertexBuffer>        _pVBuffer;
    Ptr<Gfx::IIndexBuffer>         _pIBuffer;
    bool                           _isDirty;

    Ptr<ITextureMap>               _pTexture;
    Core::Matrix4f                 _worldMatrix;

    Core::AABoxf                   _box;
    bool                           _isAlive;
    bool                           _isVisible;
};
}

#endif /* GRASSCLUSTER_H_ */
