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
#include <Core/Math/MathTools.h>
#include <Universe/NodePointLight.h>
#include <Universe/Tasks/Scale.h>

namespace Universe
{
//------------------------------------------------------------------------------
void setNodeScale(const Ptr<Node> & pNode, const Core::Vector3f & scale)
{
    switch(pNode->getNodeType())
    {
    case NODE_POINT_LIGHT:
    {
        Ptr<NodePointLight> pLight = LM_DEBUG_PTR_CAST<NodePointLight>(pNode);
        pLight->setRadius(scale.x);
        break;
    }
    default:
    {
        pNode->beginMatrixUpdate();
        pNode->setLocalScale(scale);
        pNode->endMatrixUpdate();
        break;
    }
    }
}
//------------------------------------------------------------------------------
TaskScale::TaskScale(const Ptr<Node> & pNode,
                     const Core::Vector3f & scale,
                     float duration,
                     bool relative,
                     Core::EInterpolateFunc func) :
    _pNode(pNode),
    _scale1(pNode->getLocalScale()),
    _scale2(scale),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);

    if(relative)
    {
        _scale2.x *= _scale1.x;
        _scale2.y *= _scale1.y;
        _scale2.z *= _scale1.z;
    }
}
//------------------------------------------------------------------------------
void TaskScale::update(double elapsed)
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
        
        Core::Vector3f scale;
        scale.setLerp(_scale1, _scale2, a);
        setNodeScale(_pNode, scale);
    }
}
//------------------------------------------------------------------------------
bool TaskScale::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskScaleLoop::TaskScaleLoop(const Ptr<Node> & pNode,
                             const Core::Vector3f & scaleMin,
                             const Core::Vector3f & scaleMax,
                             float period,
                             float duration) :
    _pNode(pNode),
    _scaleMin(scaleMin),
    _scaleMax(scaleMax),
    _period(period),
    _phase(0.0),
    _duration(duration)
{
    LM_ASSERT(period != 0.0);
}
//------------------------------------------------------------------------------
void TaskScaleLoop::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        double e = std::min(elapsed, _duration);
        _phase = fmod(double(_phase + e), double(_period));

        float a = 0.5f * Core::L_sin((float(_phase / _period) - 0.25f) * f_PI_MUL_2) + 0.5f;
        
        Core::Vector3f scale;
        scale.setLerp(_scaleMin, _scaleMax, a);
        setNodeScale(_pNode, scale);

        _duration -= elapsed;
    }
}
//------------------------------------------------------------------------------
bool TaskScaleLoop::internalIsFinished() const
{
    return !_pNode->isAlive() || _duration <= 0.0;
}
//------------------------------------------------------------------------------
}
