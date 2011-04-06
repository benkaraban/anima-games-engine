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
#ifndef CORE_GEOMETRY_INTERSECT_H_
#define CORE_GEOMETRY_INTERSECT_H_


#include <Core/Math/GeometryComputations.h>

#ifdef _DEBUG
#define GEOINLINE
#define GEOFUNCINLINE LM_API_COR
#else
#define GEOINLINE inline
#define GEOFUNCINLINE inline
#endif

namespace Core
{


LM_ENUM_3 (EIntersecResultType,
    IR_OUTSIDE,    //l'objet est entierement a l'exterieur de la zone d'intersection
    IR_OVERLAP,    //l'objet chevauche la zone d'intersection
    IR_INSIDE)     //l'objet est entierement a l'interieur de la zone d'intersection
    

//---------------------------------------
const double d_INTERSECT_EPSILON = 0.005;
const float f_INTERSECT_EPSILON = 0.005f;
//---------------------------------------


//------------------------------------------------------------------------------
/**
 * return true si le rayon intersect l'AABox. 
 * La variable distanceMin contient la distance d'intersection entre l'orign du rayon 
 * et le point d'intersection
 * le point d'intersection est obtenue par : ray.origin + ray.direction * distanceMin;
**/
GEOFUNCINLINE bool intersect_RayAABox(const Rayf& ray, const AABoxf& box, float &distanceMin);      
//------------------------------------------------------------------------------
/**
 * return true si le rayon intersect le triangle. 
 * La variable position contient la position de l'intersection (si elle a eu lieu)
 * 
 * Ce test d'intersection prend en compte le sens du triangle, l'intersection n'a lieu que si le 
 * triangle est visible lorsque l'on regarde à l'origine du rayon dans sa direction.
 * pour un test dans les deux sens il faut appeler deux fois la fonction en inversant le sens du triangle
**/
GEOFUNCINLINE bool intersect_RayTriangle(const Rayf &ray, const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, Vector3f& position);
//------------------------------------------------------------------------------
GEOFUNCINLINE bool intersect_RayPlane(const Rayf &ray, const Planef& plane, Vector3f& position);
//------------------------------------------------------------------------------
/**
 * return true si l'AABox intersect le plan
**/
GEOFUNCINLINE bool intersect_AABoxPlane_Fast(const AABoxf& box, const Planef& plane);

/**
 * return la position du plan par rapport au plan (Outiside | Overlap | Inside) 
 *
 *         _________
 *        |         |                                
 *        | Outside |
 *        |_________|                                         /\ normal : Plan.normal
 *                                  _________                 ||
 *     ____________________________|         |________________||____ Plan
 *                                 | Overlap |
 *         _________               |_________|
 *        |         |
 *        | Inside  |
 *        |_________|
 *
**/  
GEOFUNCINLINE EIntersecResultType intersect_AABoxPlane_Accurate(const AABoxf& box, const Planef& plane);
//------------------------------------------------------------------------------
/**
 *  Actuellement les fonctions d'intersections ne gere pas les cas difficiles. Dans ces cas la boite à l'exterieur
 *  sera classe en intersection avec le frustum.
 *                  
 * exemple :             /
 *                    __/______
 *    \               |       |
 *   __\______________| AABox |
 *      \            /|_______|
 *       \ frustum  /
 *        \        /
 *         \      /
 *   _______\____/________
 *           \  /
 *
**/

/**
 *
 *
 */
GEOFUNCINLINE bool intersect_AABoxFrustum_Fast(const AABoxf& box, const Frustumf& frustum);
//TODO
//GEOFUNCINLINE EIntersecResultType intersect_AABoxFrustum_Accurate(const AABoxf& box, const Frustumf& frustum);
//------------------------------------------------------------------------------





} //namespace Core

#ifndef _DEBUG
#include <Core/Math/GeometryIntersect.inl>
#endif

#endif /*CORE_GEOMETRY_INTERSECT_H_*/
