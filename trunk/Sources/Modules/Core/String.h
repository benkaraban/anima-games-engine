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
#ifndef CORE_STRING_H_
#define CORE_STRING_H_

#include <string>
#include <ostream>
#include <Core/Memory/STLAllocator.h>

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace Core
{
class String;

typedef std::basic_string<char, std::char_traits<char>, STLAllocator<char> > String8Parent;

/**
 * Cette classe gère des chaînes de caractères encodées en UTF8. Comme le moindre
 * caractère non Ascii tient sur plusieurs octets (ce qui inclue tous les
 * caractères accentués), elle n'est pas pratique
 * du tout à manipuler. C'est pourquoi on lui préfère la classe Core::String.
 * Cette classe ne doit servir que d'intermédiaire pour faire les conversions
 * UTF8 <-> UTF16 lorsqu'on dialogue avec l'extérieur, par exemple pour écrire dans
 * des fichiers on communiquer avec une API qui ne gère pas l'UTF16. A noter que
 * si cette API ne gère pas l'Unicode, les caractères accentués ne seront de
 * toute façon pas gérés correctement. Contrairement l'encodage UTF8 n'est pas
 *
 * Cette classe est implémentée avec une std::string à base de char. Elle offre
 * donc toutes les fonctionnalités des strings STL.
 * http://www.cplusplus.com/reference/string/string/
 *
 * \see String
 * \ingroup Standard
 */
class String8 : public String8Parent
{
public:
    /**
     * Initialise une String vide.
     */
    String8()
    {};

    /**
     * Initialise la String8 à partir d'une String en UTF16. Ce constructeur doit rester
     * explicit car cette initialisation entraîne une conversion, pas une simple recopie,
     * et s'il était implicite le compilateur risquerait de l'utiliser un peu partout
     * et on pourrait perdre beaucoup de temps dans des conversions sans s'en rendre compte.
     */
    LM_API_COR explicit String8(const String & string);

    /**
     * Initialise la String8 à partir d'une chaîne C terminée par un caractère NULL.
     */
    String8(const char * value) : String8Parent(value)
    {};

    /**
     * Ce constructeur permet de construire une String à partir d'une string
     * STL. Il est indispensable car certaines méthodes héritées retournent des strings
     * STL, il faut donc pouvoir facilement les convertir.
     */
    String8(const String8Parent & string) : String8Parent(string)
    {};

    String8(const char * value, int32 size) : String8Parent(value, size)
    {};

    /**
     * Modifie la String8 à partir d'une chaîne C terminée par un caractère NULL.
     */
    String8 & operator = (const char * value)
    {
        String8Parent::operator = (value);
        return *this;
    };

    LM_API_COR static const String8 EMPTY;
};

typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, STLAllocator<wchar_t> > StringParent;

/**
 * Cette classe permet de gérer des chaînes de caractères.
 *
 * Il s'agit de la classe "normale" à utiliser quand on manipule des chaînes.
 * Autant que possible, les String sont traitées comme s'il s'agissait de types
 * de base du langage. Pratiquement toutes les classes peuvent donc les utiliser mais
 * elles ne dépendent elles même d'aucune autre classe, si ce n'est
 * l'allocateur mémoire Core::Allocator par l'intermédiaire d'un Core::STLAllocator.
 *
 * \section Encodage
 *
 * La classe contient des chaînes encodées en UTF16 sous Windows
 * et en UTF32 sous Linux. Tous les caractères tiennent sur un seul élément
 * de la chaîne, ce qui rend sa manipulation très simple. Même les quelques
 * caractères asiatiques rarissimes qui tiennent sur plusieurs caractères ne
 * posent pas de problème car l'encodage UTF garantit qu'on ne peut pas
 * avoir de "false search" : un caractère encodé sur plusieurs éléments
 * n'est jamais composé d'autres éléments encodés sur un seul caractères.
 *
 * Pour en apprendre plus sur la problèmatique de l'encodage des strings :
 * http://www.joelonsoftware.com/articles/Unicode.html
 *
 * La FAQ des encodages UTF :
 * http://unicode.org/faq/utf_bom.html
 *
 * \section Manipulation Manipulation
 *
 * Cette classe est implémentée avec une std::string à base de wchar_t. Elle offre
 * donc toutes les fonctionnalités des strings STL.
 * http://www.cplusplus.com/reference/string/string/
 *
 * \section Literales Chaînes litérales
 *
 * Pour utiliser des chaînes litérales en UTF16/32
 * dans le code il faut utiliser le préfixe L.
 * Exemple :
 * \code
 * String maString = L"Chaîne en wide char, directement dans le code";
 * \endcode
 *
 * \section Conversion Conversion en UTF8
 * Pour convertir la chaîne en UTF8, il suffit de créer une instance de Core::String8 à partir
 * d'une String.
 *
 * \see String8
 * \ingroup Standard
 */
class String : public StringParent
{
public:
    /**
     * Initialise une String vide.
     */
    String()
    {};

    /**
     * Initialise la String à partir d'une String8 en UTF8. Ce constructeur doit rester
     * explicit car cette initialisation entraîne une conversion, pas une simple recopie,
     * et s'il était implicite le compilateur risquerait de l'utiliser un peu partout
     * et on pourrait perdre beaucoup de temps dans des conversions sans s'en rendre compte.
     */
    LM_API_COR explicit String(const String8 & string);

    /**
     * Recopie une chaîne longue dans la String. S'il s'agit d'un litéral il doit
     * être déclaré avec le préfixe L.
     */
    String(const wchar_t * value) : StringParent(value)
    {};

    String(const wchar_t * value, int32 size) : StringParent(value, size)
    {};

    /**
     * Ce constructeur permet de construire une String à partir d'une string
     * STL. Il est indispensable car certaines méthodes héritées retournent des strings
     * STL, il faut donc pouvoir facilement les convertir.
     */
    String(const StringParent & string) : StringParent(string)
    {};

    /**
     * Recopie une chaîne longue dans la String. S'il s'agit d'un litéral il doit
     * être déclaré avec le préfixe L.
     */
    String & operator = (const wchar_t * value)
    {
        StringParent::operator = (value);
        return *this;
    }

    inline int32 size() const { return (int32) StringParent::size(); }

    /**
     * Cet opérateur permet de concaténer des Strings. C'est la méthode à utiliser
     * lorsqu'on veut composer une String longue, plutôt que d'utiliser l'opérateur +
     * qui crée à chaque fois des objets temporaires. On peut concaténer n'importe
     * quel type dans une String du moment qu'une fonction toString() est définie
     * pour ce type.
     */
    template <class TElement>
    inline String & operator << (const TElement & element);

    LM_API_COR void toLower();
    LM_API_COR void toUpper();
    LM_API_COR bool startsWith(const String & prefix) const;
    LM_API_COR bool endsWith(const String & suffix) const;

    /**
     * Retourne une tranche de la chaîne, comme en Python.
     * Retourne l'intervale [iStart, iEnd] de la chaîne. Si npos
     * est égal à String::npos alors iEnd est considéré comme le dernier
     * élément de la chaîne.
     * \code
     * LM_ASSERT(String(L"0123456789").slice(0, 2) == L"012");
     * LM_ASSERT(String(L"0123456789").slice(2, 5) == L"2345");
     * LM_ASSERT(String(L"0123456789").slice(7) == L"789");
     * \endcode
     */
    LM_API_COR String slice(int32 iStart) const;

    LM_API_COR String slice(int32 iStart, int32 iEnd) const;

    /**
     * Cet opérateur permet de concaténer directement un litéral sans
     * avoir à créer une String temporaire.
     */
    inline String & operator << (const wchar_t * string)
    {
        *this += string;
        return *this;
    }

    LM_API_COR static const String EMPTY;
private:
    /**
     * Cet opérateur ne devrait jamais être utilisé. A la place, il faut utiliser
     * l'opérateur << sur des wide strings. (ie. faire string << L"toto"; plutôt que
     * string << "toto";)
     *
     * Il est déclaré en membre privé pour simplifier les messages d'erreur affichés
     * quand on oublie de passer une wide string.
     */
    String & operator << (const char * string)
    {
        return *this;
    }
};

/**
 * Cet opérateur permet d'envoyer le contenu d'une String dans une stream C++
 * comme n'importe quel type de base.
 */
inline std::ostream & operator << (std::ostream & os, const String & string)
{
    os << String8(string);
    return os;
}

//-----------------------------------------------------------------------------
/**
 * Convertit un entier en String.
 */
LM_API_COR String toString(int32 value);
/**
 * Convertit un entier en String. Si le résultat fair moins de minWidth caractères,
 * des zéros sont insérés au début pour obtenir la taille souhaitée.
 */
LM_API_COR String toString(int32 value, int32 minWidth);
LM_API_COR String toStringPadSpaces(int32 value, int32 minWidth);
/**
 * Convertit un entier en String. L'unité utilisée est le byte/bk/mb/gb selon
 * la taille utilisée.
 */
LM_API_COR String toStringByteSize(int32 value);
LM_API_COR String toStringByteSize(int64 value);
/**
 * Convertit un entier en String en base 16.
 */
LM_API_COR String toStringHex(int32 value);
/**
 * Convertit un entier en String en base 16. Si le résultat fair moins de minWidth caractères,
 * des zéros sont insérés au début pour obtenir la taille souhaitée.
 */
LM_API_COR String toStringHex(int32 value, int32 minWidth);
LM_API_COR String toString(const void * value);
/**
 * Convertit un flottant en String. Si le résultat fait plus de 11 chiffres
 * (avant la virgule), c'est la notation scientifique qui est utilisée.
 */
LM_API_COR String toString(double value);
/**
 * Convertit un flottant en String. Le résultat contient precision
 * chiffres après la virgule. Si le résultat fait plus de 11 chiffres
 * (avant la virgule), c'est la notation scientifique qui est utilisée.
 */
LM_API_COR String toString(double value, int32 precision);
LM_API_COR String toString(double value, int32 minWidth, int32 precision);
//! Convertit un entier en String.
inline String toString(int8 value) {return toString(int32(value));};
//! Convertit un entier en String.
inline String toString(uint8 value) {return toString(int32(value));};
//! Convertit un entier en String.
inline String toString(int16 value) {return toString(int32(value));};
//! Convertit un entier en String.
inline String toString(uint32 value) {return toString(int32(value));};
//! Convertit un entier en String.
LM_API_COR String toString(int64 value);
//! Convertit un entier en String.
LM_API_COR String toString(uint64 value);
LM_API_COR String toString(int64 value, int32 minWidth);
LM_API_COR String toString(uint64 value, int32 minWidth);
LM_API_COR String strYesNo(bool b);

/**
 * Convertit un flottant en String. Si le résultat fait plus de 11 chiffres
 * (avant la virgule), c'est la notation scientifique qui est utilisée.
 */
inline String toString(float value) {return toString(double(value));};
/**
 * Convertit un flottant en String. Le résultat contient precision
 * chiffres après la virgule. Si le résultat fait plus de 11 chiffres
 * (avant la virgule), c'est la notation scientifique qui est utilisée.
 */
inline String toString(float value, int32 precision)
{
    return toString(double(value), precision);
}
/**
 * Dans certains cas, des macros ou des templates peuvent appeler la
 * fonction toString sur une String, même si c'est effectivement inutile.
 */
inline String toString(const String & string) {return string;};
/**
 * Dans certains cas, des macros ou des templates peuvent appeler la
 * fonction toString sur une String8, même si c'est plutôt inutile.
 */
inline String toString(const String8 & string) {return String(string);};
/**
 * Convertit la chaîne passé en argument en entier.
 */
LM_API_COR int32 toInteger(const String & string);
LM_API_COR int64 toInteger64(const String & string);
LM_API_COR int32 toIntegerFromHex(const String & string);
LM_API_COR int64 toIntegerFromHex64(const String & string);
LM_API_COR double toFloat(const String & string);
//-----------------------------------------------------------------------------
template <class TElement>
inline String & String::operator << (const TElement & element)
{
    *this += Core::toString(element);
    return *this;
}
//-----------------------------------------------------------------------------
/**
 * Cette spécialisation du template simplifie la concaténation de String.
 * Elle évite une "conversion" inutile de String en String.
 */
template <>
inline String & String::operator << (const String & string)
{
    if(this != &string)
    {
        *this += string;
    }
    return *this;
}
//-----------------------------------------------------------------------------
template <class T>
inline String indirectToString(const T & t)
{
    // Cette fonction bidon sert uniquement à appeler les fonction toString qui ne sont pas
    // List::toString(), car à partir du moment où List définit une méthode toString, il n'y a
    // pas de moyen de lui dire qu'on veut appeler une fonction globale, sauf en précisant
    // son namespace, ce qui restreint un peu les choses.
    return toString(t);
}
//-----------------------------------------------------------------------------
}

//#pragma warning( pop )

#endif /*CORE_STRING_H_*/
