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
#ifndef CORE_EXCEPTION_H_
#define CORE_EXCEPTION_H_

#include <Core/Object.h>
#include <Core/NonCopyable.h>
namespace Core
{
/**
 * La classe mère de toutes les exceptions du projet.
 * 
 * Pour créer une exception, il faut créer une classe qui hérite de celle-ci
 * et au minimum, réimplémenter le constructeur qui prends un message d'erreur en
 * argument.
 * 
 * \ingroup Standard
 */
class LM_API_COR Exception : public Object, public NonCopyable
{
public:
    /**
     * Construit une exception avec le message d'erreur passé en argument.
     * Les classes filles doivent réimplémenter ce constructeur.
     */
    Exception(const String & message);
    
    /**
     * Construit une exception par recopie. Il semblerait que lorsqu'on
     * fait un throw sur un objet, il soit recopié via son constructeur
     * par recopie. Il faut donc définir un constructeur par recopie
     * valide pour pouvoir lever des exceptions.
     */
    Exception(const Exception & exception);
    
    /**
     * Retourne une chaîne de caractères décrivant l'exception.
     * La chaîne est de la forme TypeExactDeLException : message.
     */
    virtual String toString() const;
    
    /**
     * Retourne le message d'erreur de l'exception.
     */
    const String & getMessage() const;
    
    const String & getCallStack() const;

protected:
    /**
     * Le message d'erreur de l'exception.
     */
    String   _message;
    String   _callStack;
};

#define LM_VERIFY(x, msg) { if(!(x)) { throw Core::Exception(msg); } };

/**
 * Exception levèe lorsque l'argument passé à une fonction est incorrect.
 * Par exemple une fonction racine carré qui reçoit un nombre négatif
 * en argument pourrait lever une exception de ce type.
 * 
 * \ingroup Standard
 */
class LM_API_COR IllegalArgumentException : public Exception
{
public:
    /**
     * Construit une exception avec le message d'erreur passé en argument.
     */
    IllegalArgumentException(const String & message) : Exception(message)
    {};
};

}
#endif /*CORE_EXCEPTION_H_*/
