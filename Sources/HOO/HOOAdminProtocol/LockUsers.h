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
#ifndef HOOADMINPROTOCOL_LOCKUSERS_H_
#define HOOADMINPROTOCOL_LOCKUSERS_H_

#include <HOOAdminProtocol/IAdminMessage.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/DateAndTime.h>

namespace HOOAdminProtocol
{

class LM_API_HAP LockUsers : public Core::Object, public IAdminRequest
{
public:
    LockUsers(){}

    LockUsers(const Core::List<String>& logins) : _logins(logins){}

    virtual ~LockUsers(){}

    virtual EAdminRequestType getType() const { return LOCK_USERS; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;
    const Core::List<String>& getLogins() const {return _logins;}

protected:
    Core::List<String>  _logins;
};

class LM_API_HAP LockUsersAnswer : public Core::Object, public IAdminAnswer
{
public:
    LockUsersAnswer()
    {}

    LockUsersAnswer(const Core::List<String>& logins, const Core::List<bool>& locked) 
    : _logins(logins), _locked(locked) 
    {}

    virtual ~LockUsersAnswer(){}

    virtual EAdminAnswerType getType() const { return LOCK_USERS_ANSWER; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<String>&   getLogins() const {return _logins;}
    const Core::List<bool>&     getLocked() const {return _locked;}

protected:
    Core::List<String>  _logins;
    Core::List<bool>    _locked;
};

class LM_API_HAP UnlockUsers : public Core::Object, public IAdminRequest
{
public:
    UnlockUsers() {}

    UnlockUsers(const Core::List<String>& logins) : _logins(logins) {}

    virtual ~UnlockUsers(){}

    virtual EAdminRequestType getType() const { return UNLOCK_USERS; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<String>& getLogins() const {return _logins;}

protected:
    Core::List<String>  _logins;
};

class LM_API_HAP UnlockUsersAnswer : public Core::Object, public IAdminAnswer
{
public:
    UnlockUsersAnswer(){}

    UnlockUsersAnswer(const Core::List<String>& logins, const Core::List<bool>& unlocked) 
    : _logins(logins), _unlocked(unlocked) 
    {}

    virtual ~UnlockUsersAnswer(){}

    virtual EAdminAnswerType getType() const { return UNLOCK_USERS_ANSWER; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<String>&   getLogins() const {return _logins;}
    const Core::List<bool>&     getUnlocked() const {return _unlocked;}

protected:
    Core::List<String>  _logins;
    Core::List<bool>    _unlocked;
};

}//namespace HOOAdminProtocol

#endif/*HOOADMINPROTOCOL_LOCKUSERS_H_*/