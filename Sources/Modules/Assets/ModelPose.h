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
#ifndef ASSETS_MODELPOSE_H_
#define ASSETS_MODELPOSE_H_

#include <Core/Math/Matrix.h>
#include <Core/Standard.h>
#include <Core/List.h>

namespace Assets
{
class ModelNode;
class Model;

/**
 * Cette classe représente une modèle dans un position particulière.
 * Une "pose", comme une pose de top modèle, représente l'état du modèle
 * à l'instant T d'une animation. En pratique, la pose contient la matrice
 * de transformation de chacun des Assets::ModelNode du modèle.
 *
 * De fait, une pose permet de situer un élément du modèle, à partir du moment
 * où on connait le Assets::ModelNode auquel il est rattaché.
 */
class LM_API_ASS ModelPose : public Core::Object
{
public:
    /**
     * Retourne la matrice de transformation du ModelNode d'identifiant \a nodeId.
     */
    const Core::Matrix4f & getNodeMatrix(int32 id) const { return _nodesMat[id]; };

protected:
    friend class Model;
    friend class ModelNode;

    /**
     * Initialise la pose pour un nombre de noeuds donnés.
     * Cette méthode ne doit être appelée que par la classe Assets::Model,
     * lorsqu'elle doit retourner une pose du model. C'est pour
     * cette raison que la classe Assets::Model est amie de la classe
     * ModelPose.
     */
    void setNodeCount(int32 count) { _nodesMat.resize(count); };

    /**
     * Renseigne la matrice du noeud d'identifiant id.
     * Cette méthode ne doit être appelée que par la classe Assets::ModelNode.
     * Lorsque la classe Model doit générer une pose, chaque noeud du graphe, en
     * partant de la racine, calcule sa matrice de transformation, met à jour
     * la pose en appelant cette méthode et demande à ses enfants de faire de
     * même. C'est pour cette raison que la classe Assets::ModelNode est
     * amie de la classe ModelPose.
     */
    void setNodeMatrix(int32 id, const Core::Matrix4f & matrix) { _nodesMat[id] = matrix; };

    /**
     * Liste des matrices de transformation des noeuds. La Ième entrée
     * de la liste est la matrice du noeud d'identifiant I.
     */
    Core::List<Core::Matrix4f>  _nodesMat;
};
}

#endif /*ASSETS_MODELPOSE_H_*/
