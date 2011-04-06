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

#ifndef CORE_STLALLOCATOR_H_
#define CORE_STLALLOCATOR_H_

#include <Core/Memory/Allocator.h>

namespace Core
{
/**
 * Interface d'allocation mémoire pour les classes de la STL.
 * Cette classe peut être utilisée comme classe d'allocation par
 * les containers de la STL. Elle se contente de servir d'interface
 * vers la classe Core::Allocator.
 * 
 * \ingroup Memory
 */
template < typename T >
class STLAllocator
{
public:
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    STLAllocator() throw() {}
    STLAllocator(const STLAllocator&) throw () {}
    
    template <class U>    
    STLAllocator(const STLAllocator<U>&){}
    
    ~STLAllocator(){}

    template <class U>    
    STLAllocator & operator=(const STLAllocator<U>&) throw() 
    { return *this; }

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    STLAllocator & operator=(const STLAllocator&) { return *this; }
    void construct(pointer p, const T& val) { new ((T*) p) T(val); }
    void destroy(pointer p) { p->~T(); }

    size_type max_size() const { return size_t(-1); }

    template <class U>
    struct rebind { typedef STLAllocator<U> other; };

    pointer allocate(size_type n, const void * = 0)
    {
        return (pointer)Allocator::getInstance().allocate(int32(n) * int32(sizeof(T)));
    }
  
    void deallocate(void* p, size_type) 
    {    
        Allocator::getInstance().free(p);
    }
    
    bool operator == (const STLAllocator & allocator) const
    {
        return true;
    }    
};
}

#endif
