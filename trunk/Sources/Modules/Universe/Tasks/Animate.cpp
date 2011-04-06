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
#include <Universe/Tasks/Animate.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskAnimate::TaskAnimate(const Ptr<NodeSkeleton> & pNode,
                         const String & anim,
                         bool enqueue,
                         float triggerKey,
                         bool loop,
                         float duration,
                         float keyFrom,
                         float keyTo) :
    _pNode(pNode),
    _anim(anim),
    _triggerKey(triggerKey),
    _currentKey(-1.0f),
    _duration(0.0f),
    _keyFrom(keyFrom),
    _keyTo(keyTo),
    _enqueue(enqueue),
    _loop(loop),
    _isLaunched(false)
{
}
//------------------------------------------------------------------------------
TaskAnimate::~TaskAnimate()
{
    if(_pNode != null)
        _pNode->removeObserver(this);
}
//------------------------------------------------------------------------------
void TaskAnimate::update(double elapsed)
{
    if(!_isLaunched)
    {
        if(_enqueue)
            _pNode->enqueueAnim(_anim, _duration, _keyFrom, _keyTo, _loop, this);
        else
            _pNode->animate(_anim, _duration, _keyFrom, _keyTo, _loop, this);

        _isLaunched = true;
    }
}
//------------------------------------------------------------------------------
void TaskAnimate::animProgress(float p)
{
    _currentKey = p;
}
//------------------------------------------------------------------------------
bool TaskAnimate::internalIsFinished() const
{
    return !_pNode->isAlive() || _currentKey >= _triggerKey;
}
//------------------------------------------------------------------------------
}
