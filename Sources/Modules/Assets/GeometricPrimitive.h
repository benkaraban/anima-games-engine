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
#ifndef ASSETS_GEOMETRICPRIMITIVE_H_
#define ASSETS_GEOMETRICPRIMITIVE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/Math/Vector.h>
#include <Assets/VerticeSet.h>

namespace Assets
{
//-----------------------------------------------------------------------------
    LM_ENUM_2(EGeoSpherePolyhedron,
        GSP_OCTAHEDRON, // 8 faces
        GSP_ICOSAHEDRON) // 20 faces
//-----------------------------------------------------------------------------
/*
 *  Si l'état du rendu n'est pas configuré pour gérer de manière automatique le Texture Wrapping
 *  il est necessaire de la faire.
 *  Texture wrapping is performed before texture addressing. Be sure the texture wrapping data does not contain any texture coordinates outside of the range of [0.0, 1.0] because this will produce undefined results
 */
LM_API_ASS void textureWrapping(Core::List<Assets::Vertex>& vtx);
LM_API_ASS void textureCvtScaleV(Core::List<Assets::Vertex>& vtx, const float minV, const float maxV);
//-----------------------------------------------------------------------------
/*
* Génère l'approximation d'une sphère par subdivision recursive d'un polyèdre. Chaque augmentation du niveau
* de subdivision divise chaque face triangulaire en quatre triangles.
* Plus les divisions sont nombreuse et plus la sphere sera lisse. Le polyèdre initiale et présicé en paramètre. Si le
* nombre de division est à zéro la fonction retourne le polyèdre initiale.
* Les géosphères permettent de disposer d'une meilleur répartiton des sommets (disposition plus isotrope et plus
* économique en sommets) que les UVSphere
*
*/
LM_API_ASS void createGeoSphere(Assets::VerticeSet& vtx, const float radius, const int32 levelSubdivide, const bool faceIsCounterClock, const EGeoSpherePolyhedron polyhedron= GSP_ICOSAHEDRON);
LM_API_ASS void createUVSphere(Assets::VerticeSet& sphere, const float radius, const int32 nRings, const int32 nSegments, const bool faceIsCounterClock, float maxAngle = f_PI);
LM_API_ASS void createSkySphere(Assets::VerticeSet & vset, const int32 subdivisionLevel, const float radius, const float minV, const float maxV, float angle);
//-----------------------------------------------------------------------------
LM_API_ASS void createPlane(Assets::VerticeSet & vset, float width, float height, int32 widthSubdiv, int32 heightSubdiv, float texScale = 1.0, float randDepth = 0.0);
LM_API_ASS void createCube(Assets::VerticeSet& vtx, const Core::Vector3f& size);
LM_API_ASS void createCubeWithNormals(Assets::VerticeSet& cube, const Core::Vector3f& size);
LM_API_ASS void createCylinder(Assets::VerticeSet& vtx, const float radius,  const float height);
LM_API_ASS void createCone(Assets::VerticeSet& vtx, const float radius, const float height);
LM_API_ASS void createTorus(Assets::VerticeSet& vtx, const float innerRadius, const float outerRadius, const float factor = 1.f);
LM_API_ASS void createArrow(Assets::VerticeSet& arrow, const float arrowSize);
}  //namespace Assets


#endif /*ASSETS_GEOMETRICPRIMITIVE_H_*/
