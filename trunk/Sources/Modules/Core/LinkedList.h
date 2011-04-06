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
#ifndef CORE_LINKEDLIST_H_
#define CORE_LINKEDLIST_H_

#include <list>

#include <Core/Standard.h>
#include <Core/Memory/STLAllocator.h>

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Core
{
template <class TElement>
class LinkedList : public std::list<TElement, STLAllocator<TElement> >, public Object
{
public:

    typedef std::list<TElement, STLAllocator<TElement> > ListParent;

    LinkedList() : std::list<TElement, STLAllocator<TElement> >()
    {};

    LinkedList(const LinkedList<TElement> & list) : std::list<TElement, STLAllocator<TElement> >(list)
    {};

    template<typename TInputIterator>
    LinkedList(TInputIterator begin, TInputIterator end)
        : std::list<TElement, STLAllocator<TElement> >(begin, end)
    {};

    explicit LinkedList(int32 n, const TElement & elem = TElement())
        : std::list<TElement, STLAllocator<TElement> >(n, elem)
    {};

    inline int32 size() const { return (int32) list::size(); }

    void * data() { return &((*this)[0]); };
    const void * data() const { return &((*this)[0]); };

    LinkedList<TElement> & operator = (const LinkedList<TElement> & value)
    {
        this->ListParent::operator=(value);
        return *this;
    }

    void qerase(int32 pos)
    {
        (*this)[pos] = (*this)[this->size() - 1];
        this->pop_back();
    }

    /**
     * Retourne une chaîne de caractère avec le contenu de la liste.
     */
    virtual String toString() const;
};
//-----------------------------------------------------------------------------
template <class TElement>
String LinkedList<TElement>::toString() const
{
    String result(L"LinkedList of size ");
    result << Core::toString(this->size());
    return result;
}
//-----------------------------------------------------------------------------
template <class TElement>
String toString(const LinkedList<TElement> & list)
{
    String result(L"[");
    int32 length = list.size();
    if(length > 0)
    {
        for(int32 ii=0; ii < length - 1; ii++)
        {
            result << indirectToString(list[ii]) << L", ";
        }
        result << indirectToString(list[length - 1]);
    }
    result << L"]";
    return result;
}
//-----------------------------------------------------------------------------
}

#pragma warning( pop )

#endif /*CORE_LIST_H_*/
