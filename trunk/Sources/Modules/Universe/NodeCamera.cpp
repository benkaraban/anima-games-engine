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
#include <Universe/NodeCamera.h>

namespace Universe
{
//-----------------------------------------------------------------------------
void NodeCamera::internalPrepareForPacking()
{
    if(_tags.empty())
        kill();
}
//-----------------------------------------------------------------------------
NodeCameraPO::NodeCameraPO(const Ptr<Renderer::IAABox> & pBox,
                       const Ptr<Renderer::IPerspectiveCamera> & pPerspectiveCamera,
                       const Ptr<Renderer::IOrthoCamera> & pOrthoCamera, 
                       World * pWorld) :
    NodeCamera(pBox, pWorld),
    _pPerspectiveCamera(pPerspectiveCamera),
    _pOrthoCamera(pOrthoCamera),
    _activeCameraType(Renderer::CAMERA_PERSPECTIVE)
{
}
//-----------------------------------------------------------------------------
NodeCameraPO::~NodeCameraPO()
{
}
//-----------------------------------------------------------------------------
void NodeCameraPO::onWorldMatrixUpdate()
{
    _pPerspectiveCamera->setWorldMatrix(_localToWorldMatrix);
    _pOrthoCamera->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
Renderer::ECameraType NodeCameraPO::getActiveCameraType() const
{
    return _activeCameraType;
}
//-----------------------------------------------------------------------------
void NodeCameraPO::setActiveCameraType(const Renderer::ECameraType& type)
{
    _activeCameraType = type;
}
//-----------------------------------------------------------------------------
Ptr<Renderer::ICamera> NodeCameraPO::getBaseCamera() const
{
    if (_activeCameraType == Renderer::CAMERA_PERSPECTIVE)  return _pPerspectiveCamera;
    else                                                    return _pOrthoCamera;
}
//-----------------------------------------------------------------------------
void NodeCameraPO::setAspectRatio(float ratio)
{
    _pPerspectiveCamera->setAspectRatio(ratio);
}
//-----------------------------------------------------------------------------
void NodeCameraPO::setFOV(float angle)
{
    _pPerspectiveCamera->setFOV(angle);
}
//-----------------------------------------------------------------------------
float NodeCameraPO::getFOV() const
{
    return _pPerspectiveCamera->getFOV();
}
//-----------------------------------------------------------------------------
float NodeCameraPO::getAspectRatio() const
{
    return _pPerspectiveCamera->getAspectRatio();
}
//-----------------------------------------------------------------------------
void NodeCameraPO::setNearPlane(float near)
{
    _pPerspectiveCamera->setNearPlane(near);
}
//-----------------------------------------------------------------------------
float NodeCameraPO::getNearPlane() const
{
    return _pPerspectiveCamera->getNearPlane();
}
//-----------------------------------------------------------------------------
void NodeCameraPO::setFarPlane(float far)
{
    _pPerspectiveCamera->setFarPlane(far);
}
//-----------------------------------------------------------------------------
float NodeCameraPO::getFarPlane() const
{
    return _pPerspectiveCamera->getFarPlane();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NodePerspectiveCamera::NodePerspectiveCamera(const Ptr<Renderer::IAABox> & pBox,
                                             const Ptr<Renderer::IPerspectiveCamera> & pCamera, 
                                             World * pWorld) :
    NodeCamera(pBox, pWorld),
    _pCamera(pCamera)
{
}
//-----------------------------------------------------------------------------
void NodePerspectiveCamera::onWorldMatrixUpdate()
{
    _pCamera->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
void NodePerspectiveCamera::setAspectRatio(float ratio)
{
    _pCamera->setAspectRatio(ratio);
}
//-----------------------------------------------------------------------------
void NodePerspectiveCamera::setFOV(float angle)
{
    _pCamera->setFOV(angle);
}
//-----------------------------------------------------------------------------
float NodePerspectiveCamera::getFOV() const
{
    return _pCamera->getFOV();
}
//-----------------------------------------------------------------------------
float NodePerspectiveCamera::getAspectRatio() const
{
    return _pCamera->getAspectRatio();
}
//-----------------------------------------------------------------------------
void NodePerspectiveCamera::setNearPlane(float near)
{
    _pCamera->setNearPlane(near);
}
//-----------------------------------------------------------------------------
float NodePerspectiveCamera::getNearPlane() const
{
    return _pCamera->getNearPlane();
}
//-----------------------------------------------------------------------------
void NodePerspectiveCamera::setFarPlane(float far)
{
    _pCamera->setFarPlane(far);
}
//-----------------------------------------------------------------------------
float NodePerspectiveCamera::getFarPlane() const
{
    return _pCamera->getFarPlane();
}
//-----------------------------------------------------------------------------
void NodePerspectiveCamera::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    bool success = true;
    float fov, ratio, near, far;

    success &= pNode->getValue(L"field-of-view", fov);
    success &= pNode->getValue(L"aspect-ratio", ratio);
    success &= pNode->getValue(L"near-plane", near);
    success &= pNode->getValue(L"far-plane", far);

    _pCamera->setFOV(fov);
    _pCamera->setAspectRatio(ratio);
    _pCamera->setNearPlane(near);
    _pCamera->setFarPlane(far);

    if(!success)
        throw Core::Exception(L"NodePerspectiveCamera::importXML missing attribute.");
}
//-----------------------------------------------------------------------------
void NodePerspectiveCamera::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"field-of-view", _pCamera->getFOV());
    pNode->setValue(L"aspect-ratio", _pCamera->getAspectRatio());
    pNode->setValue(L"near-plane", _pCamera->getNearPlane());
    pNode->setValue(L"far-plane", _pCamera->getFarPlane());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NodeOrthogonalCamera::NodeOrthogonalCamera(const Ptr<Renderer::IAABox> & pBox,
                                           const Ptr<Renderer::IOrthoCamera> & pCamera,
                                           World * pWorld) :
    NodeCamera(pBox, pWorld),
    _pCamera(pCamera)
{
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::onWorldMatrixUpdate()
{
    _pCamera->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::setAspectRatio(float ratio)
{
    _pCamera->setAspectRatio(ratio);
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::setFOV(float angle)
{
    _pCamera->setFOV(angle);
}
//-----------------------------------------------------------------------------
float NodeOrthogonalCamera::getFOV() const
{
    return _pCamera->getFOV();
}
//-----------------------------------------------------------------------------
float NodeOrthogonalCamera::getWorkdistance()
{
    return _pCamera->getWorkdistance();
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::setWorkdistance(float distance)
{
    _pCamera->setWorkdistance(distance);
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    bool success = true;
    float fov, ratio, near, far;

    success &= pNode->getValue(L"field-of-view", fov);
    success &= pNode->getValue(L"aspect-ratio", ratio);
    success &= pNode->getValue(L"near-plane", near);
    success &= pNode->getValue(L"far-plane", far);

    _pCamera->setFOV(fov);
    _pCamera->setAspectRatio(ratio);
    _pCamera->setNearPlane(near);
    _pCamera->setFarPlane(far);

    if(!success)
        throw Core::Exception(L"NodePerspectiveCamera::importXML missing attribute.");
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"field-of-view", _pCamera->getFOV());
    pNode->setValue(L"aspect-ratio", _pCamera->getAspectRatio());
    pNode->setValue(L"near-plane", _pCamera->getNearPlane());
    pNode->setValue(L"far-plane", _pCamera->getFarPlane());
}
//-----------------------------------------------------------------------------
float NodeOrthogonalCamera::getAspectRatio() const
{
    return 1;
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::setNearPlane(float near)
{
}
//-----------------------------------------------------------------------------
float NodeOrthogonalCamera::getNearPlane() const
{
    return 0;
}
//-----------------------------------------------------------------------------
void NodeOrthogonalCamera::setFarPlane(float far)
{
}
//-----------------------------------------------------------------------------
float NodeOrthogonalCamera::getFarPlane() const
{
    return 0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}
