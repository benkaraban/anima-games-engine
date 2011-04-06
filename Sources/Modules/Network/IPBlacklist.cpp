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
#include "IPBlacklist.h"

namespace Network
{

ENetworkIPStatus ConnectionAttempts::AddConnectionAttempt()
{
    ENetworkIPStatus connectionAuthorised = NETWORK_IP_AUTHORISED;

    if (_blacklisted)
            return NETWORK_IP_BLACKLISTED;

    Core::Clock clock;
    Core::TimeValue timeValue;
    timeValue = clock.universalTime();

    uint32 nTickCount = (uint32)((timeValue - _timeValue).totalSeconds());

    if ( (_lastPeriodConnectionAttempts.size() >= 1)
    && (nTickCount - _latestConnectionAttempt) <= MIN_TIME_BETWEEN_TWO_CONNECTION)
    {
        connectionAuthorised = NETWORK_IP_TEMPORARY_BLACKLISTED;
    }
    else
    {
        _latestConnectionAttempt = nTickCount;
    }

    _lastPeriodConnectionAttempts.push_back(nTickCount);

    if( nTickCount > BLACKLIST_PERIOD)
    {
        Core::List<uint32>::iterator iter;
        iter = _lastPeriodConnectionAttempts.begin();
        //On enlève toutes les tentatives de connexion périmées
        while (iter != _lastPeriodConnectionAttempts.end()
                && (*iter) < nTickCount - BLACKLIST_PERIOD )
        {
            ++iter;
        }
        _lastPeriodConnectionAttempts.erase(_lastPeriodConnectionAttempts.begin(), iter);
    }

    if (_lastPeriodConnectionAttempts.size() >= BLACKLIST_THRESHOLD)
    {
        _blacklisted = true;
        connectionAuthorised = NETWORK_IP_BLACKLISTED;
    }

    return connectionAuthorised;
}

ENetworkIPStatus IPBlacklist::isClientBlacklisted(uint32 ipAddress)
{
    Core::Map<uint32, ConnectionAttempts>::iterator iIPFound =_connectionAttempsByIP.find(ipAddress);

    //nouveau client
    if (iIPFound == _connectionAttempsByIP.end())
    {
        _connectionAttempsByIP[ipAddress] = ConnectionAttempts();
        iIPFound =_connectionAttempsByIP.find(ipAddress);
    }

    return iIPFound->second.AddConnectionAttempt();

}
} // namespace Network
