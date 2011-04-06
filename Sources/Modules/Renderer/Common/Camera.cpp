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

#include <Renderer/Common/Camera.h>
#include <Renderer/Common/ViewVolume.h>
#include <Core/Math/MathTools.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
PerspectiveCamera::PerspectiveCamera() :
    _worldMatrix(Core::IDENTITY),
    _viewMatrix(Core::IDENTITY),
    _fov(f_PI_DIV_4),
    _ratio(4.0f / 3.0f),
    _near(10),
    _far(100)
{}
//-----------------------------------------------------------------------------
PerspectiveCamera::PerspectiveCamera(float fov, float ratio, float near, float far) :
    _worldMatrix(Core::IDENTITY),
    _viewMatrix(Core::IDENTITY),
    _fov(fov),
    _ratio(ratio),
    _near(near),
    _far(far)
{}
//-----------------------------------------------------------------------------
void PerspectiveCamera::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    _viewMatrix = _worldMatrix.getInverted();
}
//-----------------------------------------------------------------------------
void PerspectiveCamera::getViewMatrix(Core::Matrix4f & matrix) const
{
    matrix = _viewMatrix;
}
//-----------------------------------------------------------------------------
void PerspectiveCamera::getProjectionMatrix(Core::Matrix4f & matrix) const
{
    matrix.setPerspectiveProj(_fov, _ratio, _near, _far);
}
//-----------------------------------------------------------------------------
void PerspectiveCamera::getProjectionMatrixSplit(Core::Matrix4f & matrix, float near, float far) const
{
    matrix.setPerspectiveProj(_fov, _ratio, near, far);
}
//-----------------------------------------------------------------------------
Ptr<IViewVolume> PerspectiveCamera::createViewVolume() const
{
    return Ptr<IViewVolume>(new PerspectiveViewVolume(*this));
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
OrthoCamera::OrthoCamera() :
    _worldMatrix(Core::IDENTITY),
    _viewMatrix(Core::IDENTITY),
    _left(-1.0), _right(1.0),
    _down(-1.0), _top(1.0),
    _near(10),
    _far(100)
{
    _workDistance = 5;
}
//-----------------------------------------------------------------------------
OrthoCamera::OrthoCamera(float left, float right, float down, float top, float near, float far) :
    _worldMatrix(Core::IDENTITY),
    _viewMatrix(Core::IDENTITY),
    _left(left), _right(right),
    _down(down), _top(top),
    _near(near),
    _far(far)
{
    _workDistance = 5;
}
//-----------------------------------------------------------------------------
void OrthoCamera::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    _viewMatrix = _worldMatrix.getInverted();
}
//-----------------------------------------------------------------------------
void OrthoCamera::getViewMatrix(Core::Matrix4f & matrix) const
{
    matrix = _viewMatrix;
}
//-----------------------------------------------------------------------------
void OrthoCamera::getProjectionMatrix(Core::Matrix4f & matrix) const
{
    matrix.setOrthoProj(_left, _right, _down, _top, _near, _far);
}
//-----------------------------------------------------------------------------
void OrthoCamera::getProjectionMatrixSplit(Core::Matrix4f & matrix, float near, float far) const
{
    matrix.setOrthoProj(_left, _right, _down, _top, near, far);
}
//-----------------------------------------------------------------------------
Ptr<IViewVolume> OrthoCamera::createViewVolume() const
{
    return Ptr<IViewVolume>(new OrthoViewVolume(*this));
}
//-----------------------------------------------------------------------------
void OrthoCamera::setAspectRatio(float ratio)
{
    _ratio = ratio;
    float height = (_right-_left) / _ratio;
    _top = height/2.f;
    _down = -_top;
}
//-----------------------------------------------------------------------------
void OrthoCamera::setWorkdistance(float distance)
{
    _workDistance = distance;

    if (_workDistance<_near)  _workDistance = _near;

    _ratio = (_right-_left) / (_top - _down);

    _right =  _workDistance * Core::L_tan(f_PI/2/2);
    _left = -_right;

    float a = (_right-_left) / _ratio;
    _top = a/2;
    _down = -_top;
}
//-----------------------------------------------------------------------------
void OrthoCamera::setFOV(float angle)
{
   // _fov = angle;

    _ratio = (_right-_left) / (_top - _down);

    //_workDistance = 5;//_near + (_far - _near)/2;
    _right =  _workDistance * Core::L_tan(angle/2);
    _left = -_right;

    float a = (_right-_left) / _ratio;
    _top = a/2;
    _down = -_top;

}
//-----------------------------------------------------------------------------
float OrthoCamera::getFOV() const
{
    return _workDistance;
}
//-----------------------------------------------------------------------------
}
