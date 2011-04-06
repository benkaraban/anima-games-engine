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
#ifndef ASSETS_SKELETONPOSE_H_
#define ASSETS_SKELETONPOSE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Math/Matrix.h>
#include <Assets/VerticeSet.h>

namespace Assets
{
class Skeleton;

/**
 * Cette classe représente un squelette dans un position particulière.
 */
class LM_API_ASS SkeletonPose : public Core::Object
{
public:
    SkeletonPose();
    SkeletonPose(const SkeletonPose & pose, const Core::Matrix4f & mat);

    void setTransform(const SkeletonPose & pose, const Core::Matrix4f & mat);
    void setBindShapeMatrix(const SkeletonPose & pose, const Core::Matrix4f & mat);

    /**
     * Transforme *uniquement la position* des vertices en tenant compte uniquement du bone
     * principal. Principalement dédié au picking.
     */
    void transformPosition(const VerticeSet & src, VerticeSet & dst) const;

    /**
     * Retourne la matrice de transformation du bone d'identifiant \a boneId.
     */
    const Core::Matrix4f & getBoneMatrix(int32 boneId) const { return _bonesMat[boneId]; };
    const Core::Matrix4f * getBonesMatrix() const { return &(*_bonesMat.begin()); };
    int32 getBoneCount() const { return _bonesMat.size(); };


protected:
    friend class Skeleton;

    /**
     * Initialise la pose pour un nombre de bones donnés.
     * Cette méthode ne doit être utilisée que par la classe Assets::Skeleton,
     * lors de l'appel à getSkeletonPose().
     */
    void setBoneCount(int32 count)
    {
        _bonesMat.resize(count);
    }

    /**
     * Renseigne la matrice du bone d'identifiant id.
     * Cette méthode ne doit être utilisée que par la classe Assets::Bone,
     * lors de l'appel à getSkeletonPose().
     */
    void setBoneMatrix(int32 id, const Core::Matrix4f & matrix)
    {
        _bonesMat[id] = matrix;
    }

    /**
     * Liste des matrices de transformation des bones. La Ième entrée
     * de la liste est la matrice du bone d'identifiant I.
     */
    Core::List<Core::Matrix4f>  _bonesMat;
};
}

#endif /*ASSETS_SKELETONPOSE_H_*/
