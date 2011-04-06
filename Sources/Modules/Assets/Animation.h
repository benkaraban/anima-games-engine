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
#ifndef ASSETS_ANIMATIONKEY_H_
#define ASSETS_ANIMATIONKEY_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/Math/Transform.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Matrix.h>

namespace Assets
{
/**
 * Cette classe représente une frame clef de l'animation.
 * Toutes les frames ne sont pas des frames clef et les frames clefs
 * ne sont pas nécessairement espacées par un nombre fixe de frames
 * non clef. Il faut donc explicitement conserver la séquence des frames
 * clefs.
 */
class LM_API_ASS KeyFrame
{
public:
    KeyFrame()
        : frameNumber(0),
          key(Core::IDENTITY)
    {};

    KeyFrame(int32 frameNumber, const Core::Transformf & key)
        : frameNumber(frameNumber),
          key(key)
    {};

    KeyFrame(int32 frameNumber, const Core::Matrix4f & key)
        : frameNumber(frameNumber),
          key(key)
    {};

    /**
     * Le numéro de frame de la frame clef.
     */
    int32        frameNumber;

    /**
     * Les informations d'animation de la frame clef.
     */
    Core::Transformf key;

    bool operator == (const KeyFrame & frame) const
    {
        return frameNumber == frame.frameNumber && key == frame.key;
    }
};

LM_POD_STREAM(KeyFrame);

inline String toString(const KeyFrame & key) { return String::EMPTY; };

const int32 UNDEFINED_ANIM_ID = -1;

class LM_API_ASS Animation : public Core::Object, public Core::Serializable
{
public:
    Animation();
    Animation(int32 nodeCount);
    Animation(const Animation & anim);
    Animation(const Animation & anim, int32 startKey, int32 endKey);
    
    // Pour recopier une animation d'un modèle à l'autre
    Animation(const Animation & anim, int32 nodeCount, const Core::List<int32> & cvtNodeId);

    int32 getFrameCount() const { return _frameCount; };
    int32 getNodeCount() const { return _keyFrames.size(); };

    float getDuration() const { return _duration; };
    void setDuration(float d) { _duration = d; };

    void getNodeMatrix(Core::Matrix4f & matrix, int32 nodeId, float alpha) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    Animation & operator = (const Animation & as);

    bool operator == (const Animation & anim) const;
    bool operator != (const Animation & anim) const;

    // EDITION

    bool hasKeyFrame(int32 nodeId) const;
    void addKeyFrame(int32 nodeId, const KeyFrame & frame);

protected:
    int32    _frameCount;
    int32    _flags;
    float    _duration;
    Core::List<Core::List<KeyFrame> > _keyFrames;
};
}

#endif /*ASSETS_ANIMATIONKEY_H_*/

