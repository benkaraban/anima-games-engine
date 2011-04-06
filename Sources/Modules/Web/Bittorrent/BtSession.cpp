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
#include "BtSession.h"
#include <Web/WebException.h>

#pragma warning( push )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4800 )
#include <libtorrent/session.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/time.hpp>
#pragma warning( pop ) 

namespace Web
{
//-----------------------------------------------------------------------------
BtSession::BtSession() :
    _pS(NULL)
{
    _pS = new libtorrent::session();
}
//-----------------------------------------------------------------------------
BtSession::~BtSession()
{
    delete _pS;
}
//-----------------------------------------------------------------------------
Ptr<BtHandle> BtSession::addTorrent(BtTorrentInfo& info, const String& savePath)
{
    libtorrent::error_code ec;
    libtorrent::add_torrent_params p;

    p.save_path = String8(savePath).c_str();
    p.ti = new libtorrent::torrent_info(*info._pTi);

    libtorrent::torrent_handle h = _pS->add_torrent(p, ec);
    _handles.push_back(Ptr<BtHandle> (new BtHandle(h)));

    if(ec) 
    { 
        String errMessage (String8(ec.message().c_str()));
        throw WebException( L"An error occured while trying to add torrent : " + errMessage);
    }

    return _handles.back();
}
//-----------------------------------------------------------------------------
bool BtSession::allTorrentsFinished()
{
    bool ret = true;

    for (int32 ii=0; ii<_handles.size(); ++ii)
    {
        if (_handles[ii]->isFinished() == false)
        {
            ret = false;
            break;
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
void BtSession::pause()
{
    _pS->pause();
}
//-----------------------------------------------------------------------------
void BtSession::resume()
{
    _pS->resume();
}
//-----------------------------------------------------------------------------
bool BtSession::isPaused() const
{
    return _pS->is_paused();
}
//-----------------------------------------------------------------------------
float BtSession::progress() const
{
    float totalWanted     = 0;
    float totalWantedDone = 0;

    for (int32 ii=0; ii<_handles.size(); ++ii)
    {
        Web::TorrentStatus ts = _handles[ii]->getStatus();
        totalWanted     += ts.totalWanted;
        totalWantedDone += ts.totalWantedDone;
    }

    return totalWantedDone/totalWanted;
}
//-----------------------------------------------------------------------------
SessionStatus BtSession::getStatus() const
{
    SessionStatus ret;
    float finishedTime = 0.f;

    for (int32 ii=0; ii<_handles.size(); ++ii)
    {
        Web::TorrentStatus ts = _handles[ii]->getStatus();

        ret.totalWanted         += ts.totalWanted;
        ret.totalWantedDone     += ts.totalWantedDone;

        ret.downloadPayloadRate += ts.downloadPayloadRate;
    }

    ret.progress     = ret.totalWantedDone/ret.totalWanted;
    //ret.finishedTime = Core::TimeDuration(0, 0,finishedTime, 0);
    return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SessionStatus::SessionStatus():
    totalWanted(0),
    totalWantedDone(0),
    progress(0),
    finishedTime(0),
    downloadPayloadRate(0)
{
}
//-----------------------------------------------------------------------------
}