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
#ifndef UNIVERSE_EMITTERSHAPE_H_
#define UNIVERSE_EMITTERSHAPE_H_

#include <Core/Math/Vector.h>
#include <Core/XML/XMLNode.h>
#include <Universe/Particles/Particle.h>

namespace Universe
{

LM_ENUM_4(EShape, SHAPE_CONE, SHAPE_MODEL, SHAPE_SPLINE, SHAPE_BOX);

/**
 * Cette classe abstraite définit une interface pour gérer la forme de
 * l'émission des particules. De cette manière un émetteur de particules
 * peut générer ses particules dans des formes variées, allant du simple point
 * au mesh 3D.
 */
class IEmitterShape
{
public:
    virtual ~IEmitterShape() {};

    /**
     * Cette méthode est appelée par l'émetteur à chaque fois qu'une particule est
     * créée. Elle doit initialiser les deux vecteurs \a position et \a direction.
     * La direction doit être un vecteur normalisé. Il peut être utilisé par les
     * PartAffector pour déplacer la particule.
     */
    virtual void createParticle(Particle & part) = 0;
    
    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime) = 0;

    /**
     * Cette méthode permet de connaitre pour un IEmitterShape son type (Cone, ...)
     */
    virtual EShape getType() const = 0;
    /**
     * Cette méthode est appelé lors de l'export XML d'un Node pour récupérer la description
     * XML du Shape.
     */
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const = 0;
    /**
     * Cette méthode est appelé lors de l'import XML d'un Node pour définir la description
     * XML du Shape.
     */
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode) = 0;
};
}
#endif /*PARTICLE_EMITTERSHAPE_H_*/
