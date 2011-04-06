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
#include <Universe/Tasks/Rotate.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskRotate::TaskRotate(DurationDependant_t mode,
                       const Ptr<Node> & pNode,
                       float pitch,
                       float yaw,
                       float roll,
                       double duration,
                       Core::EInterpolateFunc func,
                       EReferential referentiel) :
    _pNode(pNode),
    _pitch(pitch),
    _yaw(yaw),
    _roll(roll),
    _elapsed(0.0),
    _duration(duration),
    _referentiel(referentiel),
    _func(func)
{
    LM_ASSERT(_pNode != null);
    _q1 = pNode->getLocalOrient();
}
//------------------------------------------------------------------------------
TaskRotate::TaskRotate(SpeedDependant_t mode,
                       const Ptr<Node> & pNode,
                       float pitch,
                       float yaw,
                       float roll,
                       double radialSpeed,
                       Core::EInterpolateFunc func,
                       EReferential referentiel) :
    _pNode(pNode),
    _pitch(pitch),
    _yaw(yaw),
    _roll(roll),
    _elapsed(0.0),
    _referentiel(referentiel),
    _func(func)
{
    LM_ASSERT(_pNode != null);
    LM_ASSERT(radialSpeed != 0.0);
    
    _q1 = pNode->getLocalOrient();

    float angle = std::max(pitch, std::max(yaw, roll));
    _duration = angle / radialSpeed;
}
//------------------------------------------------------------------------------
void TaskRotate::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0;

        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate((float)(_elapsed / _duration), _func);
        }

        Core::Quaternionf rot(a * _pitch, a * _yaw, a * _roll);
        Core::Quaternionf q2;

        switch(_referentiel)
        {
        case LOCAL_COORD:
            q2 = _q1 * rot;
            break;
        case WORLD_COORD:
            q2 = rot * _q1;
            break;
        }

        _pNode->beginMatrixUpdate();
        _pNode->setLocalOrientation(q2);
        _pNode->endMatrixUpdate();
    }
}
//------------------------------------------------------------------------------
bool TaskRotate::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskRotateLoop::TaskRotateLoop(const Ptr<Node> & pNode, 
                               float pitchSpeed, 
                               float yawSpeed,
                               float rollSpeed, 
                               double duration, 
                               EReferential referentiel) : 
    _pNode(pNode),
    _pitchSpeed(pitchSpeed),
      _yawSpeed(yawSpeed),
    _rollSpeed(rollSpeed),
    _duration(duration),
    _referentiel(referentiel)
{
    LM_ASSERT(_pNode != null);
}
//------------------------------------------------------------------------------
void TaskRotateLoop::update(double elapsed)
{
    LM_ASSERT(_pNode != null);

    if(_pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        double rotateTime = std::min(elapsed, double(_duration));

        _pNode->beginMatrixUpdate();
        switch(_referentiel)
        {
        case LOCAL_COORD:
            _pNode->localPitch(_pitchSpeed * rotateTime);
            _pNode->localYaw(_yawSpeed * rotateTime);
            _pNode->localRoll(_rollSpeed * rotateTime);
            break;
        case WORLD_COORD:
            _pNode->worldPitch(_pitchSpeed * rotateTime);
            _pNode->worldYaw(_yawSpeed * rotateTime);
            _pNode->worldRoll(_rollSpeed * rotateTime);
            break;
        }
        _pNode->endMatrixUpdate();
        _duration -= elapsed;
    }
}
//------------------------------------------------------------------------------
bool TaskRotateLoop::internalIsFinished() const
{
    LM_ASSERT(_pNode != null);
    return !_pNode->isAlive() || _duration < 0.0;
}
//------------------------------------------------------------------------------
}
