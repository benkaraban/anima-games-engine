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
#ifndef WEB_BT_HANDLE_H_
#define WEB_BT_HANDLE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Web/Bittorrent/BtTorrentInfo.h>

namespace libtorrent
{
struct torrent_handle;
}

namespace Web
{
//-----------------------------------------------------------------------------
struct LM_API_WEB TorrentStatus
{
    TorrentStatus();

    bool   paused;
    float  progress;
    String error;
    int64  totalWanted;
    int64  totalWantedDone;
    int64  downloadPayloadRate; // The rates are given as the number of bytes per second for all peers for this torrent

};
//-----------------------------------------------------------------------------
//Handle sur un torrent charge par la session
//Permet d'agir sur le torrent ou du recuperer ses informations
//Since the torrents are processed by a background thread, there is no guarantee that a handle will remain valid between two calls.
class LM_API_WEB BtHandle : public Core::Object
{
public:
    BtHandle(const libtorrent::torrent_handle& h);
    ~BtHandle();

    // -1 : unlimited connections
    int32 getMaxConnections();
    void setMaxConnections(int32 maxConnection);

    // -1 : unlimited unchokes
    int32 getMaxUploads();
    void setMaxUploads(int32 limit);

    // valid ratios are 0 (infinite ratio) or [ 1.0 , inf )
    // the ratio is uploaded / downloaded. less than 1 is not allowed
    void setRatio(float ratio);

    int32 getUploadLimit();
    void setUploadLimit(int32 limit);
    
    int32 getDownloadLimit();
    void setDownloadLimit(int32 limit);

    String getSavePath();
    String getName();

    bool isValid()      const;
    bool isSeed()       const;

    bool isFinished()   const;
    bool isPaused()     const;
    void pause()        const;
    void resume()       const;

    TorrentStatus getStatus() const;

private:
    libtorrent::torrent_handle* _pH;
};

}

#endif