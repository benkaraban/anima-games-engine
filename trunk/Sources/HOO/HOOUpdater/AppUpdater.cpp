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
#include <Core/Logger.h>
#include <Core/IO/FileStream.h>
#include <Core/Strings/Tools.h>
#include <Core/RT/Thread.h>
#include <Core/Math/MathTools.h>
#include "WindowUpdater.h"

#include <Network/Message.h>
#include <Network/NetworkException.h>
#include <HOOUpdaterProtocol/UpdaterConnect.h>
#include <Core/System/Files.h>


#include <Core/VFS/VFSDirectMapping.h>
#include <Core/IO/Tools.h>
#include <Web/Bittorrent/BtCreateTorrent.h>
//-----------------------------------------------------------------------------
String toStringCount(int32 c)
{
    if(c == -1)
        return L"INFINITE";
    else
        return Core::toString(c);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AppUpdater* pInst = NULL;
//-----------------------------------------------------------------------------
AppUpdaterConfig::AppUpdaterConfig()
{
    serverAddress               = L"192.168.0.13";
    serverPort                  = 31338;

    openSocketRetryCount        = 3;
    openSocketRetryWaitTime     = 1;
}
//-----------------------------------------------------------------------------
AppUpdater::AppUpdater(HINSTANCE hInst) :
    _networkEngine(&_conditionVar, &_conditionMutex),
    _sessionId(-1),
    _pThread(NULL),
    _synch(false)
{
    WHU::createInstance(hInst);
    initialize();
    
    {
        Core::String workDir(LM_WORKING_DIRECTORY);
        SetCurrentDirectory(Core::String8(workDir).c_str());
        Ptr<Core::InputStream> pInputDic (new Core::FileInputStream(L"./Data/HOO/Dictionaries/updater.xml"));
        
        if(pInputDic == null)
        {
            ERR << L"Unable to open dictionary file\n";
            return;
        }

        _language = Core::FRENCH;
        _pDico = Ptr<Core::Dictionary>(new Core::Dictionary(_language, *pInputDic));
    }
}
//-----------------------------------------------------------------------------
AppUpdater::~AppUpdater()
{
    initialize();

    disconnectToServer();
    
    LM_PTR_DELETE(_pDico);

    delete pInst;
    pInst = NULL;
}
//-----------------------------------------------------------------------------
void AppUpdater::initialize()
{
    if (_pThread!=NULL)
    {
        _synch = false;
        _pThread->join();
        delete _pThread;
        _pThread = NULL;
    }

    WHU::Inst().setProgressBarState(EPB_STATE_CLEAR);
    _sessionId = -1;
    _config    = AppUpdaterConfig();
}
//-----------------------------------------------------------------------------
void AppUpdater::createInstance(HINSTANCE hInst)
{
    if (pInst==NULL)
    {
        pInst = new AppUpdater(hInst);
    }
}
//-----------------------------------------------------------------------------
AppUpdater& AppUpdater::Inst()
{
    return *pInst;
}
//-----------------------------------------------------------------------------
bool AppUpdater::createApplicationWindow()
{
    String title (getDicoText(L"window-title"));
    return WHU::Inst().createMainWindows(title);
}
//-----------------------------------------------------------------------------
void AppUpdater::launch()
{
    initialize();
    _synch = true;
    _pThread = new Core::Thread(&AppUpdater::runUpdaterThread, *this);
    run();
}
//-----------------------------------------------------------------------------
void AppUpdater::run()
{
    MSG mMsg;

    while (_synch == true)
    {
        if(PeekMessage(&mMsg, 0, 0, 0, PM_REMOVE))
        {
            if(mMsg.message == WM_QUIT)
                { break; }

            TranslateMessage(&mMsg);
            DispatchMessage(&mMsg);
        }
    }
}
//-----------------------------------------------------------------------------
void AppUpdater::setProgress(float p)
{
    WHU::Inst().setProgress(p);
}
//-----------------------------------------------------------------------------
void AppUpdater::exit()
{
    PostQuitMessage(0);
}
//-----------------------------------------------------------------------------
bool AppUpdater::openSocket()
{
    bool result = false;
    int32 tryCount = 0;

    do
    {
        if(tryCount != 0)
        {
            INF << L"Socket open failed, will retry in " << Core::toString(_config.openSocketRetryWaitTime) << L" s";
            Core::Thread::sleep(_config.openSocketRetryWaitTime * 1000);
        }

        tryCount ++;
        INF << L"Opening socket " << _config.serverAddress << L":" << Core::toString(_config.serverPort)
            << L" try " << Core::toString(tryCount) << L"/" << toStringCount(_config.openSocketRetryCount) << L"...";
        _sessionId = _networkEngine.connect(_config.serverAddress, _config.serverPort);

    } while(tryCount < _config.openSocketRetryCount && _sessionId == -1);


    if(_sessionId != -1)
    {
        INF << L"Socket opened with sessionId " << Core::toString(_sessionId);
        _networkEngine.launch();
        result = true;
    }
    else
    {
        ERR << L"Socket open failed";
        return false;
        //QMessageBox::critical(this, "error", "Socket open failed");
    }

    return result;
}
//-----------------------------------------------------------------------------
bool AppUpdater::connectToServer()
{
    bool result = false;

    {
        INF << L"Sending connection request\n";
        HOOUpdaterProtocol::UpdaterConnect connectMsg;
        send(connectMsg);
    }

    {
        HOOUpdaterProtocol::UpdaterConnectAnswer connectAnswer;
        receive(connectAnswer);

        INF << L"Connect answer is " << HOOUpdaterProtocol::toString(connectAnswer.getConnectAnswerType()) << L"\n";

        result = (connectAnswer.getConnectAnswerType() == HOOUpdaterProtocol::UPDATER_CONNECT_OK);
        if (result==false)
        {
            ERR << L"connect to server failed";
            //QMessageBox::critical(this, "error", "connect to server failed");
        }
        else
        {
            int a = 5;
        }
    }

    return result;
}
//-----------------------------------------------------------------------------
void AppUpdater::disconnectToServer()
{
    INF << L"Sending disconnection request\n";
    Network::Message msg (Network::NETWORK_CLOSE_SESSION_MESSAGE, _sessionId);
    _networkEngine.sendMessage(msg);
}
//-----------------------------------------------------------------------------
void AppUpdater::send(HOOUpdaterProtocol::IUpdaterRequest & message)
{
    Network::Message messageToSend(Network::APPLICATION_MESSAGE, message, _sessionId);
    _networkEngine.sendMessage(messageToSend);
}
//-----------------------------------------------------------------------------
bool AppUpdater::receive(HOOUpdaterProtocol::IUpdaterAnswer & message, bool blocking)
{
    bool messageToProcess = false;
    
    do
    {
        Network::Message * incomingMessage = NULL;

        if(blocking)
        {
            Core::ScopedCondLock lock(_conditionMutex);
            while(!_networkEngine.getMessage(incomingMessage))
                _conditionVar.wait(lock);

            messageToProcess = true;
        }
        else
        {
            Core::ScopedCondLock lock(_conditionMutex);
            messageToProcess = _networkEngine.getMessage(incomingMessage);
        }

        if(incomingMessage != NULL)
        {
            Network::EMessageType messageType = incomingMessage->getMessageType();

            switch(messageType)
            {
            case Network::NETWORK_CLOSE_SESSION_MESSAGE:
                ERR << L"The network connection was remotely closed!";
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            case Network::APPLICATION_MESSAGE:
                {
                    if(incomingMessage->getDataSize() >= sizeof(HOOUpdaterProtocol::EUpdaterAnswerType))
                    {
                        HOOUpdaterProtocol::EUpdaterAnswerType answerType;
                        incomingMessage->readData(&answerType, sizeof(HOOUpdaterProtocol::EUpdaterAnswerType));

                        if(answerType == message.getType())
                            incomingMessage->getMessage(message);
                        else
                        {
                            INF << L"Ignoring message " << HOOUpdaterProtocol::toString(answerType) << L"\n";
                            messageToProcess = false;
                        }
                    }
                    else
                    {
                        ERR << L"Received invalid message!";
                        throw Network::NetworkException(L"Received invalid message!");
                    }
                }
                break;
            default:
                ERR << L"The network connection was remotely closed!";
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            }

            delete incomingMessage;
            incomingMessage = NULL;
        }
    } while(blocking && !messageToProcess);

    return messageToProcess;
}
//-----------------------------------------------------------------------------
bool AppUpdater::retrieveFilesList()
{
    {
        WHU::Inst().setTextSatutBar(getDicoText(L"server-connect"));
        WHU::Inst().setTextEdit(getDicoText(L"user-message-step1"));
        WHU::Inst().setProgressBarState(EPB_STATE_LOOP);
    }

    if(!openSocket())                   {WHU::Inst().setProgressBarState(EPB_STATE_CLEAR); return false;}
    if(!connectToServer())              {WHU::Inst().setProgressBarState(EPB_STATE_CLEAR); return false;}

    disconnectToServer();

    WHU::Inst().setProgressBarState(EPB_STATE_CLEAR);
    return true;
}
//-----------------------------------------------------------------------------
bool AppUpdater::launchDownloadFiles()
{
    {
        WHU::Inst().setTextSatutBar(getDicoText(L"download-process"));
        WHU::Inst().setTextEdit(getDicoText(L"download-process"));
        WHU::Inst().setProgressBarState(EPB_STATE_WITH_LIMIT);
    }

    Ptr<Core::VFSDirectMapping> pVFS(new Core::VFSDirectMapping());
    pVFS->linkDirectory(L".");

    Core::List<byte> info;
    Core::List<byte> data;
    Core::readAllStream(data, *pVFS->readFile(L"/Data/TUData/Web/test.torrent"));
    Web::BtTorrentInfo tInfo (data);


    String DirName  = L"HooUpdater/Download/";
    String tempDir  = Core::System::getTempDir();
    String finalDir = tempDir+DirName;
    _downloadDirectory = finalDir;


    _btSession.addTorrent(tInfo, _downloadDirectory);

    return true;
}
//-----------------------------------------------------------------------------
bool AppUpdater::downloadFiles()
{
    bool ret = _btSession.allTorrentsFinished();
    

    Web::SessionStatus statut = _btSession.getStatus();
    WHU::Inst().setProgress(statut.progress);

    float nbByteToDownload = statut.totalWanted - statut.totalWantedDone; 
    int32 nbSecond = 0;
    if (statut.downloadPayloadRate>0)
        nbSecond = (int32) (nbByteToDownload / statut.downloadPayloadRate);

    Core::TimeDuration finishedTime (0,0,nbSecond,0);

    float actual = statut.totalWantedDone; 
    float total  = statut.totalWanted;

    if (total > 1024)
    {
        total  = total/1024.f;
        actual = actual/1024.f;
    }

    String statutsMsg = getDicoText(L"download-process");
        statutsMsg += L"     ";
        statutsMsg += Core::toString(actual,0);
        statutsMsg += L" (kb) ";
        statutsMsg += L" / ";
        statutsMsg += Core::toString(total,0);
        statutsMsg += L" (kb) ";
        statutsMsg += L" | ";
        statutsMsg += Core::toStringByteSize(statut.downloadPayloadRate) + L" / s";
        statutsMsg += L" | ";
        statutsMsg += finishedTime.toString();
    WHU::Inst().setTextSatutBar(statutsMsg);

    return ret;
}
//-----------------------------------------------------------------------------
String AppUpdater::getDicoText(const String& id)
{
    return (*_pDico)[id];
}
//-----------------------------------------------------------------------------
void AppUpdater::runUpdaterThread()
{
    bool ret = false;
    
    //connection au serveur pour recuperer la liste des fichiers
    ret = false;
    while (_synch==true && ret == false)
    {
        ret = retrieveFilesList();
        if (ret==false)
        {
            bool retry = errorQuestionDlg (getDicoText(L"window-error-title"), getDicoText(L"connection-failed"));
            if (!retry) _synch = false;
        }
    }

    //telechargement des fichiers
    if (!launchDownloadFiles()) {messageDlg(getDicoText(L"window-error-title"), getDicoText(L"download-error")); _synch = false;}
    ret = false;
    while(_synch==true && ret == false)
    {
        ret = downloadFiles();
        Core::Thread::sleep(500);
    }
}
//-----------------------------------------------------------------------------

