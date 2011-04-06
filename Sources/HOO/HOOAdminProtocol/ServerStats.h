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
#ifndef HOOADMINPROTOCOL_SERVERSTATS_H_
#define HOOADMINPROTOCOL_SERVERSTATS_H_

#include <HOOAdminProtocol/IAdminMessage.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/DateAndTime.h>

namespace HOOAdminProtocol
{

class LM_API_HAP ServerStats : public Core::Object, public IAdminRequest
{
public:
    ServerStats()
    {}

    virtual ~ServerStats(){}

    virtual EAdminRequestType getType() const { return SERVER_STATS; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

};

class LM_API_HAP ServerStatsAnswer : public Core::Object, public IAdminAnswer
{
public:
    ServerStatsAnswer()
    {}

    ServerStatsAnswer(const Core::TimeValue& date, const Core::TimeDuration& appRunTimes)
    :   _statsDate(date), _appRunTimes(appRunTimes)
    {}

    virtual ~ServerStatsAnswer(){}

    virtual EAdminAnswerType getType() const { return SERVER_STATS_ANSWER; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    String  getStats() const;
protected:
    Core::TimeValue     _statsDate; // date a laquelle les stats on ete generees
    Core::TimeDuration  _appRunTimes;
};

}//namespace HOOAdminProtocol

#endif/*HOOADMINPROTOCOL_SERVERSTATS_H_*/