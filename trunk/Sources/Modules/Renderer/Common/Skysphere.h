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
#ifndef RENDERER_SKYSPHERE_H_
#define RENDERER_SKYSPHERE_H_

#include <Assets/VerticeSet.h>
#include <Core/Standard.h>
#include <Gfx/IDevice.h>
#include <Renderer/IScene.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
class LM_API_RDR Skysphere : public Core::Object, public IGPUResource
{
public:
    Skysphere(const Ptr<Gfx::IDevice> & pDevice, int32 divLevel, const Ptr<GPUResourceLib> & pResLib);
    ~Skysphere();

    const SkysphereSettings & getSettings() const { return _settings; };

    virtual void update(const SkysphereSettings & settings);
    virtual void sendData(const Ptr<Gfx::IDevice> & pDevice) const;

    virtual void onDeviceLost();
    virtual void onDeviceReset();


    struct Vertex
    {
        Core::Vector3f position;
        float          texcoords[2];
        byte           padding[12];
    };

protected:
    void generateSphere(const SkysphereSettings & settings);

    Ptr<Gfx::IDevice>         _pDevice;
    Ptr<GPUResourceLib>       _pResLib;

    int32                     _divLevel;
    SkysphereSettings         _settings;
    Assets::VerticeSet        _vertices;
    Ptr<Gfx::IVertexBuffer>   _pVBuffer;
    Ptr<Gfx::IIndexBuffer>    _pIBuffer;
    mutable bool              _isBufferUpdateNeeded;
};
}

#endif /* SKYSPHERE_H_ */
