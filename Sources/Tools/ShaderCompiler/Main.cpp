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
#include <Core/VFS/VFSDirectMapping.h>
#include <Gfx/GfxDx9/DriverDx9.h>
#include <Renderer/SM2/RendererSM2.h>
#include <Window/Window.h>
#include <Window/WinHandler.h>
#include <Window/InputManager.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int32 result = 0;

    try
    {
        Core::String workDir(LM_WORKING_DIRECTORY);
        SetCurrentDirectory(Core::String8(workDir).c_str());

        Ptr<Core::VFSDirectMapping> pVFS(new Core::VFSDirectMapping());
        if(!pVFS->linkDirectory(L"."))
        {
            ERR << L"Unable to initialise VFS\n";
            return false;
        }

        Ptr<Window::InputManager> pInput(new Window::InputManager());
        Ptr<Window::WinHandler> pWinHandler(new Window::WinHandler(pInput));
        Ptr<Window::Window> pRenderWindow(new Window::Window(
                hInst,
                L"ShaderCompiler",
                0, 0,    // position
                //FULLSCREEN ? 1920 : 800, 
                //FULLSCREEN ? 1080 : 600,    // dimension
                10, 
                10,    // dimension
                false,  // fullscreen
                pWinHandler));

        pInput->initialise(pRenderWindow->getHandle());
        Gfx::IDriverPtr pDriver(new Gfx::DriverDx9());

        Renderer::RendererSettings rs(Renderer::GLOBAL_VERY_HIGH);
        rs.fullscreen = false;
        rs.width = pRenderWindow->getWidth();
        rs.height = pRenderWindow->getHeight();

        Ptr<Renderer::RendererSM2> pRenderer(new Renderer::RendererSM2(
                pDriver,
                rs,
                pRenderWindow->getHandle(),
                pVFS,
                L"/Shaders",
                0));

        bool result = pRenderer->initialise();
        pRenderer->compileAllShaders();
    }
    catch(Core::Exception & e)
    {
        ERR << L"Unhandled exception : " << e.getMessage() << L"\n";
        result = 1;
    }

    return result;
}
