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
#include "AppUpdater.h"

#include <Core/System/Files.h>
#include <Core/List.h>
//-----------------------------------------------------------------------------
void starter()
{
    String exeName = Core::System::getExecutableName();
    String exePath = Core::System::getPath(exeName);

    String DirName = L"HooUpdater";
    String tempDir = Core::System::getTempDir();
    String finalDir = tempDir+DirName+L"/";

    Core::List<String>  fileName;
    fileName.push_back(L"Core.dll");
    fileName.push_back(L"Network.dll");
    fileName.push_back(L"HOOUpdaterProtocol.dll");
    fileName.push_back(L"HooUpdater.exe");

    Core::System::createDirectory(finalDir);

    for (int32 ii=0; ii<fileName.size(); ++ii)
    {
        String sourceFileName = exePath + fileName[ii];
        String newFileName = finalDir + fileName[ii]; 
        BOOL res = CopyFileW(sourceFileName.c_str(), newFileName.c_str(), FALSE);
    }

    STARTUPINFO startup_info;
	memset(&startup_info, 0, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	PROCESS_INFORMATION process_info;


    String tmpExePathAndName = finalDir + L"HooUpdater.exe";
    BOOL res = CreateProcessW(tmpExePathAndName.c_str(), L" testCmd", NULL, NULL, false, DETACHED_PROCESS, NULL, finalDir.c_str(), (LPSTARTUPINFOW)&startup_info, &process_info);
}
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    String commandLine = String(String8(lpCmdLine));

/*
    if (commandLine == L"")
    {
        starter();
    }
    else*/
    {
        AppUpdater::createInstance(hInst);

        AppUpdater::Inst().createApplicationWindow();
        AppUpdater::Inst().launch();
    }

    return 0;
}
//-----------------------------------------------------------------------------
