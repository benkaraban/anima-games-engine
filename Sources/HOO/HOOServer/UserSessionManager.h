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
#ifndef HOOSERVER_USERSESSIONMANAGER_H_
#define HOOSERVER_USERSESSIONMANAGER_H_

#include <Core/Standard.h>

#include <HOOServer/ISessionManager.h>
#include <HOOServer/ServerData.h>

#include <HOOUserProtocol/CheckLoginAvailable.h>
#include <HOOUserProtocol/CreateUserAccount.h>
#include <HOOUserProtocol/SelectCharacter.h>

namespace HOOServer
{

class LM_API_HOS UserSessionManager: public Core::Object, public ISessionManager
{
public:
    UserSessionManager(ServerData & serverData);
    virtual ~UserSessionManager();

    virtual void openSession(int32 idSession);
    virtual void closeSession(int32 idSession);

    virtual Session & getSession(int32 idSession);

    virtual bool isIdSessionValid(int32 idSession);

    virtual int32 connect(int32 version);

    virtual int32 login(int32 idSession, const String & login, const String & password, HOOUserProtocol::UserAccountInfos & infos);
    int32 loginAcCode(int32 idSession, const String & login, const String & acCode, HOOUserProtocol::UserAccountInfos & infos);
    virtual void logout(int32 idSession);

    virtual int32 getTimeBeforeShutdown() const;
    
    virtual void sendAnswer(Network::Message & networkMessage);

    HOOUserProtocol::ECheckLoginAvailableAnswerType checkLoginAvailability(const Core::String & login);
    HOOUserProtocol::ECreateUserAccountAnswerType   createUserAccount(int32 idSession, const UserAccount & account);

    /**
     * Seuls les champs character/spells/equipment sont utilis�s.
     */
    HOOUserProtocol::ESelectCharacterAnswerType     selectCharacter(int32 idSession, const UserAccount & account, HOOUserProtocol::UserAccountInfos & infos);

    bool quickMatch(int32 idSession);
    void cancelQuickMatch(int32 idSession);

    void removeFromLookingForOpponent(int32 idSession);

          ServerData & getServerData()       { return _serverData; };
    const ServerData & getServerData() const { return _serverData; };

private:
    bool isValidLogin(const String & login);
    bool isValidPassword(const String & password);
    bool isValidMail(const String & mail);

    ServerData & _serverData;
};

}//namespace HOOServer

#endif/*HOOSERVER_USERSESSIONMANAGER_H_*/