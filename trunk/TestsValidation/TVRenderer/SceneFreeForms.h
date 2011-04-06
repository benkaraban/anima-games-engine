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
#ifndef SCENEFREEFORMS_H_
#define SCENEFREEFORMS_H_

#include "ITestScene.h"
#include <Renderer/InstancesGroup.h>

class SceneFreeForms : public ITestScene
{
public:
    SceneFreeForms(const TestSceneData & data);
    virtual bool initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow);
    virtual void update(double elapsed);
    virtual void render();

protected:
    struct CubeInst
    {
        // rotate around 0/0/0
        float       alpha;
        float       beta;

        float       dist;

        // rotate around self
        float       teta;
        float       zeta;

        // rotate around self
        float       dteta;
        float       dzeta;
    };

    Ptr<Assets::Model>              _pModel;
    Ptr<Renderer::InstancesGroup>   _pGroup;
    Core::List<CubeInst>            _cubes;
};

#endif /* SCENEDECALS_H_ */