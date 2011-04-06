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
#ifndef CORE_THREAD_H_
#define CORE_THREAD_H_

#include <Core/Object.h>
#include <Core/NonCopyable.h>

namespace Core
{
/**
 * \defgroup RT Package RT
 * Ce package regroupe les classes utiles pour la programmation temps réel.
 * \ingroup Core
 */

typedef void (* ThreadFunc) ();
typedef void (Object::*ThreadMethod) ();
typedef void (* ThreadFuncInt) (int32);

class LM_API_COR Thread : public Object, public NonCopyable
{
public:
    Thread(ThreadFunc pThreadFunc);

    template <class TObject>
    Thread(void (TObject::* pThreadMethod) (), Object & object);

    void join() const;

    static void sleep(int32 periode);
    static void quickSleep(int32 periode, int32 timerResolution=1);

    static int32 getCurrentThreadID();

protected:
    static int32 threadLauncher(void * argument);
    static void * posixThreadLauncher(void * argument);

    void createThread();

    ThreadFunc    _pFunc;
    ThreadMethod  _pThreadMethod;
    union
    {
    void *              _handle;
    unsigned long int   _posixthread;
    } _osSpecific;
    Object *      _pObject;
};
//-----------------------------------------------------------------------------
template <class TObject>
Thread::Thread(void (TObject::* pThreadMethod) (), Object & object)
    : _pFunc(NULL),
      _pThreadMethod(reinterpret_cast<ThreadMethod>(pThreadMethod)),
      _pObject(&object)
{
    createThread();
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_THREAD_H_*/
