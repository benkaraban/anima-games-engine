#include <Core/Math/Operation.h>

namespace Core
{
//------------------------------------------------------------------------------
GEOINLINE Segmentf::Segmentf() : p1(ZERO), p2(ZERO)
{
}
//------------------------------------------------------------------------------
GEOINLINE Segmentf::Segmentf(const Vector3f & p1, const Vector3f & p2) : p1(p1), p2(p2)
{
}
//------------------------------------------------------------------------------
GEOINLINE Rayf::Rayf()
#ifdef _DEBUG
:origin(INIT_TO_NAN), direction(INIT_TO_NAN)
#endif
{
}
//------------------------------------------------------------------------------
GEOINLINE Rayf::Rayf(Ray_OriginDirection_SkipNormalize_t, const Vector3f& o, const Vector3f& d)
{
    origin=o;
    direction=d;
}
//------------------------------------------------------------------------------
GEOINLINE Rayf::Rayf(Ray_OriginDirection_t, const Vector3f& o, const Vector3f& d)
{
    origin=o;
    direction=d;
    direction.normalizeSafe();
}
//------------------------------------------------------------------------------
GEOINLINE Rayf::Rayf(Ray_OriginTarget_t, const Vector3f& o, const Vector3f& t)
{
    origin=o;
    direction=t-o;
    direction.normalizeSafe();
}
//------------------------------------------------------------------------------
GEOINLINE bool Rayf::operator== (const Rayf& other) const
{
    return (origin==other.origin && direction==other.direction);
    }
//------------------------------------------------------------------------------
GEOINLINE bool Rayf::operator!= (const Rayf& other) const
{
    return (origin!=other.origin || direction!=other.direction);
}
//------------------------------------------------------------------------------
GEOINLINE bool Rayf::isEquivalent(const Rayf& other, const float epsilon) const
{
    return (origin.isEquivalent(other.origin, epsilon) && direction.isEquivalent(other.direction, epsilon));
}
//------------------------------------------------------------------------------
GEOFUNCINLINE Rayf operator*(const Core::Matrix4f& mat, const Rayf ray)
{
    return Rayf(Ray_OriginDirection, mat.apply(ray.origin), mat.apply3x3(ray.direction));
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
GEOINLINE OBBoxf::OBBoxf()
#ifdef _DEBUG
:center(Core::INIT_TO_NAN), halfLength(Core::INIT_TO_NAN), orientation(Core::INIT_TO_NAN)
#endif
{

}
//------------------------------------------------------------------------------
GEOINLINE OBBoxf::OBBoxf(const Vector3f& c, const Vector3f& hl, const Matrix3f& o):
    center(c), halfLength(hl), orientation(o)
{

}
//------------------------------------------------------------------------------
GEOINLINE void OBBoxf::set(const Vector3f& c, const Vector3f& hl, const Matrix3f& o)
{
    center = c;
    halfLength = hl;
    orientation = o;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
GEOINLINE Planef::Planef()
#ifdef _DEBUG
:normal(INIT_TO_NAN), distance(f_INIT_TO_NAN)
#endif
{
}
//------------------------------------------------------------------------------
GEOINLINE Planef::Planef(Plane_NormalDistance_t, const Vector3f& n, const float& d)
{
    normal=n;
    distance=d;
    normalizeSafe();
}
//------------------------------------------------------------------------------
GEOINLINE Planef::Planef(Plane_NormalDistance_SkipNormalize_t, const Vector3f& n, const float& d)
{
    normal=n;
    distance=d;
}
//------------------------------------------------------------------------------
GEOINLINE Planef::Planef(Plane_NormalPoint_t, const Vector3f& n, const Vector3f& pt)
{
    normal=n;
    distance=dot(pt, normal);
    normalizeSafe();
}
//------------------------------------------------------------------------------
GEOINLINE Planef::Planef(const Vector3f& pt0, const Vector3f& pt1, const Vector3f& pt2)
{
    normal = cross(pt1-pt0, pt2-pt0).getNormalizedSafe();
    distance = dot(pt0, normal);
    normalizeSafe();
}
//------------------------------------------------------------------------------
GEOINLINE void Planef::set(const Vector3f& n, const float& d)
{
    normal=n;
    distance=d;
    normalizeSafe();
}
//------------------------------------------------------------------------------
GEOINLINE float Planef::distanceToPoint (const Vector3f& point) const
{
    return dot(normal, point) - distance;
}
//------------------------------------------------------------------------------
GEOINLINE bool Planef::operator==(const Planef& other)
{
    return (normal == other.normal && distance == other.distance);
}
//------------------------------------------------------------------------------
GEOINLINE bool Planef::operator!=(const Planef& other)
{
    return (normal != other.normal || distance != other.distance);
}
//------------------------------------------------------------------------------
GEOINLINE bool Planef::isEquivalent(const Planef& other, const float epsilonNormal, const float epsilonDistance) const
{
    return (normal.isEquivalent(other.normal, epsilonNormal) && (L_abs(distance-other.distance)<=epsilonDistance));
}
//------------------------------------------------------------------------------
GEOINLINE void     Planef::normalizeFast()
{
    float invLength = L_isqrt_fast(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
    normal.x*=invLength; normal.y*=invLength; normal.z*=invLength;
    distance*=invLength;
}
//------------------------------------------------------------------------------
GEOINLINE void     Planef::normalizeSafe()
{
    float invLength = L_isqrt_safe(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
    normal.x*=invLength; normal.y*=invLength; normal.z*=invLength;
    distance*=invLength;
}
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
GEOFUNCINLINE void matrixToFrustum(const Matrix4f& mat, Frustumf& f)
{
    f.planes[Core::FP_LEFT].normal.x = -mat._00 - mat._30;
    f.planes[Core::FP_LEFT].normal.y = -mat._01 - mat._31;
    f.planes[Core::FP_LEFT].normal.z = -mat._02 - mat._32;
    f.planes[Core::FP_LEFT].distance = 1*(mat._03 + mat._33);

    f.planes[Core::FP_RIGHT].normal.x = mat._00 - mat._30;
    f.planes[Core::FP_RIGHT].normal.y = mat._01 - mat._31;
    f.planes[Core::FP_RIGHT].normal.z = mat._02 - mat._32;
    f.planes[Core::FP_RIGHT].distance = -1*(mat._03 - mat._33);

    f.planes[Core::FP_BOTTOM].normal.x = -mat._10 - mat._30;
    f.planes[Core::FP_BOTTOM].normal.y = -mat._11 - mat._31;
    f.planes[Core::FP_BOTTOM].normal.z = -mat._12 - mat._32;
    f.planes[Core::FP_BOTTOM].distance = 1*(mat._13 + mat._33);
    
    f.planes[Core::FP_TOP].normal.x = mat._10 - mat._30;
    f.planes[Core::FP_TOP].normal.y = mat._11 - mat._31;
    f.planes[Core::FP_TOP].normal.z = mat._12 - mat._32;
    f.planes[Core::FP_TOP].distance = -1*(mat._13 - mat._33);

    f.planes[Core::FP_NEAR].normal.x = -mat._20;
    f.planes[Core::FP_NEAR].normal.y = -mat._21;
    f.planes[Core::FP_NEAR].normal.z = -mat._22;
    f.planes[Core::FP_NEAR].distance = 1*mat._23;
    
    f.planes[Core::FP_FAR].normal.x = mat._20 - mat._30;
    f.planes[Core::FP_FAR].normal.y = mat._21 - mat._31;
    f.planes[Core::FP_FAR].normal.z = mat._22 - mat._32;
    f.planes[Core::FP_FAR].distance = -1*(mat._23 - mat._33);
    
    
    f.planes[Core::FP_LEFT].normalizeSafe();
    f.planes[Core::FP_RIGHT].normalizeSafe();
    f.planes[Core::FP_BOTTOM].normalizeSafe();
    f.planes[Core::FP_TOP].normalizeSafe();
    f.planes[Core::FP_NEAR].normalizeSafe();
    f.planes[Core::FP_FAR].normalizeSafe();
}

}//namespace Core
