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
#include <Universe/Tasks/TimeWarp.h>

namespace Universe
{
//-----------------------------------------------------------------------------
TaskTimeWarp::TaskTimeWarp(const Ptr<Node> & pNode,
                           float startTimeMul,
                           float endTimeMul,
                           float duration,
                           Core::EInterpolateFunc func) :
    _pNode(pNode),
    _startTimeMul(startTimeMul),
    _endTimeMul(endTimeMul),
    _duration(duration),
    _func(func)
{
}
//-----------------------------------------------------------------------------
void TaskTimeWarp::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        float a = 1.0;

        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate(float(_elapsed / _duration), _func);
        }

        _pNode->setTimeMultiplier(Core::lerp(_startTimeMul, _endTimeMul, a), true);
    }
}
//-----------------------------------------------------------------------------
bool TaskTimeWarp::internalIsFinished() const
{
    return !_pNode->isAlive() || _elapsed >= _duration;
}
//-----------------------------------------------------------------------------
}
