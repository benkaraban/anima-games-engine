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
#ifndef CORE_CONDITIONVAR_H_
#define CORE_CONDITIONVAR_H_

#include <Core/RT/Mutex.h>
#include <Core/RT/Semaphore.h>
#include <Core/RT/ConditionMutex.h>
#include <Core/RT/RTException.h>

#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4290 )
#endif

namespace Core
{
class LM_API_COR ConditionVar : public Object
{
public:
    ConditionVar();
    ~ConditionVar();
    
    void wait(ScopedCondLock & lock) throw (RTException);
    void notify();
    void notifyAll();
    
private:   
#ifdef _WIN32
    Semaphore       _semaphore;
    
    Mutex           _mutex;
    volatile int32  _waitCount;
    volatile bool   _wasBroadcast;

    void *          _pEvent;
#else
    /**
     * Attention, ceci est un hack. Cet attribut contient la représentation
     * d'une ConditionVar d'un point de vue système. Sous Linux il s'agit 
     * de pthread_cond_t.
     * 
     * On pourrait faire un define WIN32 pour déclarer deux attributs 
     * diffèrents mais cela ferait inclure des headers systèmes (du genre
     * windows.h) à tous les fichiers qui dépendent de cette classe, ce qui 
     * n'est pas désirable. En règle générale, dans cette situation, le 
     * mieux serait d'utiliser un design pattern PIMPL mais cela requiert 
     * une allocation mémoire alors que la classe est censée rester très bas 
     * niveau.
     * 
     * La solution retenue est donc de déclarer un un tableau d'octets de 
     * taille suffisante et de caster le contenu dans le type spécifique
     * windows/linux.
     * 
     * Pour info sizeof(__SIZEOF_PTHREAD_COND_T) = 48 aussi
     */    
    byte    _internal[48];
#endif
};
}

#if defined _MSC_VER
#pragma warning( pop )
#endif

#endif /*CORE_CONDITIONVAR_H_*/
