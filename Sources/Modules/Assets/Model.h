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

#ifndef ASSETS_MODEL_H_
#define ASSETS_MODEL_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Map.h>
#include <Core/IO/Streams.h>
#include <Assets/Animation.h>
#include <Assets/ModelMesh.h>
#include <Assets/ModelNode.h>
#include <Assets/Skeleton.h>

namespace Assets
{
class LM_API_ASS ModelPose;

static const int32 MAGIC_TERRAIN = LM_FOURCC('T', 'E', 'R', '0');

/**
 * Cette classe représente un modèle 3D.
 * Un modèle est un ensemble complexe comprenant de multiples informations.
 * Pour commencer, le modèle défini une hierarchie de Assets::ModelNode. Ces noeuds (parfois
 * appelés Frame) composent le graphe de scène local du modèle. Les modèles les
 * plus basiques peuvent ne contenir qu'un seul noeud racine.
 *
 * Le modèle est également composé de Assets::ModelMesh. Ces meshs sont les objets 3D eux même,
 * définis par un ensemble de triangles.
 * Chaque Mesh est attaché à un Node particulier et lié à un Material qui définit
 * la manière dont il doit être rendu. Un Mesh peut exister en plusieurs versions
 * correspondants à des niveaux de détails (LOD) différents.
 *
 * Un modèle peut contenir des animations. Les animations définissent
 * les mouvements de chacun des Assets::ModelNode, et indirectement des Mesh qui y sont attachés.
 * A noter que la classe ne gère pas les animations par vertex : seuls les Node
 * peuvent être animés.
 *
 * On peut néanmoins déformer les objets avec de l'animation par squelette sur lequel
 * on plaque une skin. Dans ce cas, ce n'est pas au niveau du Assets::ModelMesh que le
 * lien vers un Assets::ModelNode est effectué, mais au niveau du vertex. Chaque vertex
 * peut être influencé par un ou plusieurs Assets::ModelNode.
 *
 * Les liens suivants traitent de la lecture des fichiers 3D .X mais représentent également
 * une introduction assez simple de l'animation par squelette :
 *
 * http://www.toymaker.info/Games/html/load_x_hierarchy.html
 * http://www.gamedev.net/reference/programming/features/xfilepc/default.asp
 */
class LM_API_ASS Model : public Core::Object, public Core::Serializable
{
public:
    Model();

    /**
     * Retourne le nombre d'animations définies dans le modèle.
     */
    int32 getAnimationCount() const { return _anims.size(); };

    /**
     * Retourne le nom d'une animation à partir de son identifiant.
     * Les identifiants commencent à 0 et sont incrémentés. Ils sont donc
     * compris entre 0 et getAnimationCount() - 1.
     */
    const String & getAnimationName(int32 id) const { return _animsName[id]; };

    float getAnimationDuration(int32 id) { return _anims[id]->getDuration(); };
    int32 getAnimationId(const String & name) const;

    /**
     * Cette méthode permet d'obtenir une pose du modèle. L'argument \a pose
     * est mis à jour avec les données correspondant à la pose du modèle
     * au moment \a key de l'animation \a animationId. Key est une valeur comprise
     * entre 0 et 1. 0 correspond au début de l'animation et 1 à la fin.
     *
     * En pratique, le Model adapte \a pose au nombre de Assets::ModelNode qu'il
     * contient. Ensuite, il appelle Assets::ModelNode::getPose() sur son noeud
     * racine. Chaque noeud renseigne sa matrice dans la pose et transmet
     * récursivement l'appel.
     */
    void getPose(ModelPose & pose, int32 animationId, float key) const;
    void getDefaultPose(ModelPose & pose) const;

          Skeleton & getSkeleton()       { return *_pSkeleton; };
    const Skeleton & getSkeleton() const { return *_pSkeleton; };
    const Ptr<Skeleton> & getSkeletonPtr() const { return _pSkeleton; };

    int32 getNodeCount() const { return _nodes.size(); };
    const ModelNode & getNode(int32 id) const { return *_nodes[id]; };
    const ModelNode & getRootNode() const { return getNode(ROOT_NODE_ID); };

    int32 getMeshCount() const { return _meshes.size(); };
    const ModelMesh & getMesh(int32 id) const { return *_meshes[id]; };
    const Ptr<ModelMesh> & getMeshPtr(int32 id) const { return _meshes[id]; };

    int32 getNodeId(const String & name) const;
    int32 getMeshId(const String & name) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    // EDITION

    int32 addNode(const String & name,
                  int32 parentId = ROOT_NODE_ID,
                  const Core::Matrix4f & matrix = Core::Matrix4f(Core::IDENTITY));
    int32 addMeshNode(const String & name,
                      int32 parentId,
                      int32 meshId,
                      const Core::Matrix4f & matrix = Core::Matrix4f(Core::IDENTITY));
    int32 addMesh(const String & name, const ModelMesh & mesh);
    int32 addAnimation(const String & name, const Animation & anim);

    ModelMesh & getMesh(int32 id) { return *_meshes[id]; };
    ModelNode & getNode(int32 id) { return *_nodes[id]; };

    void setAnimationDuration(int32 id, float d) { _anims[id]->setDuration(d); };
    void mergeAnimations(const Assets::Model & source);
    void deleteAnimation(int32 id);
    void renameAnimation(int32 id, const String & newName);

    /**
     * Technique horrible pour le reload dynamique des modèles. Permet de conserver
     * les pointeurs vers l'ancienne version.
     */
    void replaceMeshPtr(int32 id, const Ptr<ModelMesh> & pMesh);
   
    /**
     * Effectue les opérations de base pour avoir un modèle correct :
     * - Au moins 1 blending group
     * - Bounding boxes à jour
     */
    void cleanup();

protected:
    int32                       _flags;
    Core::List<Ptr<Animation> > _anims;
    Core::List<String>          _animsName;
    Core::Map<String, int32>    _animsLib;
    Core::List<Ptr<ModelNode> > _nodes;
    Core::List<Ptr<ModelMesh> > _meshes;
    Core::Map<String, int32>    _nodesLib;
    Core::Map<String, int32>    _meshesLib;
    Ptr<Skeleton>               _pSkeleton;

    bool isUsed(const String & name);
    String getUniqueNodeName(const String & name);
    String getUniqueMeshName(const String & name);
    String getUniqueAnimName(const String & name);
};
}
#endif
