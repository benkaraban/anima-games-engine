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
#ifndef ASSETS_MODELNODE_H_
#define ASSETS_MODELNODE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Math/Matrix.h>
#include <Core/IO/Streams.h>
#include <Assets/ModelPose.h>
#include <Assets/ModelMesh.h>

namespace Assets
{
/**
 * Cet identifiant représente un identifiant invalide pour un Assets::ModelNode.
 */
const int32 UNDEFINED_NODE_ID = -1;
const int32 ROOT_NODE_ID      = 0;

/**
 * Cette classe représente un noeud d'un modèle. Conceptuellement,
 * un modèle peut être vu comme un graphe. Chaque noeud est soit
 * un noeud racine, soit un noeud rattaché à un autre. Un noeud
 * exprime sa position relativement à son père.
 */
class LM_API_ASS ModelNode : public Core::Object
{
public:
    ModelNode(int32 id, const String & name, const Core::Matrix4f & matrix, int32 meshId = UNDEFINED_MESH_ID);

    /**
     * Retourne l'identifiant du noeud.
     */
    int32 getId() const { return _id; };

    int32 getMeshId() const { return _meshId; };
    void setMeshId(int32 meshId) { _meshId = meshId; };

    void setMatrix(const Core::Matrix4f & matrix) { _matrix = matrix; };
    const Core::Matrix4f & getMatrix() const { return _matrix; };

    /**
     * Retourne le nom du noeud.
     */
    const String & getName() const { return _name; };

    /**
     * Retourne le nombre de noeuds fils.
     */
    int32 getSubNodeCount() const { return _children.size(); };

    /**
     * Retourne un pointeur vers le Ième fils.
     */
    const ModelNode & getSubNode(int32 subNodeNumber) const
    { return *_children[subNodeNumber]; };

    // EDITION

    void addChild(const Ptr<ModelNode> & pChild)
    { _children.push_back(pChild); };

    bool operator == (const ModelNode & node) const;
    bool operator != (const ModelNode & node) const;

protected:
    friend class Model;

    /**
     * Cette méthode permet d'obtenir la pose d'un modèle à partir d'une animation
     * et d'un instant T. Le ModelNode modifie qu'une seule valeur dans la Assets::ModelPose,
     * celle correspondant à sa propre matrice. Il faut donc appeler cette méthode
     * sur tous les ModelNode du Assets::Model pour pour obtenir une Assets::ModelPose
     * complète.
     *
     * En pratique, le ModelNode utilise la méthode Assets::Animation::getMatrix(), multiplie
     * le résultat par la matrice du noeud parent, passée en argument,
     * et met à jour la matrice correspondant à son identifiant dans la pose.
     */
    void getPose(ModelPose & pose, const Animation & anim, float key, const Core::Matrix4f & parentMatrix) const;
    void getDefaultPose(ModelPose & pose, const Core::Matrix4f & parentMatrix) const;

    int32          _id;
    String         _name;
    int32          _meshId;
    Core::Matrix4f _matrix;
    Core::List<Ptr<ModelNode> >  _children;
};
}

#endif /*ASSETS_MODELNODE_H_*/
