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
#ifndef HUP_APPUPDATER_H_
#define HUP_APPUPDATER_H_

#include <Core/Standard.h>
#include <Core/Dictionary.h>
#include <Core/RT/Synchronized.h>
#include <Network/NetworkEngine.h>
#include <Web/Bittorrent/BtSession.h>
#include <HOOUpdaterProtocol/IUpdaterMessage.h>

struct AppUpdaterConfig
{
    AppUpdaterConfig();

    String          serverAddress;
    int32           serverPort;

    int32           openSocketRetryCount;
    int32           openSocketRetryWaitTime; // Attente entre les essais en SECONDES
};

class AppUpdater : public Core::Object
{
public:
    static void createInstance(HINSTANCE hInst);
    static AppUpdater& Inst();

    ~AppUpdater();

    bool createApplicationWindow();

    

    void setProgress(float p);  //avec p sur [0, 1]
    void launch();
    void exit();

    bool retrieveFilesList();
    bool launchDownloadFiles();
    bool downloadFiles();

    String getDicoText(const String& id);

private:
    void initialize();

    bool openSocket();
    bool connectToServer();
    void disconnectToServer();
    void send(HOOUpdaterProtocol::IUpdaterRequest & message);
    bool receive(HOOUpdaterProtocol::IUpdaterAnswer & message, bool blocking = true);

    void run();
    void runUpdaterThread();

    AppUpdater(HINSTANCE hInst);

private:
    AppUpdaterConfig                _config;
    int32                           _sessionId;

    Core::ConditionMutex            _conditionMutex;
    Core::ConditionVar              _conditionVar;
    Network::NetworkEngine          _networkEngine; 

    Core::ELanguage                 _language;
    Ptr<Core::Dictionary>           _pDico;

    Core::Thread *                  _pThread;
    Core::Synchronized<bool>        _synch;
    Web::BtSession                  _btSession;

    String                          _downloadDirectory;
};

#endif