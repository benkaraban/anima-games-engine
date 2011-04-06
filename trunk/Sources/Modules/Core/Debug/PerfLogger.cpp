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
#include <Core/Debug/PerfLogger.h>

namespace Core
{
//-----------------------------------------------------------------------------
PerfLogger PerfLogger::sPerfLogger;
//-----------------------------------------------------------------------------
PerfLogger::PerfLogger()
: _isFrameInProgress(false)
{
}
//-----------------------------------------------------------------------------
void PerfLogger::startFrame()
{
    Core::ScopedLock lock(_mutex);

    _sw.reset();
    _tracks.clear();
    _history.clear();
    _isFrameInProgress = true;
}
//-----------------------------------------------------------------------------
void PerfLogger::endFrame()
{
    Core::ScopedLock lock(_mutex);
    _isFrameInProgress = false;
}
//-----------------------------------------------------------------------------
void PerfLogger::start(const String & name)
{
    Core::ScopedLock lock(_mutex);

    int32 iTrack = 0;

    while(iTrack < _tracks.size() && _tracks[iTrack].isActive)
        iTrack ++;
        
    if(iTrack == _tracks.size())
        _tracks.push_back(Track());
    
    Track & track = _tracks[iTrack];
    
    track.isActive = true;
    track.curWork.name = name;
    track.curWork.iTrack = iTrack;
    track.curWork.startTime = _sw.elapsed().ftotalSeconds();
}
//-----------------------------------------------------------------------------
void PerfLogger::end(const String & name)
{
    Core::ScopedLock lock(_mutex);

    int32 iTrack = 0;

    while(iTrack < _tracks.size() && _tracks[iTrack].curWork.name != name)
        iTrack ++;

    if(iTrack >= _tracks.size())
        throw Core::Exception(L"PerfLogger start/end mismatch");

    Track & track = _tracks[iTrack];
    
    track.isActive = false;
    track.curWork.endTime = _sw.elapsed().ftotalSeconds();
    _history.push_back(track.curWork);    
}
//-----------------------------------------------------------------------------
const Core::List<PerfLogger::Work> & PerfLogger::getHistory() const
{ 
    Core::ScopedLock lock(_mutex);

    if(_isFrameInProgress)
        throw Core::Exception(L"Reading PerfLogger history during frame");
    return _history;
}
//-----------------------------------------------------------------------------
}
