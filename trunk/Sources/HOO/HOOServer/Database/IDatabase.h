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
#ifndef HOOSERVER_IDATABASE_H_
#define HOOSERVER_IDATABASE_H_

#include <HOOUserProtocol/CreateUserAccount.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>
#include <HOOUserProtocol/UserLogin.h>
#include <HOOServer/UserAccount.h>

namespace HOOServer
{

class LM_API_HOS IDatabase
{
public:
    virtual ~IDatabase() {};

    virtual void connect() = 0;

    //nbDays = -1 => unban user
    //nbDays = 0 => ban forever
    virtual bool banUser(const String & login, int32 nbDays) = 0;
    virtual bool lockAccount(const String & login) = 0;
    virtual bool unlockAccount(const String & login) = 0;

    virtual HOOUserProtocol::EUserLoginAnswerType login(UserAccount & account) = 0;
    virtual HOOUserProtocol::EUserLoginAnswerType loginAcCode(UserAccount & account, const String& login, const String& acCode) = 0;
    virtual void updateAccount(const UserAccount & account) = 0;

    virtual HOOUserProtocol::ECheckLoginAvailableAnswerType checkLoginAvailability(const String & login) = 0;
    virtual HOOUserProtocol::ECreateUserAccountAnswerType  createAccount(const UserAccount & account) = 0;

    virtual void getLogins(const String& mail, Core::List<String>& logins) = 0;
    virtual void setActivationcode(const Core::List<String>& logins, const Core::List<String>& acCode, const Core::TimeValue& date) = 0;
};

}

#endif
