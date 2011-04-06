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
#ifndef PARTICLE_PARTAFFECTOR_H_
#define PARTICLE_PARTAFFECTOR_H_

namespace Particle
{
/**
 * Cette classe abstraite représente une interface pour toutes les classes
 * qui peuvent affecter des particules. Ces modifications peuvent aussi bien
 * concerner la position des particules que leur couleur.
 */
class PartAffector
{
public:    
    /**
     * Cette méthode est appelée lorsqu'une nouvelle particule est créée. Si le PartAffector
     * a quelque chose de particulier à initialiser, il peut le faire ici. De plus, \a elapsedTime
     * représente le temps écoulé, en secondes, depuis la création de la particule. Elle doit
     * donc être également mise à jour en conséquence.
     */
    virtual void initParticle(Abstract::Billboard & particle, PartInfos & infos, double elapsedTime) = 0;
    
    /**
     * Cette méthode est appelée à chaque mise à jour de m'émetteur. \a elapsedTime
     * représente le temps écoulé, en secondes, depuis la dernière mise à jour.
     * \a particles et \a infos doivent avoir la même taille. Le PartInfos d'indice
     * I correspond au Billboard de même indice.
     * 
     * Si les deux listes sont séparées, c'est pour éviter des recopies et
     * des conversions inutiles. La liste \a particles contient les données
     * propre au rendu, dans le format requis par le Renderer, ce qui évite de les 
     * convertir d'un format à l'autre. En revanhce, \a infos contient toutes les 
     * informations propre à l'émetteur de particules, comme la vitesse ou la durée
     * de vie.
     * 
     * La mise à jour peut retirer des particules, il faut juste bien veiller
     * à conserver la cohérence des deux listes. L'ordre des particules n'a aucune importance
     * du moment qu'il est cohérent entre les deux listes. Pour écraser une particule I, il
     * vaut donc mieux faire un échange entre le dernier élément de la liste et l'élément I puis
     * retirer le dernier élément de la liste. De cette manière la suppression est une opération
     * de complexité constante au lieu d'être une opération en complexité linéaire.
     */
    virtual void updateParticles(Core::List<Abstract::Billboard> & particles, 
                                 Core::List<PartInfos> & infos,
                                 double elapsedTime) = 0;
};
}

#endif /*PARTICLE_PARTAFFECTOR_H_*/
