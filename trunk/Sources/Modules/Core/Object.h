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
#ifndef CORE_OBJECT_H_
#define CORE_OBJECT_H_

#if defined _MSC_VER
#pragma warning(disable: 4275)
#else
#include <typeinfo>
#endif

#include <Core/String.h>

#define CUSTOM_NEW_OPERATOR 1

namespace Core
{
template <class TObject>
class Ptr;

/**
 * Cette classe est la classe mère de toutes les classes concrêtes du projet.
 *
 * Seules quelques classes de bas niveau n'héritent pas de Object, ainsi
 * que des classes de données brutes comme les vecteurs.
 *
 * Toutes les classes héritant d'Object sont allouées avec l'allocateur LOADED
 * via Core::Allocator. Elles peuvent également être manipulées
 * par des pointeurs intelligents via Core::Ptr.
 *
 * La classe offre également quelques services de base comme une méthode toString
 * par défaut.
 *
 * \ingroup Standard
 */
class Object
{
public:
    /**
     * Cette méthode retourne une chaîne représentant l'objet. Cette méthode
     * est destinée à être redéfinie pour inclure des données pertinentes. Par
     * défaut, cette méthode retourne une chaîne de la forme "<NomDeLaClasse @ 0xADDRESSE>".
     */
    virtual String toString() const;

    /**
     * Cette méthode retourne le nom de la classe de l'instance, ce qui permet de
     * connaître le type d'une classe lorsqu'on ne connaît que son interface.
     */
    virtual String getClassName() const;

    /**
     * Le destructeur est déclaré virtuel pour pouvoir désallouer correctement
     * les classes filles.
     */
    virtual ~Object() {};

#if CUSTOM_NEW_OPERATOR
    /**
     * Alloue un nouvel objet avec l'allocateur mémoire LOADED, Core::Allocator.
     */
    inline void * operator new (size_t size);

    /**
     * Alloue un tableau d'objets avec l'allocateur mémoire LOADED, Core::Allocator.
     */
    inline void * operator new [] (size_t size);

    /**
     * Libère un objet avec l'allocateur mémoire LOADED, Core::Allocator.
     */
    inline void operator delete (void * ptr);

    /**
     * Libère un tableau d'objets avec l'allocateur mémoire LOADED, Core::Allocator.
     */
    inline void operator delete [] (void * ptr);

    inline static void* operator new(std::size_t size, void * ptr) throw()
    { return ::operator new(size, ptr); };

    inline static void operator delete(void *pMemory, void * ptr) throw()
    { return ::operator delete(pMemory, ptr); };
#endif
};
//-----------------------------------------------------------------------------
inline String Object::toString() const
{
    String result(getClassName());
    result << L" @ " << Core::toString(this);
    return result;
}
//-----------------------------------------------------------------------------
inline String Object::getClassName() const
{
    String  mangledName(typeid(*this).name());
    String  result;

#if defined _MSC_VER

    String token = L"class ";
    int32 iPos = int32(mangledName.find(token));
    if(iPos != String::npos)
    {
        result << mangledName.substr(iPos + token.length(), mangledName.length());
    }

#else

    const wchar_t DIGITS [] = L"0123456789";

    bool first = true;
    int32 iPos = mangledName.find_first_of(DIGITS);
    
    while(iPos != String::npos && iPos < mangledName.length())
    {
        int32 iStartText = mangledName.find_first_not_of(DIGITS, iPos);
        
        if(iStartText != String::npos && iStartText != iPos)
        {
            int32 textLength = toInteger(mangledName.substr(iPos, iStartText - iPos));
            
            if(first)
                first = false;
            else
                result << L"::";
            
            result << mangledName.substr(iStartText, textLength);
            
            iPos = iStartText + textLength;
        }
        else
        {
            iPos = (int32)String::npos;
        }
    }
#endif
    
    return result;
}
//-----------------------------------------------------------------------------
#if CUSTOM_NEW_OPERATOR
//-----------------------------------------------------------------------------
inline void * Object::operator new (size_t size)
{
    return Allocator::getInstance().allocate(size);
}
//-----------------------------------------------------------------------------
inline void * Object::operator new [] (size_t size)
{
    return Allocator::getInstance().allocate(size);
}
//-----------------------------------------------------------------------------
inline void Object::operator delete (void * ptr)
{
    Allocator::getInstance().free(ptr);
}
//-----------------------------------------------------------------------------
inline void Object::operator delete [] (void * ptr)
{
    Allocator::getInstance().free(ptr);
}
#endif
//-----------------------------------------------------------------------------
inline String toString(const Object & object)
{
    return object.toString();
}
}
#endif /*CORE_OBJECT_H_*/
