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

#include "Mutex.h"

namespace Core
{
#ifdef _WIN32
//-----------------------------------------------------------------------------
Mutex::Mutex()
{
    InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
//-----------------------------------------------------------------------------
Mutex::~Mutex()
{
    DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
//-----------------------------------------------------------------------------
void Mutex::lock()
{
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
//-----------------------------------------------------------------------------
void Mutex::unlock()
{
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
Mutex::Mutex()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_init(reinterpret_cast<pthread_mutex_t *>(_internal) , NULL);
}
//-----------------------------------------------------------------------------
Mutex::~Mutex()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t *>(_internal));
}
//-----------------------------------------------------------------------------
void Mutex::lock()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t *>(_internal));
}
//-----------------------------------------------------------------------------
void Mutex::unlock()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t *>(_internal));
}
//-----------------------------------------------------------------------------
#endif
}
