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
#ifndef PARTICLE_EMITTERSHAPESPHERE_H_
#define PARTICLE_EMITTERSHAPESPHERE_H_

namespace Particle
{
/**
 * Emetteur de particules dans une sphère.
 */
class LM_API EmitterShapeSphere : public EmitterShape
{
public:
    /**
     * Construit un émetteur sur une sphere de centre \a position et de rayon
     * \a radius. Si \a border est à true alors les particules sont émises sur le
     * bord de la sphère. Dans le cas contraire elles sont émises à l'intérieur
     * de la sphère.
     */
    EmitterShapeSphere(const Core::Vector3f & position, float radius, bool border);
    virtual void createParticle(Core::Vector3f & position, Core::Vector3f & direction);
    
    void setPosition(const Core::Vector3f & position);
    void setRadius(float radius);
    
protected:
    Core::Vector3f   _position;
    float            _radius;
};
}

#endif /*PARTICLE_EMITTERSHAPESPHERE_H_*/
