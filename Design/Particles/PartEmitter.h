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
#ifndef PARTICLE_PARTEMITTER_H_
#define PARTICLE_PARTEMITTER_H_

namespace Particle
{
/**
 * Cette classe représente un émetteur de particules.
 * La classe elle même est relativement simple. Pour un maximum de souplesse,
 * elle est composée de pointeurs vers des classes abstraites qui se chargent
 * du travail effectif. La génération même des particule est gérée par les
 * classe EmitterPeriod et EmitterShape.
 * 
 * Le comportement des particules, une fois émises, dépend d'un liste
 * de PartAffector. Chaque PartAffector modifie les particules d'une certaine
 * manière. Par exemple un émetteur pourrait avoir deux PartAffector : un
 * qui les rend de plus en plus transparentes (et les détruit une fois qu'elles
 * sont invisibles) et un autre qui les déplace linéairement dans leur direction
 * de départ.
 * 
 * L'émetteur de particules utilise un Abstract::BillboardGroup pour gérer
 * l'affichage des particules.
 */
class LM_API PartEmitter : public Core::Object, public Core::Serializable
{
public:
    /**
     * Construit un émetteur de particules dans la scène passée en argument.
     * Initialement, l'émetteur n'a aucun PartAffector dans sa liste. Les particules
     * restent donc au point de départ avec leur couleur initiale.
     */
    PartEmitter(const Ptr<Abstract::IScene> & pScene,
                const Ptr<PartEmitterPeriod> & pPeriod,
                const Ptr<PartEmitterShape> & pShape);
    
    virtual void setMaterial(const Ptr<Abstract::IMaterialInstance> pMaterial);
    virtual Ptr<Abstract::IMaterialInstance> getMaterial() const;
    
    virtual void addAffector(const Ptr<PartAffector> & pAffector);
    virtual void removeAffectors();
    
    /**
     * Met à jour l'émetteur de particules. Dans un premier temps, l'émetteur
     * appelle la méthode EmitterPeriod::createParticles(). L'émetteur initialise
     * alors chaque nouvelle particule avec une couleur de (1.0, 1.0, 1.0, 1.0) et
     * appelle EmitterShape::createParticle() pour obtenir la position et l'orientation
     * de départ de la particule. Pour chaque PartAffector, l'émetteur appelle la 
     * méthode PartAffector::initParticle() sur chacune des nouvelles particules.
     * 
     * Enfin l'émetteur appelle la méthode PartAffector::updateParticles() sur chacun 
     * des PartAffector de sa liste.
     */
    virtual void update(double elapsedTime);

    /**
     * Modifie le nombre maximum de particules que peut gérer l'émetteur à un
     * instant T. Normalement, si le générateur est correctement configuré,
     * le nombre de particules devrait rester à peu près stable mais si l'on oublie
     * malencontreusement de configurer un PartAffector qui élminie les particules trop
     * anciennes on risque de tout faire planter, ce qui peut arriver
     * pendant la mise au point d'émetteurs. Cette limite permet d'éviter ce genre de 
     * désagréments.
     * 
     * Par défaut la limite est à 5000.
     */
    virtual void setMaxParticleCount(int32 count);
    
    /**
     * Modifie la matrice de transformation de l'émetteur, ce qui a pour effet de
     * déplacer toutes les particules, y compris celles qui ont déjà été générées.
     */
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);
    
    /**
     * Active ou désactive l'émetteur. Un émetteur inactif continue de mettre àjour
     * les particules qu'il a émis mais n'en crée pas d'avantage.
     */
    virtual void setActive(bool active);
    
protected:
    Ptr<Abstract::BillboardGroup>  _pBillGroup;
    Ptr<PartEmitterPeriod>         _pPeriod;
    Ptr<PartEmitterShape>          _pShape;
    Core::List<Ptr<PartAffector> > _affectors;
};
}

#endif /*PARTICLEENGINE_H_*/
