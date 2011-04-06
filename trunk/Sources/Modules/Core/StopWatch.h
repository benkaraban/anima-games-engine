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
#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#ifdef _WIN32
#include <winsafe.h>
#else
#include <sys/time.h>
#endif

#include <algorithm>
#include <Core/Types.h>
#include <Core/DateAndTime.h>

namespace Core
{
#define LM_STOPWATCH_THREAD_AFFINITY_MASK 1
/**
 * StopWarch permet de mesurer le temps d'execution d'un code.
 * \code
 * Core::StopWatch chrono;
 * chrono.reset();                              // reinitialise le chronometre
 *
 * //le code devant etre mesure
 *
 * Core::TimeDuration td = chrono.elapsed();    //recupere la valeur de chronometre
 * DBG << L"temps d'execution: " << td;         //affiche la valeur
 * \endcode
 *
 */
class StopWatchBase
{
public:
    virtual void reset() = 0;
    virtual TimeDuration elapsed() = 0;
};

#ifdef _WIN32
class LM_API_COR StopWatchBaseWindows : public StopWatchBase
{
public:
	StopWatchBaseWindows();
    virtual ~StopWatchBaseWindows();

    inline virtual void reset();
    inline virtual TimeDuration elapsed();

private:
    LARGE_INTEGER _clockFrequency;
    uint32        _startCTC;
    int64         _oldElapsedTime;
    LARGE_INTEGER _startTime;
};

inline void StopWatchBaseWindows::reset()
{
    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), LM_STOPWATCH_THREAD_AFFINITY_MASK);
    QueryPerformanceCounter(&_startTime);
    SetThreadAffinityMask(GetCurrentThread(), oldMask);
    _startCTC = GetTickCount();
    _oldElapsedTime = 0;
}


#else
class LM_API_COR StopWatchBaseLinux : public StopWatchBase
{
public:
    StopWatchBaseLinux();
    virtual ~StopWatchBaseLinux();

    inline virtual void reset();
    inline virtual TimeDuration elapsed();

private:
    struct timeval     _startTime;
};

inline void StopWatchBaseLinux::reset()
{
    gettimeofday(&_startTime, 0);
}

inline TimeDuration StopWatchBaseLinux::elapsed()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, 0);

    TimeDuration usecTicks((currentTime.tv_sec - _startTime.tv_sec) * 1000000 + (currentTime.tv_usec - _startTime.tv_usec));
    return usecTicks;
}
#endif


#ifdef _WIN32
typedef StopWatchBaseWindows StopWatch;
#else
typedef StopWatchBaseLinux StopWatch;
#endif


}    // namespace Core
#endif /*STOPWATCH_H_*/
