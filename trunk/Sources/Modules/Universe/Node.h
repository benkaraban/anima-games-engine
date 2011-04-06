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
#ifndef UNIVERSE_NODE_H_
#define UNIVERSE_NODE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Set.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Vector.h>
#include <Core/Math/KDTree.h>
#include <Core/XML/XMLNode.h>
#include <Renderer/IBox.h>

namespace Assets
{
    class Material;
}

namespace Universe
{
class NodeCamera;
class World;

LM_ENUM_2(EReferential, WORLD_COORD, LOCAL_COORD);

LM_ENUM_15(ENodeType,
          NODE,
          NODE_CAMERA,
          NODE_DECAL,
          NODE_EMITTER,
          NODE_MESH,
          NODE_SKELETON,
          NODE_POINT_LIGHT,
          NODE_WATER_PATCH,
          NODE_GROUP,
          NODE_TERRAIN,
          NODE_SPLINE,
          NODE_TRAIL,
          NODE_STORM,
          NODE_SOUND,
          NODE_LISTENER);

/**
 * Les Node sont les éléments de base du monde. Ils sont invisibles et
 * assemblés en hierarchies. Leurs positions, échelles et orientations
 * sont définis dans le repère local, par rapport à leur père.
 *
 * Pour un objet donné, le référentiel local est le suivant :
 * - l'axe X va de la gauche vers la droite de l'objet
 * - l'axe Y va du bas vers le haut de l'objet
 * - l'axe Z va de l'arrière à l'avant de l'objet
 *
 * Pour un humanoïde, le référentiel est donc le suivant (plus facile à appréhender) :
 * - l'axe X suit la direction du bras droit tendu
 * - l'axe Y est suit la direction pieds -> tête
 * - l'axe Z est la direction dans laquelle regarde le personnage avec la tête droite
 *
 * En pratique, les méthodes à connaître pour un positionnement "normale" des Nodes
 * sont les suivantes :
 *
 * - setWorldPosition() / setLocalPosition()
 * - setWorldDirection() / setLocalDirection()
 * - worldLookAt() / localLookAt()
 * - localPitch() / worldPitch()
 * - localYaw() / worldYaw()
 * - localRoll() / worldRolll()
 *
 * Le Node est la classe de base des entités du monde, les classes suivantes en dérivent :
 * - Entity (mesh ?)
 * - Skeleton
 * - ParticleEmitter
 * - ...
 */
class LM_API_UNI Node : public Core::Object, public Core::IBoxable
{
public:
    Node(const Ptr<Renderer::IAABox> & pBox, World * pWorld);

    /**
     * On peut détruire un Node avec la méthode kill(). Après cela, il ne sera
     * plus référencé par son World mais d'autres objets peuvent avoir conservé une
     * référence vers cet objet. La méthode isAlive permet de savoir si l'objet pointé
     * est encore valide.
     */
    void kill();

    bool isAlive() const { return _isAlive; };
    virtual bool isExportable() const {return isAlive() && _isExportable;}
    virtual void setExportable(bool isExportable) { _isExportable = isExportable; }

    bool isInstance() const { return _isInstance; };
    bool isInstanceChild() const { return _isInstanceChild; }

    void setInstance(const String & resourceName);
    void instanciate();

    const String& getName() const    {if (_name == String::EMPTY) return getResourceName();    return _name;}
    void setName(const String& name) {_name = name;}   

    inline const Core::String & getResourceName() const { return _resourceName; }
    virtual void setStatic(bool isStatic) { _isStatic = isStatic; };
    virtual void setLightmapResolutionRatio(int32 ratio) { _lightmapResRatio = ratio; };

    virtual bool isStatic() const { return _isStatic; };
    virtual int32 getLightmapResolutionRatio() const { return _lightmapResRatio; };

    virtual ENodeType getNodeType() const { return NODE; };

    World * getWorld() const { return _pWorld; }
    /**
     * Fonctions de lecture des coordonnées du Node.
     */
    const Core::Vector3f &    getLocalPosition() const { return _localPosition; };
    const Core::Vector3f &    getLocalScale() const { return _localScale; };
    const Core::Quaternionf & getLocalOrient() const { return _localOrient; };
          Core::Vector3f      getLocalDirection() const;

          Core::Vector3f   getWorldPosition() const;
          Core::Vector3f   getWorldDirection() const; // <=> World Z vector

    const Core::Matrix4f & getLocalMatrix()        const { return _localMatrix; };
    const Core::Matrix4f & getLocalToWorldMatrix() const { return _localToWorldMatrix; };
    const Core::Matrix4f & getWorldToLocalMatrix() const { return _worldToLocalMatrix; };

    const Core::Matrix4f & getParentLocalToWorldMatrix() const { return _parentLocalToWorldMatrix; };
    const Core::Matrix4f & getParentWorldToLocalMatrix() const { return _parentWorldToLocalMatrix; };

    /**
     * La modification de la position/échelle/orientation d'un Node
     * entraîne des calculs potentiellement assez couteux. Pour éviter de tout
     * recalculer à chaque fois, toutes les modifications doivent être encapsulées
     * dans un bloc beginMatrixUpdate()/endMatrixUpdate()
     *
     * Exemple :
     * pNode->beginMatrixUpdate();
     * pNode->setLocalPosition(Core::Vector3f(1.0, 2.0, 3.0));
     * pNode->setLocalDirection(Core::Vector3f(1.0, 0.0, 0.0));
     * pNode->endMatrixUpdate();
     */
    void beginMatrixUpdate();

    void setLocalPosition(const Core::Vector3f & position);
    void setLocalScale(const Core::Vector3f & scale);
    void setLocalOrientation(const Core::Quaternionf & orient);
    void setLocalDirection(const Core::Vector3f & zVec);
    void localLookAt(const Core::Vector3f & position);

    void setWorldPosition(const Core::Vector3f & position);
    void setWorldDirection(const Core::Vector3f & direction);
    void worldLookAt(const Core::Vector3f & position);

    /**
     * Les rotations suivantes *modifient* l'orientation courante (ie.
     * les rotations décrites s'ajoutent à l'orientation actuelle, elles ne la remplacent pas).
     * Les rotations sont locales car les axes XYZ utilisés pour les pitch/yaw/roll
     * sont ceux du Node. Un localYaw(PI) fera donc toujours faire un demi-tour au Node,
     * quelle que soit son orientation actuelle.
     */
    void localPitch(double radians); // Axe X comme pivot
    void localYaw(double radians);   // Axe Y comme pivot
    void localRoll(double radians);  // Axe Z comme pivot

    /**
     * Les rotations suivantes *modifient* l'orientation courante (ie.
     * les rotations décrites s'ajoutent à l'orientation actuelle, elles ne la remplacent pas).
     * Les rotations sont globales : les axes XYZ utilisés sont ceux du monde, ce qui permet
     * d'appliquer une rotation à un objet indépendamment de son orientation actuelle.
     */
    void worldPitch(double radians); // Axe X comme pivot
    void worldYaw(double radians);   // Axe Y comme pivot
    void worldRoll(double radians);  // Axe Z comme pivot

    void endMatrixUpdate();

    /**
     * A ne surtout PAS appeler entre beginMatrixUpdate() et endMatrixUpdate()
     */
    void setLocalMatrix(const Core::Matrix4f & matrix, bool decomposeTransform = true);

    /**
     * Modifie le 'up vector' du Node. Quand on précise uniquement une direction
     * (avec setLocalDirection()/setWorldDirection()), il y a une infinité d'orientations
     * possibles. Parmis les orientations possibles, le Node choisira celle qui permet de
     * conserver un vecteur Y le plus proche possible de son 'up vector'. Par défaut, ce
     * vecteur est à (0.0, 1.0, 0.0).
     * Le 'up vector' n'a aucune incidence sur les modifications explicites de l'orientation
     * (setLocalOrientation(), setWorldOrientation(), localPitch())
     */
    void setUpVector(const Core::Vector3f & upVector) { _upVector = upVector; };
    const Core::Vector3f & getUpVector() const { return _upVector; };

    /**
     * Un Node n'a rien à faire sur une update mais des classes filles peuvent avoir des données
     * à mettre à jour : un émetteur de particules devrait mettre à jour ses particules, un
     * squelette pourrait metter à jour son animation, etc...
     */
    virtual void update(double elapsed);

    void  setTimeMultiplier(float timeMul, bool recursive = false);
    float getTimeMultiplier() const;

    /**
     * Tous les Node définissent une bounding box. La classe Node de base retourne une box
     * qui ne contient qu'un seul point : la world position du Node. Les classes filles
     * doivent redéfinir cette méthode pour retourner une box adaptée.
     */
    virtual const Core::AABoxf & getBoundingBox() const;

    virtual void setVisible(bool flag, bool recursive = false);
    virtual bool isVisible() const;

    virtual void setBoundingBoxVisible(bool flag);
    virtual bool isBoundingBoxVisible() const; 

    void addTag(const String & tag);
    void removeTag(const String & tag);
    bool hasTag(const String & tag) const;
    const Core::Set<String>& getTags() const;
    void setTags(const Core::Set<String>& tags);

    virtual void getChildrenWithTag(const String & tag, Core::List<Ptr<Node> > & nodes, bool recursive = false) const;
    virtual Ptr<Node> getChildWithTag(const String & tag, bool recursive = false) const;
    void getChildrenOfType(ENodeType type, Core::List<Ptr<Node> > & nodes, bool recursive = false) const;

    virtual bool isRoot() const;
    virtual void addChild(const Ptr<Node> & pNode, bool sameWorldPos = false, int32 indexPos=-1);  //insert l'enfant a la position indexPos de la liste d'enfant);
    virtual void removeChild(const Ptr<Node> & pNode, bool sameWorldPos = true);

    /**
     * Permet d'ajouter/supprimer un ghost child au noeud. Les ghost children suivent
     * le père (leur matrice est mise à jour) et c'est tout. Ils ne meurent pas avec
     * le père et ne sont pas importés/exportés en XML.
     */
    virtual void addGhostChild(const Ptr<Node> & pNode);
    virtual void removeGhostChild(const Ptr<Node> & pNode, bool sameWorldPos = true);

    void setNodeScript(const String8 & source);
    const String8 & getNodeScript() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

            void importInstanceXML(const Ptr<Core::XMLNode> & pNode);
            void exportInstanceXML(const Ptr<Core::XMLNode> & pNode);

    int32 getChildCount() const { return _children.size(); };
    const Ptr<Node>& getChild(int32 index) const { return _children[index]; };
    const Core::List<Ptr<Node> >& getChildren() {return _children;}

    /*NOT virtual*/ bool isIntersecting(const Core::Rayf& ray, float& distance);
    virtual bool isIntersectingBoundingBox(const Core::Rayf& ray, float& distance) const;

    virtual bool isRayOccluded(const Core::Rayf& ray) const;

    virtual void prepareForRendering(const Ptr<NodeCamera> & pCamera);

    void setFixedSize(bool flag, float widthFct) {_isFixedSize = flag; _fixedSizeWidthFct=widthFct;}
    bool isFixedSize() const {return _isFixedSize;}

    virtual void restart(bool recursive = false);
    virtual void stop(bool recursive = false, float inTime = 0.0f);

    virtual void setNodeColor(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeColor() const;

    virtual void setNodeGlow(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeGlow() const;

    virtual void setMaterial(const Assets::Material & material, bool recursive = false);
    virtual void getMaterial(      Assets::Material & material);

    void setChildInstance();

    virtual void prepareForPacking();

protected:
    void updateLocalMatrix();
    void updateWorldMatrix();
    void updateBoundingBox();
    virtual void internalPrepareForPacking();

    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);

    /**
     * Cette méthode privée est appelée par le père du Node quand sa matrice change.
     */
    void setParentMatrix(const Core::Matrix4f & worldMatrix,
                         const Core::Matrix4f & invWorldMatrix);

    void internalUpdateParentMatrix(const Ptr<Node> & pNode);

    bool                   _isAlive;
    bool                   _isVisible;
    bool                   _isBoxVisible;
    bool                   _isRoot;
    bool                   _isFixedSize;
    bool                   _isInstance;
    bool                   _isInstanceChild;
    bool                   _isExportable;
    float                  _fixedSizeWidthFct;
    Core::Vector3f         _upVector;
    Core::AABoxf           _box;

    World *                _pWorld;
    Ptr<Renderer::IAABox>  _pBox;

    Core::Matrix4f         _parentLocalToWorldMatrix; // maj. par le père
    Core::Matrix4f         _parentWorldToLocalMatrix; // maj. par le père

    Core::Vector3f         _localPosition;
    Core::Vector3f         _localScale;
    Core::Quaternionf      _localOrient;

    Core::Matrix4f         _localMatrix;        // = composition de _localPosition/Scale/Orient
    Core::Matrix4f         _localToWorldMatrix; // = _parentWorldMatrix * _localMatrix
    Core::Matrix4f         _worldToLocalMatrix; // = inverse de _localToWorldMatrix

    Core::Set<String>      _tags;

    Core::List<Ptr<Node> > _children;
    Core::List<Ptr<Node> > _ghostChildren;
    bool                   _isInMatrixUpdate;

    float                  _timeMultiplier;
    String8                _script;

    String                 _resourceName;
    String                 _name;
    bool                   _isStatic;
    int32                  _lightmapResRatio;

    friend class LuaNode;
};

class LM_API_UNI INodeCreator
{
public:
    virtual ~INodeCreator() {};
    virtual Ptr<Node> createNode() = 0;
    virtual Ptr<Renderer::IAABox> createBox() = 0;
};

    
LM_API_UNI void intersect_RayNodeRecursive(const Ptr<Node>& pNode, const Core::Rayf& ray, float& distance, Ptr<Node>& pNodeIntersecting);
}

#endif /* NODE_H_ */
