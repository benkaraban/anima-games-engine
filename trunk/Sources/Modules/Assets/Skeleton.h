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
#ifndef ASSETS_SKELETON_H_
#define ASSETS_SKELETON_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Math/Matrix.h>
#include <Core/IO/Streams.h>

namespace Assets
{
class ModelPose;
class SkeletonPose;

struct LM_API_ASS Bone
{
    Bone() {};

    Bone(int32 nodeId, const Core::Matrix4f & offsetMatrix)
        : nodeId(nodeId), offsetMatrix(offsetMatrix)
    {};

    Bone(const Bone & bone) : nodeId(bone.nodeId), offsetMatrix(bone.offsetMatrix)
    {};

    /**
     * L'identifiant du Assets::ModelNode auquel le Bone est rattaché.
     */
    int32    nodeId;

    /**
     * La matrice d'application de la skin. La peau n'est pas appliquée
     * directement sur le bone, elle est décalée et/ou réorientée.
     * On lui applique donc cette offset matrix en plus de la matrice
     * du noeud auquel le Bone est rattaché.
     */
    Core::Matrix4f offsetMatrix;
};

LM_POD_STREAM(Bone);

inline String toString(const Bone & bone) { return String::EMPTY; };

/**
 * Cette classe représente le squelette d'un modèle.
 */
class LM_API_ASS Skeleton : public Core::Object, public Core::Serializable
{
public:
    /**
     * Retourne true si le squelette est vide. Un modèle qui ne contient pas
     * de squelette retournera un squelette vide.
     */
    bool isEmpty() const { return _bones.empty(); };

    /**
     * Cette méthode permet d'obtenir la pose d'un squelette à partir de la pose
     * d'un modèle.
     *
     * \warning La pose du modèle doit avoir été générée par le même modèle que le squelette.
     *
     * Pour ce faire, le Skeleton dimensionne le SkeletonPose en fonction du
     * nombre de Bone qu'il contient. Ensuite, il appelle la méthode du même nom
     * sur chacun de ses Bone pour remplir la pose.
     */
    void getSkeletonPose(SkeletonPose & skeletonPose, const ModelPose & modelPose) const;

    // EDITION

    int32 addBone(int32 nodeId, const Core::Matrix4f & offsetMatrix);

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

protected:
    /**
     * Liste des bones du squelette. Le Ième élément de la liste est le bone
     * d'identifiant I.
     */
    Core::List<Bone> _bones;
};
}
#endif /*ASSETS_SKELETON_H_*/
