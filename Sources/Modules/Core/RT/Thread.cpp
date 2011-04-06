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
#include <unistd.h>
#endif

#include <Core/RT/RTException.h>

#include "Thread.h"

namespace Core
{
//-----------------------------------------------------------------------------
int32 Thread::threadLauncher(void * argument)
{
    Thread * pThread = reinterpret_cast<Thread*>(argument);

    if(pThread->_pObject != NULL)
    {
        (pThread->_pObject->*(pThread->_pThreadMethod))();
    }
    else if(pThread->_pFunc != NULL)
    {
        pThread->_pFunc();
    }

    return 0;
}
//-----------------------------------------------------------------------------
void * Thread::posixThreadLauncher(void * argument)
{
    threadLauncher(argument);
    return NULL;
}
//-----------------------------------------------------------------------------
Thread::Thread(ThreadFunc pThreadFunc)
    : _pFunc(pThreadFunc),
      _pThreadMethod(NULL),
      _pObject(NULL)
{
    createThread();
}
//-----------------------------------------------------------------------------
///****************************************************************************
//-----------------------------------------------------------------------------
#ifdef _WIN32
//-----------------------------------------------------------------------------
void Thread::createThread()
{
    _osSpecific._handle = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(threadLauncher), this, 0, NULL);
}
//-----------------------------------------------------------------------------
void Thread::join() const
{
    WaitForSingleObject(_osSpecific._handle, INFINITE);
}
//-----------------------------------------------------------------------------
void Thread::sleep(int32 periode)
{
    Sleep(periode);
}
//-----------------------------------------------------------------------------
void Thread::quickSleep(int32 periode, int32 timerResolution)
{
    timeBeginPeriod(timerResolution);
    Sleep(periode);
    timeEndPeriod(timerResolution);
}
//-----------------------------------------------------------------------------
int32 Thread::getCurrentThreadID()
{
    return int32(GetCurrentThreadId());
}
#else
//-----------------------------------------------------------------------------
void Thread::createThread()
{
    int32 returnCode = 0;
    returnCode = pthread_create( &_osSpecific._posixthread, NULL, posixThreadLauncher, this);
    if( returnCode != 0)
    {
        String message("Failed to create an new pthread. Posix thread error on pthread_create : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
}

void Thread::join() const
{
    int returnCode = 0;
    returnCode = pthread_join( _osSpecific._posixthread, NULL);
    if( returnCode != 0)
    {
        String message("Failed to join the thread. Posix thread error on pthread_join : 0x");
        message << Core::toStringHex(returnCode, 8);
        throw RTException(message);
    }
}
//-----------------------------------------------------------------------------
void Thread::sleep(int32 periode)
{
    usleep(periode * 1000);
}
//-----------------------------------------------------------------------------
void Thread::quickSleep(int32 periode, int32 timerResolution)
{
    usleep(periode * 1000);
}
//-----------------------------------------------------------------------------
int32 Thread::getCurrentThreadID()
{
    return int32(pthread_self());
}
#endif
//-----------------------------------------------------------------------------
}
