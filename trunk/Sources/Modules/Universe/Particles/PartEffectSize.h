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
#ifndef UNIVERSE_PARTAFFECTORSIZE_H_
#define UNIVERSE_PARTAFFECTORSIZE_H_

#include <Universe/Particles/IPartEffect.h>

namespace Universe
{
/**
 * Modificateur de particule qui modifie la couleur des particules
 * d'une couleur de départ vers une couleur de destination en un
 * temps précisé. Une fois que les particules ont la couleur destination
 * elles sont détruites.
 */
class LM_API_UNI PartEffectSize : public Core::Object, public IPartEffect
{
public:
    PartEffectSize(float width0,
                   float height0,
                   float width1,
                   float height1,
                   float duration,
                   bool killPartsAtEnd);

    void setSize0(float width, float height);
    void setSize1(float width, float height);
    void setDuration(float duration);
    void killPartsAtEnd(bool flag);
    void setSquareParticles(bool square);

    inline float getWidth0()    const { return _width0; }
    inline float getHeight0()   const { return _height0; }
    inline float getWidth1()    const { return _width1; }
    inline float getHeight1()   const { return _height1; }
    inline float getDuration()  const { return _duration; }

    inline bool getSquareParticles() const { return _squareParticles; }
    inline bool killPartsAtEnd() const { return _killParts; }

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_SIZE; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    float   _width0;
    float   _height0;
    float   _width1;
    float   _height1;
    float   _duration;
    bool    _squareParticles;
    bool    _killParts;
};

class LM_API_UNI PartEffectSize2 : public Core::Object, public IPartEffect
{
public:
    PartEffectSize2(float width0,
                    float height0,
                    float width1,
                    float height1,
                    float width2,
                    float height2,
                    float time1,
                    float time2,
                    bool killPartsAtEnd);

    void setSize0(float width, float height);
    void setSize1(float width, float height);
    void setSize2(float width, float height);
    void setTime1(float time1);
    void setTime2(float time2);
    void killPartsAtEnd(bool flag);
    void setSquareParticles(bool square);

    inline float getWidth0()    const { return _width0; }
    inline float getHeight0()   const { return _height0; }
    inline float getWidth1()    const { return _width1; }
    inline float getHeight1()   const { return _height1; }
    inline float getWidth2()    const { return _width2; }
    inline float getHeight2()   const { return _height2; }
    inline float getTime1()     const { return _time1; }
    inline float getTime2()     const { return _time2; }

    inline bool getSquareParticles() const { return _squareParticles; }
    inline bool killPartsAtEnd() const { return _killParts; }

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_SIZE2; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    float   _width0;
    float   _height0;
    float   _width1;
    float   _height1;
    float   _width2;
    float   _height2;
    float   _time1;
    float   _time2;
    bool    _squareParticles;
    bool    _killParts;
};

}

#endif /*PARTICLE_PARTAFFECTORFADE_H_*/
