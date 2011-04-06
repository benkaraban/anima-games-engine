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
#ifndef SCENETEAPOT_H_
#define SCENETEAPOT_H_

#include "ITestScene.h"

class SceneTeapot : public ITestScene
{
public:
    SceneTeapot(const TestSceneData & data);
    virtual bool initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow);
    virtual void render();

protected:
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot01;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot02;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot03;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot04;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot05;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot06;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot07;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot08;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot09;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot10;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot11;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot12;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot13;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot14;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot15;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot16;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot17;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot18;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot19;
    Ptr<Renderer::IMeshInstance>      _pMeshInstanceTeapot20;

    Core::Matrix4f _worldMatTeapot;
};


#endif /* SCENETEAPOT_H_ */
