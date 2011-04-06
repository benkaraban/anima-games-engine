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
#ifndef UNIVERSE_TASK_PARTICLES_H_
#define UNIVERSE_TASK_PARTICLES_H_

#include <Core/Math/Interpolate.h>
#include <Universe/NodeEmitter.h>
#include <Universe/Tasks/ITask.h>

namespace Universe
{
class LM_API_UNI TaskChangeEmitterPeriod : public Core::Object, public ITask
{
public:
    TaskChangeEmitterPeriod(const Ptr<NodeEmitter> & pNode,
                            float endMinPeriod,
                            float endMaxPeriod,
                            float duration,
                            Core::EInterpolateFunc func = Core::INTER_LINEAR);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<NodeEmitter>       _pNode;
    float                  _startMinPeriod;
    float                  _startMaxPeriod;
    float                  _endMinPeriod;
    float                  _endMaxPeriod;
    double                 _elapsed;
    double                 _duration;
    Core::EInterpolateFunc _func;
};

class LM_API_UNI TaskChangeEmitterStartWidth : public Core::Object, public ITask
{
public:
    TaskChangeEmitterStartWidth(const Ptr<NodeEmitter> & pNode,
                                float endMinStartWidth,
                                float endMaxStartWidth,
                                float duration,
                                Core::EInterpolateFunc func = Core::INTER_LINEAR);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<NodeEmitter>       _pNode;
    float                  _startMinStartWidth;
    float                  _startMaxStartWidth;
    float                  _endMinStartWidth;
    float                  _endMaxStartWidth;
    double                 _elapsed;
    double                 _duration;
    Core::EInterpolateFunc _func;
};

class LM_API_UNI TaskChangeEmitterStartSpeed : public Core::Object, public ITask
{
public:
    TaskChangeEmitterStartSpeed(const Ptr<NodeEmitter> & pNode,
                                float endMinStartSpeed,
                                float endMaxStartSpeed,
                                float duration,
                                Core::EInterpolateFunc func = Core::INTER_LINEAR);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<NodeEmitter>       _pNode;
    float                  _startMinStartSpeed;
    float                  _startMaxStartSpeed;
    float                  _endMinStartSpeed;
    float                  _endMaxStartSpeed;
    double                 _elapsed;
    double                 _duration;
    Core::EInterpolateFunc _func;
};

class LM_API_UNI TaskChangeEmitterRadius : public Core::Object, public ITask
{
public:
    TaskChangeEmitterRadius(const Ptr<NodeEmitter> & pNode,
                                float endMinRadius,
                                float endMaxRadius,
                                float duration,
                                Core::EInterpolateFunc func = Core::INTER_LINEAR);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<NodeEmitter>       _pNode;
    float                  _startMinRadius;
    float                  _startMaxRadius;
    float                  _endMinRadius;
    float                  _endMaxRadius;
    double                 _elapsed;
    double                 _duration;
    Core::EInterpolateFunc _func;
};
}

#endif /* PARTICLES_H_ */
