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
#include "CreateAccountHandler.h"

#include <Core/Logger.h>

#include <HOOServer/DisconnectException.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>
#include <HOOUserProtocol/CreateUserAccount.h>
#include <HOOUserProtocol/SelectCharacter.h>

namespace HOOServer
{

CreateAccountHandler::CreateAccountHandler(UserSessionManager & sessionManager)
:   _sessionManager(sessionManager)
{
    bool ok = true;
    const Core::XMLDocument & conf = sessionManager.getServerData().configFile;

    String msg(L"Starting spells       : ");
    Core::List<Ptr<Core::XMLNode> > spells;
    ok = ok && conf.getNodes(L"server.starting-spells", L"spell", spells);

    for(int32 ii=0; ii < spells.size(); ii++)
    {
        int32 spell = 0;
        ok = ok && spells[ii]->getValue(L"", spell);
        _startingSpells.push_back(spell);
        msg << Core::toString(spell) << L" ";
    }
    INF << msg << L"\n";

    Core::List<Ptr<Core::XMLNode> > characters;
    ok = ok && conf.getNodes(L"server.starting-stuff", L"character", characters);

    for(int32 iChar=0; iChar < characters.size(); iChar++)
    {
        int32 id = 0;
        ok = ok && characters[iChar]->getValue(L"id", id);

        msg = L"Starting stuff char " + Core::toString(id) + L" : ";
        Core::List<Ptr<Core::XMLNode> > stuff;
        characters[iChar]->getNodes(L"stuff", stuff);

        for(int32 iStuff=0; iStuff < stuff.size(); iStuff++)
        {
            int32 idStuff = 0;
            ok = ok && stuff[iStuff]->getValue(L"", idStuff);
            _startingStuff[id].push_back(idStuff);
            msg << Core::toString(idStuff) << L" ";
        }    
        INF << msg << L"\n";
    }

    if(!ok)
        throw Core::Exception(L"Error in config file in starting account");
}

CreateAccountHandler::~CreateAccountHandler()
{}

void CreateAccountHandler::handleMessage(HOOUserProtocol::EUserRequestType reqType, int32 idSession, Network::Message * request)
{
    if(!_sessionManager.getSession(idSession).isConnected())
        throw DisconnectException(L"Trying to log on a not connected session.", SESSION_NOT_OPENED);

    if(reqType == HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE)
    {
        HOOUserProtocol::CheckLoginAvailable checkLogin;
        request->getMessage(checkLogin);

        HOOUserProtocol::CheckLoginAvailableAnswer checkLoginAnswer(_sessionManager.checkLoginAvailability( checkLogin.getLogin()), 
                                                                                                            checkLogin.getLogin());
        Network::Message answerMessage(Network::APPLICATION_MESSAGE, checkLoginAnswer, idSession);
        _sessionManager.sendAnswer(answerMessage);
    
    }
    else if(reqType == HOOUserProtocol::USER_CREATE_ACCOUNT)
    {
        HOOUserProtocol::CreateUserAccount createAccount;
        request->getMessage(createAccount);

        HOOUserProtocol::UserAccountInfos infos;
        UserAccount account;
        account.login = createAccount.getLogin();
        account.password = createAccount.getPassword();
        account.mail = createAccount.getMail();
        account.isLogged = true;

        account.accountLocked = 0;
        account.accountBanned = -1;

        account.character = 0; // Personnage pas encore d�fini
        account.characterSelectionToken = 1;
        account.xp = 0;

        account.creationDate = Core::Clock::localTime();
        account.lastLogDate = account.creationDate;

        account.getInfos(infos);

        HOOUserProtocol::ECreateUserAccountAnswerType answer = _sessionManager.createUserAccount(idSession, account);
        HOOUserProtocol::CreateUserAccountAnswer createAccountAnswer(answer, infos);

        Network::Message answerMessage(Network::APPLICATION_MESSAGE, createAccountAnswer, idSession);
        _sessionManager.sendAnswer(answerMessage);
    }
    else if(reqType == HOOUserProtocol::USER_SELECT_CHARACTER)
    {
        HOOUserProtocol::SelectCharacter selectChar;
        request->getMessage(selectChar);

        Core::Map<int32, Core::List<int32> >::const_iterator iStuff = _startingStuff.find(selectChar.getCharacter());

        if(iStuff == _startingStuff.end())
            throw DisconnectException(L"Trying to create an account with an unknown character : " + Core::toString(selectChar.getCharacter()), WRONG_PROTOCOL);

        UserAccount account;
        account.character = selectChar.getCharacter();

        account.spells = _startingSpells;
        account.stuff = iStuff->second;

        account.equipedSpells = account.spells;
        account.equipedStuff = account.stuff;

        HOOUserProtocol::UserAccountInfos infos;
        HOOUserProtocol::ESelectCharacterAnswerType answer = _sessionManager.selectCharacter(idSession, account, infos);

        HOOUserProtocol::SelectCharacterAnswer selectCharacterAnswer(answer, infos);

        Network::Message answerMessage(Network::APPLICATION_MESSAGE, selectCharacterAnswer, idSession);
        _sessionManager.sendAnswer(answerMessage);
    }
    else
    {
        Core::String message;
        message << L"CreateAccountHandler not able to process request of type : ("<< (int32)reqType << L") - " << HOOUserProtocol::toString(reqType);
        throw Core::Exception(message);

    }
}

}//namespace HOOServer