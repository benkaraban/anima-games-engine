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
#include <stdlib.h>
#endif

#include <Core/RT/RTException.h>
#include "ConditionMutex.h"

namespace Core
{
#ifdef _WIN32
//-----------------------------------------------------------------------------
ConditionMutex::ConditionMutex()
{
    _pInternal = CreateMutex(NULL, FALSE, NULL);
}
//-----------------------------------------------------------------------------
ConditionMutex::~ConditionMutex()
{
    CloseHandle(_pInternal);    
}
//-----------------------------------------------------------------------------
void ConditionMutex::lock()
{
    if(WaitForSingleObject(_pInternal, INFINITE) == WAIT_FAILED)
    {
        String message("Unable to acquire mutex. WinError on WaitforSingleObject : 0x");
        message << Core::toStringHex(GetLastError(), 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
bool ConditionMutex::lock(int32 timeout)
{
    int32 result = WaitForSingleObject(_pInternal, timeout);
    
    if(result == WAIT_FAILED)
    {
        String message("Unable to acquire mutex. WinError on WaitforSingleObject : 0x");
        message << Core::toStringHex(GetLastError(), 8);
        throw RTException(message);
    }    
    
    return (result != WAIT_OBJECT_0);
}
//-----------------------------------------------------------------------------
void ConditionMutex::unlock()
{
    if(!ReleaseMutex(_pInternal))
    {
        String message("Unable to release mutex. WinError on ReleaseSemaphore : 0x");
        message << Core::toStringHex(GetLastError(), 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
ConditionMutex::ConditionMutex()
{
    int32 returnCode = 0;
    _pInternal = malloc(sizeof(pthread_mutex_t));
    returnCode = pthread_mutex_init(reinterpret_cast<pthread_mutex_t *>(_pInternal) , NULL);
    if( returnCode != 0)
    {
        String message("Failed to create a new ConditionMutex. Posix thread error on pthread_mutex_init : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
ConditionMutex::~ConditionMutex()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t *>(_pInternal));
    free(_pInternal);
}
//-----------------------------------------------------------------------------
void ConditionMutex::lock()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t *>(_pInternal));
    if( returnCode != 0)
    {
        String message("Failed to lock the ConditionMutex. Posix thread error on pthread_mutex_lock : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
bool ConditionMutex::lock(int32 timeout)
{
    //TODO : gérer le timeout
    int32 returnCode = 0;
    returnCode = pthread_mutex_trylock(reinterpret_cast<pthread_mutex_t *>(_pInternal));
    if( returnCode != 0)
    {
        String message("Failed to trylock the ConditionMutex. Posix thread error on pthread_mutex_trylock : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
    return (returnCode == 0);
}
//-----------------------------------------------------------------------------
void ConditionMutex::unlock()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t *>(_pInternal));
    if( returnCode != 0)
    {
        String message("Failed to unlock the ConditionMutex. Posix thread error on pthread_mutex_unlock : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
#endif
}
