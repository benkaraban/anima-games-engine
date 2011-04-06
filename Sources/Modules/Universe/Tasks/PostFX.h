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
#ifndef UNIVERSE_TASKPOSTFX_H_
#define UNIVERSE_TASKPOSTFX_H_

#include <Core/Math/Interpolate.h>
#include <Universe/NodeCamera.h>
#include <Universe/Tasks/ITask.h>

namespace Universe
{
class LM_API_UNI TaskFX : public Core::Object, public ITask
{
public:
    TaskFX(const Ptr<NodeCamera> & pCamera,
           double duration,
           Core::EInterpolateFunc func);

    virtual void update(double elapsed);

protected:
    virtual void updatePostFX(Renderer::PostFX & fx, float a) = 0;
    virtual bool internalIsFinished() const;

    Ptr<NodeCamera> _pCamera;
    double          _elapsed;
    double          _duration;
    Core::EInterpolateFunc _func;
};

class LM_API_UNI TaskFXSaturate : public TaskFX
{
public:
    TaskFXSaturate(const Ptr<NodeCamera> & pCamera,
                   float saturate1,
                   float saturate2,
                   double duration,
                   Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
protected:
    virtual void updatePostFX(Renderer::PostFX & fx, float a);

    float           _saturate1;
    float           _saturate2;
};

class LM_API_UNI TaskFXFade : public TaskFX
{
public:
    TaskFXFade(const Ptr<NodeCamera> & pCamera,
               const Core::Vector4f & color1,
               const Core::Vector4f & color2,
               double duration,
               Core::EInterpolateFunc func = Core::INTER_LINEAR);

protected:
    virtual void updatePostFX(Renderer::PostFX & fx, float a);

    Core::Vector4f  _color1;
    Core::Vector4f  _color2;
};

class LM_API_UNI TaskFXBlur : public TaskFX
{
public:
    TaskFXBlur(const Ptr<NodeCamera> & pCamera,
               float blur1,
               float blur2,
               double duration,
               Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
protected:
    virtual void updatePostFX(Renderer::PostFX & fx, float a);

    float           _blur1;
    float           _blur2;
};

class LM_API_UNI TaskFXDirBlur : public TaskFX
{
public:
    TaskFXDirBlur(const Ptr<NodeCamera> & pCamera,
               const Core::Vector2f & dir,
               float blur1,
               float blur2,
               double duration,
               Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
protected:
    virtual void updatePostFX(Renderer::PostFX & fx, float a);

    Core::Vector2f  _dir;
    float           _blur1;
    float           _blur2;
};

class LM_API_UNI TaskFXRadialBlur : public TaskFX
{
public:
    TaskFXRadialBlur(const Ptr<NodeCamera> & pCamera,
               const Core::Vector2f & center,
               float blur1,
               float blur2,
               double duration,
               Core::EInterpolateFunc func = Core::INTER_LINEAR);

    TaskFXRadialBlur(const Ptr<NodeCamera> & pCamera,
               const Ptr<Node> & pTarget,
               float blur1,
               float blur2,
               double duration,
               Core::EInterpolateFunc func = Core::INTER_LINEAR);

protected:
    virtual void updatePostFX(Renderer::PostFX & fx, float a);

    Ptr<Node>       _pTarget;
    Core::Vector2f  _center;
    float           _blur1;
    float           _blur2;
};

}

#endif
