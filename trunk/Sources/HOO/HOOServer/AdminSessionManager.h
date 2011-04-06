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
#ifndef HOOSERVER_ADMINSESSIONMANAGER_H_
#define HOOSERVER_ADMINSESSIONMANAGER_H_

#include <Core/Standard.h>

#include <HOOServer/ISessionManager.h>
#include <HOOServer/ServerData.h>

namespace HOOServer
{

class LM_API_HOS AdminSessionManager: public Core::Object, public ISessionManager
{
public:
    AdminSessionManager(ServerData & serverData, UserSessionManager & userSessionManager);
    virtual ~AdminSessionManager();

    virtual void openSession(int32 idSession);
    virtual void closeSession(int32 idSession);

    virtual Session & getSession(int32 idSession);

    virtual bool isIdSessionValid(int32 idSession);

    virtual int32 connect(int32 version);

    virtual int32 login(int32 idSession, const String & login, const String & password, HOOUserProtocol::UserAccountInfos & infos);
    virtual void logout(int32 idSession);

    virtual int32 getTimeBeforeShutdown() const;

    virtual void sendAnswer(Network::Message & networkMessage);

    int32 countConnectedUsers();

    void banUsers(const Core::List<String>& logins, int32 nbDay, Core::List<bool> & banned);
    void unbanUsers(const Core::List<String>& logins, Core::List<bool> & unbanned);
    void lockUsers(const Core::List<String>& logins, Core::List<bool> & locked);
    void unlockUsers(const Core::List<String>& logins, Core::List<bool> & unlocked);
    void sendMessageToUsers(const String& messageText, bool forAllUsers, const Core::List<String>& logins);
    void shutdown(int32 timeBeforeShutdown);
    void getUsersInfos(Core::List<bool> & loggedIn, Core::List<String>& logins, Core::List<EQuickMatchStatus>& qmatchStatus);

private:
    ServerData &            _serverData;
    UserSessionManager &    _userSessionManager;
};

}//namespace HOOServer

#endif/*HOOSERVER_ADMINSESSIONMANAGER_H_*/