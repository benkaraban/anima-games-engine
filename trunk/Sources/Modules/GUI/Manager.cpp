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
#include <GUI/Manager.h>

namespace GUI
{
//-------------------------------------------------------------------------
Manager::Manager(const Ptr<Core::VirtualFileSystem> & pVFS,
            const Ptr<Window::Window> & pWindow,
            const Ptr<Window::InputManager> & pInput,            
            const Ptr<Renderer::IRenderer> & pRenderer,
            const Ptr<Renderer::ITextureGrabber> & pTexGrabber
            ) :
    _pWindow(pWindow),
    _pRenderer(pRenderer),
    _pResPool(new ResourcePool(pVFS, pRenderer, pTexGrabber)),
    _pInput(pInput),
    _pHUD(pRenderer->createHUD()),
    _xSpot(0),
    _ySpot(0),
    _lastMousePos(-1.0f, -1.0f),
    _screenWidth(0),
    _screenHeight(0)
{
    _keyNav.ignoreMouse = false;
    _keyNav.focusJustChanged = false;
}
//-------------------------------------------------------------------------
void Manager::setDialog(const Ptr<Widget> & pDialog)
{
    for(int32 ii=0; ii < _dialogStack.size(); ii++)
    {
        _dialogStack[ii]->setParentActive(false);
        _dialogStack[ii]->setParentVisible(false);
    }

    _dialogStack.clear();
    _focus.setOwner(NULL);
    pushDialog(pDialog);
}
//-------------------------------------------------------------------------
void Manager::reset()
{
    _focus.setOwner(NULL);
    for(int32 ii=0; ii < _dialogStack.size(); ii++)
    {
        _dialogStack[ii]->setParentActive(false);
        _dialogStack[ii]->setParentVisible(false);
    }

    _dialogStack.clear();
}
//-------------------------------------------------------------------------
void Manager::pushDialog(const Ptr<Widget> & pDialog)
{
    if(!_dialogStack.empty())
    {
        _dialogStack.back()->setParentActive(false);
    }

    _focus.setOwner(NULL);
    _dialogStack.push_back(pDialog);
    pDialog->setParentActive(true);
    pDialog->setParentVisible(true);
    pDialog->setParentPriority((_dialogStack.size() - 1) * 10000);
    pDialog->startAsDialog();
}
//-------------------------------------------------------------------------
void Manager::popDialog()
{
    LM_ASSERT(!_dialogStack.empty());

    _dialogStack.back()->setParentActive(false);
    _dialogStack.back()->setParentVisible(false);

    _focus.setOwner(NULL);
    _dialogStack.pop_back();

    if(!_dialogStack.empty())
    {
        _dialogStack.back()->setParentActive(true);
        _dialogStack.back()->setParentVisible(true);
    }
}
//-------------------------------------------------------------------------
void Manager::setCursor(const Ptr<Core::Bitmap> & pCursor, int32 xSpot, int32 ySpot)
{
    _pCursor = pCursor;
    _xSpot = xSpot;
    _ySpot = ySpot;
}
//-------------------------------------------------------------------------
void Manager::update(double elapsed)
{
    _pWindow->setCursor(_pCursor, _xSpot, _ySpot);
    _keyNav.focusJustChanged = false;

    if(_lastMousePos == Core::Vector2f(-1.0f, -1.0f))
        _lastMousePos = _pInput->getMousePos();

    if(_lastMousePos != _pInput->getMousePos())
    {
        _keyNav.ignoreMouse = false;
        _lastMousePos = _pInput->getMousePos();
    }

    for(int32 ii=0; ii < _dialogStack.size(); ii++)
        _dialogStack[ii]->update(elapsed);

    if(_focus.getOwner())
    {
        if(_pInput->isKeyTyped(VK_TAB))
        {
            if(_pInput->isKeyDown(VK_SHIFT))
                _focus.setOwner(_focus.getOwner()->getPrevWidget());
            else
                _focus.setOwner(_focus.getOwner()->getNextWidget());
        }
    }
}
//-------------------------------------------------------------------------
void Manager::centerScreen(const Ptr<Widget> & pWidget)
{
    Core::Vector2f size = pWidget->getSize();
    Core::Vector2f pos;
    pos.x = 0.5f * (float(_screenWidth) - size.x);
    pos.y = 0.5f * (float(_screenHeight) - size.y);
    pWidget->setPosition(pos);
}
//-------------------------------------------------------------------------
void Manager::render(const Ptr<Renderer::IRenderView> & pView)
{
    _screenWidth = pView->getWidth();
    _screenHeight = pView->getHeight();
    _pRenderer->renderHUD(pView, _pHUD);
}
//-------------------------------------------------------------------------
}

