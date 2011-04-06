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
#include <errno.h>
#include <semaphore.h>
#include <stdlib.h>
#endif

#include <iostream>
#include "Semaphore.h"

namespace Core
{
#ifdef _WIN32
//-----------------------------------------------------------------------------
Semaphore::Semaphore(int32 maxValue, int32 initialValue)
{
    _pInternal = CreateSemaphore(NULL, initialValue, maxValue, NULL);
}
//-----------------------------------------------------------------------------
Semaphore::~Semaphore()
{
    CloseHandle(_pInternal);
}
//-----------------------------------------------------------------------------
void Semaphore::acquire()
{
    if(WaitForSingleObject(_pInternal, INFINITE) == WAIT_FAILED)
    {
        String message("Unable to acquire semaphore. WinError on WaitforSingleObject : 0x");
        message << Core::toStringHex(GetLastError(), 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
bool Semaphore::acquire(int32 timeout)
{
    int32 result = WaitForSingleObject(_pInternal, timeout);
    
    if(result == WAIT_FAILED)
    {
        String message("Unable to acquire semaphore. WinError on WaitforSingleObject : 0x");
        message << Core::toStringHex(GetLastError(), 8);
        throw RTException(message);
    }    
    
    return (result != WAIT_OBJECT_0);
}
//-----------------------------------------------------------------------------
void Semaphore::release()
{
    if(ReleaseSemaphore(_pInternal, 1, NULL) == 0)
    {
        String message("Unable to release semaphore. WinError on ReleaseSemaphore : 0x");
        message << Core::toStringHex(GetLastError(), 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
void Semaphore::release(int32 count)
{
    if(ReleaseSemaphore(_pInternal, count, NULL) == 0)
    {
        String message("Unable to release semaphore. WinError on ReleaseSemaphore : 0x");
        message << Core::toStringHex(GetLastError(), 8);
        throw RTException(message);
    }
}
#else
//-----------------------------------------------------------------------------
Semaphore::Semaphore(int32 maxValue, int32 initialValue)
{
    _pInternal = malloc(sizeof(sem_t));
    sem_init(reinterpret_cast<sem_t *>(_pInternal), 0, initialValue);
}
//-----------------------------------------------------------------------------
Semaphore::~Semaphore()
{
    sem_destroy(reinterpret_cast<sem_t *>(_pInternal));
    free(_pInternal);    
}
//-----------------------------------------------------------------------------
void Semaphore::acquire()
{
    if( sem_wait(reinterpret_cast<sem_t *>(_pInternal)) != 0)
    {
        String message("Unable to acquire semaphore. Errno on sem_wait : 0x");
        message << Core::toStringHex(errno, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
bool Semaphore::acquire(int32 timeout)
{
    //TODO : gérer le timeout
    bool semGetSuccessfull = false;
    if( sem_trywait(reinterpret_cast<sem_t *>(_pInternal)) != 0)
    {
        semGetSuccessfull = false;
        if( errno != EAGAIN )
        {
            String message("Unable to acquire semaphore. Errno on sem_wait : 0x");
            message << Core::toStringHex(errno, 8);
            throw RTException(message);
        }
    }
    else
        semGetSuccessfull = true;
    return semGetSuccessfull;
}
//-----------------------------------------------------------------------------
void Semaphore::release()
{
    if( sem_post(reinterpret_cast<sem_t *>(_pInternal)) != 0)
    {
        String message("Unable to release semaphore. Errno on sem_post : 0x");
        message << Core::toStringHex(errno, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
void Semaphore::release(int32 count)
{
    for(int32 ii = 0; ii < count; ++ii)
    {
        if( sem_post(reinterpret_cast<sem_t *>(_pInternal)) != 0)
        {
            String message("Unable to release semaphore. Errno on sem_post : 0x");
            message << Core::toStringHex(errno, 8);
            throw RTException(message);
        }
    }
}
#endif
//-----------------------------------------------------------------------------
}    
