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
#include <Core/Math/Collisions.h>

namespace Core
{
//-----------------------------------------------------------------------------
static inline float sqr(float x)
{
    return x * x;
}
//-----------------------------------------------------------------------------
bool collides(const Sphere & s1, const Sphere & s2)
{
    float minDist   = s1.radius + s2.radius;
    float sqMinDist = minDist * minDist;
    return s1.center.getDistanceSquared(s2.center) < sqMinDist;
}
//-----------------------------------------------------------------------------
bool collides(const Sphere & s,  const AABoxf & b)
{
    float d = 0.0f;

    if     (s.center.x < b.min.x) d += sqr(b.min.x - s.center.x);
    else if(s.center.x > b.max.x) d += sqr(s.center.x - b.max.x);

    if     (s.center.y < b.min.y) d += sqr(b.min.y - s.center.y);
    else if(s.center.y > b.max.y) d += sqr(s.center.y - b.max.y);

    if     (s.center.z < b.min.z) d += sqr(b.min.z - s.center.z);
    else if(s.center.z > b.max.z) d += sqr(s.center.z - b.max.z);

    return d < sqr(s.radius);
}
//-----------------------------------------------------------------------------
LM_API_COR bool collidesOverTime(const Core::Vector3f & centerA0,
                                 const Core::Vector3f & centerA1,
                                 float radiusA,
                                 const Core::Vector3f & centerB0,
                                 const Core::Vector3f & centerB1,
                                 float radiusB)
{
    // http://www.gamasutra.com/view/feature/3383/simple_intersection_tests_for_games.php?page=2
    Core::Vector3f va(centerA1 - centerA0);
    Core::Vector3f vb(centerB1 - centerB0);
    Core::Vector3f ab   (centerB0 - centerA0);
    Core::Vector3f vab  (vb - va); // Vitesse relative
    float radius = radiusA + radiusB;
    float a = Core::dot(vab, vab);
    float b = 2.0f * Core::dot(vab, ab);
    float c = Core::dot(ab, ab) - radius*radius;
    float u0 = 0.0f;
    float u1 = 0.0f;

    if(Core::dot(ab, ab) <= radius*radius)
    {
        return true;
    }
    else if(a > 0.0f && Core::quadraticFormula(a, b, c, u0, u1))
    {
        return (u0 >= 0.0f && u0 <= 1.0f) || (u1 >= 0.0f && u1 <= 1.0f);
    }
    else
    {
        return false;
    }
}
//-----------------------------------------------------------------------------
CollisionShape::CollisionShape() :
    _localToWorld0(IDENTITY),
    _localToWorld1(IDENTITY),
    _worldToLocal0(IDENTITY),
    _worldToLocal1(IDENTITY),
    _firstUpdate(true)
{
}
//-----------------------------------------------------------------------------
CollisionShape::CollisionShape(const Core::Sphere & sphere) :
    _localToWorld0(IDENTITY),
    _localToWorld1(IDENTITY),
    _worldToLocal0(IDENTITY),
    _worldToLocal1(IDENTITY),
    _firstUpdate(true)
{
    addSphere(sphere);
}
//-----------------------------------------------------------------------------
CollisionShape::CollisionShape(const Core::AABoxf & box) :
    _localToWorld0(IDENTITY),
    _localToWorld1(IDENTITY),
    _worldToLocal0(IDENTITY),
    _worldToLocal1(IDENTITY),
    _firstUpdate(true)
{
    addAABox(box);
}
//-----------------------------------------------------------------------------
void CollisionShape::addSphere(const Core::Sphere & sphere)
{
    _localSpheres.push_back(sphere);

    _worldSpheres0.push_back(sphere);
    _worldSpheres0.back().center = _localToWorld0.apply(sphere.center);

    _worldSpheres1.push_back(sphere);
    _worldSpheres1.back().center = _localToWorld1.apply(sphere.center);
}
//-----------------------------------------------------------------------------
void CollisionShape::addAABox (const Core::AABoxf & box)
{
    _localAABoxes.push_back(box);
    _worldAABoxes0.push_back(AABoxf(box, _localToWorld0));
    _worldAABoxes1.push_back(AABoxf(box, _localToWorld1));
}
//-----------------------------------------------------------------------------
void CollisionShape::updateMatrices(const Core::Matrix4f & localToWorld, const Core::Matrix4f & worldToLocal)
{
    if(_firstUpdate)
    {
        setMatrices(localToWorld, worldToLocal);
        _firstUpdate = false;
    }
    else
    {
        _localToWorld0 = _localToWorld1;
        _worldToLocal0 = _worldToLocal1;

        _worldSpheres0 = _worldSpheres1;
        _worldAABoxes0 = _worldAABoxes1;

        _localToWorld1 = localToWorld;
        _worldToLocal1 = worldToLocal;

        for(int32 ii=0; ii < _worldSpheres1.size(); ii++)
            _worldSpheres1[ii].center = _localToWorld1.apply(_localSpheres[ii].center);

        for(int32 ii=0; ii < _worldAABoxes1.size(); ii++)
            _worldAABoxes1[ii].set(_localAABoxes[ii], _localToWorld1);
    }
}
//-----------------------------------------------------------------------------
void CollisionShape::setMatrices(const Core::Matrix4f & localToWorld, const Core::Matrix4f & worldToLocal)
{
    _localToWorld1 = _localToWorld0 = localToWorld;
    _worldToLocal1 = _worldToLocal0 = worldToLocal;

    for(int32 ii=0; ii < _worldSpheres1.size(); ii++)
        _worldSpheres1[ii].center = _localToWorld1.apply(_localSpheres[ii].center);

    for(int32 ii=0; ii < _worldAABoxes1.size(); ii++)
        _worldAABoxes1[ii].set(_localAABoxes[ii], _localToWorld1);

    _worldSpheres0 = _worldSpheres1;
    _worldAABoxes0 = _worldAABoxes1;
}
//-----------------------------------------------------------------------------
bool CollisionShape::collidesOverTime(const CollisionShape & shape) const
{
    for(int32 i1=0; i1 < _worldSpheres1.size(); i1++)
    {
        for(int32 i2=0; i2 < shape._worldSpheres1.size(); i2++)
        {
            const Core::Sphere & sA0 = _worldSpheres0[i1];
            const Core::Sphere & sA1 = _worldSpheres1[i1];
            const Core::Sphere & sB0 = shape._worldSpheres0[i2];
            const Core::Sphere & sB1 = shape._worldSpheres1[i2];

            if(Core::collidesOverTime(sA0.center, sA1.center, sA0.radius,
                                      sB0.center, sB1.center, sB1.radius))
            {
                return true;
            }
        }
    }

    for(int32 i1=0; i1 < _worldAABoxes1.size(); i1++)
        for(int32 i2=0; i2 < shape._worldAABoxes1.size(); i2++)
            if(Core::collides(_worldAABoxes1[i1], shape._worldAABoxes1[i2]))
                return true;

    for(int32 i1=0; i1 < _worldSpheres1.size(); i1++)
        for(int32 i2=0; i2 < shape._worldAABoxes1.size(); i2++)
            if(Core::collides(_worldSpheres1[i1], shape._worldAABoxes1[i2]))
                return true;

    for(int32 i1=0; i1 < _worldAABoxes1.size(); i1++)
        for(int32 i2=0; i2 < shape._worldSpheres1.size(); i2++)
            if(Core::collides(_worldAABoxes1[i1], shape._worldSpheres1[i2]))
                return true;

    return false;
}
//-----------------------------------------------------------------------------
bool CollisionShape::collides(const CollisionShape & shape) const
{
    for(int32 i1=0; i1 < _worldSpheres1.size(); i1++)
        for(int32 i2=0; i2 < shape._worldSpheres1.size(); i2++)
            if(Core::collides(_worldSpheres1[i1], shape._worldSpheres1[i2]))
                return true;

    for(int32 i1=0; i1 < _worldAABoxes1.size(); i1++)
        for(int32 i2=0; i2 < shape._worldAABoxes1.size(); i2++)
            if(Core::collides(_worldAABoxes1[i1], shape._worldAABoxes1[i2]))
                return true;

    for(int32 i1=0; i1 < _worldSpheres1.size(); i1++)
        for(int32 i2=0; i2 < shape._worldAABoxes1.size(); i2++)
            if(Core::collides(_worldSpheres1[i1], shape._worldAABoxes1[i2]))
                return true;

    for(int32 i1=0; i1 < _worldAABoxes1.size(); i1++)
        for(int32 i2=0; i2 < shape._worldSpheres1.size(); i2++)
            if(Core::collides(_worldAABoxes1[i1], shape._worldSpheres1[i2]))
                return true;

    return false;
}
//-----------------------------------------------------------------------------
}
