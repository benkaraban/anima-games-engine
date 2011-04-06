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
#ifndef CORE_SYNCQUEUE_H_
#define CORE_SYNCQUEUE_H_

#include <Core/Object.h>
#include <Core/Queue.h>
#include <Core/RT/Mutex.h>
#include <Core/RT/ConditionVar.h>

namespace Core
{
template <class TElement>
class SyncQueue : public Object, public NonCopyable
{
public:
    SyncQueue();
    void push(const TElement & element);
    TElement pop();
    bool popIfAvailable(TElement & element);
    void kill();
    
private:    
    Queue<TElement> _queue;
    bool            _killed;
    
    ConditionMutex _mutex;
    ConditionVar   _condition;
};
//-----------------------------------------------------------------------------
template <class TElement>
SyncQueue<TElement>::SyncQueue() : _killed(false)
{}
//-----------------------------------------------------------------------------
template <class TElement>
void SyncQueue<TElement>::kill()
{
    ScopedCondLock lock(_mutex);
    _killed = true;
    _condition.notifyAll();
}
//-----------------------------------------------------------------------------
template <class TElement>
void SyncQueue<TElement>::push(const TElement & element)
{
    ScopedCondLock lock(_mutex);
    _queue.push(element);
    _condition.notify();
}
//-----------------------------------------------------------------------------
template <class TElement>
TElement SyncQueue<TElement>::pop()
{
    ScopedCondLock lock(_mutex);

    while(_queue.empty() && !_killed)
    {
        _condition.wait(lock);
    }
    
    if(_killed)
        throw RTInterruptedException(L"SyncQueue pop error : queue was killed while attempting a pop.");
    
    TElement result(_queue.front());
    _queue.pop();
    
    return result;
}
//-----------------------------------------------------------------------------
template <class TElement>
bool SyncQueue<TElement>::popIfAvailable(TElement & element)
{
    ScopedCondLock lock(_mutex);
    
    if(_queue.empty())
    {
        return false;
    }
    else
    {
        element = _queue.front();
        _queue.pop();
        return true;
    }
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_SYNCQUEUE_H_*/
