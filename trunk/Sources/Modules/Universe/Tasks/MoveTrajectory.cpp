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
#include <Universe/Tasks/MoveTrajectory.h>
#include <Core/Math/Transform.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskMoveTrajectory::TaskMoveTrajectory(const Ptr<Node> & pNode,
                                       const Core::List<Core::CheckPoint> & points,
                                       float durationMul,
                                       double step) :
    _pNode(pNode),
    _trajectory(points, step),
    _durationMul(durationMul),
    _elapsed(0.0),
    _duration(durationMul * _trajectory.getDuration()),
    _localStart(true)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(!Core::isEquivalent(durationMul, 0.0f));
    _time2=_duration;
    _baseMat = pNode->getLocalToWorldMatrix();
    _reorthoNeeded = false;
}
//------------------------------------------------------------------------------
TaskMoveTrajectory::TaskMoveTrajectory(const Ptr<Node> & pNode,
                                       const Core::Trajectory & traj,
                                       float durationMul) :
    _pNode(pNode),
    _trajectory(traj),
    _durationMul(durationMul),
    _elapsed(0.0),
    _duration(durationMul * _trajectory.getDuration()),
    _time2(_duration),
    _localStart(true)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(!Core::isEquivalent(durationMul, 0.0f));
    _time2=_duration;
    _baseMat = pNode->getLocalToWorldMatrix();
    _reorthoNeeded = false;
}
//------------------------------------------------------------------------------
TaskMoveTrajectory::TaskMoveTrajectory(const Ptr<Node> & pNode,
                                       const Ptr<Node> & pPivot,
                                       const Core::Trajectory & traj,
                                       float durationMul,
                                       float key1,
                                       float key2) :
    _pNode(pNode),
    _trajectory(traj),
    _durationMul(durationMul),
    _elapsed(0.0),
    _duration(durationMul * _trajectory.getDuration()),
    _localStart(false)
{
    _time2 = _duration * key2;
    _elapsed = _duration * key1;
    LM_ASSERT(pNode != null);
    LM_ASSERT(!Core::isEquivalent(durationMul, 0.0f));
    _baseMat = pPivot->getLocalToWorldMatrix();

    const Core::Vector3f & scale = pPivot->getLocalScale();

    if(scale.x < 0.0f)
    {
        _reorthoNeeded = true;
        _reorthoAxis = Core::ROT_X_PITCH;
    }
    else if(scale.y < 0.0f)
    {
        _reorthoNeeded = true;
        _reorthoAxis = Core::ROT_Y_YAW;
    }
    else if(scale.z < 0.0f)
    {
        _reorthoNeeded = true;
        _reorthoAxis = Core::ROT_Z_ROLL;
    }
    else
    {
        _reorthoNeeded = false;
    }
}
//------------------------------------------------------------------------------
void TaskMoveTrajectory::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        _elapsed = std::min(_elapsed + elapsed, _time2);

        Core::Vector3f position;
        Core::Quaternionf orient;

        _trajectory.getORPO(position, orient, (float)(_elapsed / _durationMul));

        if(_localStart)
            position = position - _trajectory.getPosition(0.0f);

        Core::Matrix4f mat(orient.toMatrix3(), position);

        mat = _baseMat * mat;

        if(_reorthoNeeded)
        {
            if(_reorthoAxis == Core::ROT_X_PITCH)
                mat.setXVector(Core::cross(mat.getYVector(), mat.getZVector()).getNormalizedSafe());
            else if(_reorthoAxis == Core::ROT_Y_YAW)
                mat.setXVector(Core::cross(mat.getZVector(), mat.getXVector()).getNormalizedSafe());
            else
                mat.setZVector(Core::cross(mat.getXVector(), mat.getYVector()).getNormalizedSafe());
        }

        _pNode->setLocalMatrix(mat);
    }
}
//------------------------------------------------------------------------------
bool TaskMoveTrajectory::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _time2;
}
//------------------------------------------------------------------------------
}
