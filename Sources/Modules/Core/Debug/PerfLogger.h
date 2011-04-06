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
#ifndef CORE_PERFLOGGER_H_
#define CORE_PERFLOGGER_H_

#include <Core/List.h>
#include <Core/RT/Mutex.h>
#include <Core/StopWatch.h>

#define LM_IS_PERF_LOGGER_ON    1

#if LM_IS_PERF_LOGGER_ON

#define LM_START_PERF_LOG_FRAME() Core::PerfLogger::sPerfLogger.startFrame()
#define LM_END_PERF_LOG_FRAME() Core::PerfLogger::sPerfLogger.endFrame()
#define LM_START_PERF_LOG(name) Core::PerfLogger::sPerfLogger.start(name)
#define LM_END_PERF_LOG(name) Core::PerfLogger::sPerfLogger.end(name)

#else

#define LM_START_PERF_LOG_FRAME()
#define LM_END_PERF_LOG_FRAME()
#define LM_START_PERF_LOG(name)
#define LM_END_PERF_LOG(name)

#endif

namespace Core
{
class LM_API_COR PerfLogger
{
public:
    PerfLogger();

    void startFrame();
    void endFrame();

    void start(const String & name);
    void end(const String & name);

    struct Work
    {
        String      name;
        int32       iTrack;
        double      startTime;
        double      endTime;
    };

    const Core::List<PerfLogger::Work> & getHistory() const;

    static PerfLogger sPerfLogger;

protected:
    struct Track
    {
        bool        isActive;
        Work        curWork;
    };

    mutable Core::Mutex _mutex;
    Core::StopWatch     _sw;
    Core::List<Track>   _tracks;
    Core::List<Work>    _history;
    volatile bool       _isFrameInProgress;
};
}

#endif
