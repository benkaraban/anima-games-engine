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
#include <Core/VFS/VFSXMLTranslator.h>

#include "AppTVGUI.h"
#include "GUISimple.h"
#include "ScrollBarTest.h"
#include <Assets/GeometricPrimitive.h>

//-----------------------------------------------------------------------------
AppTVGUI::AppTVGUI()
{
}
//-----------------------------------------------------------------------------
AppTVGUI::~AppTVGUI()
{
    _pTextFPS = null;
    _pFontFPS = null;
    _pHUD = null;

    _pRenderer = null;
    _pDriver = null;
    _pRenderWindow = null;
    _pVFS = null;

    _pWinHandler = null;
    _pInput = null;
}
//-----------------------------------------------------------------------------
bool AppTVGUI::initialise()
{
    const bool FULLSCREEN = false;

    Core::String workDir(LM_WORKING_DIRECTORY);
    SetCurrentDirectory(Core::String8(workDir).c_str());

    // Initialisation VFS ---------------------------------------------

    {
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./Data/VFS-TVUniverse.xml"))
        {
            _pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
            INF << L"VirtualFileSystem layout :\n";
            INF << _pVFS->toString() << "\n";
        }
        else
            throw Core::Exception(L"Unable to mount VFS.");

    }

    _pInput = Ptr<Window::InputManager>(new Window::InputManager());
    _pWinHandler = Ptr<Window::WinHandler>(new Window::WinHandler(_pInput));
    _pRenderWindow = Ptr<Window::Window>(new Window::Window(
            getInstance(),
            L"TVUniverse",
            100, 100,    // position
            800, 600,    // dimension
            FULLSCREEN,  // fullscreen
            _pWinHandler));

    _pInput->initialise(_pRenderWindow->getHandle());
    _pDriver = Gfx::IDriverPtr(new Gfx::DriverDx9());

    Renderer::RendererSettings rs(Renderer::GLOBAL_VERY_HIGH);
    rs.fullscreen = FULLSCREEN;
    rs.width = _pRenderWindow->getWidth();
    rs.height = _pRenderWindow->getHeight();

    _pRenderer = Ptr<Renderer::IRenderer>(new Renderer::RendererSM2(
            _pDriver,
            rs,
            _pRenderWindow->getHandle(),
            _pVFS,
            L"/Shaders"));

    bool result = _pRenderer->initialise();

    if(result)
    {
        _pTestSimple = Ptr<ITestGUI>(new GUISimple());
        _pTestScrollBar = Ptr<ITestGUI>(new GUIScrollBar());

        setTest(_pTestScrollBar);
    }

    // Compteur FPS

    _pHUD = _pRenderer->createHUD();
    _pFontFPS = _pRenderer->createFont(L"Arial", 10);
    _pTextFPS = _pHUD->createText(_pFontFPS, L"");
    _pTextFPS->setColor(Core::Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
    _pTextFPS->setPosition(Core::Vector2f(5.0f, 5.0f));

    return result;
}
//-----------------------------------------------------------------------------
void AppTVGUI::setTest(const Ptr<ITestGUI> & pTest)
{
    try
    {
        if(!pTest->isInitialised())
        {
            pTest->initalise(_pRenderer, _pRenderWindow, _pVFS, _pInput);
            pTest->setInitialised();
        }

        _pCurrentTest = pTest;
    }
    catch(Core::Exception & e)
    {
        ERR << L"Error initialising world : " << e.getMessage() << L"\n";
        ERR << e.getCallStack();
    }
}
//-----------------------------------------------------------------------------
void AppTVGUI::update(double elapsed)
{
    if(_pInput->isKeyTyped(VK_ESCAPE))
        PostQuitMessage(0);

    if(_pCurrentTest != null)
    {
        _pCurrentTest->update(elapsed);
    }
}
//-----------------------------------------------------------------------------
void AppTVGUI::render()
{
    if(_pInput->isKeyTyped(VK_ESCAPE))
        PostQuitMessage(0);

    if(_pCurrentTest != null)
    {
        Ptr<Renderer::IRenderView> pView(_pRenderer->getDefaultView());
        pView->resize(_pWinHandler->getWindowWidth(), _pWinHandler->getWindowHeight());

        _pCurrentTest->render(pView);

        _fps.nextFrame();
        _pTextFPS->setText(_fps.getText());

        _pRenderer->renderHUD(pView, _pHUD);
        pView->present();
    }
}
//-----------------------------------------------------------------------------
