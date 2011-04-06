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
#ifndef CORE_NONCOPYABLE_H_
#define CORE_NONCOPYABLE_H_
namespace Core
{
/**
 * Cette classe permet de créer très facilement des classes dont les instances
 * ne peuvent pas être recopiées. Cette faculté peut être intéressante pour
 * des classes qui sont liées à des ressources systèmes comme des mutex, des 
 * fichiers, des sockets, etc... Recopier des instances de ces classes n'a pas
 * forcément de sens ou alors cela peut être ambigü.
 * 
 * Il est donc assez fréquent de devoir développer des classes dont les instances
 * ne doivent pas pouvoir être recopiées. Pour ce faire, il suffit d'hériter de cette
 * classe, ce qui a de plus le mérite d'être explicite.
 * 
 * Pour éviter la recopie d'objet, cette classe se contente de déclarer le constructeur
 * par recopie et l'opérateur d'affectation comme des methodes privées, ce qui génère
 * des erreurs à la compilation si on essaye de recopier un objet héritant de cette
 * classe.
 */
class LM_API_COR NonCopyable
{
public:
    virtual ~NonCopyable() {};

protected:
    /**
     * Constructeur par défaut. Il est déclaré uniquement pour permettre aux classes
     * filles de l'appeler implicitement.
     */
    NonCopyable() {}
    
private:
    /**
     * Le constructeur par recopie est déclaré private pour éviter qu'il soit utilisé.
     * Si on n'en déclarait pas le compilateur en générerais un public par défaut.
     */
    NonCopyable(const NonCopyable & nonCopyable){}

    /**
     * L'opérateur d'affectation est déclaré private pour éviter qu'il soit utilisé.
     * Si on n'en déclarait pas le compilateur en générerais un public par défaut.
     */
    NonCopyable & operator = (const NonCopyable & nonCopyable){ return *this; }
};
}
#endif /*CORE_NONCOPYABLE_H_*/
