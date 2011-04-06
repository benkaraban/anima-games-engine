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
#include <Universe/Tasks/TurnAround.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskTurnAround::TaskTurnAround(const Ptr<Node> & pNode,
                               const Core::Vector3f & startPos,
                               const Core::Vector3f & endPos,
                               const Core::Vector3f & pivot,
                               float duration,
                               Core::EInterpolateFunc func,
                               bool shortestPath) :
    _pNode(pNode),
    _elapsed(0.0),
    _duration(duration),
    _func(func),
    _pivot(pivot),
    _y1(startPos.y),
    _y2(endPos.y)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(!startPos.isEquivalent(pivot));
    LM_ASSERT(!endPos.isEquivalent(pivot));
    
    Core::Vector3f v1(startPos - pivot);
    Core::Vector3f v2(endPos - pivot);

    v1.y = 0.0;
    v2.y = 0.0;

    _length1 = v1.getLengthSafe();
    _length2 = v2.getLengthSafe();

    LM_ASSERT(_length1 != 0.0);
    LM_ASSERT(_length2 != 0.0);

    Core::Vector3f n1(v1 / _length1);
    Core::Vector3f n2(v2 / _length2);

    _vecStart = n1;
    _angle = Core::L_acos(Core::dot(n1, n2));

    if(Core::cross(n1, n2).y < 0.0)
        _angle = -_angle;

    if(!shortestPath)
        _angle = -(f_PI_MUL_2 - _angle);
}
//------------------------------------------------------------------------------
void TaskTurnAround::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0f;
        
        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate(float(_elapsed / _duration), _func);
        }
        
        Core::Matrix3f mat;
        mat.setRotationYaw(a * _angle);

        float length = Core::lerp(_length1, _length2, a);
        Core::Vector3f position(_pivot + length * (mat * _vecStart));

        position.y = Core::lerp(_y1, _y2, a);

        _pNode->beginMatrixUpdate();
        _pNode->setWorldPosition(position);
        _pNode->endMatrixUpdate();
    }
}
//------------------------------------------------------------------------------
bool TaskTurnAround::internalIsFinished() const
{
    LM_ASSERT(_pNode != null);
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskTurnAroundLoop::TaskTurnAroundLoop(const Ptr<Node> & pNode,
                                       const Core::Vector3f & pivot,
                                       float pitchSpeed, 
                                       float yawSpeed,
                                       float rollSpeed, 
                                       float duration) :
    _pNode(pNode),
    _duration(duration),
    _pitchSpeed(pitchSpeed),
    _yawSpeed(yawSpeed),
    _rollSpeed(rollSpeed),
    _pivot(pivot),
    _yaw(0.0),
    _pitch(0.0)
{
    LM_ASSERT(pNode != null);
    
    _vecStart = pNode->getWorldPosition() - pivot;
}
//------------------------------------------------------------------------------
void TaskTurnAroundLoop::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        double e = std::min(elapsed, _duration);
        _duration -= e;

        _yaw = (float)fmod(double(_yaw + _yawSpeed * e), d_PI_MUL_2);
        _pitch = (float)fmod(double(_pitch + _pitchSpeed * e), d_PI_MUL_2);

        Core::Matrix4f mat1(Core::ROTATION, _yaw, Core::ROT_Y_YAW);
        Core::Vector3f vec(mat1.apply3x3(_vecStart));
        
        Core::Vector3f pitchVec(Core::cross(vec.getNormalizedSafe(), Core::Vector3f(0.0, 1.0, 0.0)));
        pitchVec.normalizeSafe();

        Core::Matrix4f mat2(Core::ROTATION, _pitch, pitchVec);

        Core::Vector3f position(_pivot + mat2.apply3x3(vec));

        _pNode->beginMatrixUpdate();
        _pNode->setWorldPosition(position);
        _pNode->endMatrixUpdate();
    }
}
//------------------------------------------------------------------------------
bool TaskTurnAroundLoop::internalIsFinished() const
{
    LM_ASSERT(_pNode != null);
    return !_pNode->isAlive() || _duration <= 0.0;
}
//------------------------------------------------------------------------------
}
