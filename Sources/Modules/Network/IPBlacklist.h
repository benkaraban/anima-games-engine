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
#ifndef NETWORK_IPBLACKLIST_H_
#define NETWORK_IPBLACKLIST_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Map.h>
#include <Core/DateAndTime.h>

#include <Network/Enum.h>

namespace Network
{

const uint32 MIN_TIME_BETWEEN_TWO_CONNECTION = 1;//1 secondes
//Blacklist si on dépasse les cent tentatives de connexion en moins de 10 minutes
const uint32 BLACKLIST_PERIOD = 600;//10 minutes (600 secondes)
const uint32 BLACKLIST_THRESHOLD = 100;

class ConnectionAttempts : public Core::Object
{
public:
    ConnectionAttempts()
    :   _latestConnectionAttempt(0),
        _blacklisted(false)
    {
        Core::Clock clock;
        _timeValue = clock.universalTime();
    }
    ConnectionAttempts(const ConnectionAttempts & ConnectionAttempts)
    :   _latestConnectionAttempt(ConnectionAttempts._latestConnectionAttempt),
        _blacklisted(ConnectionAttempts._blacklisted),
        _timeValue(ConnectionAttempts._timeValue)
    {}

    ENetworkIPStatus AddConnectionAttempt();

private:
    uint32          _latestConnectionAttempt;
    bool            _blacklisted;
    Core::TimeValue _timeValue;

    Core::List<uint32> _lastPeriodConnectionAttempts;

};

class LM_API_NET IPBlacklist : public Core::Object
{
public:
    IPBlacklist(){}
    ENetworkIPStatus isClientBlacklisted(uint32 ipAddress);
private:
    Core::Map<uint32, ConnectionAttempts> _connectionAttempsByIP;
};

} // namespace Network

#endif /* NETWORK_IPBLACKLIST_H_ */
