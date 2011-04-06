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
#ifndef CORE_MAP_H_
#define CORE_MAP_H_

#include <map>
#include <Core/Object.h>
#include <Core/Standard.h>

#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace Core
{
template <class TKey> struct Hash {};

template <> struct Hash<int8> { size_t operator () (int8 key) const { return size_t(key); } };
template <> struct Hash<uint8> { size_t operator () (uint8 key) const { return size_t(key); } };
template <> struct Hash<int16> { size_t operator () (int16 key) const { return size_t(key); } };
template <> struct Hash<uint16> { size_t operator () (uint16 key) const { return size_t(key); } };
template <> struct Hash<int32> { size_t operator () (int32 key) const { return size_t(key); } };
template <> struct Hash<uint32> { size_t operator () (uint32 key) const { return size_t(key); } };
template <> struct Hash<int64> { size_t operator () (int64 key) const { return size_t(key); } };
template <> struct Hash<uint64> { size_t operator () (uint64 key) const { return size_t(key); } };

template <class TObject>
struct Hash<Ptr<TObject> >
{
    size_t operator () (const Ptr<TObject> & key) const { return size_t(key.get()); };
};

template <> struct Hash<String>
{
    size_t operator () (const String & string) const
    {
        int32 result = 0;
		for(String::const_iterator iChar = string.begin(); iChar != string.end(); ++iChar)
        {
            result += 5 * result + int32(*iChar);
        }
        return size_t(result);
    }
};

template <> struct Hash<String8>
{
    size_t operator () (const String8 & string) const
    {
        int32 result = 0;
		for(String8::const_iterator iChar = string.begin(); iChar != string.end(); ++iChar)
        {
            result += 5 * result + int32(*iChar);
        }
        return size_t(result);
    }
};

/**
 * Cette classe implémente une dictionnaire générique.
 * Le dictionnaire associe un élément unique à chaque clef.
 *
 * Remarque : les itérateurs des Map ont deux attributs publiques :
 * first, qui correspond à la clef et second qui contient la valeur
 * assiciée à la clef.
 *
 * Exemple d'utilisation :
 * \code
 * Map<String, int32> ages;
 * ages[L"Bill"] = 27;
 * ages[L"Jo"] = 32;
 *
 * int32 joAge = ages[L"Jo"];
 *
 * foreach(iCurrent, ages)
 * {
 *     std::cout << iCurrent->first << " is "
 *               << ages->second << " years old." << std::endl;
 * }
 *
 * Map<String, int32>::const_iterator iRoger = ages.find(L"Roger");
 *
 * if(iRoger == ages.end())
 *     std::cout << "Roger is not in the map." << std::endl;
 * else
 *     std::cout << "Roger is " << iRoger->second << std::endl;
 * \endcode
 *
 * La classe List hérite de la classe STL vector.
 * http://www.cplusplus.com/reference/stl/vector/
 *
 * \ingroup Containers
 */
template <class TKey, class TElement>
class Map : public std::map<TKey, TElement, std::less<TKey>, STLAllocator<std::pair<TKey, TElement> > >, public Object
{
public:

    const TElement & get(const TKey & key) const
    {
        return find(key)->second;
    }

    inline int32 size() const { return (int32) std::map<TKey, TElement, std::less<TKey>, STLAllocator<std::pair<TKey, TElement> > >::size(); }

    /**
     * Retourne une chaîne de caractères décrivant le contenu de la Map.
     */
    virtual String toString() const;
};
//-----------------------------------------------------------------------------
template <class TKey, class TElement>
String Map<TKey, TElement>::toString() const
{
    String result(L"Map of size ");
    result << Core::toString(int32(this->size()));
    return result;
}
//-----------------------------------------------------------------------------
template <class TKey, class TElement>
String toString(const Map<TKey, TElement> & map)
{
    String result(L"(");
    bool first = true;

    for(typename Map<TKey, TElement>::const_iterator iCurrent = map.begin(); iCurrent != map.end(); ++iCurrent)
    {
        if(first)
            first = false;
        else
            result << L", ";
        result << L"{" << indirectToString(iCurrent->first)
               << L" : " <<  indirectToString(iCurrent->second) << L"}";
    }
    result << L")";

    return result;
}
//-----------------------------------------------------------------------------
}

#if defined _MSC_VER
#pragma warning( pop )
#endif

#endif /*CORE_MAP_H_*/
