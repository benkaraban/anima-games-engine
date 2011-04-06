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
#include "GeometryComputations.h"

#ifdef _DEBUG
#include <Core/Math/GeometryComputations.inl>
#endif

namespace Core
{
//------------------------------------------------------------------------------
LM_API_COR String toString(const Rayf & ray)
{
    String result;
    result << L"| origin : " << toString(ray.origin) << L" - " << L"direction : " << toString(ray.direction) << L"|";
    return result;
}
//------------------------------------------------------------------------------
LM_API_COR String toString(const Planef & plane)
{
    String result;
    result << L"| normal : " << toString(plane.normal) << L" - " << L"distance : " << toString(plane.distance) << L"|";
    return result;
}
//------------------------------------------------------------------------------
Vector3f Planef::project(const Vector3f & point, const Vector3f & dir) const
{
    LM_ASSERT(dir.isUnit());
    LM_ASSERT(normal.isUnit());

    float dist = distanceToPoint(point);
    float dotDN = Core::dot(dir, normal);

    if(Core::isEquivalent(dotDN, 0.0f))
    {
        return point;
    }
    else
    {
        return point + dir * (-dist / dotDN);
    }
}
//------------------------------------------------------------------------------
Vector3f Planef::project(const Vector3f & point) const
{
    LM_ASSERT(normal.isUnit());

    float dist = distanceToPoint(point);

    Vector3f test = point - normal * dist;
    return point - normal * dist;
}
//------------------------------------------------------------------------------
void Planef::clip(Segmentf & segment) const
{
    float d1 = distanceToPoint(segment.p1);
    float d2 = distanceToPoint(segment.p2);

    if(d1 > 0.0 && d2 > 0.0)
    {
        segment.p1 = project(segment.p1);
        segment.p2 = project(segment.p2);
    }
    else if(d1 > 0.0)
    {
        segment.p1 = project(segment.p1, (segment.p2 - segment.p1).getNormalizedSafe());
    }
    else if(d2 > 0.0)
    {
        segment.p2 = project(segment.p2, (segment.p1 - segment.p2).getNormalizedSafe());
    }
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
AABoxf::AABoxf()
    : min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{}
//------------------------------------------------------------------------------
AABoxf::AABoxf(const Vector3f & pt)
    : min(pt), max(pt)
{}
//------------------------------------------------------------------------------
AABoxf::AABoxf(const Vector3f & min, const Vector3f & max) : min(min), max(max)
{}
//------------------------------------------------------------------------------
AABoxf::AABoxf(const AABoxf & box) : min(box.min), max(box.max)
{}
//------------------------------------------------------------------------------
AABoxf::AABoxf(Union_t u, const AABoxf & box1, const AABoxf & box2)
    : min(box1.min), max(box1.max)
{
    setUnion(box2);
}
//------------------------------------------------------------------------------
AABoxf::AABoxf(Inter_t i, const AABoxf & box1, const AABoxf & box2)
    : min(box1.min), max(box1.max)
{
    setInter(box2);
}
//------------------------------------------------------------------------------
AABoxf::AABoxf(const AABoxf & box, const Matrix4f & transform)
{
    set(box, transform);
}
//------------------------------------------------------------------------------
void AABoxf::clear()
{
    min._x = min._y = min._z = FLT_MAX;
    max._x = max._y = max._z = -FLT_MAX;
}
//------------------------------------------------------------------------------
bool AABoxf::isEmpty() const
{
    return min._x > max._x || min._y > max._y || min._z > max._z;
}
//------------------------------------------------------------------------------
void AABoxf::getSegments(Segmentf segments []) const
{
    Vector3f iii(min.x, min.y, min.z);
    Vector3f iia(min.x, min.y, max.z);
    Vector3f iai(min.x, max.y, min.z);
    Vector3f iaa(min.x, max.y, max.z);
    Vector3f aii(max.x, min.y, min.z);
    Vector3f aia(max.x, min.y, max.z);
    Vector3f aai(max.x, max.y, min.z);
    Vector3f aaa(max.x, max.y, max.z);

    segments[ 0].p1 = iii; segments[ 0].p2 = aii;
    segments[ 1].p1 = iii; segments[ 1].p2 = iai;
    segments[ 2].p1 = iii; segments[ 2].p2 = iia;
    segments[ 3].p1 = iia; segments[ 3].p2 = aia;
    segments[ 4].p1 = iia; segments[ 4].p2 = iaa;
    segments[ 5].p1 = iai; segments[ 5].p2 = aai;
    segments[ 6].p1 = iai; segments[ 6].p2 = iaa;
    segments[ 7].p1 = iaa; segments[ 7].p2 = aaa;
    segments[ 8].p1 = aaa; segments[ 8].p2 = aai;
    segments[ 9].p1 = aaa; segments[ 9].p2 = aia;
    segments[10].p1 = aii; segments[10].p2 = aia;
    segments[11].p1 = aii; segments[11].p2 = aai;
}
//------------------------------------------------------------------------------
void AABoxf::set(const AABoxf & box, const Matrix4f & transform)
{
    if(box.isEmpty())
    {
        min = box.min;
        max = box.max;
    }
    else
    {
        Vector3f p1(transform.apply(Vector3f(box.min.x, box.min.y, box.min.z)));
        Vector3f p2(transform.apply(Vector3f(box.max.x, box.min.y, box.min.z)));
        Vector3f p3(transform.apply(Vector3f(box.min.x, box.max.y, box.min.z)));
        Vector3f p4(transform.apply(Vector3f(box.max.x, box.max.y, box.min.z)));
        Vector3f p5(transform.apply(Vector3f(box.min.x, box.min.y, box.max.z)));
        Vector3f p6(transform.apply(Vector3f(box.max.x, box.min.y, box.max.z)));
        Vector3f p7(transform.apply(Vector3f(box.min.x, box.max.y, box.max.z)));
        Vector3f p8(transform.apply(Vector3f(box.max.x, box.max.y, box.max.z)));

        min.x = std::min(p1.x, std::min(p2.x, std::min(p3.x, std::min(p4.x, std::min(p5.x, std::min(p6.x, std::min(p7.x, p8.x)))))));
        min.y = std::min(p1.y, std::min(p2.y, std::min(p3.y, std::min(p4.y, std::min(p5.y, std::min(p6.y, std::min(p7.y, p8.y)))))));
        min.z = std::min(p1.z, std::min(p2.z, std::min(p3.z, std::min(p4.z, std::min(p5.z, std::min(p6.z, std::min(p7.z, p8.z)))))));

        max.x = std::max(p1.x, std::max(p2.x, std::max(p3.x, std::max(p4.x, std::max(p5.x, std::max(p6.x, std::max(p7.x, p8.x)))))));
        max.y = std::max(p1.y, std::max(p2.y, std::max(p3.y, std::max(p4.y, std::max(p5.y, std::max(p6.y, std::max(p7.y, p8.y)))))));
        max.z = std::max(p1.z, std::max(p2.z, std::max(p3.z, std::max(p4.z, std::max(p5.z, std::max(p6.z, std::max(p7.z, p8.z)))))));
    }
}
//------------------------------------------------------------------------------
void AABoxf::addPoint(const Vector3f & p)
{
    min._x = std::min(p._x, min._x);
    min._y = std::min(p._y, min._y);
    min._z = std::min(p._z, min._z);
    max._x = std::max(p._x, max._x);
    max._y = std::max(p._y, max._y);
    max._z = std::max(p._z, max._z);
}
//------------------------------------------------------------------------------
void AABoxf::setUnion(const AABoxf & box)
{
    min._x = std::min(box.min._x, min._x);
    min._y = std::min(box.min._y, min._y);
    min._z = std::min(box.min._z, min._z);
    max._x = std::max(box.max._x, max._x);
    max._y = std::max(box.max._y, max._y);
    max._z = std::max(box.max._z, max._z);
}
//------------------------------------------------------------------------------
void AABoxf::setInter(const AABoxf & box)
{
    min._x = std::max(box.min._x, min._x);
    min._y = std::max(box.min._y, min._y);
    min._z = std::max(box.min._z, min._z);
    max._x = std::min(box.max._x, max._x);
    max._y = std::min(box.max._y, max._y);
    max._z = std::min(box.max._z, max._z);
}
//------------------------------------------------------------------------------
bool AABoxf::contains(const Vector3f & p) const
{
    return p._x >= min._x && p._x <= max._x &&
           p._y >= min._y && p._y <= max._y &&
           p._z >= min._z && p._z <= max._z;
}
//------------------------------------------------------------------------------
Vector3f AABoxf::clamp(const Vector3f & v) const
{
    return Vector3f(std::max(min.x, std::min(max.x, v.x)),
                    std::max(min.y, std::min(max.y, v.y)),
                    std::max(min.z, std::min(max.z, v.z)));
}
//------------------------------------------------------------------------------
Vector3f AABoxf::wrap(const Vector3f & v) const
{
    Vector3f size(getSize());

    return Vector3f(min.x + ((size.x == 0.0f) ? 0.0f : fmod((v.x - min.x), size.x)),
                    min.y + ((size.y == 0.0f) ? 0.0f : fmod((v.y - min.y), size.y)),
                    min.z + ((size.z == 0.0f) ? 0.0f : fmod((v.z - min.z), size.z)));
}
//------------------------------------------------------------------------------
bool AABoxf::includes(const AABoxf & box) const
{
    return AABoxf(INTER, *this, box) == box;
}
//------------------------------------------------------------------------------
bool AABoxf::isInside(const AABoxf & box) const
{
    return AABoxf(INTER, *this, box) == *this;
}
//------------------------------------------------------------------------------
bool AABoxf::inter(const AABoxf & box) const
{
    return !AABoxf(INTER, *this, box).isEmpty();
}
//------------------------------------------------------------------------------
bool AABoxf::operator == (const AABoxf & box) const
{
    return (this->min == box.min) && (this->max == box.max);
}
//------------------------------------------------------------------------------
bool AABoxf::isValid() const
{
    if (isEmpty() == true) return false;
    if (getSize() != Core::Vector3f(Core::ZERO))    return true;
    return false;
}
//------------------------------------------------------------------------------
String toString(const AABoxf & box)
{
    String result(L"|");
    result << toString(box.min) << L" -> " << toString(box.max) << L"|";
    return result;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
Sphere::Sphere(float radius) :
    center(Core::Vector3f::ZERO),
    radius(radius)
{
}
//-----------------------------------------------------------------------------
Sphere::Sphere(const Core::Vector3f & center, float radius) :
    center(center),
    radius(radius)
{
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
void Frustumf::clip(Segmentf & segment) const
{
    for(int32 iPlane=0; iPlane < EFrustumPlan_COUNT; iPlane++)
        planes[iPlane].clip(segment);
}
//------------------------------------------------------------------------------
} //namespace Core
