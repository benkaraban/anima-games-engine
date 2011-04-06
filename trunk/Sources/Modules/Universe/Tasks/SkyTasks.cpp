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
#include <Universe/Tasks/SkyTasks.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskSkyColorFade::TaskSkyColorFade(World * pWorld,
                   const Core::Vector4f & color1,
                   float duration,
                   Core::EInterpolateFunc func) :
    _pWorld(pWorld),
    _color1(color1),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pWorld != 0);
    _color0 = pWorld->getSkyColor();
}
//------------------------------------------------------------------------------
void TaskSkyColorFade::update(double elapsed)
{
    elapsed *= _pWorld->getTimeMultiplier();
    float a = 1.0;

    if(_duration != 0.0)
    {
        _elapsed = std::min(_duration, _elapsed + elapsed);
        a = Core::interpolate((float)(_elapsed / _duration), _func);
    }

    Core::Vector4f color;
    color.setLerp(_color0, _color1, a);
    
    _pWorld->setSkyColor(color);
}
//------------------------------------------------------------------------------
bool TaskSkyColorFade::internalIsFinished() const
{
    return _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TaskSkyGlowFade::TaskSkyGlowFade(World * pWorld,
                   const Core::Vector4f & color1,
                   float duration,
                   Core::EInterpolateFunc func) :
    _pWorld(pWorld),
    _color1(color1),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pWorld != 0);
    _color0 = pWorld->getSkyGlow();
}
//------------------------------------------------------------------------------
void TaskSkyGlowFade::update(double elapsed)
{
    elapsed *= _pWorld->getTimeMultiplier();
    float a = 1.0;

    if(_duration != 0.0)
    {
        _elapsed = std::min(_duration, _elapsed + elapsed);
        a = Core::interpolate((float)(_elapsed / _duration), _func);
    }

    Core::Vector4f color;
    color.setLerp(_color0, _color1, a);
    
    _pWorld->setSkyGlow(color);
}
//------------------------------------------------------------------------------
bool TaskSkyGlowFade::internalIsFinished() const
{
    return _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskGlobalColorFade::TaskGlobalColorFade(World * pWorld,
                   float globalFade,
                   float duration,
                   Core::EInterpolateFunc func) :
    _pWorld(pWorld),
    _fade1(globalFade),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pWorld != 0);
    _fade0 = _pWorld->getMainLightSettings().globalFade;
}
//------------------------------------------------------------------------------
void TaskGlobalColorFade::update(double elapsed)
{
    elapsed *= _pWorld->getTimeMultiplier();
    float a = 1.0;

    if(_duration != 0.0)
    {
        _elapsed = std::min(_duration, _elapsed + elapsed);
        a = Core::interpolate((float)(_elapsed / _duration), _func);
    }

    Renderer::MainLightSettings light = _pWorld->getMainLightSettings();
    light.globalFade = Core::lerp(_fade0, _fade1, a);
    _pWorld->setMainLightSettings(light);
}
//------------------------------------------------------------------------------
bool TaskGlobalColorFade::internalIsFinished() const
{
    return _elapsed >= _duration;
}
//------------------------------------------------------------------------------
}
