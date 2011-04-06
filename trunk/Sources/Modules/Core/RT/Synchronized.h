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
#ifndef CORE_SYNCHRONIZED_H_
#define CORE_SYNCHRONIZED_H_

#include <Core/RT/Mutex.h>
#include <Core/RT/ConditionVar.h>

namespace Core
{
template <class TSyncVar>
class Synchronized : public Object
{
public:
    Synchronized() {};
    Synchronized(const TSyncVar & value);
    
    void operator = (const TSyncVar & value);
    void waitEquals(const TSyncVar & value);
    void waitGreaterThan(const TSyncVar & value);
    void waitGreatEquals(const TSyncVar & value);
    void waitLessEquals(const TSyncVar & value);
    void waitLesserThan(const TSyncVar & value);
    
    template <class TCondition>
    void waitCondition(const TCondition & condition);
    
    bool operator == (const TSyncVar & value);
    bool operator != (const TSyncVar & value);
    
    virtual String toString() const;
    
private:
    Synchronized(const Synchronized<TSyncVar> & sync);
    Synchronized<TSyncVar> & operator = (const Synchronized<TSyncVar> sync);
    
    typedef volatile TSyncVar TVolatileSyncVar;

    ConditionMutex      _mutex;
    ConditionVar        _condition;
    TVolatileSyncVar    _variable;
};
//-----------------------------------------------------------------------------
template <class TSyncVar>
Synchronized<TSyncVar>::Synchronized(const TSyncVar & value) : _variable(value)
{
    
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
void Synchronized<TSyncVar>::operator = (const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    _variable = value;
    _condition.notifyAll();
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
void Synchronized<TSyncVar>::waitEquals(const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    while(_variable != value)
    {
        _condition.wait(lock);
    }
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
void Synchronized<TSyncVar>::waitGreaterThan(const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    while(_variable <= value)
    {
        _condition.wait(lock);
    }
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
void Synchronized<TSyncVar>::waitLesserThan(const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    while(_variable >= value)
    {
        _condition.wait(lock);
    }
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
void Synchronized<TSyncVar>::waitGreatEquals(const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    while(_variable < value)
    {
        _condition.wait(lock);
    }
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
void Synchronized<TSyncVar>::waitLessEquals(const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    while(_variable > value)
    {
        _condition.wait(lock);
    }
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
template <class TCondition>
void Synchronized<TSyncVar>::waitCondition(const TCondition & condition)
{
    ScopedCondLock lock(_mutex);
    while(!condition(_variable))
    {
        _condition.wait(lock);
    }
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
bool Synchronized<TSyncVar>::operator == (const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    return _variable == value;
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
bool Synchronized<TSyncVar>::operator != (const TSyncVar & value)
{
    ScopedCondLock lock(_mutex);
    return _variable != value;
}
//-----------------------------------------------------------------------------
template <class TSyncVar>
String Synchronized<TSyncVar>::toString() const
{
    String result(L"Synchro(");
    result << Core::toString(_variable) << L")";
    return result;
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_SYNCHRONIZED_H_*/
