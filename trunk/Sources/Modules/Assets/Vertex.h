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
#ifndef ASSETS_VERTEX_H_
#define ASSETS_VERTEX_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>
#include <Core/Math/Vector.h>

namespace Assets
{
typedef uint16 float16;

/**
 * Nombre maximum de bones qui peuvent influencer un vertex.
 * 4 est une bonne valeur car elle correspond à une valeur pratique à implémenter
 * lorsqu'on réalise des animations par GPU (les registres du GPU sont des vecteurs
 * à 4 éléments). De plus, il est rare qu'un vertex soit influencé par plus
 * de 4 bones.
 */
const int32 MAX_BONES_PER_VERTEX = 4;
const int32 MAX_BONES_PER_MODEL  = 256;

/**
 * Format standard pour un vertex.
 * Ce format contient toutes les informations dont on pourrait avoir
 * besoin dans un vertex, quitte à ne pas les utiliser. Tout est stocké
 * dans des float pour avoir une bonne précision. C'est le format utilisé
 * lorsque l'on édite des modèles, pour faire tous les calculs intermédiaires,
 * avant qu'ils ne soient exportés dans un format "packed".
 *
 * Cette structure correspond à la valeur VERTEX_STANDARD de l'énuméré EVertexFormat.
 *
 * Comme toutes les structures de données de vertex, cette structure doit rester
 * une structure de données brutes et ne doit donc pas comporter de vtable. Pour
 * cette raison elle ne doit pas hériter de classes incluant des
 * méthodes virtuelles.
 *
 * De fait, la structure n'offre pas de méthode pour la sérialiser mais comme
 * il s'agit de données brutes, il suffit d'écrire ou de lire directement les données
 * de la structure.
 *
 * \code
 * BufferStream buffer;
 * VertexStandard vertex;
 *
 * buffer.write(&vertex, sizeof(vertex));
 *
 * List<VertexStandard>  vertices;
 * vertices.resize(100);
 * // ...
 *
 * buffer.write(&vertices[0], sizeof(VertexStandard) * vertices.size());
 * \endcode
 */
struct LM_API_ASS Vertex
{
    Vertex();

    Core::Vector3f    position;
    Core::Vector3f    normale;
    Core::Vector3f    tangent;
    Core::Vector3f    bitangent;
    Core::Vector2f    texCoords;

    int32             blendingGroup;

    float             dummy; // Pour rester compatible

    /**
     * Identifiants des bones qui influencent le vertex.
     * Ces identifiants correspondent aux identifiants des bones au sein
     * de la palette de matrices du Assets::BlendingGroup auquel appartient
     * le vertex. Il s'agit donc d'un identifiant local, pas de l'identifiant
     * du Assets::Bone au sein du Assets::Skeleton, même si on peut retrouver
     * le Assets::Bone correspondant à travers le Assets::BlendingGroup du
     * vertex.
     */
    int32             boneId[MAX_BONES_PER_VERTEX];

    /**
     * Poids des bones qui influencent le vertex.
     * Pour calculer la position d'un vertex lors d'une animation, on multiplie
     * le vertex par la matrice de transformation de chacun des bones qui l'influencent,
     * puis on multiplie chacun de ces résultats par leur poids respectif avant d'en faire
     * la somme. Les poids d'un bone est une valeur comprise entre 0 et 1 et la somme des
     * poids doit faire 1.
     */
    float             boneWeight[MAX_BONES_PER_VERTEX];

    bool operator == (const Vertex & vert) const;
    bool operator != (const Vertex & vert) const;
};

LM_POD_STREAM(Vertex);

LM_API_ASS String toString(const Vertex & v);

LM_API_ASS uint8 packUInt8N(float v);
LM_API_ASS uint8 packUInt8UN(float v);
LM_API_ASS int16 packInt16N(float v);

LM_API_ASS void packUInt8Vec3(const Core::Vector3f & vec, uint8 packed []);
LM_API_ASS void packUInt8Vec4(const Core::Vector4f & vec, uint8 packed []);

LM_API_ASS void packColor(const Core::Vector4f & color, uint8 packed []);

LM_API_ASS void packInt16Vec4(const Core::Vector3f & vec, int16 packed []);
LM_API_ASS void packInt16Vec3(const Core::Vector3f & vec, int16 packed []);
LM_API_ASS void packInt16Vec2(const Core::Vector3f & vec, int16 packed []);
LM_API_ASS void packInt16Vec2(const Core::Vector2f & vec, int16 packed []);

LM_API_ASS float unpackUInt8N(uint8 v);
LM_API_ASS float unpackUInt8UN(uint8 v);
LM_API_ASS float unpackInt16N(int16 v);

LM_API_ASS Core::Vector3f unpackUInt8Vec3(const uint8 v []);
LM_API_ASS Core::Vector3f unpackInt16Vec3(const int16 v []);
LM_API_ASS Core::Vector3f unpackInt16Vec2(const int16 v []);

LM_API_ASS Core::Vector4f unpackColor(const uint8 c []);

LM_API_ASS void packFloat16Vec2(const Core::Vector2f & vec, float16 packed []);
LM_API_ASS Core::Vector2f unpackFloat16Vec2(const float16 packed []);
}

#endif /*ASSETS_VERTEX_H_*/
