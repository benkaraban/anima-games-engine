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
#ifndef PARTICLE_EMITTERPERIOD_H_
#define PARTICLE_EMITTERPERIOD_H_

namespace Particle
{
/**
 * Cette classe abstraite contrôle la période de fréquence d'émission
 * des particules. Si la plupart des émetteurs de particules émettent des particules
 * à une fréquence fixe, certains peuvent émettre de manière plus complexe,
 * avec une fréquence partiellement aléatoire ou périodique.
 */
class PartEmitterPeriod
{
public:
    /**
     * Cette méthode est appelée à chaque mise à jour de l'émetteur de particules.
     * \a elapsedTime représente le nombre de secondes écoulées depuis
     * la dernière mise à jour.
     * 
     * En entrée, \a partTime est vide. En sortie, la taille de partTime correspond
     * au nombre de particules créées et le tableau contient, pour chacune de ces
     * particules, le nombre de secondes écoulées depuis sa création.
     * 
     * Cette durée est importante car si plusieurs particules sont créées pendant une même
     * mise à jour, toutes n'ont pas été créées exactement au même moment et il faut 
     * pouvoir ajuster cette différence. C'est d'autant plus vrai que le framerate est
     * bas. Si l'on ne tenait pas compte de cette différence dans l'ancienneté des particules,
     * l'émetteur se comporterait différemment selon le framerate.
     */
    virtual void createParticles(Core::List<double> & partTime,
                                 double elapsedTime) = 0;
};
}
#endif /*PARTICLE_EMITTERPERIOD_H_*/
