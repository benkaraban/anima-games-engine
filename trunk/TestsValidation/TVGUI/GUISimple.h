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
#ifndef GUISIMPLE_H_
#define GUISIMPLE_H_

#include <GUI/Manager.h>
#include <GUI/Button.h>
#include <GUI/ComboBox.h>

#include "ITestGUI.h"

class GUISimple : public ITestGUI, public GUI::IEventHandler
{
public:
    virtual void initalise(
            const Ptr<Renderer::IRenderer> & pRenderer,
            const Ptr<Window::Window> & pRenderWindow,
            const Ptr<Core::VirtualFileSystem> & pVFS,
            const Ptr<Window::InputManager> & pInput);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

protected:
    virtual void update(double elapsed);
    virtual void render(const Ptr<Renderer::IRenderView> & pView);

    Ptr<Renderer::IRenderer>     _pRenderer;
    Ptr<Core::VirtualFileSystem> _pVFS;
    double                       _elapsed;

    Ptr<GUI::Manager>           _pGUI;
    Ptr<GUI::Widget>            _pDialog1;
    Ptr<GUI::Widget>            _pModalDialog1;

    Ptr<GUI::Button>            _pButton1;
    Ptr<GUI::Button>            _pButton2;

    Ptr<GUI::Button>            _pButtonOK;
    Ptr<GUI::Button>            _pButtonCancel;

    Ptr<GUI::ComboBox>          _pCombo;

    Ptr<Universe::NodeMesh>     _pMesh1;
};


#endif /* WORLD2_H_ */
