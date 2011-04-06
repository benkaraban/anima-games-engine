/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include <Core/StopWatch.h>

namespace Core
{
#ifdef _WIN32
StopWatchBaseWindows::StopWatchBaseWindows()
{
    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), LM_STOPWATCH_THREAD_AFFINITY_MASK);
    QueryPerformanceFrequency(&_clockFrequency);
    SetThreadAffinityMask(GetCurrentThread(), oldMask);
    reset();
}

StopWatchBaseWindows::~StopWatchBaseWindows()
{
}
// Donne le temps passé depuis le dernier appel de la fonction reset
// Le code permet de corriger certains leap causés par l'architecture ISA
// Pour plus d'infos : http://support.microsoft.com/?scid=kb%3Ben-us%3B274323&x=2&y=11
TimeDuration StopWatchBaseWindows::elapsed()
{
    LARGE_INTEGER currentTime;
    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), LM_STOPWATCH_THREAD_AFFINITY_MASK);
    QueryPerformanceCounter(&currentTime);
    SetThreadAffinityMask(GetCurrentThread(), oldMask);
    int64 elapsedTime = currentTime.QuadPart - _startTime.QuadPart;

    // Compute the number of millisecond ticks elapsed.
    uint32 msecTicks = (uint32)((1000 * elapsedTime) / _clockFrequency.QuadPart);

    // Check for unexpected leaps in the Win32 performance counter.
    // (This is caused by unexpected data across the PCI to ISA
    // bridge, aka south bridge.  See Microsoft KB274323.)
    unsigned long elapsedTicks = GetTickCount() - _startCTC;
    signed long msecOff = (signed long)(msecTicks - elapsedTicks);
    if (msecOff < -100 || msecOff > 100)
    {
        // Adjust the starting time forwards.
        LONGLONG msecAdjustment = std::min((msecOff * _clockFrequency.QuadPart) / 1000, elapsedTime - _oldElapsedTime);
        _startTime.QuadPart += msecAdjustment;
        elapsedTime -= msecAdjustment;
    }

    // Store the current elapsed time for adjustments next time.
    _oldElapsedTime = elapsedTime;

    // convertion en microseconde.
    TimeDuration usecTicks = microseconds((Core::int32)((1000000 * elapsedTime) / _clockFrequency.QuadPart));

    return usecTicks;
};
#else

StopWatchBaseLinux::StopWatchBaseLinux()
{
    reset();
}

StopWatchBaseLinux::~StopWatchBaseLinux()
{
}
#endif
} // namespace Core
