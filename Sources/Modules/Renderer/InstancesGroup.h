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
#ifndef RENDERER_INSTANCESGROUP_H_
#define RENDERER_INSTANCESGROUP_H_

#include <Renderer/IFreeForm.h>

namespace Renderer
{
struct LM_API_RDR Instance
{
    Core::Matrix4f  worldMat;
    Core::Vector4f  color;
    Core::Vector4f  glow;
};

class LM_API_RDR InstancesGroup
{
public:
    InstancesGroup(const Ptr<IFreeForm> & pFreeForm, const Ptr<Assets::VerticeSet> & pVSet);
    Core::List<Instance> & beginUpdate() { return _instances; };
    void endUpdate();

    bool isAlive() const { return _pFreeForm->isAlive(); }
    void kill() { _pFreeForm->kill(); }

protected:
    Ptr<IFreeForm>              _pFreeForm;
    Ptr<Assets::VerticeSet>     _pVSet;
    Core::List<uint16>       &  _indices;
    Core::List<FreeFormVertex>  _vertices;
    Core::List<Instance>        _instances;
};

}

#endif
