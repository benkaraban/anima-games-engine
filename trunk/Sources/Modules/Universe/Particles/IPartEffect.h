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
#ifndef UNIVERSE_PARTAFFECTOR_H_
#define UNIVERSE_PARTAFFECTOR_H_

#include <Core/List.h>
#include <Core/XML/XMLNode.h>
#include <Universe/Particles/Particle.h>

namespace Universe
{

LM_ENUM_14(EEffect, EFFECT_FADE,
                    EFFECT_FADE2,
                    EFFECT_FADE3,
                    EFFECT_FORCE,
                    EFFECT_ATTRACTOR,
                    EFFECT_SPEED_LIMIT,
                    EFFECT_RESISTANCE,
                    EFFECT_ANIMATE,
                    EFFECT_SIZE,
                    EFFECT_SIZE2,
                    EFFECT_ROLL,
                    EFFECT_TRANSLATE,
                    EFFECT_COLOR_BY_POS,
                    EFFECT_TURN);

/**
 * Cette classe abstraite représente une interface pour toutes les classes
 * qui peuvent affecter des particules. Ces modifications peuvent aussi bien
 * concerner la position des particules que leur couleur.
 */
class IPartEffect
{
public:
    virtual ~IPartEffect() {};

    /**
     * Cette méthode est appelée à chaque mise à jour de m'émetteur. \a elapsedTime
     * représente le temps écoulé, en secondes, depuis la dernière mise à jour.
     */
    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime) = 0;
    /**
     * Cette méthode permet de connaitre pour un IPartEffect son type (fade, force, ...)
     */
    virtual EEffect getType() const = 0;
    /**
     * Cette méthode est appelé lors de l'export XML d'un Node pour récupérer la description
     * XML de l'effet.
     */
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const = 0;
    /**
     * Cette méthode est appelé lors de l'import XML d'un Node pour définir la description
     * XML de l'effet.
     */
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode) = 0;
};
}

#endif /*PARTICLE_PARTAFFECTOR_H_*/
