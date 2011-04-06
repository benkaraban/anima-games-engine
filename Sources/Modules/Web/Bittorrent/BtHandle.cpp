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
#include "BtHandle.h"
#include <Web/WebException.h>
#include <libtorrent/torrent_handle.hpp>


namespace Web
{
//-----------------------------------------------------------------------------
BtHandle::BtHandle(const libtorrent::torrent_handle& h) :
    _pH(NULL)
{
    _pH = new libtorrent::torrent_handle(h);
}
//-----------------------------------------------------------------------------
BtHandle::~BtHandle()
{
    delete _pH;
}
//-----------------------------------------------------------------------------
int32 BtHandle::getMaxConnections()
{
    int32 ret = 0;
   
    try
    {
        ret = (int32)(_pH->max_connections());
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::getMaxConnections : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
void BtHandle::setMaxConnections(int32 maxConnection)
{
    try
    {
        _pH->set_max_connections(maxConnection);
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::setMaxConnections : " + errMessage);
    }
}
//-----------------------------------------------------------------------------
int32 BtHandle::getMaxUploads()
{
    int32 ret = 0;

    try
    {
        ret = (int32)(_pH->max_uploads());
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::getMaxUploads : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
void BtHandle::setMaxUploads(int32 limit)
{
    try
    {
        _pH->set_max_uploads(limit);
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::setMaxUploads : " + errMessage);
    }

}
//-----------------------------------------------------------------------------
void BtHandle::setRatio(float ratio)
{
    try
    {
        _pH->set_ratio(ratio);
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::setRatio : " + errMessage);
    }
}
//-----------------------------------------------------------------------------
int32 BtHandle::getUploadLimit()
{
    int32 ret = 0;

    try
    {
        ret = _pH->upload_limit();
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::getUploadLimit : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
void BtHandle::setUploadLimit(int32 limit)
{
    try
    {
        _pH->set_upload_limit(limit);
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::setUploadLimit : " + errMessage);
    }
}
//-----------------------------------------------------------------------------
int32 BtHandle::getDownloadLimit()
{
    int32 ret = 0;

    try
    {
        ret = _pH->download_limit();
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::getDownloadLimit : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
void BtHandle::setDownloadLimit(int32 limit)
{
    try
    {
        _pH->set_download_limit(limit);
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::setDownloadLimit : " + errMessage);
    }
}
//-----------------------------------------------------------------------------
String BtHandle::getSavePath()
{
    String ret;

    try
    {
        ret = String (_pH->save_path().file_string().c_str());
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::getSavePath : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
String BtHandle::getName()
{
    String ret;

    try
    {
        ret = String (_pH->name().c_str());
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::getName : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
bool BtHandle::isSeed() const
{
    bool ret = false;

    try
    {
        _pH->is_seed();
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::isSeed : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
bool BtHandle::isFinished() const
{
    return _pH->is_finished();
}
//-----------------------------------------------------------------------------
bool BtHandle::isPaused() const
{
    bool ret = false;

    try
    {
        ret = _pH->is_paused();
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::isPaused : " + errMessage);
    }

    return ret;
}
//-----------------------------------------------------------------------------
void BtHandle::pause() const
{
    try
    {
        _pH->pause();
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::pause : " + errMessage);
    }
}
//-----------------------------------------------------------------------------
void BtHandle::resume() const
{
    try
    {
        _pH->resume();
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::resume : " + errMessage);
    }
}
//-----------------------------------------------------------------------------
bool BtHandle::isValid() const
{
    return _pH->is_valid(); // is_valid will never throw.
}
//-----------------------------------------------------------------------------
TorrentStatus BtHandle::getStatus() const
{
    TorrentStatus  ret;
    libtorrent::torrent_status statuts;
     
    try
    {
        statuts = _pH->status();
    }
    catch (libtorrent::libtorrent_exception e)
    {
        String errMessage (e.what());
        throw  WebException(L"error BtHandle::getStatus : " + errMessage);
    }

    ret.paused              = statuts.paused;
    ret.progress            = statuts.progress;
    ret.error               = String(statuts.error.c_str());
    ret.totalWanted         = statuts.total_wanted;
    ret.totalWantedDone     = statuts.total_wanted_done;
    ret.downloadPayloadRate = statuts.download_payload_rate;

    return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TorrentStatus::TorrentStatus() :
    paused(false),
    progress(0.f),
    totalWanted(0),
    totalWantedDone(0),
    downloadPayloadRate(0)
{
}
//-----------------------------------------------------------------------------
}