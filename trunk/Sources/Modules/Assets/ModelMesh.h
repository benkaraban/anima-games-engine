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
#ifndef ASSETS_MODELMESH_H_
#define ASSETS_MODELMESH_H_

#include <Core/Standard.h>
#include <Core/Math/GeometryComputations.h>
#include <Core/IO/Streams.h>
#include <Assets/VerticeSet.h>
#include <Assets/Material.h>

namespace Assets
{
const int32 UNDEFINED_MESH_ID = -1;

LM_ENUM_3(EDetailLevel, LOD_HIGH, LOD_MEDIUM, LOD_LOW);

/**
 * Représente un mesh dans un modèle.
 *
 * Si un modèle représente une petite scène complète, le mesh représente
 * un unique élément géométrique défini comme un ensemble de triangles.
 *
 * Un mesh est rattaché à un Assets::ModelNode qui fourni sa position, à moins que le
 * mesh ne soit une "peau" à plaquer sur un squelette. Dans ce cas le mesh n'est
 * pas rattaché à un noeud particulier puisque chacun de ses vertex peut
 * être influencé par plusieurs bones.
 *
 * Un mesh peut être défini en plusieurs niveaux de détails (LOD, Level Of Detail).
 * Cette classe permet d'accéder aux différents LODs d'un mesh. C'est la classe
 * Assets::VerticeSet qui contient les informations brutes qui définissent
 * le mesh géométriquement.
 */
class LM_API_ASS ModelMesh : public Core::Object, public Core::Serializable
{
public:
    ModelMesh();
    ModelMesh(int32 id, const String & name);
    ModelMesh(int32 id, const String & name, const ModelMesh & mesh);
    ModelMesh(const ModelMesh & mesh);
    ModelMesh & operator = (const ModelMesh & mesh);

    /**
     * Retourne true si le mesh est une peau à plaquer sur un squelette.
     */
    bool isSkin() const { return _isSkin; };
    const Core::Matrix4f & getBindShapeMatrix() const { return _bindShapeMatrix; };

    /**
     * Retourne le nom du mesh au sein du modèle.
     */
    const String & getName() const { return _name; };

    const Material & getMaterial() const { return _material; };

    int32 getVertexCount() const { return _LODs[0]->getVertices().size(); };
    int32 getTriangleCount() const { return _LODs[0]->getIndices().size() / 3; };

    /**
     * Retourne le nombre de niveaux de détails du mesh.
     */
    int32 getLODCount() const { return _LODs.size(); };

    /**
     * Retourne une version du mesh pour le niveau de détail sélectionné.
     */
    Ptr<VerticeSet> getLOD(EDetailLevel LOD) const
    {
        switch(LOD)
        {
        case LOD_HIGH:   return _LODs[0];
        case LOD_MEDIUM: return _LODs[std::max(0, int32(_LODs.size()) - 2)];
        case LOD_LOW:    return _LODs[_LODs.size() - 1];
        }
        return _LODs[0];
    };

    const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    // EDITION

    void setName(const String & name) { _name = name; };
    void setSkin(bool isSkin) { _isSkin = isSkin; };

    void setLODCount(int32 lodCount);
    void setBindShapeMatrix(const Core::Matrix4f & bindShapeMatrix) { _bindShapeMatrix = bindShapeMatrix; };
    VerticeSet & getLODByID(int32 id) { return *_LODs[id]; };
    Material &  getMaterial() { return _material; };

    bool operator == (const ModelMesh & mesh) const;
    bool operator != (const ModelMesh & mesh) const;

    /**
     * Effectue les opérations de base pour avoir un modèle correct :
     * - Au moins 1 blending group
     * - Bounding boxes à jour
     */
    void cleanup();

protected:
    int32       _id;
    String      _name;
    bool        _isSkin;
    int32       _flags;
    Material    _material;
    Core::AABoxf _box;
    Core::Matrix4f _bindShapeMatrix;
    Core::List<Ptr<VerticeSet> > _LODs;

    void copyLODs(const ModelMesh & mesh);
    void updateBoundingBox();
};
}

#endif /*ASSETS_MODELMESH_H_*/
