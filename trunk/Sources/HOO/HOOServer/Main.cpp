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
#include <Core/Logger.h>
#include <Core/System/Files.h>
#include <HOOServer/Server.h>

int main(int argc, char **argv)
{
    HOOServer::Server * pServer = NULL;
    int returnCode = 0;

    try
    {
        Core::Logger & log = *Core::LoggerManager::getInstance().getDefaultLogger();
        log.setShowTagAppTime(false);
        log.setShowTagIdMsg(false);
        log.setShowTagThreadId(true);
        log.setShowTagLocalTime(true);

#ifdef WIN32
#ifdef LM_DEVMODE
        Core::String workDir(LM_WORKING_DIRECTORY);
        INF << L"-- DEVMODE ON --\n";
        INF << L"Set working directory '" << workDir << L"'\n";
        SetCurrentDirectory(Core::String8(workDir).c_str());
#endif
#endif

        String configFileName(L"hoo-server.conf");

        if(argc >= 2)
            configFileName = String(String8(argv[1]));

        pServer = new HOOServer::Server(configFileName);
        pServer->launch();
    }
    catch(Core::Exception & e)
    {
        ERR << L"Unhandled exception : " << e.getMessage() << L"\n";
        ERR << e.getCallStack() << L"\n";
        returnCode = 1;
    }
    catch(...)
    {
        ERR << L"Unhandled system exception\n";
        returnCode = 1;
    }

    delete pServer;

    return returnCode;
}
