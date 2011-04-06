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
#include <Core/Debug/FPSCounter.h>

namespace Core
{
//-----------------------------------------------------------------------------
FPSCounter::FPSCounter() :
    _framesCount(0),
    _frameDuration(0.0),
    _FPS(0.0)
{
    _chrono.reset();
}
//-----------------------------------------------------------------------------
void FPSCounter::nextFrame()
{
    _framesCount ++;

    Core::TimeDuration currentTime = _chrono.elapsed();
    Core::TimeDuration deltaTime   = (currentTime - _previousTime);

    if(deltaTime.ftotalSeconds() >= 1.0 && _framesCount > 0) // une seconde min
    {
        _frameDuration = 1000.0 * (deltaTime.ftotalSeconds() / _framesCount);
        _FPS = _framesCount / deltaTime.ftotalSeconds();

        _previousTime = currentTime;
        _framesCount  = 0;

        _text = Core::toString(_FPS, 1) + L" fps\n" + Core::toString(_frameDuration, 1) + L" ms/f";
        //_text = Core::toString(_frameDuration, 1) + L" ms\n  (" + Core::toString(_FPS, 1) + L" FPS)";
    }
}
//-----------------------------------------------------------------------------
}
