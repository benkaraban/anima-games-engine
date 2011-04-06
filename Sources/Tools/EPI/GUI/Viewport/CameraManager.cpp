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
#include "CameraManager.h"
#include <EPI/GUI/WorldOverlay/NodeCameraEdition.h>

#include <EPI/GUI/WorldOverlay/WorldOverlay.h>

namespace EPI
{
class CameraManager::implementation
{
public:
    implementation(const Ptr<WorldOverlay>& pWorld, const Core::Orientationf& defaultCameraUserOrientation) {_pWorld = pWorld;_defaultCameraUserOrientation=defaultCameraUserOrientation;}

    ~implementation() {}

    const Ptr<WorldOverlay>&    getWorld() const    {return _pWorld;}
    Ptr<WorldOverlay>&          getWorld()          {return _pWorld;}

    const Core::Orientationf&   getDefaultCameraUserOrientation() const {return _defaultCameraUserOrientation;}
    Core::Orientationf&         getDefaultCameraUserOrientation()       {return _defaultCameraUserOrientation;}

private:
    Ptr<WorldOverlay>       _pWorld;
    Core::Orientationf      _defaultCameraUserOrientation;
};



const String TAG_NODE_DEFAULT_CAMERA_EDITION = L"CAMERA_DEFAULT_EDITION";
const String TAG_NODE_DEFAULT_CAMERA_USER = L"CAMERA_DEFAULT_USER";
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CameraManager::CameraManager(const Ptr<WorldOverlay>& pWorld, const Core::Orientationf& defaultCameraUserOrientation) :
    _impl (new implementation(pWorld, defaultCameraUserOrientation))
{
    createCameraEdition(TAG_NODE_DEFAULT_CAMERA_EDITION);
    createCameraUser(TAG_NODE_DEFAULT_CAMERA_USER, L"User");
}
//-----------------------------------------------------------------------------
CameraManager::~CameraManager()
{
    //_nodes.clear();
    delete _impl;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> CameraManager::createCameraEdition(const String& tagCamera)
{
    Ptr<Universe::NodeCamera> pNode = _impl->getWorld()->createCameraEdition();
    pNode->addTag(tagCamera);
   // _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera>   CameraManager::createCameraUser(const String& tagCamera, const String& name)
{
    Ptr<Universe::NodeCamera> pNode = _impl->getWorld()->createPerspectiveCamera(f_PI_DIV_4, 1.0f, 0.1f, 1000.0f);
    pNode->addTag(tagCamera);
    pNode->setName(name);

    pNode->localPitch(_impl->getDefaultCameraUserOrientation()._pitch);
    pNode->localYaw(_impl->getDefaultCameraUserOrientation()._yaw);
    pNode->localRoll(_impl->getDefaultCameraUserOrientation()._roll);
    //pNode->setLocalDirection(_defaultCameraUserDirection);

    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> CameraManager::getDefaultCameraEdition() const
{
    Ptr<Universe::NodeCamera> pRet = LM_DEBUG_PTR_CAST<Universe::NodeCamera> (_impl->getWorld()->getFirstNodeWithTag(TAG_NODE_DEFAULT_CAMERA_EDITION));
    return pRet;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> CameraManager::getDefaultCameraUser() const
{
    Ptr<Universe::NodeCamera> pRet = LM_DEBUG_PTR_CAST<Universe::NodeCamera> (_impl->getWorld()->getFirstNodeWithTag(TAG_NODE_DEFAULT_CAMERA_USER));
    return pRet;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace EPI

