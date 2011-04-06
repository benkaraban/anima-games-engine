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
#ifndef ASSETS_VERTICESET_H_
#define ASSETS_VERTICESET_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/Math/Vector.h>
#include <Core/Math/GeometryIntersect.h>
#include <Assets/Vertex.h>

namespace Assets
{
/**
 * Lorsque l'on désire animer un modèle avec animation par squelette
 * sur le GPU, il faut fournir au GPU les matrices de transformation
 * de tous les bones du modèle. Or, le GPU ne peut contenir, à un instant
 * T, qu'un nombre limité N de matrices de transformation. Si un modèle
 * contient M matrices supérieur à N, on ne peut donc à priori pas animer
 * le modèle en une fois sur le GPU.
 *
 * Pour contourner le problème, on divise les Assets::VerticesSet des
 * Assets::ModelMesh du Assets::Model en BlendingGroup. Chaque groupe
 * contient une palette de matrice. Cette palette contient un sous ensemble
 * des matrices de transformation du squelette. Les vertices contenues
 * dans un BlendingGroup n'utilisent que les matrices utilisées dans cette
 * palette.
 *
 * Les indices des vertices du même groupe sont regroupés
 * ensembles dans le Assets::VerticeSet. Un BlendingGroup est
 * simplement défini comme un intervale d'indices. Tous les indices
 * des vertices du Assets::VerticeSet compris dans cet intervale
 * appartiennent au BlendingGroup correspondant.
 */
class LM_API_ASS BlendingGroup : public Core::Object, public Core::Serializable
{
public:
    BlendingGroup();
    BlendingGroup(int32 startIndice, int32 indiceCount, Core::List<int32> & palette);
    BlendingGroup(const BlendingGroup & group);

    /**
     * Retourne l'indice de départ du groupe. Cet indice est l'indice du premier
     * élément du tableau d'indices retourné par Assets::VerticeSet::getIndices().
     */
    int32 getStartIndice() const { return _startIndice; };

    /**
     * Retourne le nombre d'indices inclus dans le groupe.
     */
    int32 getIndiceCount() const { return _indiceCount; };

    /**
     * Retourne la palette de matrices. Le Ième élément de cette
     * liste contient l'identifiant global du bone réellement référencé par les
     * vertex qui est influencé par la matrice d'identifiant I.
     */
    const Core::List<int32> & getMatrixPalette() const { return _palette; };

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    bool operator == (const BlendingGroup & set) const;
    bool operator != (const BlendingGroup & set) const;

protected:
    int32             _startIndice;
    int32             _indiceCount;
    Core::List<int32> _palette;
};

/**
 * Le VerticeSet est l'entité atomique d'un mesh.
 *
 * C'est ensemble de triangles, défini par une liste de vertices et une
 * liste d'indice. Les indices doivent être inteprétés par groupe de 3
 * pour former des triangles. Les indices I0, I1 et I2 forment le premier
 * triangle, les indices I3, I4 et I5 le deuxième, etc...
 */
class LM_API_ASS VerticeSet : public Core::Object, public Core::Serializable
{
public:
    VerticeSet();

    const Core::List<Vertex> & getVertices() const { return _vertices; };
    const Core::List<uint16> & getIndices() const { return _indices; };
    const Core::List<BlendingGroup> & getBlendingGroups() const { return _blendGroups; };

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    bool operator == (const VerticeSet & set) const;
    bool operator != (const VerticeSet & set) const;

    // EDITION

    Core::List<Vertex> & getVertices() { return _vertices; };
    Core::List<uint16> & getIndices() { return _indices; };
    Core::List<BlendingGroup> & getBlendingGroups() { return _blendGroups; };

    /**
     * Effectue les opérations de base pour avoir un modèle correct :
     * - Au moins 1 blending group
     */
    void cleanup();

protected:
    Core::List<Vertex>        _vertices;
    Core::List<uint16>        _indices;
    Core::List<BlendingGroup> _blendGroups;
};

LM_API_ASS bool intersect_RayVerticeSet(const Core::Rayf& ray, const VerticeSet& vset, Core::Vector3f&  posIntersec);      
}

#endif /*ASSETS_VERTICESET_H_*/
