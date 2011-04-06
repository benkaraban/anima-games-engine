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
#ifndef CORE_QUEUE_H_
#define CORE_QUEUE_H_

#include <queue>
#include <Core/Object.h>

namespace Core
{
/**
 * Cette classe implémente une file FIFO générique.  
 * 
 * La classe hérite de la classe STL queue.
 * http://www.cplusplus.com/reference/stl/queue/
 * 
 * \ingroup Containers
 */
template <class TElement>
class Queue : public Object, public std::queue<TElement, std::deque<TElement, STLAllocator<TElement> > >
{
public:
    /**
     * Retourne une chaîne de caractères contenant la taille de la file et
     * l'élément en tête de la file.
     */
    virtual String toString() const;

    void clear()
    {
        while(!this->empty())
            this->pop();
    }
};
//-----------------------------------------------------------------------------
template <class TElement>
String Queue<TElement>::toString() const
{
    String result(L"Queue(size = ");
    result << Core::toString(int32(this->size()));
    result << L")";
    return result;
}
//-----------------------------------------------------------------------------
template <class TElement>
String toString(const Queue<TElement> & q)
{
    String result(L"Queue(size = ");
    result << Core::toString(int32(q.size()));
    if(!q.empty())
    {
        result << L", first = " << Core::toString(q.front());
    }
    result << L")";
    return result;    
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_QUEUE_H_*/

