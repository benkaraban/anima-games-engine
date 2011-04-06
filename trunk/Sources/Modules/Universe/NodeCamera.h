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
#ifndef NODECAMERA_H_
#define NODECAMERA_H_

#include <Universe/Node.h>
#include <Renderer/ICamera.h>

namespace Universe
{
//-----------------------------------------------------------------------------
class LM_API_UNI NodeCamera : public Node
{
public:
    NodeCamera(const Ptr<Renderer::IAABox> & pBox, World * pWorld) : Node(pBox, pWorld)
    {};

    virtual ENodeType getNodeType() const { return NODE_CAMERA; };
    virtual Ptr<Renderer::ICamera> getBaseCamera() const = 0;

    virtual void onWorldMatrixUpdate() = 0;

    virtual void setFOV(float angle) = 0;
    virtual float getFOV() const = 0;

    virtual void setNearPlane(float near) = 0;
    virtual float getNearPlane() const = 0;

    virtual void setFarPlane(float far) = 0;
    virtual float getFarPlane() const = 0;

    virtual void setAspectRatio(float ratio) = 0;
    virtual float getAspectRatio() const = 0;

    virtual void internalPrepareForPacking();
};
//-----------------------------------------------------------------------------
//camera perspective et orthogonale
class LM_API_UNI NodeCameraPO : public NodeCamera
{
public:
    NodeCameraPO(const Ptr<Renderer::IAABox> & pBox,
               const Ptr<Renderer::IPerspectiveCamera> & pPerspectiveCamera,
               const Ptr<Renderer::IOrthoCamera> & pOrthoCamera, 
               World * pWorld);

    virtual ~NodeCameraPO();

    Renderer::ECameraType getActiveCameraType() const;
    void setActiveCameraType(const Renderer::ECameraType& type);

    virtual ENodeType getNodeType() const { return NODE_CAMERA; };
    Ptr<Renderer::ICamera> getBaseCamera() const;

    virtual void setAspectRatio(float ratio);
    virtual float getAspectRatio() const;
    virtual void setFOV(float angle);
    virtual float getFOV() const;
    virtual void setNearPlane(float near);
    virtual float getNearPlane() const;
    virtual void setFarPlane(float far);
    virtual float getFarPlane() const;

    virtual void onWorldMatrixUpdate();

protected:
    Ptr<Renderer::IPerspectiveCamera> _pPerspectiveCamera;
    Ptr<Renderer::IOrthoCamera>       _pOrthoCamera;
    Renderer::ECameraType             _activeCameraType;
    float _ratio;
    float _fov;
};
//-----------------------------------------------------------------------------
class LM_API_UNI NodePerspectiveCamera : public NodeCamera
{
public:
    NodePerspectiveCamera(const Ptr<Renderer::IAABox> & pBox,
                          const Ptr<Renderer::IPerspectiveCamera> & pCamera,
                          World * pWorld);

    const Ptr<Renderer::IPerspectiveCamera> & getCamera() const { return _pCamera; };

    virtual Ptr<Renderer::ICamera> getBaseCamera() const { return _pCamera; };
    virtual void onWorldMatrixUpdate();
    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    virtual void setAspectRatio(float ratio);
    virtual float getAspectRatio() const;
    virtual void setFOV(float angle);
    virtual float getFOV() const;
    virtual void setNearPlane(float near);
    virtual float getNearPlane() const;
    virtual void setFarPlane(float far);
    virtual float getFarPlane() const;

protected:
    Ptr<Renderer::IPerspectiveCamera> _pCamera;
};
//-----------------------------------------------------------------------------
class LM_API_UNI NodeOrthogonalCamera : public NodeCamera
{
public:
    NodeOrthogonalCamera(const Ptr<Renderer::IAABox> & pBox,
                          const Ptr<Renderer::IOrthoCamera> & pCamera,
                          World * pWorld);

    const Ptr<Renderer::IOrthoCamera> & getCamera() const { return _pCamera; };

    virtual Ptr<Renderer::ICamera> getBaseCamera() const { return _pCamera; };
    virtual void onWorldMatrixUpdate();
    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    virtual void setAspectRatio(float ratio);
    virtual void setFOV(float angle);
    virtual float getFOV() const;
    float getWorkdistance();
    void setWorkdistance(float distance);

    virtual float getAspectRatio() const;
    virtual void setNearPlane(float near);
    virtual float getNearPlane() const;
    virtual void setFarPlane(float far);
    virtual float getFarPlane() const;

protected:
    Ptr<Renderer::IOrthoCamera> _pCamera;
};
//-----------------------------------------------------------------------------
}

#endif /* NODECAMERA_H_ */
