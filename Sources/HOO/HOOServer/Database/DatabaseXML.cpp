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
#include <HOOServer/Database/DatabaseXML.h>
#include <Core/Strings/Tools.h>

namespace HOOServer
{
DatabaseXML::DatabaseXML(const String & fileName) : _fileName(fileName)
{
    if(!_doc.loadDocument(fileName))
        throw Core::Exception(L"Unable to open XML database file '" + fileName + L"'");
}

String catList(const Core::List<int32> & list)
{
    String result;

    for(int32 ii=0; ii < list.size(); ii++)
    {
        if(ii > 0)
            result << L" ";
        result << Core::toString(list[ii]);
    }

    return result;
}

void uncatList(Core::List<int32> & list, const String & value)
{
    Core::List<String> tokens;
    Core::tokenize(value, tokens, L" ");

    list.resize(tokens.size());

    for(int32 ii=0; ii < list.size(); ii++)
        list[ii] = Core::toInteger(tokens[ii]);
}

void DatabaseXML::connect()
{
    Core::List<Ptr<Core::XMLNode> > accounts;
    _doc.getAllNodes(L"db.accounts", accounts);

    for(int32 ii=0; ii < accounts.size(); ii++)
        accounts[ii]->setValue(L"is-logged", false);

    _doc.saveDocument(_fileName);
}

bool DatabaseXML::banUser(const String & login, int32 nbDays)
{
    bool returnCode = false;
    String path(L"db.accounts." + login);

    if(_doc.getNode(path) != null)
    {
        Core::TimeValue localTime = Core::Clock::localTime();
        if(nbDays != 0)
            localTime += Core::TimeDuration(nbDays * 24, 0, 0, 0);
        else // 100 ans <=> forever
            localTime += Core::TimeDuration(24 * 365 * 100, 0, 0, 0);

        _doc.saveDocument(_fileName);
        _doc.setValue(path + L".account-banned", localTime);

        returnCode = true;
    }

    return returnCode;
}

bool DatabaseXML::lockAccount(const String & login)
{
    bool returnCode = false;
    String path(L"db.accounts." + login);

    if(_doc.getNode(path) != null)
    {
        _doc.setValue(path + L".account-locked", true);
        _doc.saveDocument(_fileName);

        returnCode = true;
    }

    return returnCode;
}

bool DatabaseXML::unlockAccount(const String & login)
{
    bool returnCode = false;
    String path(L"db.accounts." + login);

    if(_doc.getNode(path) != null)
    {
        _doc.setValue(path + L".account-locked", false);
        _doc.saveDocument(_fileName);

        returnCode = true;
    }

    return returnCode;
}

HOOUserProtocol::ECheckLoginAvailableAnswerType DatabaseXML::checkLoginAvailability(const String & login)
{
    String path(L"db.accounts." + login);

    if(_doc.getNode(path) != null)
        return HOOUserProtocol::USER_LOGIN_NOT_AVAILABLE;
    else
        return HOOUserProtocol::USER_LOGIN_AVAILABLE;
}

HOOUserProtocol::EUserLoginAnswerType DatabaseXML::login(UserAccount & account)
{
    String path(L"db.accounts." + account.login);
    String password;
    bool isLogged = false;

    if(_doc.getNode(path) == null || account.login == String::EMPTY)
        return HOOUserProtocol::USER_LOGIN_FAILED;

    bool accountLocked = false;

    _doc.getValue(path + L".account-locked", accountLocked);
    if(accountLocked)
        return HOOUserProtocol::USER_ACCOUNT_LOCKED;

    Core::TimeValue bannedTime;
    _doc.getValue(path + L".account-banned", bannedTime);
    Core::TimeValue localTime = Core::Clock::localTime();
    if(localTime <= bannedTime)
        return HOOUserProtocol::USER_ACCOUNT_BANNED;

    account.accountBanned = -1;

    _doc.getValue(path + L".is-logged", isLogged);
    _doc.getValue(path + L".password", password);

    if(password != account.password)
        return HOOUserProtocol::USER_PASSWD_FAILED;

    if(isLogged)
        return HOOUserProtocol::USER_ALREADY_LOGGED;

    _doc.getValue(path + L".mail", account.mail);
    _doc.getValue(path + L".character", account.character);
    _doc.getValue(path + L".character-selection-token", account.characterSelectionToken);
    _doc.getValue(path + L".xp", account.xp);

    String spells, stuff, equipedSpells, equipedStuff;

    _doc.getValue(path + L".spells", spells);
    _doc.getValue(path + L".stuff", stuff);

    _doc.getValue(path + L".equiped-spells", equipedSpells);
    _doc.getValue(path + L".equiped-stuff", equipedStuff);

    uncatList(account.spells, spells);
    uncatList(account.stuff, stuff);

    uncatList(account.equipedSpells, equipedSpells);
    uncatList(account.equipedStuff, equipedStuff);

    Core::List<Ptr<Core::XMLNode> > savedDecks;
    _doc.getAllNodes(path + L".saved-decks", savedDecks);

    account.savedDecks.clear();

    for(int32 ii=0; ii < savedDecks.size(); ii++)
        uncatList(account.savedDecks[savedDecks[ii]->getName()], savedDecks[ii]->getValue());

    _doc.getValue(path + L".creation-date", account.creationDate);
    _doc.getValue(path + L".log-date", account.lastLogDate);

    // login flag
    account.isLogged = true;
    _doc.setValue(path + L".is-logged", true);
    _doc.saveDocument(_fileName);

    return HOOUserProtocol::USER_LOGIN_OK;
}

HOOUserProtocol::EUserLoginAnswerType DatabaseXML::loginAcCode(UserAccount & account, const String& log, const String& acCode)
{
    String path(L"db.accounts." + log);

    String activationCode;
    Core::TimeValue activationCodeOutDate;
    Core::TimeValue actual = Core::Clock::universalTime();
    _doc.getValue(path + L".activationCode", activationCode);
    _doc.getValue(path + L".activationCodeOutDate", activationCodeOutDate);

    String tmp1;
    String tmp2;

    Core::removeControlChars(acCode, tmp1);
    Core::removeControlChars(activationCode, tmp2);

    if (actual < activationCodeOutDate && 
        tmp1 == tmp2 )
    {
        String password;
        

        _doc.getValue(path + L".password", password);
        account.login = log;
        account.password = password;

        return login(account);
    }
    
    return HOOUserProtocol::USER_LOGIN_FAILED;
}

HOOUserProtocol::ECreateUserAccountAnswerType DatabaseXML::createAccount(const UserAccount & account)
{
    String path(L"db.accounts." + account.login);

    if(_doc.getNode(path) != null)
        return HOOUserProtocol::LOGIN_ALREADY_USED;

    updateAccount(account);

    return HOOUserProtocol::USER_ACCOUNT_CREATED;
}

void DatabaseXML::updateAccount(const UserAccount & account)
{
    String path(L"db.accounts." + account.login);

    _doc.setValue(path + L".password", account.password);
    _doc.setValue(path + L".mail", account.mail);
    _doc.setValue(path + L".is-logged", account.isLogged);
    _doc.setValue(path + L".account-locked", account.accountLocked);

    Core::TimeValue localTime = Core::Clock::localTime();
    if(account.accountBanned >= 0)
    {
        if(account.accountBanned > 0)
            localTime += Core::TimeDuration(account.accountBanned * 24, 0, 0, 0);
        else if(account.accountBanned == 0)// 100 ans <=> forever
            localTime += Core::TimeDuration(24 * 365 * 100, 0, 0, 0);

        _doc.setValue(path + L".account-banned", localTime);
    }

    _doc.setValue(path + L".character", account.character);
    _doc.setValue(path + L".character-selection-token", account.characterSelectionToken);
    _doc.setValue(path + L".xp", account.xp);

    _doc.setValue(path + L".spells", catList(account.spells));
    _doc.setValue(path + L".stuff", catList(account.stuff));

    _doc.setValue(path + L".equiped-spells", catList(account.equipedSpells));
    _doc.setValue(path + L".equiped-stuff", catList(account.equipedStuff));

    for(Core::Map<String, Core::List<int32> >::const_iterator iDeck=account.savedDecks.begin(); iDeck != account.savedDecks.end(); ++iDeck)
        _doc.setValue(path + L".saved-decks." + iDeck->first, catList(iDeck->second));

    _doc.setValue(path + L".creation-date", account.creationDate);
    _doc.setValue(path + L".log-date", account.lastLogDate);

    _doc.saveDocument(_fileName);
}

void DatabaseXML::getLogins(const String& mail, Core::List<String>& logins)
{
    String path(L"db.accounts.");
    logins.clear();

    Core::List<Ptr<Core::XMLNode> > children;
    _doc.getAllNodes(path, children);

    String accountMail;
    for (int32 ii=0; ii<children.size(); ++ii)
    {
        children[ii]->getValue(L"mail", accountMail);
        if (accountMail == mail)
            logins.push_back(children[ii]->getName());
    }
}


void DatabaseXML::setActivationcode(const Core::List<String>& logins, const Core::List<String>& acCode, const Core::TimeValue& date)
{
    for (int32 ii=0; ii<logins.size(); ++ii)
    {
        String path(L"db.accounts." + logins[ii]);
        _doc.setValue(path + L".activationCode", acCode[ii]);
        _doc.setValue(path + L".activationCodeOutDate", date);
    }

    _doc.saveDocument(_fileName);
}

}

