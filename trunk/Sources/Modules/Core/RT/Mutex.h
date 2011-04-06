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
#ifndef CORE_MUTEX_H_
#define CORE_MUTEX_H_

#include <Core/Object.h>

namespace Core
{
/**
 * Cette classe implémente un mécanisme d'exclusion mutuel (mutex).
 * 
 * Il est conseillé, lorsque c'est possible d'utiliser la classe
 * ScopedLock qui réduit grandement les risques d'erreur de manipulation.
 * 
 * \ingroup RT
 */
class LM_API_COR Mutex : public Object
{
public:
    /**
     * Initialise le mutex. Initialement le mutex n'est pas verrouillé.
     */
    Mutex();
    
    /**
     * Détruit le mutex. Si des threads sont en attente sur le mutex,
     * le comportement n'est pas défini. Il faut donc éviter le destruction
     * des mutex susceptibles d'être encore utilisés.
     */
    ~Mutex();
    
    /**
     * Verrouille le Mutex. Si le mutex est déjà vérouillé lors de l'appel à cette
     * fonction, le thread appeleant est bloqué jusqu'à ce qu'un autre thread
     * déverrouille le Mutex. Lorsque c'est possible, il est préférable de passer
     * par un ScopedLock plutôt que d'appeler directement cette méthode.
     */
    void lock();
    
    /**
     * Déverrouille le Mutex. Si un ou plusieurs threads sont en attente du mutex,
     * l'un d'entre eux va être débloqué. C'est le thread qui a verrouillé le mutex
     * qui doit le déverrouiller. De plus, c'est une erreur d'appeler cette méthode
     * qur un mutex non verrouillé. Dans un cas comme dans l'autre, le comportement
     * est indéfini.
     */
    void unlock();
    
private:
    /**
     * Attention, ceci est un hack. Cet attribut contient la représentation
     * du mutex d'un point de vue système. Sous Windows c'est un objet de type
     * CRITICAL_SECTION.
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
     * Pour info sizeof(CRITICAL_SECTION) = 24.
     *        et sizeof(__SIZEOF_PTHREAD_MUTEX_T) = 24 aussi
     */    
    byte    _internal[24];
};
//-----------------------------------------------------------------------------
/**
 * 
 */
class ScopedLock
{
public:
    /**
     * Initialise le ScopedLock. A l'initialisation, le ScopedLock appelle la méthode
     * lock du Mutex.
     */
    inline ScopedLock(Mutex & mutex);

    /**
     * Détruit le ScopedLock. Avant d'être détruit, le ScopedLock appelle la méthode
     * unlock du Mutex.
     */
    inline ~ScopedLock();
    
private:
    ScopedLock();
    
    Mutex & _rMutex;
};
//-----------------------------------------------------------------------------
ScopedLock::ScopedLock(Mutex & mutex) : _rMutex(mutex)
{
    _rMutex.lock();
}
//-----------------------------------------------------------------------------
ScopedLock::~ScopedLock()
{
    _rMutex.unlock();
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_MUTEX_H_*/
