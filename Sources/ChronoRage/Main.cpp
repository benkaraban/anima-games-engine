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
#include <ChronoRage.h>
#include <Core/Debug/SysException.h>

Ptr<Window::Window>         spWindow;
Ptr<ChronoRage::ChronoRage> spChronoRage;

int32 internalMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int32 result = 0;

    try
    {
        Core::Logger & log = *Core::LoggerManager::getInstance().getDefaultLogger();
        log.setShowTagAppTime(false);
        log.setShowTagIdMsg(false);
        log.setShowTagThreadId(false);
        log.setShowTagLocalTime(true);

        INF << L"---------------------------------------------------------------------\n";
#ifdef CHRONORAGE_DEMO
        INF << L" Chrono Rage DEMO start v0.9.0 build " << String(__DATE__) << L" " << String(__TIME__) << L"\n";
#else
        INF << L" Chrono Rage start v0.9.0 build " << String(__DATE__) << L" " << String(__TIME__) << L"\n";
#endif
        INF << L"---------------------------------------------------------------------\n";

        spChronoRage = Ptr<ChronoRage::ChronoRage>(new ChronoRage::ChronoRage());
        spChronoRage->setMainArgs(hInst, hPrevInstance, lpCmdLine, nCmdShow);
        spChronoRage->run();
        spChronoRage = null;
    }
    catch(Core::Exception & e)
    {
        ERR << L"Unhandled exception : " << e.getMessage() << L"\n";
#ifdef _DEBUG
        ERR << e.getCallStack() << L"\n";
#endif
        if(spChronoRage != null)
            spWindow = spChronoRage->getWindow();
        spChronoRage = null;
        Core::System::errorMessageBox(e.getMessage(), (spWindow == null) ? NULL : spWindow->getHandle());
        result = 1;
    }
    catch(...)
    {
        ERR << L"Unkown exception\n";
        if(spChronoRage != null)
            spWindow = spChronoRage->getWindow();
        spChronoRage = null;
        Core::System::errorMessageBox(L"Unkown exception", (spWindow == null) ? NULL : spWindow->getHandle());
        result = 2;
        int i = 0;
        ++i;
    }

    INF << L"---------------------------------------------------------------------\n";
    INF << L" Chrono exit with code 0x" << Core::toStringHex(result, 8) << L"\n";
    INF << L"---------------------------------------------------------------------\n";

    return result;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int32 result = 0;

    SYS_EXCEPTIONS_TRY
    {
        result = internalMain(hInst, hPrevInstance, lpCmdLine, nCmdShow);
    }
    SYS_EXCEPTIONS_CATCH
    {
        spWindow = spChronoRage->getWindow();
        spChronoRage = null;
        MessageBoxW((spWindow == null) ? NULL : (HWND)spWindow->getHandle(), L"System error : ", NULL, MB_OK | MB_ICONERROR);
        result = 3;
    }

    // Destruction effective de la fen�tre
    LM_ASSERT(spWindow.getCount() <= 1);
    spWindow = null;

	return result;
}
