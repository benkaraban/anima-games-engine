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

#ifdef _WIN32
#include <winsafe.h>
#else
#include <sys/types.h>
#include <pthread.h>
#endif

#include <Core/Debug/Assert.h>
#include <iostream>
#include <limits>
#include "ConditionVar.h"

namespace Core
{
#ifdef _WIN32

#pragma warning( push )
#pragma warning( disable : 4290 )

// Implémentation Windows tirée de :
// http://www.cse.wustl.edu/~schmidt/win32-cv-1.html

//-----------------------------------------------------------------------------
ConditionVar::ConditionVar() : _semaphore(0x7FFFFF, 0), _waitCount(0), _wasBroadcast(false), _pEvent(NULL)
{
    _pEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    LM_ASSERT(_pEvent != NULL);
}
//-----------------------------------------------------------------------------
ConditionVar::~ConditionVar()
{    
}
//-----------------------------------------------------------------------------
void ConditionVar::wait(ScopedCondLock & lock) throw (RTException)
{
    _mutex.lock();
    _waitCount ++;
    _mutex.unlock();

    if(SignalObjectAndWait(lock._rMutex._pInternal, _semaphore._pInternal, INFINITE, FALSE) == WAIT_FAILED)
    {
        LM_ASSERT(false);
        String message("Unable to wait for ConditionVar. WinError on SignalObjectAndWait : 0x");
        message << toStringHex(GetLastError(), 8);
        throw RTException(message);
    }

    _mutex.lock();
    _waitCount --;
    bool lastWaiter = _wasBroadcast && (_waitCount == 0);
    _mutex.unlock();

    if(lastWaiter)
    {
        if(SignalObjectAndWait(_pEvent, lock._rMutex._pInternal, INFINITE, FALSE) == WAIT_FAILED)
        {
            LM_ASSERT(false);
            String message("Unable to wait for condition var mutex. WinError on SignalObjectAndWait : 0x");
            message << toStringHex(GetLastError(), 8);
            throw RTException(message);
        }
    }
    else
        lock._rMutex.lock();
}
//-----------------------------------------------------------------------------
void ConditionVar::notify()
{
    _mutex.lock();
    bool hasWaiter = (_waitCount > 0);
    _mutex.unlock();

    if(hasWaiter)
        _semaphore.release();
}
//-----------------------------------------------------------------------------
void ConditionVar::notifyAll()
{
    _mutex.lock();
    bool hasWaiter = false;

    if(_waitCount > 0)
    {
        hasWaiter = true;
        _wasBroadcast = true;
    }

    if(hasWaiter)
    {
        _semaphore.release(_waitCount);
        _mutex.unlock();

        if(WaitForSingleObject(_pEvent, INFINITE) == WAIT_FAILED)
        {
            LM_ASSERT(false);
            String message("Unable to wait for condition var broadcast event. WinError on SignalObjectAndWait : 0x");
            message << toStringHex(GetLastError(), 8);
            throw RTException(message);
        }
        _wasBroadcast = false;
    }
    else
    {
        _mutex.unlock();
    }
}

#pragma warning( pop )
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
ConditionVar::ConditionVar()
{
    pthread_cond_init(reinterpret_cast<pthread_cond_t *>(_internal), NULL);
}
//-----------------------------------------------------------------------------
ConditionVar::~ConditionVar()
{   
    pthread_cond_destroy(reinterpret_cast<pthread_cond_t *>(_internal));
}
//-----------------------------------------------------------------------------
void ConditionVar::wait(ScopedCondLock & lock) throw (RTException)
{
    int32 returnCode = 0;
    returnCode = pthread_cond_wait( reinterpret_cast<pthread_cond_t *>(_internal), 
                                    reinterpret_cast<pthread_mutex_t *>(lock._rMutex._pInternal) );
    if( returnCode != 0)
    {
        String message("Failed to wait on the ConditionVar. Posix thread error on pthread_cond_wait : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
    
}
//-----------------------------------------------------------------------------
void ConditionVar::notify()
{
    int32 returnCode = 0;
    returnCode = pthread_cond_signal(reinterpret_cast<pthread_cond_t *>(_internal));
    if( returnCode != 0)
    {
        String message("Failed to notify the ConditionVar. Posix thread error on pthread_cond_signal : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
void ConditionVar::notifyAll()
{
    int32 returnCode = 0;
    returnCode = pthread_cond_broadcast(reinterpret_cast<pthread_cond_t *>(_internal));
    if( returnCode != 0)
    {
        String message("Failed to notifyAll the ConditionVar. Posix thread error on pthread_cond_broadcast : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
#endif
}
