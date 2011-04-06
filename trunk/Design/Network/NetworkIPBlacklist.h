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
#ifndef NETWORK_NETWORKIPBLACKLIST_H_
#define NETWORK_NETWORKIPBLACKLIST_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Map.h>
#include <Core/DateAndTime.h>

#include "NetworkEnum.h"

namespace Network
{

const uint32 MIN_TIME_BETWEEN_TWO_CONNECTION = 3;//3 secondes
//Blacklist si on dépasse les cent tentatives de connexion en moins de 10 minutes
const uint32 BLACKLIST_PERIOD = 600;//10 minutes (600 secondes)
const uint32 BLACKLIST_THRESHOLD = 100;

class NetworkConnectionAttempts : public Core::Object
{
public:
    NetworkConnectionAttempts()
    :   _latestConnectionAttempt(0),
        _blacklisted(false)
    {
        Core::Clock clock;
        _timeValue = clock.universalTime();
    }
    NetworkConnectionAttempts(const NetworkConnectionAttempts & networkConnectionAttempts)
    :   _latestConnectionAttempt(networkConnectionAttempts._latestConnectionAttempt),
        _blacklisted(networkConnectionAttempts._blacklisted),
        _timeValue(networkConnectionAttempts._timeValue)
    {}

    ENetworkIPStatus AddConnectionAttempt();

private:
    uint32          _latestConnectionAttempt;
    bool            _blacklisted;
    Core::TimeValue _timeValue;

    Core::List<uint32> _lastPeriodConnectionAttempts;

};

class NetworkIPBlacklist : public Core::Object
{
public:
    NetworkIPBlacklist(){}
    ENetworkIPStatus NetworkIPBlacklist::IsClientBlacklisted(uint32 ipAddress);
private:
    Core::Map<uint32, NetworkConnectionAttempts> _connectionAttempsByIP;
};

} // namespace Network

#endif /* NETWORK_NETWORKIPBLACKLIST_H_ */
