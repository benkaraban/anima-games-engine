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
#ifndef GUI_MANAGER_H_
#define GUI_MANAGER_H_

#include <GUI/ResourcePool.h>
#include <GUI/Widget.h>

#include <Renderer/IHUD.h>

#include <Window/Window.h>
#include <Window/InputManager.h>

namespace GUI
{
class LM_API_GUI Manager
{
public:
    Manager(const Ptr<Core::VirtualFileSystem> & pVFS,
            const Ptr<Window::Window> & pWindow,
            const Ptr<Window::InputManager> & pInput,            
            const Ptr<Renderer::IRenderer> & pRenderer,
            const Ptr<Renderer::ITextureGrabber> & pTexGrabber
        );

    Ptr<Renderer::IRenderer>  getRenderer() const { return _pRenderer; };
    Ptr<Window::InputManager> getInput() const { return _pInput; };
    Ptr<Renderer::IHUD> getHUD() const { return _pHUD; };
    Ptr<ResourcePool> getResPool() const { return _pResPool; };
    Focus * getFocus() { return &_focus; };

    void setDialog(const Ptr<Widget> & pDialog);
    void reset();

    void pushDialog(const Ptr<Widget> & pDialog);
    void popDialog();

    void update(double elapsed);
    void render(const Ptr<Renderer::IRenderView> & pView);

    void centerScreen(const Ptr<Widget> & pWidget);

    void setCursor(const Ptr<Core::Bitmap> & pCursor, int32 xSpot, int32 ySpot);

    KeyNavigation * getKeyNavigation() { return &_keyNav; }

protected:
    Ptr<Window::Window>       _pWindow;
    Ptr<Renderer::IRenderer>  _pRenderer;
    Ptr<ResourcePool>         _pResPool;
    Ptr<Window::InputManager> _pInput;
    Ptr<Renderer::IHUD>       _pHUD;

    Ptr<Core::Bitmap>         _pCursor;
    int32                     _xSpot;
    int32                     _ySpot;

    Core::Vector2f            _lastMousePos;
    int32                     _screenWidth;
    int32                     _screenHeight;

    Core::List<Ptr<Widget> >  _dialogStack;
    Focus                     _focus;
    KeyNavigation             _keyNav;
};
}

#endif
