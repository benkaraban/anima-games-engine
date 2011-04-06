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
#include <Core/Logger.h>
#include <Core/XML/XMLMath.h>
#include <Universe/NodeSkeleton.h>
#include <Universe/World.h>

#include <Core/Math/GeometryIntersect.h>

namespace Universe
{
//-----------------------------------------------------------------------------
SkeletonSkin::SkeletonSkin(const Ptr<Renderer::IAABox> & pBox,
                           const Ptr<Renderer::ISkinMeshInstance> & pSkin,
                           const Ptr<RessourcePool> & pRessourcesPool) :
    _pBox(pBox),
    _pSkin(pSkin),
    _pRessourcesPool(pRessourcesPool),
    _isAlive(true),
    _isVisible(true),
    _isBoxVisible(false)
{
    _pBox->setColor(Core::Vector4f(0.0, 1.0, 0.0, 0.0));
    _pBox->setVisible(isBoundingBoxVisible());
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setMesh(const String & modelName)
{
    setMesh(modelName, String::EMPTY);
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setMesh(const String & modelName, const String & meshName)
{
    Ptr<Assets::ModelMesh> pModelMesh;
    Ptr<Renderer::ISkinMesh> pMesh;

    if(modelName == String::EMPTY)
    {
        _modelName = String::EMPTY;
        _meshName = String::EMPTY;
        pModelMesh = _pRessourcesPool->getDefaultModelMesh();
        pMesh = _pRessourcesPool->getDefaultSkinMesh();
    }
    else
    {
       Ptr<Assets::Model> pModel(_pRessourcesPool->getModel(modelName));

       int32 meshId = 0;

       if(meshName != String::EMPTY)
           meshId = pModel->getMeshId(meshName);

        if(meshId == Assets::UNDEFINED_MESH_ID)
        {
            String message(L"Unable to create mesh, '");
            message << meshName << L"' not found in model '" << modelName <<L"'";
            ERR << message << L"\n";
            throw Core::Exception(message);
        }

        _modelName = modelName;
        _meshName = meshName;

        pModelMesh = pModel->getMeshPtr(meshId);
        pMesh = _pRessourcesPool->getSkinMesh(pModelMesh);
    }

    const Assets::Material & mat = pModelMesh->getMaterial();
    _diffuseTexName = mat.diffuseTexName;
    _specularTexName = mat.specularTexName;
    _normalTexName = mat.normalTexName;

    _pSkin->setMesh(pMesh);
    _pSkin->setMaterial(pMesh->getMaterial());
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setDiffuseTexture(const String & texName)
{
    Renderer::Material material = _pSkin->getMaterial();

    if(texName == String::EMPTY)
    {
        material.pDiffuseMap = null;
    }
    else
    {
        material.pDiffuseMap = _pRessourcesPool->getTexture(texName);
    }

    _diffuseTexName = texName;
    _pSkin->setMaterial(material);
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setSpecularTexture(const String & texName)
{
    Renderer::Material material = _pSkin->getMaterial();

    if(texName == String::EMPTY)
    {
        material.pSpecularMap = null;
    }
    else
    {
        material.pSpecularMap = _pRessourcesPool->getTexture(texName);
    }

    _specularTexName = texName;
    _pSkin->setMaterial(material);
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setNormalTexture(const String & texName)
{
    Renderer::Material material = _pSkin->getMaterial();

    if(texName == String::EMPTY)
    {
        material.pNormalMap = null;
    }
    else
    {
        material.pNormalMap = _pRessourcesPool->getTexture(texName);
    }

    _normalTexName = texName;
    _pSkin->setMaterial(material);
}
//-----------------------------------------------------------------------------
void SkeletonSkin::getDiffuseTexture(String & texName)
{
    texName = _diffuseTexName;
}
//-----------------------------------------------------------------------------
void SkeletonSkin::getSpecularTexture(String & texName)
{
    texName = _specularTexName;
}
//-----------------------------------------------------------------------------
void SkeletonSkin::getNormalTexture(String & texName)
{
    texName = _normalTexName;
}
//-----------------------------------------------------------------------------
void SkeletonSkin::getMaterial(Assets::Material & mat)
{
    const Renderer::Material & material = _pSkin->getMaterial();

    mat.diffuseTexName  = _diffuseTexName;
    mat.specularTexName = _specularTexName;
    mat.normalTexName   = _normalTexName;

    mat.emissive    = material.emissive;
    mat.diffuse     = material.diffuse;
    mat.specular    = material.specular;
    mat.shininess   = material.shininess;
    
    mat.glowColor   = material.glowColor;
    mat.glowExtent  = material.glowExtent;
    mat.reflexions  = material.reflexions;

    mat.glowColor   = material.glowColor;
    mat.glowExtent  = material.glowExtent;
    mat.reflexions  = material.reflexions;

    mat.refracColor = material.refracColor;
    mat.refracScale = material.refracScale;
    mat.refracIndex = material.refracIndex;

    mat.type        = Assets::MAT_STANDARD;
    mat.flags       = 0;

    if(material.transDiffuseTex)
        mat.flags |= Assets::MAT_FLAG_TRANSLUCID_TEXTURE;

    if(material.clampTextureBorder)
        mat.flags |= Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER;
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setMaterial(const Assets::Material & material)
{
    setDiffuseTexture(material.diffuseTexName);
    setSpecularTexture(material.specularTexName);
    setNormalTexture(material.normalTexName);

    Renderer::Material mat = _pSkin->getMaterial();
    mat.emissive    = material.emissive;
    mat.diffuse     = material.diffuse;
    mat.specular    = material.specular;
    mat.shininess   = material.shininess;
    
    mat.glowColor   = material.glowColor;
    mat.glowExtent  = material.glowExtent;
    mat.reflexions  = material.reflexions;

    mat.refracColor = material.refracColor;
    mat.refracScale = material.refracScale;
    mat.refracIndex = material.refracIndex;

    mat.transDiffuseTex = ( (material.flags & Assets::MAT_FLAG_TRANSLUCID_TEXTURE) != 0);
    mat.clampTextureBorder = ( (material.flags & Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER) != 0);

    _pSkin->setMaterial(mat);
}
//-----------------------------------------------------------------------------
void SkeletonSkin::importXML(const Ptr<Core::XMLNode> & pNode)
{
    String modelName;
    String meshName;
    String diffuseTexName;
    String specularTexName;
    String normalTexName;

    pNode->getValue(L"model", modelName);
    pNode->getValue(L"mesh",  meshName);
    pNode->getValue(L"diffuse-texture",  diffuseTexName);
    pNode->getValue(L"specular-texture",  specularTexName);
    pNode->getValue(L"normal-texture",  normalTexName);

    String strEmissive;
    String strDiffuse;
    String strSpecular;
    String strShininess;

    pNode->getValue(L"emissive",  strEmissive);
    pNode->getValue(L"diffuse",   strDiffuse);
    pNode->getValue(L"specular",  strSpecular);

    setMesh(modelName, meshName);
    setDiffuseTexture(diffuseTexName);
    setSpecularTexture(specularTexName);
    setNormalTexture(normalTexName);

    Renderer::Material mat = _pSkin->getMaterial();

    Core::fromXMLString(strEmissive,  mat.emissive);
    Core::fromXMLString(strDiffuse,   mat.diffuse);
    Core::fromXMLString(strSpecular,  mat.specular);

    pNode->getValue(L"shininess", mat.shininess);

    _pSkin->setMaterial(mat);
}
//-----------------------------------------------------------------------------
void SkeletonSkin::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Renderer::Material material = _pSkin->getMaterial();

    pNode->setValue(L"model", _modelName);
    pNode->setValue(L"mesh",  _meshName);
    pNode->setValue(L"diffuse-texture",  _diffuseTexName);
    pNode->setValue(L"specular-texture",  _specularTexName);
    pNode->setValue(L"normal-texture",  _normalTexName);

    pNode->setValue(L"emissive",  Core::toXMLString(material.emissive));
    pNode->setValue(L"diffuse",   Core::toXMLString(material.diffuse));
    pNode->setValue(L"specular",  Core::toXMLString(material.specular));
    pNode->setValue(L"shininess", material.shininess);
}
//-----------------------------------------------------------------------------
bool SkeletonSkin::isAlive() const
{
    return _isAlive;
}
//-----------------------------------------------------------------------------
void SkeletonSkin::kill()
{
    _isAlive = false;
    _pSkin->kill();
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setVisible(bool flag)
{
    _isVisible = flag;
    _pSkin->setVisible(_isVisible);
    _pBox->setVisible(_isVisible && _isBoxVisible);
}
//-----------------------------------------------------------------------------
bool SkeletonSkin::isVisible() const
{
    return _isVisible;
}
//-----------------------------------------------------------------------------
void SkeletonSkin::setBoundingBoxVisible(bool flag)
{
    _isBoxVisible = flag;
    _pBox->setVisible(_isVisible && _isBoxVisible);
}
//-----------------------------------------------------------------------------
bool SkeletonSkin::isBoundingBoxVisible() const
{
    return _isBoxVisible;
}
//-----------------------------------------------------------------------------
bool SkeletonSkin::isIntersectingBoundingBox(const Core::Rayf& ray, float& distance) const
{
    const Core::AABoxf& box = getSkin()->getBoundingBox();

    return intersect_RayAABox(ray, box, distance);
}
//-----------------------------------------------------------------------------
bool SkeletonSkin::isIntersecting(const Core::Rayf& ray, float& distance) const
{
    bool result = false;

    if(isVisible() && isIntersectingBoundingBox(ray, distance))
    {
        Core::Vector3f posInter;
        Assets::VerticeSet vset;

        Assets::SkeletonPose pose;
        pose.setBindShapeMatrix(_pSkin->getSkeleton()->getPose(), _pSkin->getMesh()->getBindShapeMatrix());
        pose.transformPosition(*_pSkin->getMesh()->getVerticeSet(), vset);

        result = intersect_RayVerticeSet(ray, vset, posInter);
        if(result)
            distance = ray.origin.getDistance(posInter);
    }

    return result;
}
//-----------------------------------------------------------------------------
void SkeletonSkin::updateBoundingBox()
{
    _pBox->setBox(_pSkin->getBoundingBox());
}
//-----------------------------------------------------------------------------
const Core::AABoxf & SkeletonSkin::getBoundingBox() const
{
    return getSkin()->getBoundingBox();
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
NodeSkeleton::AnimInfo::AnimInfo() : id(0), loop(false), keyFrom(0.0f), keyTo(1.0f), duration(1.0), pObserver(NULL)
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
NodeSkeleton::NodeSkeleton(const Ptr<Renderer::IAABox> & pBox,
                           const Ptr<Renderer::IScene> & pScene,
                           const Ptr<RessourcePool> & pRessourcesPool,
                           World * pWorld) :
    Node(pBox, pWorld),
    _pScene(pScene),
    _pRessourcesPool(pRessourcesPool),
    _pSkelPose((new Assets::SkeletonPose()))
{
    Ptr<Assets::Model> pModel(_pRessourcesPool->getDefaultModel());
    _pSkeleton = _pScene->createSkeletonInstance(pModel->getSkeletonPtr());
    _pSkeleton->setPose(_pSkelPose);
}
//-----------------------------------------------------------------------------
void NodeSkeleton::internalPrepareForPacking()
{
    if(_skins.empty())
        kill();
}
//-----------------------------------------------------------------------------
void NodeSkeleton::setModel(const String & modelName)
{
    _modelName = modelName;

    if(modelName == String::EMPTY)
        _pModel = _pRessourcesPool->getDefaultModel();
    else
        _pModel = _pRessourcesPool->getModel(modelName);

    _pSkeleton->setSkeleton(_pModel->getSkeletonPtr());

    for(Core::List<Ptr<Node> >::const_iterator ipNode = _modelNodes.begin(); ipNode != _modelNodes.end(); ++ipNode)
    {
        removeChild(*ipNode, false);
        (*ipNode)->kill();
    }

    _modelNodes.clear();
    _modelNodes.resize(_pModel->getNodeCount());

    for(int32 iNode=0; iNode < _pModel->getNodeCount(); iNode++)
    {
        String name(_pModel->getNode(iNode).getName());

        if(name.startsWith(L"_BOX"))
        {
            Ptr<Node> pNode(_pWorld->createNode());
            pNode->addTag(name);
            addChild(pNode);
            _modelNodes[iNode] = pNode;
        }
    }

    setDefaultPose();
    onWorldMatrixUpdate();
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeSkeleton::addModelNode(int32 iNode, const Ptr<Node> & pNode)
{
    if(_modelNodes[iNode] == null)
    {
        _modelNodes[iNode] = pNode;
        addChild(_modelNodes[iNode]);
    }
    else
    {
        _modelNodes[iNode]->addChild(pNode);
    }
}
//-----------------------------------------------------------------------------
int32 NodeSkeleton::getAnimCount() const
{
    return _pModel->getAnimationCount();
}
//-----------------------------------------------------------------------------
const String & NodeSkeleton::getAnimName(int32 id) const
{
    return _pModel->getAnimationName(id);
}
//-----------------------------------------------------------------------------
void NodeSkeleton::setDefaultPose()
{
#if DEBUG_NODE_SKELETON_ANIM
    _lastAnim = Assets::UNDEFINED_ANIM_ID;
#endif
    _anims.clear();
    _elapsed = 0.0;
    updateSkeleton(Assets::UNDEFINED_ANIM_ID, 0.0);
}
//-----------------------------------------------------------------------------
void NodeSkeleton::removeObserver(IAnimObserver * pObs)
{
    Core::Queue<AnimInfo> filteredQueue;

    while(!_anims.empty())
    {
        AnimInfo & anim = _anims.front();

        if(anim.pObserver == pObs)
            anim.pObserver = NULL;

        filteredQueue.push(anim);
        _anims.pop();
    }

    _anims = filteredQueue;
}
//-----------------------------------------------------------------------------
void NodeSkeleton::enqueueAnim(int32 id,   double duration, float keyFrom, float keyTo, bool loop, IAnimObserver * pObserver)
{
    if(duration == 0.0)
    {
        float nativeDuration = _pModel->getAnimationDuration(id);
        duration = nativeDuration * (keyTo - keyFrom);
    }

    AnimInfo infos;
    infos.id = id;
    infos.duration = duration;
    infos.keyFrom = keyFrom;
    infos.keyTo = keyTo;
    infos.loop = loop;
    infos.pObserver = pObserver;

    _anims.push(infos);
}
//-----------------------------------------------------------------------------
void NodeSkeleton::enqueueAnim(const String & name, double duration, float keyFrom, float keyTo, bool loop, IAnimObserver * pObserver)
{
    int32 id = _pModel->getAnimationId(name);
    if(id == Assets::UNDEFINED_ANIM_ID)
        throw Core::Exception(L"Unkown animation : '" + name + L"'");
    enqueueAnim(id, duration, keyFrom, keyTo, loop, pObserver);
}
//-----------------------------------------------------------------------------
void NodeSkeleton::animate(int32 id, double duration, float keyFrom, float keyTo, bool loop, IAnimObserver * pObserver)
{
    _anims.clear();
    enqueueAnim(id, duration, keyFrom, keyTo, loop, pObserver);
    _elapsed = 0.0;
}
//-----------------------------------------------------------------------------
void NodeSkeleton::animate(const String & name, double duration, float keyFrom, float keyTo, bool loop, IAnimObserver * pObserver)
{
    int32 id = _pModel->getAnimationId(name);
    if(id == Assets::UNDEFINED_ANIM_ID)
        throw Core::Exception(L"Unkown animation : '" + name + L"'");
    animate(id, duration, keyFrom, keyTo, loop, pObserver);
}
//-----------------------------------------------------------------------------
void NodeSkeleton::onWorldMatrixUpdate()
{
    _pSkeleton->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
void NodeSkeleton::onVisibilityChange(bool flag)
{
    for(Core::List<Ptr<SkeletonSkin> >::const_iterator ipSkin = _skins.begin(); ipSkin != _skins.end(); ++ipSkin)
        (*ipSkin)->setVisible(flag);
}
//-----------------------------------------------------------------------------
const Ptr<Renderer::ISkeletonInstance> & NodeSkeleton::getSkeleton() const
{
    return _pSkeleton;
}
//-----------------------------------------------------------------------------
void NodeSkeleton::update(double elapsed)
{
    Node::update(elapsed);

    for(int32 ii=0; ii < _skins.size(); )
    {
        if(_skins[ii]->isAlive())
            ii++;
        else
            _skins.qerase(ii);
    }


    if(!_anims.empty())
    {
        _elapsed += elapsed;

        while(!_anims.empty() && _elapsed > _anims.front().duration)
        {
            AnimInfo & anim = _anims.front();

            // Une animation ne doit pas boucler s'il y a d'autres animations dans la file
            anim.loop = (anim.loop && _anims.size() == 1);

            if(anim.loop)
                _elapsed = fmod(_elapsed, anim.duration);
            else
                _elapsed -= anim.duration;

            updateSkeleton(anim.id, anim.keyTo);
            
            if(anim.pObserver != NULL)
                anim.pObserver->animProgress(anim.keyTo);

            if(!anim.loop || anim.duration == 0.0)
                _anims.pop();
        }

        if(!_anims.empty())
        {
            const AnimInfo & anim = _anims.front();
            float key = anim.keyFrom + float(_elapsed / anim.duration) * (anim.keyTo - anim.keyFrom);
            updateSkeleton(anim.id, key);

            if(anim.pObserver != NULL)
                anim.pObserver->animProgress(key);
        }
    }
}
//-----------------------------------------------------------------------------
void NodeSkeleton::updateSkeleton(int32 id, float key)
{
#if DEBUG_NODE_SKELETON_ANIM
    if(id != _lastAnim)
    {
        INF << L"Anim " << Core::toString(id) << L"\n";
        _lastAnim = id;
    }
#endif
    if(id == Assets::UNDEFINED_ANIM_ID)
        _pModel->getDefaultPose(_pose);
    else
        _pModel->getPose(_pose, id, key);

    for(int32 iNode=0; iNode < _modelNodes.size(); iNode++)
    {
        if(_modelNodes[iNode] != null)
            _modelNodes[iNode]->setLocalMatrix(_pose.getNodeMatrix(iNode), true);
    }
    
    _pModel->getSkeleton().getSkeletonPose(*_pSkelPose, _pose);
    _pSkeleton->setPose(_pSkelPose);

    for(int32 iSkin=0; iSkin < _skins.size(); iSkin++)
        _skins[iSkin]->updateBoundingBox();

    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeSkeleton::onKill()
{
    for(Core::List<Ptr<SkeletonSkin> >::const_iterator ipSkin = _skins.begin(); ipSkin != _skins.end(); ++ ipSkin)
        (*ipSkin)->kill();

    _pSkeleton->kill();
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeSkeleton::internalGetBoundingBox() const
{
    Core::AABoxf result;

    for(Core::List<Ptr<SkeletonSkin> >::const_iterator ipSkin = _skins.begin(); ipSkin != _skins.end(); ++ ipSkin)
        result.setUnion((*ipSkin)->getSkin()->getBoundingBox());

    return result;
}
//-----------------------------------------------------------------------------
void NodeSkeleton::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    String modelName;
    pNode->getValue(L"model", modelName);
    setModel(modelName);

    Ptr<Core::XMLNode> pAnim(pNode->firstChildNode(L"anim"));

    Core::List<Ptr<Core::XMLNode> > children;
    pNode->getNodes(L"skins", L"skin-mesh", children);

    for(Core::List<Ptr<Core::XMLNode> >::const_iterator ipChild = children.begin(); ipChild != children.end(); ++ipChild)
    {
        Ptr<SkeletonSkin> pSkin(createSkin());
        pSkin->importXML(*ipChild);
    }

    _modelNodes.clear();
    _modelNodes.resize(_pModel->getNodeCount());

    for(int32 iNode=0; iNode < _pModel->getNodeCount(); iNode++)
    {
        _modelNodes[iNode] = getChildWithTag(_pModel->getNode(iNode).getName());
    }

    _anims.clear();

    if(pAnim != null)
    {
        AnimInfo anim;

        pAnim->getValue(L"id", anim.id);
        pAnim->getValue(L"loop", anim.loop);
        pAnim->getValue(L"key-from", anim.keyFrom);
        pAnim->getValue(L"key-to", anim.keyTo);
        pAnim->getValue(L"duration", anim.duration);
        pAnim->getValue(L"elapsed", _elapsed);

        _anims.push(anim);
        update(0.0);
    }
    else
    {
        updateSkeleton(Assets::UNDEFINED_ANIM_ID, 0.0);
    }
}
//-----------------------------------------------------------------------------
void NodeSkeleton::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"model", _modelName);

    if(!_anims.empty())
    {
        const AnimInfo & anim = _anims.front();
        Ptr<Core::XMLNode> pAnim(new Core::XMLNode(L"anim"));

        pAnim->setValue(L"id", anim.id);
        pAnim->setValue(L"loop", anim.loop);
        pAnim->setValue(L"key-from", anim.keyFrom);
        pAnim->setValue(L"key-to", anim.keyTo);
        pAnim->setValue(L"duration", anim.duration);
        pAnim->setValue(L"elapsed", _elapsed);

        pNode->addNode(pAnim);
    }

    for(Core::List<Ptr<SkeletonSkin> >::const_iterator ipSkin = _skins.begin(); ipSkin != _skins.end(); ++ipSkin)
    {
        Ptr<Core::XMLNode> pChild(new Core::XMLNode(L"skin-mesh"));
        (*ipSkin)->exportXML(pChild);
        pNode->addNode(L"skins", pChild);
    }
}
//-----------------------------------------------------------------------------
Ptr<SkeletonSkin> NodeSkeleton::createSkin()
{
    Ptr<Assets::Model> pModel(_pRessourcesPool->getDefaultModel());
    Ptr<Renderer::ISkinMesh> pMesh(_pRessourcesPool->getSkinMesh(pModel->getMeshPtr(0)));
    Ptr<Renderer::ISkinMeshInstance> pSkinMesh(_pScene->createSkinMeshInstance(pMesh, _pSkeleton));
    Ptr<SkeletonSkin> pSkin(new SkeletonSkin(_pWorld->createBox(), pSkinMesh, _pRessourcesPool));

    _skins.push_back(pSkin);
    updateBoundingBox();

    return pSkin;
}
//-----------------------------------------------------------------------------
Ptr<SkeletonSkin> NodeSkeleton::createSkin(const String & modelName)
{
    Ptr<SkeletonSkin> pSkin = createSkin();
    pSkin->setMesh(modelName);
    return pSkin;
}
//-----------------------------------------------------------------------------
Ptr<SkeletonSkin> NodeSkeleton::createSkin(const String & modelName, const String & meshName)
{
    Ptr<SkeletonSkin> pSkin = createSkin();
    pSkin->setMesh(modelName, meshName);
    return pSkin;
}
//-----------------------------------------------------------------------------
void NodeSkeleton::createAllSkins(const String & modelName)
{
    Ptr<Assets::Model> pModel(_pRessourcesPool->getModel(modelName));
    
    for(int32 ii=0; ii < pModel->getMeshCount(); ii++)
    {
        const Assets::ModelMesh & mesh = pModel->getMesh(ii);

        if(mesh.isSkin())
            createSkin(modelName, mesh.getName());
    }
}
//-----------------------------------------------------------------------------
Ptr<SkeletonSkin> NodeSkeleton::getSkin(const String & meshName) const
{
    return getSkin(_modelName, meshName);
}
//-----------------------------------------------------------------------------
Ptr<SkeletonSkin> NodeSkeleton::getSkin(const String & modelName, const String & meshName) const
{
    Ptr<SkeletonSkin> pResult;
    int32 iSkin = 0;

    while(pResult == null && iSkin < _skins.size())
    {
        if(_skins[iSkin]->getModelName() == modelName && _skins[iSkin]->getMeshName() == meshName)
            pResult = _skins[iSkin];
        iSkin++;
    }

    return pResult;
}
//-----------------------------------------------------------------------------
bool NodeSkeleton::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{
    bool result = false;
    const Core::AABoxf& box = getBoundingBox();
    distance = FLT_MAX;

    if(intersect_RayAABox(ray, box, distance))
    {
        for(int32 iSkin=0; iSkin < _skins.size(); iSkin++)
        {
            float skinDist;

            if(_skins[iSkin]->isIntersecting(ray, skinDist))
            {
                result = true;
                distance = std::min(distance, skinDist);
            }
        }
    }

    return result;
}
//-----------------------------------------------------------------------------
}
