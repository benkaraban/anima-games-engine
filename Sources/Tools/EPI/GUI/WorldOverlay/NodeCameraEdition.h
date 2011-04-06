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
#ifndef NODE_CAMERA_EDITION_H_
#define NODE_CAMERA_EDITION_H_

#include <Core/Standard.h>

#include <Universe/NodeCamera.h>
/*
namespace Universe
{
    class NodeOrthogonalCamera;
} // namespace Universe
*/

namespace EPI
{
//-----------------------------------------------------------------------------
LM_ENUM_6   (ECameraEditionView,
             CAM_EDIT_TOP,
             CAM_EDIT_BOTTOM,
             CAM_EDIT_BACK,
             CAM_EDIT_FRONT,
             CAM_EDIT_RIGHT,
             CAM_EDIT_LEFT)

class LM_API_EPI NodeCameraEdition : public Universe::NodeCamera
{
public:
    NodeCameraEdition(
        const Ptr<Universe::NodeOrthogonalCamera>& pCamTop,   const Ptr<Universe::NodeOrthogonalCamera>& pCamBottom, 
        const Ptr<Universe::NodeOrthogonalCamera>& pCamBack,  const Ptr<Universe::NodeOrthogonalCamera>& pCamFront, 
        const Ptr<Universe::NodeOrthogonalCamera>& pCamRight, const Ptr<Universe::NodeOrthogonalCamera>& pCamLeft,
        const Ptr<Renderer::IAABox> & pBox,
        Universe::World * pWorld);
    virtual ~NodeCameraEdition();

    virtual Ptr<Renderer::ICamera> getBaseCamera() const;
    virtual Ptr<Universe::NodeCamera>  getCameraView(ECameraEditionView view) const;

    virtual void setAspectRatio(float ratio);
    virtual float getAspectRatio() const;

    virtual void setNearPlane(float near);
    virtual float getNearPlane() const;

    virtual void setFarPlane(float far);
    virtual float getFarPlane() const;

    

    float getWorkdistance() const;
    void setWorkdistance(float distance);

    virtual void onWorldMatrixUpdate();

    void setActiveView(ECameraEditionView view);
    ECameraEditionView getActiveView() const;
    Ptr<Universe::NodeCamera> getActiveViewCamera() const;

    virtual void setFOV(float angle);
    virtual float getFOV() const;

private:
    ECameraEditionView _activeView;
    float _workDistance;
    float _fov;
};
//-----------------------------------------------------------------------------
} //namespace EPI

#endif //NODE_CAMERA_EDITION_H_