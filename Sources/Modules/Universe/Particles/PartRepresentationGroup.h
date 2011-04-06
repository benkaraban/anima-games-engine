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
#ifndef UNIVERSE_PARTREPRESENTATIONGROUP_H_
#define UNIVERSE_PARTREPRESENTATIONGROUP_H_

#include <Core/Map.h>
#include <Renderer/IBillboard.h>
#include <Universe/RessourcePool.h>
#include <Universe/Particles/IPartRepresentation.h>

namespace Universe
{
class Node;
class World;

class LM_API_UNI PartRepresentationGroup : public Core::Object, public IPartRepresentation
{
public:
    PartRepresentationGroup(World * pWorld, const String & group);
    ~PartRepresentationGroup();

    virtual const String & getGroupName() const { return _groupName; }
    virtual void setGroupName(const String & group) { _groupName = group; }

    virtual EPartRepresentation getType() const { return PR_GROUP; };

    virtual void setMaxSize(int32 maxSize) {};
    virtual int32 getMaxSize() const { return 1; };

    virtual void setPartScale(float s) {};
    virtual float getPartScale() const { return 1.0f; };

    virtual void setColorFactor(float f) { };
    virtual float getColorFactor() const { return 1.0f; };

    virtual void setGlowFactor(float f) { };
    virtual float getGlowFactor() const { return 0.0f; };

    virtual void setZBias(float z) {};
    virtual float getZBias() const { return 0.0f; };

    virtual void setFogFactor(float f) {};
    virtual float getFogFactor() const { return 1.0f; };

    virtual void setAlignOnDir(bool flag) { _alignOnDir = flag; }
    virtual bool getAlignOnDir() const { return _alignOnDir; }

    virtual void setRestartGroups(bool flag) { _restartGroups = flag; }
    virtual bool getRestartGroups() const { return _restartGroups; }

    virtual void setAffectSize(bool flag) { _affectSize = flag; }
    virtual bool getAffectSize() const { return _affectSize; }

    virtual void setAffectRoll(bool flag) { _affectRoll = flag; }
    virtual bool getAffectRoll() const { return _affectRoll; }

    virtual void setGlowAsColor(bool flag) { _glowAsColor = flag; }
    virtual bool getGlowAsColor() const { return _glowAsColor; }

    virtual void update(const Core::List<Particle> & particles);
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);
    virtual void setVisible(bool flag);
    virtual void kill();
    virtual Core::AABoxf getBoundingBox() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

protected:
    World *                         _pWorld;
    Ptr<Node>                       _pNode;
    Core::Map<int32, Ptr<Node> >    _groups;
    String                          _groupName;
    bool                            _alignOnDir;
    bool                            _restartGroups;
    bool                            _affectSize;
    bool                            _affectRoll;
    bool                            _glowAsColor;
};

}

#endif /* PARTREPRESENTATIONBILLBOARD_H_ */
