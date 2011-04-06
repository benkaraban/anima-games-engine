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
#ifndef CORE_DOUBLEQUEUE_H_
#define CORE_DOUBLEQUEUE_H_

#include <deque>
#include <Core/Memory/STLAllocator.h>

namespace Core
{
/**
 * Cette classe implémente une file à double entrée générique. On peut
 * aussi bien insérer et retirer des éléments en tête qu'en queue
 * de la file. C'est une généralisation de la file FIFO.  
 * 
 * La classe hérite de la classe STL queue.
 * http://www.cplusplus.com/reference/stl/deque/
 * 
 * \ingroup Containers
 */
template <class TElement>
class DoubleQueue : public Object, public std::deque<TElement, STLAllocator<TElement> >
{
public:
    /**
     * Retourne une chaîne de caractère avec le contenu de la DoubleQueue.
     */
    virtual String toString() const;

    inline int32 size() const { return (int32) std::deque<TElement, STLAllocator<TElement> >::size(); }
};
//-----------------------------------------------------------------------------
template <class TElement>
String DoubleQueue<TElement>::toString() const
{
    String result(L"DoubleQueue of size ");
    result << Core::toString(this->size());
    return result;
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_DOUBLEQUEUE_H_*/
