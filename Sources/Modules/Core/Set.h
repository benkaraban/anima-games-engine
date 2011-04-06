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
#ifndef CORE_SET_H_
#define CORE_SET_H_

#include <set>
#include <Core/Standard.h>

namespace Core
{
/**
 * Cette classe implémente un ensemble générique.
 * Cet ensemble ne contient que des éléments uniques. Les recherches
 * d'élément sont très rapides mais si l'on se contente de parcourir
 * un Set sans faire d'opérations de recherches fréquentes, il peut
 * être beaucoup plus efficace d'utiliser une Core::List, quitte
 * à ne pas tenir compte de l'ordre des éléments. De fait, même si
 * en pratique vous avez besoin d'un ensemble en terme mathématique,
 * c'est souvent la Core::List qu'il faut préférer au Set.
 *
 * La classe Set est implémentée avec la classe STL set.
 * http://www.cplusplus.com/reference/stl/set/
 *
 * \ingroup Containers
 */
template <class TElement>
class Set : public Object, public std::set<TElement, std::less<TElement>, STLAllocator<TElement> >
{
public:
    Set() : std::set<TElement, std::less<TElement>, STLAllocator<TElement> >()
    {};

    Set(const Set<TElement> & set) : std::set<TElement, std::less<TElement>, STLAllocator<TElement> >(set)
    {};

    template<class TInputIterator>
    Set(TInputIterator first, TInputIterator last)
        : std::set<TElement, std::less<TElement>, STLAllocator<TElement> >(first, last)
    {};

    /**
     * Retourne une chaîne de caractère avec le contenu de la liste.
     */
    virtual String toString() const;
};
//-----------------------------------------------------------------------------
template <class TElement>
String Set<TElement>::toString() const
{
    String result(L"Set of size ");
    result << Core::toString(int32(this->size()));
    return result;
}
//-----------------------------------------------------------------------------
template <class TElement>
String toString(const Set<TElement> & s)
{
    String result("(");
    bool   first = true;
    
    for(typename Set<TElement>::const_iterator iElement = s.begin(); iElement != s.end(); ++iElement)
    {
        if(first)
            first = false;
        else
            result << L", ";

        result << indirectToString(*iElement);
    }
    result << L")";
    return result;
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_SET_H_*/
