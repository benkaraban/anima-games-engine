/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef WORKFLOW_MODELOPTIMIZE_H_
#define WORKFLOW_MODELOPTIMIZE_H_

#include <Assets/Vertex.h>
#include <Assets/Model.h>

#include <Core/IProgressObs.h>
#include <Core/VFS/VirtualFileSystem.h>

namespace Workflow
{
//-----------------------------------------------------------------------------
struct LM_API_WKF ModelPackingOptions
{
    ModelPackingOptions() :
        maxBonesPerVertex(Assets::MAX_BONES_PER_VERTEX),
        maxBonesPerBlendingGroup(70),
        generateNormals(false),
        generateTangentBasis(true),
        optimizeForCache(true)
    {};

    Ptr<Core::IProgressObs> pObs;

    /**
     * Nombre maximal de bones qui peuvent influencer un vertex.
     * Utilisé uniquement pour les modèles avec un squelette.
     * 1 <= maxBonesPerVertex <= Assets::MAX_BONES_PER_VERTEX (= 4)
     */
    int32   maxBonesPerVertex;

    /**
     * Le nombre maximal de bones qui peuvent influencer un même blending group.
     * Utilisé uniquement pour les modèles avec un squelette.
     * maxBonesPerVertex <= maxBonesPerPalette <= 70
     */
    int32   maxBonesPerBlendingGroup;

    /**
     * Génère les normales. Normalement, on devrait plutôt utiliser celles importées du modèle,
     * mais on peut être amenés à les générer nous même si le modèle n'en n'a pas (mauvais signe).
     */
    bool    generateNormals;

    /**
     * Génère les tangentes/bitangentes pour chaque vertex. Nécessaire pour utiliser du bump,
     * à moins de réutiliser les tangentes importées (ça dépend de l'importer).
     */
    bool    generateTangentBasis;

    /**
     * Optimise l'ordre des vertices et des index pour être "cache friendly".
     * A priori, il n'y a pas de raison de ne pas l'activer, d'autant plus que c'est rapide.
     */
    bool    optimizeForCache;
};

LM_API_WKF void packModel(const ModelPackingOptions & options, Assets::Model & model);

LM_API_WKF void setupTextures(Assets::Model & model, const Ptr<Core::VirtualFileSystem> & pVFS, const String & baseName);

//-----------------------------------------------------------------------------
LM_API_WKF void removeUnusedVertices(Assets::Model & model);
LM_API_WKF void removeDegeneratedTriangles(Assets::Model & model);

LM_API_WKF void removeBlendingGroups(Assets::VerticeSet & set);
LM_API_WKF void removeBlendingGroups(Assets::Model & model);

LM_API_WKF void createBlendingGroups(Assets::VerticeSet & set, int32 maxPaletteSize, const Ptr<Core::IProgressObs> & pObs = null);
LM_API_WKF void createBlendingGroups(Assets::Model & model, int32 maxPaletteSize, const Ptr<Core::IProgressObs> & pObs = null);

LM_API_WKF void mergeIdenticalVertices(Assets::VerticeSet & vset, const Ptr<Core::IProgressObs> & pObs = null);
LM_API_WKF void mergeIdenticalVertices(Assets::Model & model, const Ptr<Core::IProgressObs> & pObs = null);

LM_API_WKF void autoFlipInvertedFaces(Assets::VerticeSet & vset);
LM_API_WKF void autoFlipInvertedFaces(Assets::Model & model, const Ptr<Core::IProgressObs> & pObs);

LM_API_WKF void flipFaces(Assets::VerticeSet & vset);
LM_API_WKF void flipFaces(Assets::Model & model);

LM_API_WKF void normalizeBonesWeights(Assets::Model & model, int32 maxBones = Assets::MAX_BONES_PER_VERTEX);

LM_API_WKF void optimizeVerticesForCache(Assets::Model & model);

LM_API_WKF void generateNormals(Assets::VerticeSet & vset);
LM_API_WKF void generateNormals(Assets::Model & model);

LM_API_WKF void generateTBN(Assets::Model & model, bool generateNormals, const Ptr<Core::IProgressObs> & pObs = null);

}

#endif /*WORKFLOW_MODELOPTIMIZE_H_*/
