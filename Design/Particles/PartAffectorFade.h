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
#ifndef PARTICLE_PARTAFFECTORFADE_H_
#define PARTICLE_PARTAFFECTORFADE_H_

namespace Particle
{
/**
 * Modificateur de particule qui modifie la couleur des particules
 * d'une couleur de départ vers une couleur de destination en un
 * temps précisé. Une fois que les particules ont la couleur destination
 * elles sont détruites.
 * 
 * Cette classe est juste un exemple d'application de PartAffector.
 */
class LM_API PartAffectorFade : public PArtAffector
{
public:
    PartAffectorFade(const Core::Color4f & sourceColor,
                     const Core::Color4f & targetColor, 
                     float time);

    void setSourceColor(const Core::Color4f & sourceColor);
    void setTargetColor(const Core::Color4f & targetColor);
    void setTime(float time);
    
    virtual void initParticle(Abstract::Billboard & particle, PartInfos & infos, double elapsedTime);
    virtual void updateParticles(Core::List<Abstract::Billboard> & particles, 
                                 Core::List<PartInfos> & infos,
                                 double elapsedTime);
    
protected:
    Core::Color4f  _sourceColor;
    Core::Color4f  _targetColor;
    float          _time;
};
}

#endif /*PARTICLE_PARTAFFECTORFADE_H_*/