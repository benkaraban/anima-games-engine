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
#ifndef CORE_STANDARD_H_
#define CORE_STANDARD_H_

#include <Core/Types.h>
#include <Core/String.h>
#include <Core/Enum.h>
#include <Core/Object.h>
#include <Core/Exception.h>
#include <Core/Ptr.h>
#include <Core/Debug/Assert.h>

/**
 * \defgroup Core Module Core
 *
 * Le module Core regroupe toutes les classes de base du projet. Ce module
 * peut être référencé par tous les autres modules. A l'inverse, il ne peut
 * lui même référencer aucun autre module.
 */

/**
 * \defgroup Standard Classes standards
 *
 * Ce groupe contient toutes les classes qui sont à la base du framework LOADED
 * et qui devraient presque être considérées comme faisant partie intégrante du
 * langage. L'intégralité de ce groupe est incluse en incluant le fichier
 * Core/Standard.h. Ce fichier devrait généralement être inclus dans tous les headers
 * du projet.
 *
 * \section Types Types de base
 *
 * Ce fichier inclue les types de base Core::byte, Core::int8, Core::int32, etc... Il faut
 * toujours utiliser ces types quand on déclare un entier, car ils garantissent une taille
 * identique sur tous les compilateurs et toutes les plateformes du projet, contrairement
 * aux type C++ standards (int, short, long) qui n'ont pas toujours la même taille. Par défaut,
 * c'est le type Core::int32 qui devrait être utilisé pour tous les entiers qu'on manipule, à
 * moins d'avoir une raison particulière d'utiliser un type différent.
 *
 * Les classes Core::String et Core::String8, si elles ne représentent pas des types de base réels du C++
 * ont été faites pour être utilisées comme si elles étaient des types de base, elles
 * sont elles aussi inluses par le fichier Core/Standard.h
 *
 * \section Enums Types énumérés
 *
 * A tout type énuméré du projet doit correspondre une fonction toString qui convertit
 * un énuméré en chaîne de caractères et une fonction isValid qui retourne true si l'énuméré
 * est valide (ie. l'entier utilisé pour le représenter contient une valeur valide pour l'énuméré).
 *
 * Afin d'éviter d'écrire ces fonctions pour chaque énuméré que l'on crée, on utilise des macros
 * définies dans le fichier Core/Enum.h. Ces macros sont de la forme
 * LM_ENUM_xxx (NomEnumere, val1, val2 ... ) ou xxx est le nombre de valeurs possibles pour les
 * énumérés. Cette macro définie l'énuméré lui même et les fonction toString et isValid
 * correspondantes.
 *
 * Exemple d'utilisation :
 * \code
 * LM_ENUM_4(EDirection, DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_LEFT);
 *
 * EDirection direction = DIR_UP;
 * String string = toString(direction);
 * // Après cela string = "DIR_UP";
 *
 * int32 encodedDir = getValueFromFileOrNetwork();
 * direction = EDirection(encodedDir);
 *
 * // On a récupéré direction à partir d'une valeur entière
 * // qui pourrait bien être incorrecte, il faut vérifier
 * // sa validité.
 * if(!isValid(direction))
 *     throw Exception(L"Incorrect value");
 * \endcode
 *
 * \section Classes Classes de base
 *
 * Le fichier inclue la classe Core::Object qui est les classes mères de toutes
 * les classes du projet. Seules quelques classes de très bas niveau
 * (comme l'allocateur mémoire Core::Allocator ou Core::String) n'en héritent pas.
 *
 * La classe Core::Exception est également incluse. Toutes les exceptions définies dans le projet
 * doivent hériter de cette classe.
 *
 * \section Using Using et namespace
 *
 * En plus d'inclure directement les classes de base, ce header définit un using des
 * types de base, des classes Core::String, Core::String8 et Core::Ptr. Ces classes peuvent donc être
 * utilisées comme si elles étaient dans le namespace global, bien qu'en cas de conflit on
 * puisse toujours utiliser explicitement leur namespace pour les différencier de
 * leurs homonymes.
 *
 * \section Macros Macros
 *
 * Enfin, le fichier standard définit quelques macros utiles comme LM_DEBUG_CAST ou foreach.
 *
 * \ingroup Core
 */
using Core::String8;
using Core::String;
using Core::Ptr;
using Core::null;

using Core::byte;
using Core::int8;
using Core::uint8;
using Core::int16;
using Core::uint16;
using Core::int32;
using Core::uint32;
using Core::int64;
using Core::uint64;

#ifdef LM_GLOBAL_NEW
//-----------------------------------------------------------------------------
inline void * operator new (size_t size)
{
    return Core::Allocator::getInstance().allocate(size);
}
//-----------------------------------------------------------------------------
inline void * operator new [] (size_t size)
{
    return Core::Allocator::getInstance().allocate(size);
}
//-----------------------------------------------------------------------------
inline void operator delete (void * ptr)
{
    Core::Allocator::getInstance().free(ptr);
}
//-----------------------------------------------------------------------------
inline void operator delete [] (void * ptr)
{
    Core::Allocator::getInstance().free(ptr);
}
//-----------------------------------------------------------------------------
#endif

/**
 * La macro foreach permet d'itérer sur un container de manière très simple,
 * en ne fournissant que le nom de l'itérateur et celui du container. Notez bien
 * qu'il s'agit du nom de l'itérateur ; si vous voulez accéder à l'élément lui
 * même il faut utiliser l'opérateur * ou l'opérateur -> pour accéder à ses
 * attributs et ses méthodes s'il s'agit d'une classe. Si le container
 * contient des Ptr alors il faut faire une double indirection pour accéder aux
 * éléments effectifs.
 *
 * Exemples d'utilisation :
 * \code
 * List<int32> myList;
 *
 * // Incrémente tous les éléments d'une liste.
 * foreach(iValue, myList)
 * {
 *     *iValue++;
 * }
 *
 * class MyClass
 * {
 * public:
 *     void doStuff() {};
 * };
 *
 * // Appelle la méthode doStuff sur tous les élements
 * List<MyClass> classList;
 *
 * foreach(iMyClass, classList)
 * {
 *     iMyClass->doStuff();
 * }
 *
 * // Appelle la méthode doStuff sur tous les élements
 * List<Ptr<MyClass> > classList;
 *
 * foreach(ipMyClass, classList)
 * {
 *     (*ipMyClass)->doStuff();
 * }
 *
 * // Avec une Map
 * Map<String, int32> myMap;
 *
 * foreach(iCurrent, myMap)
 * {
 *     std::cout << "Key   : " << String8(iCurrent->first) << std::endl;
 *     std::cout << "Value : " << toString(iCurrent->second) << std::endl;
 * }
 * \endcode
 *
 * \ingroup Standard
 */
#ifdef foreach
#undef foreach
#endif
#define foreach(iterName, containerName) for(typeof((containerName).begin()) iterName = (containerName).begin(); iterName != (containerName).end(); ++iterName)

#define LM_FOURCC(a, b, c, d) (((int)a) | (((int)b) << 8) | (((int)c) << 16) | (((int)d) << 24))

#ifdef _DEBUG
#define LM_DEBUG_CAST dynamic_cast
#else
/**
 * Cette macro permet de faire un cast différent selon le mode de compilation.
 * En mode Debug, il s'agit d'un dynamic_cast, qui va vérifier la classe et
 * retourner NULL si on essaye de caster un objet d'une classe dans une classe
 * incompatible. En mode Release, la macro ne fait qu'un simple static_cast,
 * beaucoup plus rapide mais qui ne fait aucune vérification.
 *
 * La macro doit être utilisée dans les cas où, à moins d'un gros bug dans notre code,
 * il ne peut pas y avoir d'erreur et donc qu'un static_cast suffirait.
 * Néanmoins, pour pouvoir plus facilement détecter les erreurs, on fait
 * un dynamic_cast en mode Debug, mode dans lequel on ne se soucie pas autant
 * des performances.
 *
 * \ingroup Standard
 */
#define LM_DEBUG_CAST static_cast
#endif


//LM_DEBUG_CAST sur un ptr
//LM_DEBUG_PTR_CAST<B> (A)
//ne pas oublier les parenthèses autour de A
#ifdef _DEBUG
/*#define LM_DEBUG_PTR_CAST Core::ptrDynamicCast*/
template<class T, class TypeBase>
inline Ptr<T> LM_DEBUG_PTR_CAST (TypeBase A)
{
    Ptr<T> pRet = Core::ptrDynamicCast<T>(A);

    if(pRet == null && A != null)
    {
        LM_ASSERT(false);
        throw(Core::Exception(L"ptrDynamicCast failed"));
    }

    return pRet;
}
#else
#define LM_DEBUG_PTR_CAST Core::ptrStaticCast
#endif

#define LM_DELETE(x) \
    delete x; \
    x = NULL;

#define LM_PTR_DELETE(x) \
    do { \
        LM_ASSERT(x.getCount() <= 1); \
        x = null; \
    } while(0);

/**
 * Cette macro permet de copier le contenu d'une liste de Ptr<A> en une liste de Ptr<B>,
 * à la condition que B soit un ancêtre de A (la conversion ne nécessite pas de cast).
 *
 * Exemple :
 *
 * List<Ptr<A> >  valuesa; // A hérite de B
 * List<Ptr<B> >  valuesb;
 *
 * ...
 *
 * Copie le contenue de valuesa dans valuesb
 * LM_CVT_PTR_LIST(valuesa, valuesb, B);
 *
 */
#define LM_CVT_PTR_LIST(source, destination, dstType) \
    (destination).clear(); \
    foreach(iValue, (source)) \
    { \
        (destination).push_back(Ptr<dstType>(*iValue)); \
    }
#endif /*CORE_STANDARD_H_*/
