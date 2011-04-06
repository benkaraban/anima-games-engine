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
#ifndef CORE_GEOMETRY_COMPUTATIONS_H_
#define CORE_GEOMETRY_COMPUTATIONS_H_

#include <Core/Math/Vector.h>
#include <Core/Math/Matrix.h>

#ifdef _DEBUG
#define GEOINLINE
#define GEOFUNCINLINE LM_API_COR
#else
#define GEOINLINE inline
#define GEOFUNCINLINE inline
#endif

namespace Core
{

LM_ENUM_1(Union_t, UNION);
LM_ENUM_1(Inter_t, INTER);

struct LM_API_COR Segmentf
{
    GEOINLINE Segmentf();
    GEOINLINE Segmentf(const Vector3f & p1, const Vector3f & p2);

    Vector3f p1;
    Vector3f p2;
};

/**
 * Ray
 */
LM_ENUM_1(Ray_OriginDirection_t, Ray_OriginDirection);
LM_ENUM_1(Ray_OriginDirection_SkipNormalize_t, Ray_OriginDirection_SkipNormalize);
LM_ENUM_1(Ray_OriginTarget_t, Ray_OriginTarget);

struct LM_API_COR Rayf
{
public:
    GEOINLINE Rayf();
    GEOINLINE Rayf(Ray_OriginDirection_SkipNormalize_t, const Vector3f& o, const Vector3f& d);
    GEOINLINE Rayf(Ray_OriginDirection_t, const Vector3f& o, const Vector3f& d);
    GEOINLINE Rayf(Ray_OriginTarget_t, const Vector3f& o, const Vector3f& t);

    GEOINLINE bool operator == (const Rayf& other) const;
    GEOINLINE bool operator != (const Rayf& other) const;
    GEOINLINE bool isEquivalent(const Rayf& other, const float epsilon=f_COMMON_EPSILON) const;

public:
    Vector3f origin;
    Vector3f direction;     //doit reste normalise
};
LM_API_COR String toString(const Rayf & ray);
GEOFUNCINLINE Rayf operator*(const Core::Matrix4f& mat, const Rayf ray);
/**
 * Axis Aligned box
 */
struct LM_API_COR AABoxf
{
    AABoxf();
    AABoxf(const Vector3f & pt);
    AABoxf(const Vector3f & min, const Vector3f & max);
    AABoxf(const AABoxf & box);
    AABoxf(Union_t u, const AABoxf & box1, const AABoxf & box2);
    AABoxf(Inter_t i, const AABoxf & box1, const AABoxf & box2);
    AABoxf(const AABoxf & box, const Matrix4f & transform);

    void clear();
    void set(const AABoxf & box, const Matrix4f & transform);
    void addPoint(const Vector3f & p);
    void setUnion(const AABoxf & box);
    void setInter(const AABoxf & box);

    float getWidth()  const { return max._x - min._x; };
    float getHeight() const { return max._y - min._z; };
    float getDepth()  const { return max._z - min._z; };

    Vector3f getSize() const { return max - min; };
    Vector3f getCenter() const { return 0.5 * (min + max); };

    Vector3f clamp(const Vector3f & v) const;
    Vector3f wrap(const Vector3f & v) const;

    /**
     * segments doit avoir une taille de 12
     */
    void getSegments(Segmentf segments []) const;

    bool isEmpty() const;
    bool contains(const Vector3f & p) const;
    bool includes(const AABoxf & box) const;
    bool isInside(const AABoxf & box) const;
    bool inter(const AABoxf & box) const;

    bool operator == (const AABoxf & box) const;

    bool isValid() const;
    Vector3f  min;
    Vector3f  max;
};
LM_API_COR String toString(const AABoxf & box);


struct LM_API_COR Sphere
{
public:
    Sphere(float radius);
    Sphere(const Core::Vector3f & center, float radius);

    Core::Vector3f  center;
    float           radius;
};

/**
 * Oriented Bounding Boxes
 */
struct LM_API_COR OBBoxf
{
    GEOINLINE OBBoxf();
    GEOINLINE OBBoxf(const Vector3f& c, const Vector3f& hl, const Matrix3f& o);
    //GEOINLINE ~GEOINLINE OBBoxf(const OBBoxf& other);
    //GEOINLINE ~GEOINLINE OBBoxf();

    GEOINLINE void set(const Vector3f& c, const Vector3f& hl, const Matrix3f& o);

    Vector3f center;
    Vector3f halfLength;    //le vecteur stocke la valeur de la moitie de la longeur de chaque coté de la boite
    Matrix3f orientation;   //les axes x, y, z locaux
};

/**
 * Plane
 * ax + by + cz = d
 */
LM_ENUM_1(Plane_NormalDistance_t, Plane_NormalDistance);
LM_ENUM_1(Plane_NormalDistance_SkipNormalize_t, Plane_NormalDistance_SkipNormalize);
LM_ENUM_1(Plane_NormalPoint_t, Plane_NormalPoint);

struct LM_API_COR Planef
{
public:
    GEOINLINE Planef();
    GEOINLINE Planef(Plane_NormalDistance_t, const Vector3f& n, const float& d);
    GEOINLINE Planef(Plane_NormalDistance_SkipNormalize_t, const Vector3f& n, const float& d);
    GEOINLINE Planef(Plane_NormalPoint_t, const Vector3f& n, const Vector3f& pt);
    //construit le plan defini par trois points
    GEOINLINE Planef(const Vector3f& pt0, const Vector3f& pt1, const Vector3f& pt2);

    GEOINLINE void set(const Vector3f& n, const float& d);  //affecte les membre et effectue une normùalisation

    //calcul la distance signee d'un point a un plan
    GEOINLINE float distanceToPoint(const Vector3f& point) const;

    GEOINLINE bool operator==(const Planef& other);
    GEOINLINE bool operator!=(const Planef& other);

    GEOINLINE bool isEquivalent(const Planef& other, const float epsilonNormal=f_COMMON_EPSILON, const float epsilonDistance=0.01f) const;

    GEOINLINE void     normalizeFast();
    GEOINLINE void     normalizeSafe();

    Vector3f project(const Vector3f & point) const;
    Vector3f project(const Vector3f & point, const Vector3f & dir) const;

    void clip(Segmentf & segment) const;

public:
    Vector3f normal;    //doit reste normalise
    float distance;
};
LM_API_COR String toString(const Planef& box);

/**
 * Frustum
 */
LM_ENUM_6 (EFrustumPlan,
    FP_NEAR,
    FP_FAR,
    FP_RIGHT,
    FP_LEFT,
    FP_TOP,
    FP_BOTTOM)

struct LM_API_COR Frustumf
{
    Planef planes[EFrustumPlan_COUNT];

    void clip(Segmentf & segment) const;
};

/**
 *
 * Si mat est la matrice de projection : f est exprimer dans le repère de la caméra (view space)
 * Si mat est la matrice Projection * View : f est exprimer dans le repere monde
 * Si mat est la matrice Projection * View * World : f est exprime dans l'espace objet
 */

GEOINLINE LM_API_COR void matrixToFrustum(const Matrix4f& mat, Frustumf& f);
} //namespace Core


#ifndef _DEBUG
#include <Core/Math/GeometryComputations.inl>
#endif

#endif /*CORE_GEOMETRY_COMPUTATIONS_H_*/
