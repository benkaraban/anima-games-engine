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
#include <Universe/Tasks/Move.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskMove::TaskMove(DurationDependant_t mode,
                   const Ptr<Node> & pNode,
                   const Core::Vector3f & target,
                   float duration,
                   Core::EInterpolateFunc func) :
    _pNode(pNode),
    _startPoint(pNode->getWorldPosition()),
    _endPoint(target),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);
}
//------------------------------------------------------------------------------
TaskMove::TaskMove(DurationDependant_t mode,
                   const Ptr<Node> & pNode,
                   const Core::Vector3f & from,
                   const Core::Vector3f & to,
                   float duration,
                   Core::EInterpolateFunc func) :
    _pNode(pNode),
    _startPoint(from),
    _endPoint(to),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);
}
//------------------------------------------------------------------------------
TaskMove::TaskMove(SpeedDependant_t mode,
                   const Ptr<Node> & pNode,
                   const Core::Vector3f & target,
                   float speed,
                   Core::EInterpolateFunc func) :
    _pNode(pNode),
    _startPoint(pNode->getWorldPosition()),
    _endPoint(target),
    _elapsed(0.0),
    _duration(0.0),
    _func(func)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(speed != 0.0);

    if(speed != 0)
    {
        float len = (target - pNode->getWorldPosition()).getLengthSafe();
        _duration = len / speed;
    }
}
//------------------------------------------------------------------------------
TaskMove::TaskMove(SpeedDependant_t mode,
                   const Ptr<Node> & pNode,
                   const Core::Vector3f & from,
                   const Core::Vector3f & to,
                   float speed,
                   Core::EInterpolateFunc func) :
    _pNode(pNode),
    _startPoint(from),
    _endPoint(to),
    _elapsed(0.0),
    _duration(0.0),
    _func(func)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(speed != 0.0);

    if(speed != 0)
    {
        float len = (to - from).getLengthSafe();
        _duration = len / speed;
    }
}
//------------------------------------------------------------------------------
void TaskMove::update(double elapsed)
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
        
        Core::Vector3f position;
        position.setLerp(_startPoint, _endPoint, a);
        
        _pNode->beginMatrixUpdate();
        _pNode->setWorldPosition(position);
        _pNode->endMatrixUpdate();
    }
}
//------------------------------------------------------------------------------
bool TaskMove::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TaskMoveORPO::TaskMoveORPO(const Ptr<Node> & pNode,
                           const Core::Vector3f & position,
                           const Core::Quaternionf & orient,
                           float duration,
                           Core::EInterpolateFunc func) :
    _pNode(pNode),
    _elapsed(0.0),
    _duration(duration),
    _func(func),
    _p1(pNode->getLocalPosition()),
    _p2(position),
    _q1(pNode->getLocalOrient()),
    _q2(orient)
{
}
//------------------------------------------------------------------------------
void TaskMoveORPO::update(double elapsed)
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
        
        _pNode->beginMatrixUpdate();
        _pNode->setLocalPosition(Core::Vector3f(_p1, _p2, a));
        _pNode->setLocalOrientation(Core::Quaternionf(_q1, _q2, a));
        _pNode->endMatrixUpdate();
    }
}
//------------------------------------------------------------------------------
bool TaskMoveORPO::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
}
