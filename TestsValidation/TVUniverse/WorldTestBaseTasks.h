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
#ifndef WORLD2_H_
#define WORLD2_H_

#include "IWorldTest.h"

class WorldTestBaseTasks : public IWorldTest
{
public:
    virtual void initalise(
            const Ptr<Renderer::IRenderer> & pRenderer,
            const Ptr<Core::VirtualFileSystem> & pVFS,
            const Ptr<Window::InputManager> & pInput,
            const Ptr<Audio::IDevice> & pDevice);

protected:
    virtual void alignMeshes(float x, const Core::Vector3f & dir, const Core::Vector3f & scale);
    virtual void alignDecals();
    virtual void addTask(const Ptr<Universe::ITask> & pTask);
    virtual void update(double elapsed);

    Ptr<Renderer::IRenderer>     _pRenderer;
    Ptr<Core::VirtualFileSystem> _pVFS;
    double                       _elapsed;
    bool                         _pingPong;

    Ptr<Universe::NodeMesh>     _pMesh1;
    Ptr<Universe::NodeMesh>     _pMesh2;
    Ptr<Universe::NodeMesh>     _pMesh3;
    Ptr<Universe::NodeMesh>     _pMesh4;

    Ptr<Universe::NodeDecal>     _pDecal1;
    Ptr<Universe::NodeDecal>     _pDecal2;
    Ptr<Universe::NodeDecal>     _pDecal3;
    Ptr<Universe::NodeDecal>     _pDecal4;

    Core::List<Ptr<Universe::ITask> > _tasks;
};


#endif /* WORLD2_H_ */
