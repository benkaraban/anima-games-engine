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
#ifndef HOOADMINPROTOCOL_BANUSERS_H_
#define HOOADMINPROTOCOL_BANUSERS_H_

#include <HOOAdminProtocol/IAdminMessage.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/DateAndTime.h>

namespace HOOAdminProtocol
{

class LM_API_HAP BanUsers : public Core::Object, public IAdminRequest
{
public:
    BanUsers() : _nbDay(-1) {}

    BanUsers(const Core::List<String>& logins, int32 nbDay) : _logins(logins), _nbDay(nbDay) {}

    virtual ~BanUsers(){}

    virtual EAdminRequestType getType() const { return BAN_USERS; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<String>& getLogins() const {return _logins;}
    int32 getNbDay() const {return _nbDay;}

protected:
    Core::List<String>  _logins;
    int32               _nbDay;
};

class LM_API_HAP BanUsersAnswer : public Core::Object, public IAdminAnswer
{
public:
    BanUsersAnswer()
    {}

    BanUsersAnswer(const Core::List<String>& logins, const Core::List<bool>& banned) 
    : _logins(logins), _banned(banned) 
    {}

    virtual ~BanUsersAnswer(){}

    virtual EAdminAnswerType getType() const { return BAN_USERS_ANSWER; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<String>&   getLogins() const {return _logins;}
    const Core::List<bool>&     getBanned() const {return _banned;}

protected:
    Core::List<String>  _logins;
    Core::List<bool>    _banned;
};

class LM_API_HAP UnbanUsers : public Core::Object, public IAdminRequest
{
public:
    UnbanUsers() {}

    UnbanUsers(const Core::List<String>& logins) : _logins(logins) {}

    virtual ~UnbanUsers(){}

    virtual EAdminRequestType getType() const { return UNBAN_USERS; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<String>& getLogins() const {return _logins;}

protected:
    Core::List<String>  _logins;
};

class LM_API_HAP UnbanUsersAnswer : public Core::Object, public IAdminAnswer
{
public:
    UnbanUsersAnswer()
    {}

    UnbanUsersAnswer(const Core::List<String>& logins, const Core::List<bool>& unbanned) 
    : _logins(logins), _unbanned(unbanned) 
    {}

    virtual ~UnbanUsersAnswer(){}

    virtual EAdminAnswerType getType() const { return UNBAN_USERS_ANSWER; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<String>&   getLogins() const {return _logins;}
    const Core::List<bool>&     getUnbanned() const {return _unbanned;}

protected:
    Core::List<String>  _logins;
    Core::List<bool>    _unbanned;
};
}//namespace HOOAdminProtocol

#endif/*HOOADMINPROTOCOL_BANUSERS_H_*/