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
#ifndef UNIVERSE_PARTAFFECTORFADE_H_
#define UNIVERSE_PARTAFFECTORFADE_H_

#include <Universe/Particles/IPartEffect.h>

namespace Universe
{
/**
 * Modificateur de particule qui modifie la couleur des particules
 * d'une couleur de départ vers une couleur de destination en un
 * temps précisé. Une fois que les particules ont la couleur destination
 * elles sont détruites.
 */
class LM_API_UNI PartEffectFade : public Core::Object, public IPartEffect
{
public:
    PartEffectFade(const Core::Vector4f & sourceColor,
                   const Core::Vector4f & targetColor,
                   float duration,
                   bool killPartsAtEnd);

    void setSourceColor(const Core::Vector4f & sourceColor);
    void setTargetColor(const Core::Vector4f & targetColor);
    void setDuration(float duration);
    void killPartsAtEnd(bool flag);
    void onlyAlpha(bool flag);

    inline const Core::Vector4f & getSourceColor() const { return _sourceColor; }
    inline const Core::Vector4f & getTargetColor() const { return _targetColor; }
    inline float getDuration() const { return _duration; }
    inline bool killPartsAtEnd() const { return _killParts; }
    inline bool onlyAlpha() const { return _onlyAlpha; };

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_FADE; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    Core::Vector4f  _sourceColor;
    Core::Vector4f  _targetColor;
    float           _duration;
    bool            _killParts;
    bool            _onlyAlpha;
};

class LM_API_UNI PartEffectFade2 : public Core::Object, public IPartEffect
{
public:
    PartEffectFade2(
            const Core::Vector4f & color0,
            const Core::Vector4f & color1,
            const Core::Vector4f & color2,
            float time1,
            float time2,
            bool killPartsAtEnd,
            bool loop);

    void setColor0(const Core::Vector4f & color0);
    void setColor1(const Core::Vector4f & color1);
    void setColor2(const Core::Vector4f & color2);
    void setTime1(float time1);
    void setTime2(float time2);
    void killPartsAtEnd(bool flag);
    void loop(bool flag);
    void onlyAlpha(bool flag);

    inline const Core::Vector4f & getColor0() const { return _color0; }
    inline const Core::Vector4f & getColor1() const { return _color1; }
    inline const Core::Vector4f & getColor2() const { return _color2; }
    inline float getTime1() const { return _time1; }
    inline float getTime2() const { return _time2; }
    inline bool killPartsAtEnd() const { return _killParts; }
    inline bool loop() const { return _loop; }
    inline bool onlyAlpha() const { return _onlyAlpha; };

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_FADE2; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    Core::Vector4f  _color0;
    Core::Vector4f  _color1;
    Core::Vector4f  _color2;
    float           _time1;
    float           _time2;
    bool            _killParts;
    bool            _loop;
    bool            _onlyAlpha;
};

class LM_API_UNI PartEffectFade3 : public Core::Object, public IPartEffect
{
public:
    PartEffectFade3(
            const Core::Vector4f & color0,
            const Core::Vector4f & color1,
            const Core::Vector4f & color2,
            const Core::Vector4f & color3,
            float time1,
            float time2,
            float time3,
            bool killPartsAtEnd,
            bool loop);

    void setColor0(const Core::Vector4f & color0);
    void setColor1(const Core::Vector4f & color1);
    void setColor2(const Core::Vector4f & color2);
    void setColor3(const Core::Vector4f & color3);
    void setTime1(float time1);
    void setTime2(float time2);
    void setTime3(float time3);
    void killPartsAtEnd(bool flag);
    void loop(bool flag);
    void onlyAlpha(bool flag);

    inline const Core::Vector4f & getColor0() const { return _color0; }
    inline const Core::Vector4f & getColor1() const { return _color1; }
    inline const Core::Vector4f & getColor2() const { return _color2; }
    inline const Core::Vector4f & getColor3() const { return _color3; }
    inline float getTime1() const { return _time1; }
    inline float getTime2() const { return _time2; }
    inline float getTime3() const { return _time3; }
    inline bool killPartsAtEnd() const { return _killParts; }
    inline bool loop() const { return _loop; }
    inline bool onlyAlpha() const { return _onlyAlpha; };

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_FADE3; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    Core::Vector4f  _color0;
    Core::Vector4f  _color1;
    Core::Vector4f  _color2;
    Core::Vector4f  _color3;
    float           _time1;
    float           _time2;
    float           _time3;
    bool            _killParts;
    bool            _loop;
    bool            _onlyAlpha;
};

}

#endif /*PARTICLE_PARTAFFECTORFADE_H_*/
