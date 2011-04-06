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
#ifndef WEB_BT_SESSION_H_
#define WEB_BT_SESSION_H_

#include <Core/Standard.h>
#include <Core/DateAndTime.h>
#include <Core/List.h>
#include <Web/Bittorrent/BtTorrentInfo.h>
#include <Web/Bittorrent/BtHandle.h>

namespace libtorrent
{
class session;
}

namespace Web
{
//-----------------------------------------------------------------------------
struct SessionStatus
{
    SessionStatus();

    float  totalWanted;
    float  totalWantedDone;
    float  progress; //[0,1]
    int64  downloadPayloadRate;
    Core::TimeDuration finishedTime;
};
//-----------------------------------------------------------------------------
class LM_API_WEB BtSession
{
public:
    BtSession();
    virtual ~BtSession();

    Ptr<BtHandle> addTorrent(BtTorrentInfo& info, const String& savePath);

    const Core::List<Ptr<BtHandle> >& getTorrentsHandles() const {return _handles;}

    bool allTorrentsFinished();

    void pause();
    void resume();
    bool isPaused() const;

    float progress() const;
    SessionStatus getStatus() const;

private:
    libtorrent::session*        _pS;
    Core::List<Ptr<BtHandle> >  _handles;
};
//-----------------------------------------------------------------------------
}// Network

#endif /*WEB_BT_CREATE_TORRENT_H_*/
