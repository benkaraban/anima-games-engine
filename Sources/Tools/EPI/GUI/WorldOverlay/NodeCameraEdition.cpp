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
#include "NodeCameraEdition.h"


namespace EPI
{
const String TAG_CAM_TOP = L"CamTop";
const String TAG_CAM_BOTTOM = L"CamBottom";
const String TAG_CAM_BACK = L"CamBack";
const String TAG_CAM_FRONT = L"CamFront";
const String TAG_CAM_RIGHT = L"CamRight";
const String TAG_CAM_LEFT = L"CamLeft";
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NodeCameraEdition::NodeCameraEdition(
        const Ptr<Universe::NodeOrthogonalCamera>& pCamTop,   const Ptr<Universe::NodeOrthogonalCamera>& pCamBottom, 
        const Ptr<Universe::NodeOrthogonalCamera>& pCamBack,  const Ptr<Universe::NodeOrthogonalCamera>& pCamFront, 
        const Ptr<Universe::NodeOrthogonalCamera>& pCamRight, const Ptr<Universe::NodeOrthogonalCamera>& pCamLeft,
        const Ptr<Renderer::IAABox> & pBox,
        Universe::World * pWorld) :
    NodeCamera(pBox, pWorld),
    _activeView(CAM_EDIT_TOP),
    _workDistance(50)
{
    pCamTop->addTag(TAG_CAM_TOP);           addChild(pCamTop);
    pCamBottom->addTag(TAG_CAM_BOTTOM);     addChild(pCamBottom);
    pCamBack->addTag(TAG_CAM_BACK);         addChild(pCamBack);
    pCamFront->addTag(TAG_CAM_FRONT);       addChild(pCamFront);
    pCamRight->addTag(TAG_CAM_RIGHT);       addChild(pCamRight);
    pCamLeft->addTag(TAG_CAM_LEFT);         addChild(pCamLeft);

    const float deltaPosition = 500.f;

/*
        void localPitch(double radians); // Axe X comme pivot
    void localYaw(double radians);   // Axe Y comme pivot
    void localRoll(double radians);  // Axe Z comme pivot

    */

    _fov = pCamTop->getFOV();
    //Top - Bottom
    pCamTop->beginMatrixUpdate();
        pCamTop->setLocalPosition(Core::Vector3f(0.0, deltaPosition, 0.0));
        pCamTop->localPitch(Core::deg2rad(90.f));
    pCamTop->endMatrixUpdate();

    pCamBottom->beginMatrixUpdate();
        pCamBottom->setLocalPosition(Core::Vector3f(0.0, -deltaPosition, 0.0));
        pCamBottom->localPitch(Core::deg2rad(-90.f));
    pCamBottom->endMatrixUpdate();

    //Back - Front
    pCamBack->beginMatrixUpdate();
        pCamBack->setLocalPosition(Core::Vector3f(0.0, 0.0, deltaPosition));
        pCamBack->localYaw(Core::deg2rad(180.f));
    pCamBack->endMatrixUpdate();

    pCamFront->beginMatrixUpdate();
        pCamFront->setLocalPosition(Core::Vector3f(0.0, 0.0, -deltaPosition));
    pCamFront->endMatrixUpdate();

    //Right - Left
    pCamRight->beginMatrixUpdate();
        pCamRight->setLocalPosition(Core::Vector3f(deltaPosition, 0.0, 0.0));
        pCamRight->localYaw(Core::deg2rad(-90.f));
    pCamRight->endMatrixUpdate();

    pCamLeft->beginMatrixUpdate();
        pCamLeft->setLocalPosition(Core::Vector3f(-deltaPosition, 0.0, 0.0));
        pCamLeft->localYaw(Core::deg2rad(90.f));
    pCamLeft->endMatrixUpdate();

    setWorkdistance(_workDistance);
}
//-----------------------------------------------------------------------------
NodeCameraEdition::~NodeCameraEdition()
{
}
//-----------------------------------------------------------------------------
void NodeCameraEdition::setFOV(float angle)
{
    _fov = angle;

    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_TOP))->setFOV(_fov);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_BOTTOM))->setFOV(_fov);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_BACK))->setFOV(_fov);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_FRONT))->setFOV(_fov);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_RIGHT))->setFOV(_fov);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_LEFT))->setFOV(_fov);
}
//-----------------------------------------------------------------------------
float NodeCameraEdition::getFOV() const
{
    return _fov;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> NodeCameraEdition::getActiveViewCamera() const
{
    return getCameraView(_activeView);
}
//-----------------------------------------------------------------------------
Ptr<Renderer::ICamera> NodeCameraEdition::getBaseCamera() const
{
    return getActiveViewCamera()->getBaseCamera();
}
//-----------------------------------------------------------------------------
void NodeCameraEdition::setAspectRatio(float ratio)
{
    getActiveViewCamera()->setAspectRatio(ratio);
}
//-----------------------------------------------------------------------------
float NodeCameraEdition::getAspectRatio() const
{
    return 1;
}
//-----------------------------------------------------------------------------
void NodeCameraEdition::setNearPlane(float near) 
{
}
//-----------------------------------------------------------------------------
float NodeCameraEdition::getNearPlane() const
{
    return 0;
}
//-----------------------------------------------------------------------------
void NodeCameraEdition::setFarPlane(float far)
{
}
//-----------------------------------------------------------------------------
float NodeCameraEdition::getFarPlane() const
{
    return 0;
}
//-----------------------------------------------------------------------------
void NodeCameraEdition::onWorldMatrixUpdate()
{
    getActiveViewCamera()->onWorldMatrixUpdate();
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera>  NodeCameraEdition::getCameraView(ECameraEditionView view) const
{
    Ptr<Universe::NodeCamera> pRet;

    Ptr<Node> pNode;
    switch(view)
    {
        case CAM_EDIT_TOP:
            pNode = getChildWithTag(TAG_CAM_TOP);
        break;

        case CAM_EDIT_BOTTOM:
            pNode = getChildWithTag(TAG_CAM_BOTTOM);
        break;

        case CAM_EDIT_BACK:
            pNode = getChildWithTag(TAG_CAM_BACK);
        break;

        case CAM_EDIT_FRONT:
            pNode = getChildWithTag(TAG_CAM_FRONT);
        break;

        case CAM_EDIT_RIGHT:
            pNode = getChildWithTag(TAG_CAM_RIGHT);
        break;

        case CAM_EDIT_LEFT:
            pNode = getChildWithTag(TAG_CAM_LEFT);
        break;
    }

    pRet = LM_DEBUG_PTR_CAST<Universe::NodeCamera>(pNode);

    return pRet;
}
//-----------------------------------------------------------------------------
void NodeCameraEdition::setActiveView(ECameraEditionView view)
{
    _activeView = view;
}
//-----------------------------------------------------------------------------
ECameraEditionView NodeCameraEdition::getActiveView() const
{
    return _activeView;
}
//-----------------------------------------------------------------------------
float NodeCameraEdition::getWorkdistance() const
{
    return _workDistance;
}
//-----------------------------------------------------------------------------
void NodeCameraEdition::setWorkdistance(float distance)
{
    _workDistance = distance;

    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_TOP))->setWorkdistance(_workDistance);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_BOTTOM))->setWorkdistance(_workDistance);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_BACK))->setWorkdistance(_workDistance);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_FRONT))->setWorkdistance(_workDistance);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_RIGHT))->setWorkdistance(_workDistance);
    LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_LEFT))->setWorkdistance(_workDistance);

    _workDistance = LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(getCameraView(CAM_EDIT_TOP))->getWorkdistance();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

             