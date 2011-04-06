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
#include <Universe/Tasks/Shake.h>

namespace Universe
{
TaskShake::TaskShake(const Ptr<Node> & pNode,
                      float dx, 
                      float periodx,
                      float dy,
                      float periody,
                      float duration,
                      bool isLocal,
                      bool isGain,
                      Core::EInterpolateFunc func)
: _pNode(pNode)
, _dx(dx)
, _periodx(periodx)
, _dy(dy)
, _periody(periody)
, _elapsed(0.0)
, _duration(duration)
, _isLocal(isLocal)
, _isGain(isGain)
, _func(func)
, _pos(pNode->getWorldPosition())
, _xVec(pNode->getLocalToWorldMatrix().getXVector())
, _yVec(pNode->getLocalToWorldMatrix().getYVector())
{
}

void TaskShake::update(double elapsed)
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

        float dx = 0.0f;
        float dy = 0.0f;

        float u = fmod(Core::L_frac(float(_elapsed) / _periodx) + 0.25f, 1.0f);
        float v = fmod(Core::L_frac(float(_elapsed) / _periody) + 0.25f, 1.0f);

        if(u <= 0.5f)
            dx = _dx * ((u - 0.25f) * 2.0f);
        else
            dx = -_dx * ((u - 0.75f) * 2.0f);

        if(u <= 0.5f)
            dy = _dy * ((v - 0.25f) * 2.0f);
        else
            dy = -_dy * ((v - 0.75f) * 2.0f);

        if(_isGain)
        {
            dx *= a;
            dy *= a;
        }
        else
        {
            dx *= (1.0f - a);
            dy *= (1.0f - a);
        }

        if(_isLocal)
            _pNode->setLocalPosition(Core::Vector3f(dx, dy, 0.0f));
        else
            _pNode->setWorldPosition(_pos + dx * _xVec + dy * _yVec);
    }
}

bool TaskShake::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}

};