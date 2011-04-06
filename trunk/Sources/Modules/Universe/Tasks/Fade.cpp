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
#include <Universe/NodeDecal.h>
#include <Universe/NodePointLight.h>
#include <Universe/Tasks/Fade.h>
#include <Universe/Tools.h>

namespace Universe
{
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
void getColorNodes(Core::List<Ptr<Node> > & nodes, const Ptr<Node> & pParent)
{
    getAllChildren(nodes, *pParent);
    nodes.push_back(pParent);
}
//------------------------------------------------------------------------------
TaskFade::TaskFade(const Ptr<Node> & pNode,
                   const Core::Vector4f & color0,
                   const Core::Vector4f & color1,
                   float duration,
                   Core::EInterpolateFunc func,
                   bool recursive) :
    _pNode(pNode),
    _color1(color1),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);

    _colors0.resize(_nodes.size());

    for(int32 ii=0; ii < _nodes.size(); ii++)
        _colors0[ii] = color0;
}
//------------------------------------------------------------------------------
TaskFade::TaskFade(const Ptr<Node> & pNode,
                   const Core::Vector4f & color1,
                   float duration,
                   Core::EInterpolateFunc func,
                   bool recursive) :
    _pNode(pNode),
    _color1(color1),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);

    _colors0.resize(_nodes.size());

    for(int32 ii=0; ii < _nodes.size(); ii++)
        _colors0[ii] = _nodes[ii]->getNodeColor();
}
//------------------------------------------------------------------------------
void TaskFade::update(double elapsed)
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

        for(int32 ii=0; ii < _nodes.size(); ii++)
        {
            Core::Vector4f color;
            color.setLerp(_colors0[ii], _color1, a);
            
            _nodes[ii]->setNodeColor(color);
        }
    }
}
//------------------------------------------------------------------------------
bool TaskFade::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskFade2::TaskFade2(const Ptr<Node> & pNode,
                     const Core::Vector4f & color0,
                     const Core::Vector4f & color1,
                     const Core::Vector4f & color2,
                     float time1,
                     float time2,
                     Core::EInterpolateFunc func1,
                     Core::EInterpolateFunc func2,
                     bool recursive) :
    _pNode(pNode),
    _color0(color0),
    _color1(color1),
    _color2(color2),
    _elapsed(0.0),
    _time1(time1),
    _time2(time2),
    _func1(func1),
    _func2(func2)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(time2 >= time1);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);
}
//------------------------------------------------------------------------------
void TaskFade2::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0;

        Core::Vector4f color;
        _elapsed = std::min(_time2, _elapsed + elapsed);

        if(_elapsed < _time1)
        {
            if(_time1 != 0.0)
                a = Core::interpolate(float(_elapsed / _time1), _func1);
            color.setLerp(_color0, _color1, a);
        }
        else
        {
            double dtime = _time2 - _time1;
            if(dtime != 0.0)
                a = Core::interpolate(float((_elapsed - _time1) / dtime), _func2);
            color.setLerp(_color1, _color2, a);
        }

        for(int32 ii=0; ii < _nodes.size(); ii++)
            _nodes[ii]->setNodeColor(color);
    }
}
//------------------------------------------------------------------------------
bool TaskFade2::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _time2;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskFadeLoop::TaskFadeLoop(const Ptr<Node> & pNode,
                           const Core::Vector4f & color0,
                           const Core::Vector4f & color1,
                           float period,
                           float duration,
                           bool recursive) :
    _pNode(pNode),
    _color0(color0),
    _color1(color1),
    _period(period),
    _duration(duration)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(period != 0.0);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);
}
//------------------------------------------------------------------------------
void TaskFadeLoop::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        double e = std::min(elapsed, _duration);
        _duration -= e;
        _phase = (float)fmod(double(_phase + e), double(_period));

        float a = 0.5f * Core::L_sin(((_phase / _period) - 0.25f) * f_PI_MUL_2) + 0.5f;
        
        Core::Vector4f color;
        color.setLerp(_color0, _color1, a);
        
        for(int32 ii=0; ii < _nodes.size(); ii++)
            _nodes[ii]->setNodeColor(color);
    }
}
//------------------------------------------------------------------------------
bool TaskFadeLoop::internalIsFinished() const
{
    return !_pNode->isAlive() || _duration <= 0.0;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskGlowFade::TaskGlowFade(const Ptr<Node> & pNode,
                   const Core::Vector4f & color0,
                   const Core::Vector4f & color1,
                   float duration,
                   Core::EInterpolateFunc func,
                   bool recursive) :
    _pNode(pNode),
    _color1(color1),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);

    _colors0.resize(_nodes.size());

    for(int32 ii=0; ii < _nodes.size(); ii++)
        _colors0[ii] = color0;
}
//------------------------------------------------------------------------------
TaskGlowFade::TaskGlowFade(const Ptr<Node> & pNode,
                   const Core::Vector4f & color1,
                   float duration,
                   Core::EInterpolateFunc func,
                   bool recursive) :
    _pNode(pNode),
    _color1(color1),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pNode != null);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);

    _colors0.resize(_nodes.size());

    for(int32 ii=0; ii < _nodes.size(); ii++)
        _colors0[ii] = _nodes[ii]->getNodeGlow();
}
//------------------------------------------------------------------------------
void TaskGlowFade::update(double elapsed)
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

        for(int32 ii=0; ii < _nodes.size(); ii++)
        {
            Core::Vector4f color;
            color.setLerp(_colors0[ii], _color1, a);
            
            _nodes[ii]->setNodeGlow(color);
        }
    }
}
//------------------------------------------------------------------------------
bool TaskGlowFade::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskGlowFade2::TaskGlowFade2(const Ptr<Node> & pNode,
                     const Core::Vector4f & color0,
                     const Core::Vector4f & color1,
                     const Core::Vector4f & color2,
                     float time1,
                     float time2,
                     Core::EInterpolateFunc func1,
                     Core::EInterpolateFunc func2,
                     bool recursive) :
    _pNode(pNode),
    _color0(color0),
    _color1(color1),
    _color2(color2),
    _elapsed(0.0),
    _time1(time1),
    _time2(time2),
    _func1(func1),
    _func2(func2)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(time2 >= time1);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);
}
//------------------------------------------------------------------------------
void TaskGlowFade2::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        float a = 1.0;

        Core::Vector4f color;
        _elapsed = std::min(_time2, _elapsed + elapsed);

        if(_elapsed < _time1)
        {
            if(_time1 != 0.0)
                a = Core::interpolate(float(_elapsed / _time1), _func1);
            color.setLerp(_color0, _color1, a);
        }
        else
        {
            double dtime = _time2 - _time1;
            if(dtime != 0.0)
                a = Core::interpolate(float((_elapsed - _time1) / dtime), _func2);
            color.setLerp(_color1, _color2, a);
        }

        for(int32 ii=0; ii < _nodes.size(); ii++)
            _nodes[ii]->setNodeGlow(color);
    }
}
//------------------------------------------------------------------------------
bool TaskGlowFade2::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _time2;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskGlowFadeLoop::TaskGlowFadeLoop(const Ptr<Node> & pNode,
                           const Core::Vector4f & color0,
                           const Core::Vector4f & color1,
                           float period,
                           float duration,
                           bool recursive) :
    _pNode(pNode),
    _color0(color0),
    _color1(color1),
    _period(period),
    _duration(duration)
{
    LM_ASSERT(pNode != null);
    LM_ASSERT(period != 0.0);

    if(recursive)
        getColorNodes(_nodes, pNode);
    else
        _nodes.push_back(pNode);
}
//------------------------------------------------------------------------------
void TaskGlowFadeLoop::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        elapsed *= _pNode->getTimeMultiplier();
        double e = std::min(elapsed, _duration);
        _duration -= e;
        _phase = float(fmod(double(_phase + e), double(_period)));

        float a = 0.5f * Core::L_sin(((_phase / _period) - 0.25f) * f_PI_MUL_2) + 0.5f;

        Core::Vector4f color;
        color.setLerp(_color0, _color1, a);

        for(int32 ii=0; ii < _nodes.size(); ii++)
            _nodes[ii]->setNodeGlow(color);
    }
}
//------------------------------------------------------------------------------
bool TaskGlowFadeLoop::internalIsFinished() const
{
    return !_pNode->isAlive() || _duration <= 0.0;
}
//------------------------------------------------------------------------------
}
