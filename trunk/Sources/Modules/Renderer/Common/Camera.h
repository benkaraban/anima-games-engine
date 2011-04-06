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
#ifndef RENDERER_CAMERA_H_
#define RENDERER_CAMERA_H_

#include <Renderer/ICamera.h>

namespace Renderer
{
class LM_API_RDR PerspectiveCamera : public Core::Object, public IPerspectiveCamera
{
public:
    PerspectiveCamera();
    PerspectiveCamera(float fov, float ratio, float near, float far);
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const { matrix = _worldMatrix; };

    virtual void getViewMatrix(Core::Matrix4f & matrix) const;
    virtual void getProjectionMatrix(Core::Matrix4f & matrix) const;
    virtual void getProjectionMatrixSplit(Core::Matrix4f & matrix, float near, float far) const;

    virtual void setPostFX(const PostFX & fx) { _fx = fx; };
    virtual const PostFX & getPostFX() const { return _fx; };

    virtual void setFOV(float angle) { _fov = angle; };
    virtual void setAspectRatio(float ratio) { _ratio = ratio; };
    virtual void setNearPlane(float near) { _near = near; };
    virtual void setFarPlane(float far) { _far = far; };

    virtual float getFOV() const { return _fov; };
    virtual float getAspectRatio() const { return _ratio; };
    virtual float getNearPlane() const { return _near; };
    virtual float getFarPlane() const { return _far; };

    virtual Ptr<IViewVolume> createViewVolume() const;

protected:
    PostFX         _fx;
    Core::Matrix4f _worldMatrix;
    Core::Matrix4f _viewMatrix;
    float   _fov;
    float   _ratio;
    float   _near;
    float   _far;
};

class LM_API_RDR OrthoCamera : public Core::Object, public IOrthoCamera
{
public:
    OrthoCamera();
    OrthoCamera(float left, float right, float down, float top, float near, float far);
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const { matrix = _worldMatrix; };

    virtual void setPostFX(const PostFX & fx) { _fx = fx; };
    virtual const PostFX & getPostFX() const { return _fx; };

    virtual void getViewMatrix(Core::Matrix4f & matrix) const;
    virtual void getProjectionMatrix(Core::Matrix4f & matrix) const;
    virtual void getProjectionMatrixSplit(Core::Matrix4f & matrix, float near, float far) const;

    virtual float getWorkdistance() {return _workDistance;}
    virtual void setWorkdistance(float distance);
    virtual void setNearPlane(float near) { _near = near; };
    virtual void setFarPlane(float far) { _far = far; };
    virtual void setOrthoView(float left, float right, float down, float top)
    {
        _left = left;
        _right = right;
        _down = down;
        _top = top;
        _near = -1.0;
        _far = 1.0;
    };

    virtual void setAspectRatio(float ratio);
    virtual float getAspectRatio() const {return _ratio;};

    virtual float getNearPlane() const { return _near; };
    virtual float getFarPlane() const { return _far; };

    virtual float getLeftPlane() const { return _left; }
    virtual float getRightPlane() const{ return _right; }
    virtual float getTopPlane() const{ return _top; }
    virtual float getDownPlane() const{ return _down; }

    /**
     * Modifie le Field Of View de la caméra.
     * La camera ortho ne dispose reel d'un field of view. par contre la largeur de la camera 
     * est détermine par le fov et le near plan far plan
     */
    virtual void setFOV(float angle);
    virtual float getFOV() const;

    virtual Ptr<IViewVolume> createViewVolume() const;

protected:
    PostFX         _fx;
    Core::Matrix4f _worldMatrix;
    Core::Matrix4f _viewMatrix;
    float   _left;
    float   _right;
    float   _down;
    float   _top;
    float   _near;
    float   _far;
    float   _workDistance;
    float   _ratio;
};
}
#endif RENDERER_CAMERA_H_