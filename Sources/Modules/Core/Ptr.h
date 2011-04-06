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
#ifndef CORE_PTR_H_
#define CORE_PTR_H_

#include <Core/String.h>
#include <Core/Object.h>
#include <typeinfo>

namespace Core
{
struct RefCounter
{
    int32  count;
};

LM_ENUM_1(Null_t, null);

LM_ENUM_1 (EPTR_STATIC_CAST_TYPE, PTR_STATIC_CAST);
LM_ENUM_1 (EPTR_CONST_CAST_TYPE, PTR_CONST_CAST);
LM_ENUM_1 (EPTR_DYNAMIC_CAST_TYPE, PTR_DYNAMIC_CAST);


/**
 * La classe Ptr permet d'utiliser des pointeurs intelligents.
 *
 * \section Principe Principe
 *
 * Ces pointeurs désallouent automatiquement les objets pointés lorsqu'ils
 * ne sont plus référencés. Outre cette gestion automatique de la mémoire,
 * ces pointeurs suivent la même syntaxe que des pointeurs C++ standard. En
 * particulier ils supportent les opérateurs * et ->.
 *
 * Cette classe ne peut pas être utilisée pour manipuler des tableaux d'objets,
 * elle ne gère que des objets séparés.On peut donc être obligé d'utiliser des pointeurs standards C++. On
 * peut néanmoins l'éviter dans la plupart des cas. Par exemple si on doit allouer X
 * octets, on peut créer une Core::List de Core::byte de taille X et considérer
 * le contenu de la liste comme des données brutes en utilisant l'adresse
 * du premier élément comme adresse de base.
 *
 * \section Utilisation Utilisation
 *
 * En pratique chaque objet pointé dispose d'un compteur de références. Lorsque
 * ce compteur arrive à zéro, l'objet est libéré. Si ce système simplifie
 * sensiblement la gestion de la mémoire, ce n'est pas une solution complète
 * au problème des leaks. Par exemple, si deux objets se référencent mutuellement,
 * ils ne seront jamais libérés. Il faut donc veiller à briser ces cycles lorsque
 * les objets ne sont plus utiles (ou les éviter quand c'est possible).
 *
 * \warning <b>Il ne faut jamais référencer d'objet local avec un pointeur</b>.
 *
 * En effet, cela aurait pour effet de libérer une zone mémoire allouée sur la pile
 * comme s'il s'agissait d'une zone allouée sur le tas, ce qui entraîne des plantages.
 * Les Ptr ne doivent être utilisés qu'avec des objets créés avec l'opérateur new,
 * pas des objets créés sur la pile.
 *
 * Pour cette raison, <b>la règle est qu'on ne peut affecter à un Ptr que le
 * résultat d'un new ou un autre Ptr.</b> On ne doit jamais affecter à un pointeur
 * l'adresse d'une variable car on ne sait pas si elle a été allouée sur la pile
 * ou pas. De fait, dès qu'un objet a vocation à être partagé et référencé par
 * plusieurs objets, il faut le créer avec new.
 *
 * \warning C'est l'allocateur new qu'il faut utiliser et pas l'allocateur new []
 *
 * Les opérateurs sont différents et c'est avec l'opérateur delete que sera libéré
 * l'objet, c'est donc avec new (sans crochets) qu'il faut le créer.
 *
 * Exemples d'utilisation :
 *
 * \code
 * void func1(const MyClass & instance)
 * {
 *     instance.doStuff();
 * }
 *
 * void func2()
 * {
 *     Ptr<MyClass> pMyObject1 = new MyClass();
 *
 *     // Utilisation de -> pour accéder aux méthodes de l'objet pointé
 *     pMyObject1->doStuff();
 *
 *     Ptr<MyClass> pMyObject2 = new MyClass();
 *
 *     // Utilisation de * pour accéder à l'objet pointé
 *     func1(*pMyObject2);
 *
 *     // Le premier objet alloué va être désalloué car plus personne
 *     // ne pointe dessus
 *     pMyObject1 = pMyObject2;
 *
 *     // Cette affectation ne désalloue pas le deuxième objet,
 *     // toujours pointé par pMyObject1
 *     pMyObject2 = NULL;
 *
 *     if(!pMyObject2.isNull())
 *         std::cout << "ERREUR!" << std::endl;
 *
 *     // A la fin de la fonction, plus personne ne pointera
 *     // sur l'objet restant et il sera désalloué.
 * }
 * // A la fin de la fonction l'objet est désalloué.
 * \endcode
 *
 * \ingroup Standard
 */
template <class TObject>
class Ptr
{
public:
    Ptr();

    /**
     * Construit un Ptr à partir d'un pointeur brut. Ce pointeur doit
     * impérativement être une valeur retournée par l'opérateur new
     * (et pas l'opérateur new []). En aucun cas il ne faut lui fournir
     * l'adresse d'un objet alloué sur la pile.
     */
    explicit Ptr(TObject * pObject);

    Ptr(Null_t n);

    /**
     * Construit un Ptr à partir d'un autre. Cette initialisation
     * est toujours safe.
     */
    Ptr(const Ptr<TObject> & ptr);

    /**
     * Destruit le pointeur. L'objet pointé sera désalloué s'il s'agissait
     * du dernier pointeur qui le référençait.
     */
    ~Ptr();

    /**
     * Initialise le pointeur avec un Ptr vers un autre type
     * que celui du pointeur. Comme avec un pointeur normal, l'assignation est
     * acceptable (à la compilation) si la classe TOtherElement est une classe
     * fille de la classe TObject, mais pas l'inverse. Et on peut encore moins affecter
     * un pointeur de type arbitraire.
     */
    template <class TOtherElement>
    Ptr(Ptr<TOtherElement> ptr);

    /**
     * Effectue un cast sur un Ptr
     */
    template <class TOtherElement>
    inline Ptr(const Ptr<TOtherElement> & ptr, EPTR_STATIC_CAST_TYPE cast);
    template <class TOtherElement>
    inline Ptr(const Ptr<TOtherElement> & ptr, EPTR_CONST_CAST_TYPE cast);
    template <class TOtherElement>
    inline Ptr(const Ptr<TOtherElement> & ptr, EPTR_DYNAMIC_CAST_TYPE cast);

    /**
     * Modifie un Ptr à partir d'un autre. Cette opération
     * est toujours safe.
     *
     * Si le Ptr pointait sur un objet avant cette affectation,
     * son compteur de référence est mis à jour et il est désalloué
     * si nécessaire.
     */
    Ptr<TObject> & operator = (const Ptr<TObject> & ptr);

    /**
     * Accéde aux attributs et aux méthodes de l'objet pointé.
     */
    TObject * operator -> () const;

    /**
     * Retourne une référence constante vers l'objet pointé. Permet d'accéder
     * à l'instance réelle de l'objet.
     */
    TObject & operator * () const;

    /**
     * Retourne un pointeur brut constant vers l'objet pointé. En temps normal cette
     * méthode ne devrait pas être utilisée car elle permet de conserver une
     * instance de l'objet sans mettre à jour le compteur de référence. Cette
     * méthode ne doit être utilisée que pour dialoguer avec des libraries extérieures
     * qui ne connaissent que les pointeurs C++.
     *
     * \warning A utiliser le moins possible.
     */
    TObject * get() const;

    int32 getCount() const;

    /**
     * Retourne vrai si le Ptr est à NULL.
     */
    bool isNull() const;

    bool operator == (const Ptr<TObject> & other) const;
    bool operator != (const Ptr<TObject> & other) const;
    bool operator <  (const Ptr<TObject> & other) const;

    bool operator == (Null_t n) const;
    bool operator != (Null_t n) const;
    Ptr<TObject> & operator = (Null_t n);

    template <class TOtherElement> friend class Ptr;

    template <class TObject1, class TObject2>
    friend Ptr<TObject1> ptrDynamicCast(const Ptr<TObject2>& ptr);

protected:

    /**
     * Incrémente le compteur de références de l'objet pointé.
     */
    void incRef();

    /**
     * Décrémente le compteur de références de l'objet pointé.
     * Si le compteur arrive à 0 l'objet est désalloué avec l'opérateur delete.
     */
    void decRef();

    /**
     * Le pointeur brut vers l'objet pointé.
     */
    TObject *    _pObject;
    RefCounter * _pCounter;
};
//-----------------------------------------------------------------------------
template <class TObject>
Ptr<TObject>::Ptr() : _pObject(NULL), _pCounter(NULL)
{}
//-----------------------------------------------------------------------------
template <class TObject>
Ptr<TObject>::Ptr(Null_t n) : _pObject(NULL), _pCounter(NULL)
{}
//-----------------------------------------------------------------------------
template <class TObject>
Ptr<TObject>::Ptr(TObject * pObject) : _pObject(pObject), _pCounter(NULL)
{
    if(pObject != NULL)
    {
        _pCounter = (RefCounter*)Allocator::getInstance().allocate(sizeof(RefCounter));
        _pCounter->count = 0;
        incRef();
    }
}
//-----------------------------------------------------------------------------
template <class TObject>
Ptr<TObject>::Ptr(const Ptr<TObject> & ptr) : _pObject(ptr._pObject), _pCounter(ptr._pCounter)
{
    incRef();
}
//-----------------------------------------------------------------------------
template <class TObject>
Ptr<TObject>::~Ptr()
{
    decRef();
}
//-----------------------------------------------------------------------------
template <class TObject>
void Ptr<TObject>::incRef()
{
    if(_pCounter != NULL)
        _pCounter->count ++;
}
//-----------------------------------------------------------------------------
template <class TObject>
void Ptr<TObject>::decRef()
{
    if(_pCounter != NULL)
    {
        _pCounter->count --;
        if(_pCounter->count <= 0)
        {
            delete _pObject;
            Allocator::getInstance().free(_pCounter);
            _pCounter = NULL;
        }
    }
}
//-----------------------------------------------------------------------------
template <class TObject>
int32 Ptr<TObject>::getCount() const
{
    if(_pCounter == NULL)
        return 0;
    else
        return _pCounter->count;
}
//-----------------------------------------------------------------------------
template <class TObject>
template <class TOtherElement>
Ptr<TObject>::Ptr(Ptr<TOtherElement> ptr) : _pObject(ptr._pObject), _pCounter(ptr._pCounter)
{
    incRef();
}
//-----------------------------------------------------------------------------
template <class TObject>
template <class TOtherElement>
inline Ptr<TObject>::Ptr(const Ptr<TOtherElement> & ptr, EPTR_STATIC_CAST_TYPE cast) : _pObject(static_cast<TObject*>(ptr._pObject)), _pCounter(ptr._pCounter)
{
    incRef();
}
//-----------------------------------------------------------------------------
template <class TObject>
template <class TOtherElement>
inline Ptr<TObject>::Ptr(const Ptr<TOtherElement> & ptr, EPTR_CONST_CAST_TYPE cast) : _pObject(const_cast<TObject*>(ptr._pObject)), _pCounter(ptr._pCounter)
{
    incRef();
}
//-----------------------------------------------------------------------------
template <class TObject>
template <class TOtherElement>
inline Ptr<TObject>::Ptr(const Ptr<TOtherElement> & ptr, EPTR_DYNAMIC_CAST_TYPE cast) : _pObject(dynamic_cast<TObject*>(ptr._pObject)), _pCounter(ptr._pCounter)
{
    //TODO voir ce que l'on fait si le dynamic cast se passe mal
    incRef();
}
//-----------------------------------------------------------------------------
template <class TObject>
Ptr<TObject> & Ptr<TObject>::operator = (const Ptr<TObject> & ptr)
{
    if(&ptr != this)
    {
        decRef();
        _pObject = ptr._pObject;
        _pCounter = ptr._pCounter;
        incRef();
    }
    return *this;
}
//-----------------------------------------------------------------------------
template <class TObject>
TObject * Ptr<TObject>::operator -> () const
{
    return _pObject;
}
//-----------------------------------------------------------------------------
template <class TObject>
TObject & Ptr<TObject>::operator * () const
{
    return *_pObject;
}
//-----------------------------------------------------------------------------
template <class TObject>
TObject * Ptr<TObject>::get() const
{
    return _pObject;
}
//-----------------------------------------------------------------------------
template <class TObject>
bool Ptr<TObject>::isNull() const
{
    return _pObject == NULL;
}
//-----------------------------------------------------------------------------
template <class TObject>
bool Ptr<TObject>::operator == (const Ptr<TObject> & other) const
{
    return _pObject == other._pObject;
}
//-----------------------------------------------------------------------------
template <class TObject>
bool Ptr<TObject>::operator != (const Ptr<TObject> & other) const
{
    return _pObject != other._pObject;
}
//-----------------------------------------------------------------------------
template <class TObject>
bool Ptr<TObject>::operator < (const Ptr<TObject> & other) const
{
    return _pObject < other._pObject;
}
//-----------------------------------------------------------------------------
template <class TObject>
bool Ptr<TObject>::operator == (Null_t n) const
{
    return _pObject == NULL;
}
//-----------------------------------------------------------------------------
template <class TObject>
bool Ptr<TObject>::operator != (Null_t n) const
{
    return _pObject != NULL;
}
//-----------------------------------------------------------------------------
template <class TObject>
Ptr<TObject> & Ptr<TObject>::operator = (Null_t n)
{
    decRef();
    _pObject = NULL;
    _pCounter = NULL;
    return *this;
}
//-----------------------------------------------------------------------------
template <class TObject>
inline String toString(const Ptr<TObject> & ptr)
{
    String result(L"<");
    const Object * pObject = dynamic_cast<const Object*>(ptr.get());

    if(pObject != NULL)
    {
        result << pObject->getClassName();
    }
    else
    {
        result << String(typeid(TObject).name());
    }

    result << L" @ 0x" << Core::toString(ptr.get()) << L">";
    return result;
}
//-----------------------------------------------------------------------------
//static_cast sur un Ptr
template <class TObject1, class TObject2>
inline Ptr<TObject1> ptrStaticCast(const Ptr<TObject2>& ptr)
{
    return Ptr<TObject1> (ptr, PTR_STATIC_CAST);
}

//const_cast sur un Ptr
template <class TObject1, class TObject2>
inline Ptr<TObject1> ptrConstCast(const Ptr<TObject2>& ptr)
{
    return Ptr<TObject1> (ptr, PTR_CONST_CAST);
}

//dynamic_cast sur un Ptr
template <class TObject1, class TObject2>
inline Ptr<TObject1> ptrDynamicCast(const Ptr<TObject2>& ptr)
{
    return Ptr<TObject1> (ptr, PTR_DYNAMIC_CAST);
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_PTR_H_*/
