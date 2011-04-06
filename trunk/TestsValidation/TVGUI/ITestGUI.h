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
#ifndef IWORLDTEST_H_
#define IWORLDTEST_H_

#include <Core/Math/CameraController.h>
#include <Universe/World.h>
#include <Window/InputManager.h>
#include <Window/Window.h>

class ITestGUI
{
public:
    ITestGUI();
    virtual ~ITestGUI() {};

    virtual void initalise(
            const Ptr<Renderer::IRenderer> & pRenderer,
            const Ptr<Window::Window> & pRenderWindow,
            const Ptr<Core::VirtualFileSystem> & pVFS,
            const Ptr<Window::InputManager> & pInput) = 0;

    virtual void update(double elapsed);
    virtual void render(const Ptr<Renderer::IRenderView> & pView);

    bool isInitialised() const { return _isInitialised; };
    void setInitialised() { _isInitialised = true; };

protected:
    Core::CameraQuake                    _camQuake;
    Ptr<Universe::NodePerspectiveCamera> _pCamera;
    double                               _camSpeed;

    Ptr<Universe::World>      _pWorld;
    Ptr<Window::InputManager> _pInput;
    bool                      _isInitialised;
};

#endif /* IWORLDTEST_H_ */
