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
#ifndef CORE_COLLISIONS_H_
#define CORE_COLLISIONS_H_

#include <Core/Math/GeometryComputations.h>
#include <Core/List.h>

namespace Core
{
LM_API_COR bool collides(const Sphere & s1, const Sphere & s2);
LM_API_COR bool collides(const Sphere & s,  const AABoxf & b);

LM_API_COR bool collidesOverTime(const Core::Vector3f & centerAt0,
                                 const Core::Vector3f & centerAt1,
                                 float radiusA,
                                 const Core::Vector3f & centerBt0,
                                 const Core::Vector3f & centerBt1,
                                 float radiusB);

inline bool collides(const AABoxf & b1, const AABoxf & b2) { return !AABoxf(INTER, b1, b2).isEmpty(); }
inline bool collides(const AABoxf & b,  const Sphere & s)  { return collides(s, b); }


class LM_API_COR CollisionShape
{
public:
    CollisionShape();
    CollisionShape(const Core::Sphere & sphere);
    CollisionShape(const Core::AABoxf & sphere);

    void addSphere(const Core::Sphere & sphere);
    void addAABox (const Core::AABoxf & box);

    void updateMatrices(const Core::Matrix4f & localToWorld, const Core::Matrix4f & worldToLocal);

    void setMatrices(const Core::Matrix4f & localToWorld, const Core::Matrix4f & worldToLocal);

    bool collides(const CollisionShape & shape) const;
    bool collidesOverTime(const CollisionShape & shape) const;

protected:
    Core::Matrix4f                  _localToWorld0;
    Core::Matrix4f                  _localToWorld1;
    Core::Matrix4f                  _worldToLocal0;
    Core::Matrix4f                  _worldToLocal1;

    Core::List<Core::Sphere>        _localSpheres;
    Core::List<Core::Sphere>        _worldSpheres0;
    Core::List<Core::Sphere>        _worldSpheres1;

    Core::List<Core::AABoxf>        _localAABoxes;
    Core::List<Core::AABoxf>        _worldAABoxes0;
    Core::List<Core::AABoxf>        _worldAABoxes1;

    bool                            _firstUpdate;
};

}

#endif
