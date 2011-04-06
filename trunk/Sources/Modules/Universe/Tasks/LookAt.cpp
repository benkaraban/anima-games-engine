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
#include <Universe/Tasks/LookAt.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskLookAt::TaskLookAt(DurationDependant_t mode,
                       const Ptr<Node> & pNode,
                       const Core::Vector3f & target,
                       float duration,
                       Core::EInterpolateFunc func) :
    _pNode(pNode),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);
    init(target);
}
//------------------------------------------------------------------------------
TaskLookAt::TaskLookAt(SpeedDependant_t mode,
                       const Ptr<Node> & pNode,
                       const Core::Vector3f & target,
                       float radialSpeed,
                       Core::EInterpolateFunc func) :
    _pNode(pNode),
    _elapsed(0.0),
    _func(func)
{
    LM_ASSERT(pNode != null);
    init(target);

    float angle = 2.0f * Core::L_acos(Core::L_abs(Core::dot(_q1, _q2)));
    _duration = angle / radialSpeed;
}
//------------------------------------------------------------------------------
void TaskLookAt::init(const Core::Vector3f & target)
{
    Core::Vector3f localTarget(_pNode->getWorldToLocalMatrix().apply(target));

    Core::Matrix3f mat;
    mat.setZVec(localTarget.getNormalizedSafe(), _pNode->getUpVector(), _pNode->getLocalMatrix().getYVector());
    
    _q1 = _pNode->getLocalOrient();
    _q2 = Core::Quaternionf(mat);
}
//------------------------------------------------------------------------------
void TaskLookAt::update(double elapsed)
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

        Core::Quaternionf q(_q1, _q2, a);
        
        _pNode->beginMatrixUpdate();
        _pNode->setLocalOrientation(q);
        _pNode->endMatrixUpdate();
    }
}
//------------------------------------------------------------------------------
bool TaskLookAt::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskLockLookAt::TaskLockLookAt(const Ptr<Node> & pNode,
                               const Core::Vector3f & target,
                               float duration,
                               float radialSpeed) :
    _pNode(pNode),
    _pTarget(null),
    _target(target),
    _duration(duration),
    _speed(radialSpeed)
{    
    LM_ASSERT(pNode != null);
}
//------------------------------------------------------------------------------
TaskLockLookAt::TaskLockLookAt(const Ptr<Node> & pNode,
                               const Ptr<Node> & pTarget,
                               float duration,
                               float radialSpeed) :
    _pNode(pNode),
    _pTarget(pTarget),
    _target(pTarget->getWorldPosition()),
    _duration(duration),
    _speed(radialSpeed)
{
    LM_ASSERT(pNode != null);
}
//------------------------------------------------------------------------------
void TaskLockLookAt::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();

        if(_pTarget != null)
            _target = _pTarget->getWorldPosition();

        Core::Vector3f localTarget(_pNode->getParentWorldToLocalMatrix().apply(_target));
        Core::Vector3f localDir(localTarget - _pNode->getLocalPosition());
        Core::Matrix3f mat;
        mat.setZVec(localDir.getNormalizedSafe(), _pNode->getUpVector(), _pNode->getLocalMatrix().getYVector());

        Core::Quaternionf q1(_pNode->getLocalOrient());
        Core::Quaternionf q2(mat);

        double rotTime = std::min(elapsed, double(_duration));
        float angle = 2.0f * Core::L_acos(Core::L_abs(Core::dot(q1, q2)));
        float maxAngle = (_speed == INFINITY) ? angle : float(_speed * rotTime);
        float alpha = 1.0;

        if(angle != 0.0)
            alpha = std::min(1.0f, maxAngle / angle);

        Core::Quaternionf newOrient(q1, q2, alpha);

        _pNode->beginMatrixUpdate();
        _pNode->setLocalOrientation(newOrient);
        _pNode->endMatrixUpdate();

        _duration -= elapsed;
    }
}
//------------------------------------------------------------------------------
bool TaskLockLookAt::internalIsFinished() const
{
    return !_pNode->isAlive() || _duration <= 0.0 || (_pTarget != null && !_pTarget->isAlive());
}
//------------------------------------------------------------------------------

}
