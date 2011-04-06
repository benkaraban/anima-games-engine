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
#include <Core/Math/CameraController.h>

namespace Core
{
//------------------------------------------------------------------------------
CameraQuake::CameraQuake() :
    _position(0.0, 0.0, 0.0),
    _yaw(0.0),
    _pitch(0.0)
{
    updateMatrix();
}
//------------------------------------------------------------------------------
CameraQuake::CameraQuake(const Core::Vector3f & position) :
    _position(position),
    _yaw(0.0),
    _pitch(0.0)
{
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::turnLeftRight(float delta)
{
    _yaw += delta;
    _yaw = fmod(_yaw, f_PI_MUL_2);
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::turnUpDown(float delta)
{
    _pitch = std::max(-f_PI_DIV_2, std::min(f_PI_DIV_2, _pitch + delta));
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::forward(float delta)
{
    _position += delta * _matrix.getZVector();
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::backward(float delta)
{
    _position -= delta * _matrix.getZVector();
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::strafeRight(float delta)
{
    _position += delta * _matrix.getXVector();
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::strafeLeft(float delta)
{
    _position -= delta * _matrix.getXVector();
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::moveUp(float delta)
{
    _position += delta * _matrix.getYVector();
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::moveDown(float delta)
{
    _position -= delta * _matrix.getYVector();
    updateMatrix();
}
//------------------------------------------------------------------------------
Core::Matrix4f CameraQuake::getWorldMatrix() const
{
    return _matrix;
}
//------------------------------------------------------------------------------
void CameraQuake::setPosition(const Vector3f & position)
{
    _position = position;
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraQuake::updateMatrix()
{
    Core::Matrix4f matYaw(Core::ROTATION, _yaw, Core::ROT_Y_YAW);
    Core::Matrix4f matPitch(Core::ROTATION, _pitch, Core::ROT_X_PITCH);
    Core::Matrix4f matTrans(Core::TRANSLATION, _position);
    _matrix = matTrans * (matYaw * matPitch);
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
CameraOrbital::CameraOrbital() :
    _matOrient(Core::IDENTITY),
    _matTrans(Core::IDENTITY),
    _distance(200.0)
{
    updateMatrix();
}
//------------------------------------------------------------------------------
CameraOrbital::CameraOrbital(const Core::Vector3f & observedPosition, float distance) :
    _matOrient(Core::ROTATION, f_PI, Core::ROT_Y_YAW),
    _matTrans(Core::TRANSLATION, observedPosition),
    _distance(distance)
{
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraOrbital::turnLeftRight(float delta)
{
    Core::Matrix4f matYaw(Core::ROTATION, delta, Core::ROT_Y_YAW);
    _matOrient = _matOrient * matYaw;
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraOrbital::turnUpDown(float delta)
{
    Core::Matrix4f matPitch(Core::ROTATION, delta, Core::ROT_X_PITCH);
    _matOrient = _matOrient * matPitch;
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraOrbital::relativeMove(float dx, float dy)
{
    Core::Vector4f vecTrans(-dx, -dy, 0.0, 1.0);
    vecTrans = _matOrient * vecTrans;

    Core::Matrix4f matTrans(Core::TRANSLATION, vecTrans.x, vecTrans.y, vecTrans.z);
    _matTrans = _matTrans * matTrans;

    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraOrbital::zoomInOut(float delta)
{
    _distance = std::max(0.0f, _distance - delta);
    updateMatrix();
}
//------------------------------------------------------------------------------
void CameraOrbital::setObservedPoint(const Core::Vector3f & position)
{
    _matTrans = Core::Matrix4f(Core::TRANSLATION, position);
    updateMatrix();
}
//------------------------------------------------------------------------------
Core::Matrix4f CameraOrbital::getWorldMatrix() const
{
    return _matrix;
}
//------------------------------------------------------------------------------
void CameraOrbital::updateMatrix()
{
    Core::Matrix4f matTrans(Core::TRANSLATION, 0.0, 0.0, -_distance);
    _matrix = (_matTrans * _matOrient) * matTrans;
}
//------------------------------------------------------------------------------
void CameraOrbital::setDistance(float d)
{
    _distance = d;
    updateMatrix();
}
//------------------------------------------------------------------------------
}
