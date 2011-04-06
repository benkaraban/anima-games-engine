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
#ifndef DOCUMENTATION_H_
#define DOCUMENTATION_H_

/*
 * Ce fichier contient de la documentation doxygen. Il n'a pas vocation à être inclus.
 */

/**
 * \mainpage
 * 
 * Ceci est la documentation du code du projet LOADED.
 * 
 * \section FAQ FAQ
 * 
 * \subsection FAQ1 Par où commencer ?
 * 
 * Pour commencer, le mieux est d'étudier le \ref Core, en particulier les \ref Standard et les 
 * \ref Containers. Outre la documentation des classes elles même, la documentation des modules 
 * et des packages en question contient des informations sur la manière d'utiliser les classes
 * du Core.
 * 
 * Une fois familiarisé avec les classes de base, il est utile de s'interesser aux classe du 
 * \ref IO qui gère les entrées/sorties. Après celà le \ref RT peut être intéressant
 * mais il est moins indispensable dans le sens où il n'est pas très utile tant qu'on ne travaille
 * pas sur du code multi threadé. Il faut au moins apprendre à utiliser la classe Core::Task
 * puisqu'elle permet de tirer partie des processeurs multicoeurs sans trop avoir à mettre les
 * mains dans le combouis.
 * 
 * \subsection Arguments Comment passer des arguments à une fonction ?
 * 
 * Tout d'abord, tout dépend du type passé en argument. Si il s'agit d'un type scalaire
 * (entier, flottant), on doit le passer par recopie si il est passé en lecture seule
 * et en référence si il est modifié dans la fonction.
 * 
 * Pour un type composé (objet, structure), il faut passer par une référence. Cette
 * référence est constante si l'objet est passé en lecture seule.
 * 
 * Enfin, on doit passer par un pointeur intelligent (un Core::Ptr) si et seulement si
 * on veut conserver un pointeur vers l'objet après l'appel à la fonction. Le pointeur
 * étant lui même un type composé, il est passé en référence constante, que l'objet soit
 * modifié ou pas. En effet, c'est l'objet qu'on modifie, pas le pointeur. Le seul cas
 * où on peut avoir un argument de type pointeur passé en référence non constante est
 * celui où on veut retourner un pointeur, souvent alloué dans la fonction même.
 * 
 * \code
 * // Type scalaire, argument en lecture seule
 * float getInverse(float value)
 * {
 *     return 1.0f / value;
 * }
 * 
 * // Type scalaire, argument en lecture/écriture
 * void inverse(float & value)
 * {
 *     value /= 1.0f;
 * }
 * 
 * class TestClass1
 * {
 * public:
 *     void getStuff() {} const;
 *     void setValue(int32 value) {};
 * };
 * 
 * // Type composé, objet en lecture seule
 * void toString(const TestClass1 & myObject)
 * {
 *     return toString(myObject.getStuff());
 * }
 * 
 * // Type composé, objet en lecture/écriture
 * void update(TestClass1 & myObject)
 * {
 *     myObject.setValue(10);
 * }
 * 
 * class TestClass2
 * {
 * public:
 *     // Pointeur en référence constante (cas le plus courant)
 *     void setTarget(const Ptr<TestClass1> & pMyObject)
 *     {
 *         _pObject = pMyObject;
 *     };
 * private:
 *     Ptr<TestClass1> _pObject;
 * };
 * 
 * // Pointeur en référence non constante
 * void clone(const Ptr<TestClass1> & pSrc, Ptr<TestClass1> & pDst)
 * {
 *     pDst = new TestClass1(*pSrc);
 * }
 * 
 * // En pratique on préférerait plutôt la version suivante :
 * Ptr<TestClass1> clone(const Ptr<TestClass1> & pSrc)
 * {
 *     return new TestClass1(*pSrc);
 * }
 * 
 * \endcode
 * 
 * 
 */
#endif /*DOCUMENTATION_H_*/
