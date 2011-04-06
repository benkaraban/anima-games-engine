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
#include <Universe/Node.h>
#include <Core/Math/Transform.h>
#include <Core/XML/XMLMath.h>

#include <algorithm>

#include <Core/Math/GeometryIntersect.h>
#include <Universe/NodeCamera.h>
#include <Universe/Tools.h>

namespace Universe
{
//-----------------------------------------------------------------------------
Node::Node(const Ptr<Renderer::IAABox> & pBox, World * pWorld) :
    _isAlive(true),
    _isVisible(true),
    _isBoxVisible(false),
    _isRoot(true),
    _isFixedSize(false),
    _isInstance(false),
    _isInstanceChild(false),
    _isExportable(true),
    _fixedSizeWidthFct(0.1f),
    _upVector(0.0, 1.0, 0.0),
    _pBox(pBox),
    _pWorld(pWorld),
    _parentLocalToWorldMatrix(Core::IDENTITY),
    _parentWorldToLocalMatrix(Core::IDENTITY),
    _localPosition(Core::ZERO),
    _localScale(Core::ONE),
    _localOrient(Core::IDENTITY),
    _localMatrix(Core::IDENTITY),
    _localToWorldMatrix(Core::IDENTITY),
    _worldToLocalMatrix(Core::IDENTITY),
    _isInMatrixUpdate(false),
    _timeMultiplier(1.0f),
    _isStatic(false),
    _lightmapResRatio(2)
{
    updateBoundingBox();
    pBox->setColor(Core::Vector4f(0.0, 1.0, 0.0, 1.0));
    pBox->setVisible(isBoundingBoxVisible());
}
//-----------------------------------------------------------------------------
void Node::kill()
{
    _pBox->kill();
    onKill();
    for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
        (*ipChild)->kill();
    _children.clear();
    _ghostChildren.clear();
    _isAlive = false;
}
//-----------------------------------------------------------------------------
void Node::onKill()
{
}
//-----------------------------------------------------------------------------
void Node::setInstance(const String & resourceName)
{ 
    _resourceName = resourceName; 
    _isInstance = true; 

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->setChildInstance();

    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void Node::setChildInstance()
{
    _isInstanceChild = true;

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->setChildInstance();
}
//-----------------------------------------------------------------------------
void Node::instanciate()
{
    _isInstance = false; 
    _isInstanceChild = false;

    for(int32 ii=0; ii < _children.size(); ii++)
    {
        if(_children[ii]->isInstanceChild())
        {
            _children[ii]->setStatic(_isStatic);
            _children[ii]->setLightmapResolutionRatio(_lightmapResRatio);
            _children[ii]->instanciate();
        }
    }
}
//-----------------------------------------------------------------------------
void Node::prepareForPacking()
{
    if(_isInstance)
        instanciate();

    for(int32 ii=0; ii < _children.size(); ii++)
    {
        Node & node = *_children[ii];

        node.prepareForPacking();

        Core::Matrix4f worldMatrix(node._localToWorldMatrix);
        node.setParentMatrix(Core::Matrix4f::IDENTITY_MATRIX, Core::Matrix4f::IDENTITY_MATRIX);
        node.setLocalMatrix(worldMatrix);
        node.updateWorldMatrix();
        node._isRoot = true;
    }

    _children.clear();
    internalPrepareForPacking();
}
//-----------------------------------------------------------------------------
void Node::internalPrepareForPacking()
{
    if(_tags.empty())
        kill();
}
//-----------------------------------------------------------------------------
Core::Vector3f Node::getLocalDirection() const
{
    return _localMatrix.getZVector().getNormalizedSafe();
}
//-----------------------------------------------------------------------------
Core::Vector3f Node::getWorldPosition() const
{
    return _localToWorldMatrix.getTVector();
}
//-----------------------------------------------------------------------------
Core::Vector3f Node::getWorldDirection() const
{
    return _localToWorldMatrix.getZVector().getNormalizedSafe();
}
//-----------------------------------------------------------------------------
void Node::beginMatrixUpdate()
{
    LM_ASSERT(!_isInMatrixUpdate);
    _isInMatrixUpdate = true;
}
//-----------------------------------------------------------------------------
void Node::setLocalPosition(const Core::Vector3f & position)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    _localPosition = position;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::setLocalScale(const Core::Vector3f & scale)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    _localScale = scale;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::setLocalOrientation(const Core::Quaternionf & orient)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    _localOrient = orient;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::setLocalDirection(const Core::Vector3f & zVec)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    static const float EPSILON = 0.1f;

    Core::Vector3f  upVector(_upVector);

    if(upVector.isEquivalent(zVec, EPSILON))
        upVector = _localMatrix.getYVector();

    if(upVector.isEquivalent(zVec, EPSILON))
        upVector = Core::Vector3f(1.0, 0.0, 0.0);

    if(upVector.isEquivalent(zVec, EPSILON))
        upVector = Core::Vector3f(0.0, 1.0, 0.0);

    Core::Vector3f xVec(Core::cross(upVector, zVec).getNormalizedSafe());
    Core::Vector3f yVec(Core::cross(zVec, xVec).getNormalizedSafe());
    Core::Matrix3f mat(xVec._x, yVec._x, zVec._x,
                       xVec._y, yVec._y, zVec._y,
                       xVec._z, yVec._z, zVec._z);
    _localOrient = matrixToQuaternion(mat);

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::localLookAt(const Core::Vector3f & position)
{
    Core::Vector3f dir(position - _localPosition);
    if(dir.isZero())
        dir.set(0.0, 0.0, 1.0);
    else
        dir.normalizeFast();
    setLocalDirection(dir);
}
//-----------------------------------------------------------------------------
void Node::setWorldPosition(const Core::Vector3f & position)
{
    setLocalPosition(_parentWorldToLocalMatrix.apply(position));
}
//-----------------------------------------------------------------------------
void Node::setWorldDirection(const Core::Vector3f & direction)
{
    Core::Vector3f localDir(_parentWorldToLocalMatrix.apply3x3(direction));
    localDir.normalizeSafe();
    setLocalDirection(localDir);
}
//-----------------------------------------------------------------------------
void Node::worldLookAt(const Core::Vector3f & position)
{
    localLookAt(_parentWorldToLocalMatrix.apply(position));
}
//-----------------------------------------------------------------------------
void Node::localPitch(double radians)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    Core::Quaternionf q(Core::PITCH, float(radians));
    _localOrient = _localOrient * q;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::localYaw(double radians)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    Core::Quaternionf q(Core::YAW, float(radians));
    _localOrient = _localOrient * q;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::localRoll(double radians)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    Core::Quaternionf q(Core::ROLL, float(radians));
    _localOrient = _localOrient * q;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::worldPitch(double radians)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    Core::Quaternionf q(Core::PITCH, float(radians));
    _localOrient = q * _localOrient;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::worldYaw(double radians)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    Core::Quaternionf q(Core::YAW, float(radians));
    _localOrient = q * _localOrient;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::worldRoll(double radians)
{
    bool matUpdate = _isInMatrixUpdate;
    if(!matUpdate)
        beginMatrixUpdate();

    Core::Quaternionf q(Core::ROLL, float(radians));
    _localOrient = q * _localOrient;

    if(!matUpdate)
        endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void Node::endMatrixUpdate()
{
    LM_ASSERT(_isInMatrixUpdate);
    updateLocalMatrix();
    _isInMatrixUpdate = false;
}
//-----------------------------------------------------------------------------
void Node::setLocalMatrix(const Core::Matrix4f & matrix, bool decomposeTransform)
{
    LM_ASSERT(!_isInMatrixUpdate);

    if(decomposeTransform)
    {
        Core::Transformf transform(matrix);
        _localPosition = transform._translation;
        _localOrient = transform._rotation;
        _localScale = transform._scale;
    }
    
    _localMatrix = matrix;
    updateWorldMatrix();
}
//-----------------------------------------------------------------------------
void Node::update(double elapsed)
{}
//-----------------------------------------------------------------------------
void Node::setTimeMultiplier(float timeMul, bool recursive)
{
    _timeMultiplier = timeMul;

    if(recursive)
    {
        for(int32 iChild=0; iChild < _children.size(); iChild++)
            _children[iChild]->setTimeMultiplier(timeMul, true);
    }
}
//-----------------------------------------------------------------------------
float Node::getTimeMultiplier() const
{
    return _timeMultiplier;
}
//-----------------------------------------------------------------------------
const Core::AABoxf & Node::getBoundingBox() const
{
    return _box;
}
//-----------------------------------------------------------------------------
bool Node::isRoot() const
{
    return _isRoot;
}
//-----------------------------------------------------------------------------
void Node::addGhostChild(const Ptr<Node> & pNode)
{
    if(pNode->isAlive())
    {
        _ghostChildren.push_back(pNode);
        //pNode->_isRoot = false;
        pNode->setParentMatrix(_localToWorldMatrix, _worldToLocalMatrix);
        pNode->updateWorldMatrix();
    }
}
//-----------------------------------------------------------------------------
void Node::removeGhostChild(const Ptr<Node> & pNode, bool sameWorldPos)
{
    Core::List<Ptr<Node> >::iterator ipNode = std::find(_ghostChildren.begin(), _ghostChildren.end(), pNode);
    if(ipNode != _ghostChildren.end())
    {
        Node & node = **ipNode;
        if(sameWorldPos)
        {
            Core::Matrix4f worldMatrix(node._localToWorldMatrix);
            node.setParentMatrix(Core::Matrix4f::IDENTITY_MATRIX, Core::Matrix4f::IDENTITY_MATRIX);
            node.setLocalMatrix(worldMatrix);
        }
        else
        {
            node.setParentMatrix(Core::Matrix4f::IDENTITY_MATRIX, Core::Matrix4f::IDENTITY_MATRIX);
        }
        node.updateWorldMatrix();
        node._isRoot = true;
        _ghostChildren.erase(ipNode);
    }
}
//-----------------------------------------------------------------------------
void Node::addChild(const Ptr<Node> & pNode, bool sameWorldPos, int32 indexPos)
{
    if(pNode->isAlive())
    {
        if (indexPos==-1)
            _children.push_back(pNode);
        else
        {
            LM_ASSERT(indexPos<=_children.size());
        
            _children.insert(_children.begin()+indexPos, pNode);
        }

        if(sameWorldPos)
        {
            Core::Matrix4f newLocal = _worldToLocalMatrix * pNode->_localToWorldMatrix;
            pNode->setLocalMatrix(newLocal, true);
        }

        pNode->_isRoot = false;
        pNode->setParentMatrix(_localToWorldMatrix, _worldToLocalMatrix);
        pNode->updateWorldMatrix();
    }
}
//-----------------------------------------------------------------------------
void Node::removeChild(const Ptr<Node> & pNode, bool sameWorldPos)
{
    Core::List<Ptr<Node> >::iterator ipNode = std::find(_children.begin(), _children.end(), pNode);
    if(ipNode != _children.end())
    {
        Node & node = **ipNode;
        if(sameWorldPos)
        {
            Core::Matrix4f worldMatrix(node._localToWorldMatrix);
            node.setParentMatrix(Core::Matrix4f::IDENTITY_MATRIX, Core::Matrix4f::IDENTITY_MATRIX);
            node.setLocalMatrix(worldMatrix);
        }
        else
        {
            node.setParentMatrix(Core::Matrix4f::IDENTITY_MATRIX, Core::Matrix4f::IDENTITY_MATRIX);
        }
        node.updateWorldMatrix();
        node._isRoot = true;
        _children.erase(ipNode);
    }
}
//-----------------------------------------------------------------------------
void Node::updateLocalMatrix()
{
    _localMatrix.setTransform(_localPosition, _localOrient, _localScale);
    updateWorldMatrix();
}
//-----------------------------------------------------------------------------
void Node::updateWorldMatrix()
{
    _localToWorldMatrix = _parentLocalToWorldMatrix * _localMatrix;
    _worldToLocalMatrix = _localToWorldMatrix.getInverted();
    for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
    {
        (*ipChild)->setParentMatrix(_localToWorldMatrix, _worldToLocalMatrix);
        (*ipChild)->updateWorldMatrix();
    }
    for(Core::List<Ptr<Node> >::const_iterator ipChild = _ghostChildren.begin(); ipChild != _ghostChildren.end(); ++ipChild)
    {
        (*ipChild)->setParentMatrix(_localToWorldMatrix, _worldToLocalMatrix);
        (*ipChild)->updateWorldMatrix();
    }

    onWorldMatrixUpdate();
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void Node::internalUpdateParentMatrix(const Ptr<Node> & pNode)
{
    pNode->_isRoot = false;
    pNode->setParentMatrix(_localToWorldMatrix, _worldToLocalMatrix);
    pNode->updateWorldMatrix();
}
//-----------------------------------------------------------------------------
void Node::setParentMatrix(const Core::Matrix4f & worldMatrix,
                           const Core::Matrix4f & invWorldMatrix)
{
    _parentLocalToWorldMatrix = worldMatrix;
    _parentWorldToLocalMatrix = invWorldMatrix;
}
//-----------------------------------------------------------------------------
void Node::updateBoundingBox()
{
    if(_isInstance)
    {
        _box = internalGetBoundingBox();
        Universe::getInstanceHierarchyBox(_box, *this);
    }
    else
    {
        _box = internalGetBoundingBox();
    }

    _pBox->setBox(_box);
}
//-----------------------------------------------------------------------------
Core::AABoxf Node::internalGetBoundingBox() const
{
    return Core::AABoxf(getWorldPosition());
}
//-----------------------------------------------------------------------------
void Node::setVisible(bool flag, bool recursive)
{
    _isVisible = flag;
    _pBox->setVisible(isBoundingBoxVisible());
    onVisibilityChange(flag);

    if(recursive)
    {
        for(int32 iChild=0; iChild < _children.size(); iChild++)
            _children[iChild]->setVisible(flag, true);
    }
}
//-----------------------------------------------------------------------------
bool Node::isVisible() const
{
    return _isVisible;
}
//-----------------------------------------------------------------------------
void Node::onVisibilityChange(bool flag)
{
}
//-----------------------------------------------------------------------------
void Node::setBoundingBoxVisible(bool flag)
{
    _isBoxVisible = flag;
    _pBox->setVisible(_isVisible && _isBoxVisible);
}
//-----------------------------------------------------------------------------
bool Node::isBoundingBoxVisible() const
{
    return _isVisible && _isBoxVisible;
}
//-----------------------------------------------------------------------------
void Node::onWorldMatrixUpdate()
{
    // Rien à faire pour les Node, la méthode virtuelle est juste là pour
    // les classes enfants.
}
//-----------------------------------------------------------------------------
void Node::importXML(const Ptr<Core::XMLNode> & pNode)
{
    bool success = true;
    
    String strPosition;
    String strScale;
    String strOrient;
    String strUpVector;
    String strName;

    success &= pNode->getValue(L"local-position", strPosition);
    success &= pNode->getValue(L"local-scale",    strScale);
    success &= pNode->getValue(L"local-orient",   strOrient);
    success &= pNode->getValue(L"up-vector",      strUpVector);
               pNode->getValue(L"node-name",      strName);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML Node values.");

    Core::fromXMLString(strPosition, _localPosition);
    Core::fromXMLString(strScale,    _localScale);
    Core::fromXMLString(strOrient,   _localOrient);
    Core::fromXMLString(strUpVector, _upVector);
    Core::fromXMLString(strName,     _name);


    updateLocalMatrix();

    Core::List<String> tags;
    pNode->getValues(L"tags.tag", tags);

    _tags.clear();
    _tags.insert(tags.begin(), tags.end());

    String script;
    pNode->getValue(L"script", script);
    _script = String8(script);
}
//-----------------------------------------------------------------------------
void Node::importInstanceXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    pNode->getValue(L"resource-name", _resourceName);
    pNode->getValue(L"is-static", _isStatic);
    pNode->getValue(L"lm-resolution-ratio", _lightmapResRatio);
}
//-----------------------------------------------------------------------------
void Node::exportInstanceXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"resource-name", _resourceName);
    pNode->setValue(L"is-static", _isStatic);
    pNode->setValue(L"lm-resolution-ratio", _lightmapResRatio);
}
//-----------------------------------------------------------------------------
void Node::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    pNode->setValue(L"local-position", Core::toXMLString(_localPosition));
    pNode->setValue(L"local-scale",    Core::toXMLString(_localScale));
    pNode->setValue(L"local-orient",   Core::toXMLString(_localOrient));
    pNode->setValue(L"up-vector",      Core::toXMLString(_upVector));
    pNode->setValue(L"node-name",      _name);

    Core::List<String> tags(_tags.begin(), _tags.end());
    pNode->setValues(L"tags.tag", tags);

    pNode->setValue(L"script", String(_script));
}
//-----------------------------------------------------------------------------
void Node::addTag(const String & tag)
{
    _tags.insert(tag);
}
//-----------------------------------------------------------------------------
void Node::removeTag(const String & tag)
{
    _tags.erase(tag);
}
//-----------------------------------------------------------------------------
bool Node::hasTag(const String & tag) const
{
    return _tags.count(tag) >= 1;
}
//-----------------------------------------------------------------------------
const Core::Set<String>& Node::getTags() const
{
    return _tags;
}
//-----------------------------------------------------------------------------
void Node::setTags(const Core::Set<String>& tags)
{
    _tags = tags;
}
//-----------------------------------------------------------------------------
void Node::setNodeScript(const String8 & source)
{
    _script = source;
}
//-----------------------------------------------------------------------------
const String8 & Node::getNodeScript() const
{
    return _script;
}
//-----------------------------------------------------------------------------
void Node::getChildrenWithTag(const String & tag, Core::List<Ptr<Node> > & nodes, bool recursive) const
{
    for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
    {
        if((*ipChild)->hasTag(tag))
            nodes.push_back(*ipChild);
        if(recursive)
            (*ipChild)->getChildrenWithTag(tag, nodes, true);
    }
}
//-----------------------------------------------------------------------------
void Node::getChildrenOfType(ENodeType type, Core::List<Ptr<Node> > & nodes, bool recursive) const
{
   for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
    {
        if((*ipChild)->getNodeType() == type)
            nodes.push_back(*ipChild);
        if(recursive)
            (*ipChild)->getChildrenOfType(type, nodes, true);
    }
}

//-----------------------------------------------------------------------------
Ptr<Node> Node::getChildWithTag(const String & tag, bool recursive) const
{
    Ptr<Node> result;
    int32 iChild = 0;

    while(result == null && iChild < _children.size())
    {
        if(_children[iChild]->hasTag(tag))
            result = _children[iChild];
        else if(recursive)
            result = _children[iChild]->getChildWithTag(tag, true);
        iChild++;
    }
    
    return result;
}
//-----------------------------------------------------------------------------
void Node::prepareForRendering(const Ptr<NodeCamera> & pCamera)
{
    if (_isFixedSize==true)
    {
        setLocalScale(Core::Vector3f(1.f));
        update(0.0);

        if (pCamera->getBaseCamera()->getCameraType() == Renderer::CAMERA_PERSPECTIVE)
        {
            const float angle = pCamera->getFOV() * _fixedSizeWidthFct;

            Core::Vector3f posNodeInWorldAxis = getWorldPosition();
            Core::Vector3f posNodeInCamAxis   = pCamera->getWorldToLocalMatrix().apply(posNodeInWorldAxis);
            
            //calcul du plan parallele a la camera et qui passe par le centre du node 
            Core::Planef nodePlaneInCamAxis (Core::Plane_NormalPoint, posNodeInCamAxis.getNormalizedSafe(), posNodeInCamAxis);
        
            Core::Quaternionf rotation (Core::YAW, angle);
            Core::Rayf ray(Core::Ray_OriginDirection, Core::Vector3f(Core::ZERO), (rotation * nodePlaneInCamAxis.normal));
            float t = - (dot(ray.origin, -nodePlaneInCamAxis.normal) + nodePlaneInCamAxis.distance) / dot(ray.direction, -nodePlaneInCamAxis.normal);
            Core::Vector3f p = ray.origin + t * ray.direction;

            //calcul du rayon souhaite
            float  goodRadius  = (p - posNodeInCamAxis).getLengthSafe();
           
            //calcul du rayon entre le pivot du node et sa AaBox
            float nodeRadius;

            const Core::AABoxf & nodeBox   =  getBoundingBox();
            Core::Vector3f nodeRadiusPtMin = (nodeBox.min - posNodeInWorldAxis);
            Core::Vector3f nodeRadiusPtMax = (nodeBox.max - posNodeInWorldAxis);

            nodeRadius = Core::max(nodeRadiusPtMin.getLengthSafe(), nodeRadiusPtMax.getLengthSafe());

            //on modifie l'echelle du node
            float factor = goodRadius / nodeRadius;
            setLocalScale(Core::Vector3f (factor));
        }
        else if(pCamera->getBaseCamera()->getCameraType() == Renderer::CAMERA_ORTHOGONAL)
        {
            Core::Vector3f posNodeInWorldAxis = getWorldPosition();

            const Ptr<Renderer::IOrthoCamera> camOtho = LM_DEBUG_PTR_CAST<Renderer::IOrthoCamera> (pCamera->getBaseCamera());

            float width = camOtho->getRightPlane() - camOtho->getLeftPlane();

            //calcul du rayon souhaite
            float  goodRadius = _fixedSizeWidthFct*width;
           
            //calcul du rayon entre le pivot du node et sa AaBox
            float nodeRadius;

            const Core::AABoxf & nodeBox   =  getBoundingBox();
            Core::Vector3f nodeRadiusPtMin = (nodeBox.min - posNodeInWorldAxis);
            Core::Vector3f nodeRadiusPtMax = (nodeBox.max - posNodeInWorldAxis);

            nodeRadius = Core::max(nodeRadiusPtMin.getLengthSafe(), nodeRadiusPtMax.getLengthSafe());

            //on modifie l'echelle du node
            float factor = goodRadius / nodeRadius;
            setLocalScale(Core::Vector3f (factor));
        }
    }
}
//-----------------------------------------------------------------------------
bool Node::isIntersectingBoundingBox(const Core::Rayf& ray, float& distance) const
{
    const Core::AABoxf& box = getBoundingBox();

    return intersect_RayAABox(ray, box, distance);
}
//-----------------------------------------------------------------------------
bool Node::isRayOccluded(const Core::Rayf& ray) const
{
    return false;
}
//-----------------------------------------------------------------------------
bool Node::isIntersecting(const Core::Rayf& ray, float& distance)
{
    if(_isInstance)
    {
        if(isIntersectingBoundingBox(ray, distance))
        {
            float d= FLT_MAX;
            bool result = false;
            Core::List<Node *> instNodes;
            Universe::getFlattenInstances(instNodes, this);

            for(int32 ii=0; ii < instNodes.size(); ii++)
            {
                if(instNodes[ii]->internalIsIntersecting(ray, d))
                {
                    result = true;
                    distance = std::min(distance, d);
                }
            }

            return result;
        }
        else
        {
            return false;
        }
    }
    else
        return internalIsIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
bool Node::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{
    return false;
}
//-----------------------------------------------------------------------------
void Node::restart(bool recursive)
{
    if(recursive)
    {
        for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
        {
            (*ipChild)->restart(recursive);
        }
    }
}
//-----------------------------------------------------------------------------
void Node::stop(bool recursive, float inTime)
{
    if(recursive)
    {
        for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
        {
            (*ipChild)->stop(recursive, inTime);
        }
    }
}
//-----------------------------------------------------------------------------
void Node::setNodeColor(const Core::Vector4f & color, bool recursive)
{
    if(recursive)
    {
        for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
        {
            (*ipChild)->setNodeColor(color, true);
        }
    }
}
//-----------------------------------------------------------------------------
const Core::Vector4f & Node::getNodeColor() const
{
    return Core::Vector4f::ONE;
}
//-----------------------------------------------------------------------------
void Node::setNodeGlow(const Core::Vector4f & color, bool recursive)
{
    if(recursive)
    {
        for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
        {
            (*ipChild)->setNodeGlow(color, true);
        }
    }
}
//-----------------------------------------------------------------------------
void Node::setMaterial(const Assets::Material & material, bool recursive)
{
    if(recursive)
    {
        for(Core::List<Ptr<Node> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
        {
            (*ipChild)->setMaterial(material, true);
        }
    }
}
//-----------------------------------------------------------------------------
void Node::getMaterial(Assets::Material & material)
{
}
//-----------------------------------------------------------------------------
const Core::Vector4f & Node::getNodeGlow() const
{
    return Core::Vector4f::ZERO;
}
//-----------------------------------------------------------------------------
void intersect_RayNodeRecursive_travel(const Ptr<Node>& pNode, const Core::Rayf& ray, float& distance, Ptr<Node>& pNodeIntersecting)
{
    if (pNode->getChildCount()==0)
    {
        if (pNode->isIntersecting(ray, distance)==true)
        {
            pNodeIntersecting = pNode;
        }
    }
    else
    {
        for (int32 iChild=0; iChild<pNode->getChildCount(); ++iChild)
        {
            intersect_RayNodeRecursive_travel(pNode->getChild(iChild),ray, distance, pNodeIntersecting);

            if (pNodeIntersecting!=null)
                break;
        }
    }
}

LM_API_UNI void intersect_RayNodeRecursive(const Ptr<Node>& pNode, const Core::Rayf& ray, float& distance, Ptr<Node>& pNodeIntersecting)
{
    intersect_RayNodeRecursive_travel(pNode, ray, distance, pNodeIntersecting);
}
//-----------------------------------------------------------------------------
}
