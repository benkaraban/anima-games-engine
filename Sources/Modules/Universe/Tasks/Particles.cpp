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
#include <Universe/Tasks/Particles.h>
#include <Universe/Particles/EmitterShapeCone.h>

namespace Universe
{
//-----------------------------------------------------------------------------
TaskChangeEmitterPeriod::TaskChangeEmitterPeriod(const Ptr<NodeEmitter> & pNode,
                                                 float endMinPeriod,
                                                 float endMaxPeriod,
                                                 float duration,
                                                 Core::EInterpolateFunc func) :
    _pNode(pNode),
    _endMinPeriod(endMinPeriod),
    _endMaxPeriod(endMaxPeriod),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    _pNode->getPeriod(_startMinPeriod, _startMaxPeriod);
}
//-----------------------------------------------------------------------------
void TaskChangeEmitterPeriod::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0;

        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate(float(_elapsed / _duration), _func);
        }

        _pNode->setPeriod(Core::lerp(_startMinPeriod, _endMinPeriod, a),
                          Core::lerp(_startMaxPeriod, _endMaxPeriod, a));
    }
}
//-----------------------------------------------------------------------------
bool TaskChangeEmitterPeriod::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TaskChangeEmitterStartWidth::TaskChangeEmitterStartWidth(const Ptr<NodeEmitter> & pNode,
                                                 float endMinStartWidth,
                                                 float endMaxStartWidth,
                                                 float duration,
                                                 Core::EInterpolateFunc func) :
    _pNode(pNode),
    _endMinStartWidth(endMinStartWidth),
    _endMaxStartWidth(endMaxStartWidth),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    _pNode->getStartWidth(_startMinStartWidth, _startMaxStartWidth);
}
//-----------------------------------------------------------------------------
void TaskChangeEmitterStartWidth::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0;

        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate(float(_elapsed / _duration), _func);
        }

        _pNode->setStartWidth(Core::lerp(_startMinStartWidth, _endMinStartWidth, a),
                              Core::lerp(_startMaxStartWidth, _endMaxStartWidth, a));
    }
}
//-----------------------------------------------------------------------------
bool TaskChangeEmitterStartWidth::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TaskChangeEmitterStartSpeed::TaskChangeEmitterStartSpeed(const Ptr<NodeEmitter> & pNode,
                                                 float endMinStartSpeed,
                                                 float endMaxStartSpeed,
                                                 float duration,
                                                 Core::EInterpolateFunc func) :
    _pNode(pNode),
    _endMinStartSpeed(endMinStartSpeed),
    _endMaxStartSpeed(endMaxStartSpeed),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    Ptr<IEmitterShape> pShape = _pNode->getShape();

    switch(pShape->getType())
    {
    case SHAPE_CONE:
    {
        Ptr<EmitterShapeCone> pCone = LM_DEBUG_PTR_CAST<EmitterShapeCone>(pShape);
        pCone->getSpeed(_startMinStartSpeed, _startMaxStartSpeed);
        break;
    }
    }
}
//-----------------------------------------------------------------------------
void TaskChangeEmitterStartSpeed::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0;

        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate(float(_elapsed / _duration), _func);
        }

        Ptr<IEmitterShape> pShape = _pNode->getShape();

        switch(pShape->getType())
        {
        case SHAPE_CONE:
        {
            Ptr<EmitterShapeCone> pCone = LM_DEBUG_PTR_CAST<EmitterShapeCone>(pShape);
            pCone->setSpeed(Core::lerp(_startMinStartSpeed, _endMinStartSpeed, a),
                            Core::lerp(_startMaxStartSpeed, _endMaxStartSpeed, a));
            break;
        }
        }
    }
}
//-----------------------------------------------------------------------------
bool TaskChangeEmitterStartSpeed::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TaskChangeEmitterRadius::TaskChangeEmitterRadius(const Ptr<NodeEmitter> & pNode,
                                                 float endMinRadius,
                                                 float endMaxRadius,
                                                 float duration,
                                                 Core::EInterpolateFunc func) :
    _pNode(pNode),
    _endMinRadius(endMinRadius),
    _endMaxRadius(endMaxRadius),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    Ptr<IEmitterShape> pShape = _pNode->getShape();

    switch(pShape->getType())
    {
    case SHAPE_CONE:
    {
        Ptr<EmitterShapeCone> pCone = LM_DEBUG_PTR_CAST<EmitterShapeCone>(pShape);
        pCone->getRadius(_startMinRadius, _startMaxRadius);
        break;
    }
    }
}
//-----------------------------------------------------------------------------
void TaskChangeEmitterRadius::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0;

        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate(float(_elapsed / _duration), _func);
        }

        Ptr<IEmitterShape> pShape = _pNode->getShape();

        switch(pShape->getType())
        {
        case SHAPE_CONE:
        {
            Ptr<EmitterShapeCone> pCone = LM_DEBUG_PTR_CAST<EmitterShapeCone>(pShape);
            pCone->setRadius(Core::lerp(_startMinRadius, _endMinRadius, a),
                             Core::lerp(_startMaxRadius, _endMaxRadius, a));
            break;
        }
        }
    }
}
//-----------------------------------------------------------------------------
bool TaskChangeEmitterRadius::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//-----------------------------------------------------------------------------
}
