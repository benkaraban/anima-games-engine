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
#include <Universe/Tasks/PostFX.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskFX::TaskFX(const Ptr<NodeCamera> & pCamera,
           double duration,
           Core::EInterpolateFunc func) :
    _pCamera(pCamera),
    _elapsed(0.0),
    _duration(duration),
    _func(func)
{
    LM_ASSERT(pCamera != null);
}
//------------------------------------------------------------------------------
void TaskFX::update(double elapsed)
{
    if(_pCamera != null && _pCamera->isAlive())
    {
        elapsed *= _pCamera->getTimeMultiplier();
        float a = 1.0;
        
        if(_duration != 0.0)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            a = Core::interpolate((float)(_elapsed / _duration), _func);
        }
        
        Renderer::PostFX fx = _pCamera->getBaseCamera()->getPostFX();
        updatePostFX(fx, a);
        _pCamera->getBaseCamera()->setPostFX(fx);
    }
}
//------------------------------------------------------------------------------
bool TaskFX::internalIsFinished() const
{
    return !_pCamera->isAlive() || _elapsed >= _duration;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskFXSaturate::TaskFXSaturate(const Ptr<NodeCamera> & pCamera,
                               float saturate1,
                               float saturate2,
                               double duration,
                               Core::EInterpolateFunc func) :
    TaskFX(pCamera, duration, func),
    _saturate1(saturate1),
    _saturate2(saturate2)
{
    LM_ASSERT(pCamera != null);
}
//------------------------------------------------------------------------------
void TaskFXSaturate::updatePostFX(Renderer::PostFX & fx, float a)
{
    fx.saturation = Core::lerp(_saturate1, _saturate2, a);
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskFXFade::TaskFXFade(const Ptr<NodeCamera> & pCamera,
                       const Core::Vector4f & color1,
                       const Core::Vector4f & color2,
                       double duration,
                       Core::EInterpolateFunc func) :
    TaskFX(pCamera, duration, func),
    _color1(color1),
    _color2(color2)
{
    LM_ASSERT(pCamera != null);
}
//------------------------------------------------------------------------------
void TaskFXFade::updatePostFX(Renderer::PostFX & fx, float a)
{
    fx.fadeColor.setLerp(_color1, _color2, a);
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskFXBlur::TaskFXBlur(const Ptr<NodeCamera> & pCamera,
                       float blur1,
                       float blur2,
                       double duration,
                       Core::EInterpolateFunc func) :
    TaskFX(pCamera, duration, func),
    _blur1(blur1),
    _blur2(blur2)
{
    LM_ASSERT(pCamera != null);
}
//------------------------------------------------------------------------------
void TaskFXBlur::updatePostFX(Renderer::PostFX & fx, float a)
{
    float b = Core::lerp(_blur1, _blur2, a);
    fx.blurDir1 = Core::Vector2f(b, 0.0f);
    fx.blurDir2 = Core::Vector2f(0.0f, b);
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskFXDirBlur::TaskFXDirBlur(const Ptr<NodeCamera> & pCamera,
                             const Core::Vector2f & dir,
                             float blur1,
                             float blur2,
                             double duration,
                             Core::EInterpolateFunc func) :
    TaskFX(pCamera, duration, func),
    _dir(dir),
    _blur1(blur1),
    _blur2(blur2)
{
    LM_ASSERT(pCamera != null);
}
//------------------------------------------------------------------------------
void TaskFXDirBlur::updatePostFX(Renderer::PostFX & fx, float a)
{
    float b = Core::lerp(_blur1, _blur2, a);
    fx.blurDir1 = fx.blurDir2 = b * _dir;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
TaskFXRadialBlur::TaskFXRadialBlur(const Ptr<NodeCamera> & pCamera,
                                   const Core::Vector2f & center,
                                   float blur1,
                                   float blur2,
                                   double duration,
                                   Core::EInterpolateFunc func) :
    TaskFX(pCamera, duration, func),
    _center(center),
    _blur1(blur1),
    _blur2(blur2)
{
    LM_ASSERT(pCamera != null);
}
//------------------------------------------------------------------------------
TaskFXRadialBlur::TaskFXRadialBlur(const Ptr<NodeCamera> & pCamera,
                                   const Ptr<Node> & pTarget,
                                   float blur1,
                                   float blur2,
                                   double duration,
                                   Core::EInterpolateFunc func) :
    TaskFX(pCamera, duration, func),
    _pTarget(pTarget),
    _blur1(blur1),
    _blur2(blur2)
{
    LM_ASSERT(pCamera != null);
}
//------------------------------------------------------------------------------
void TaskFXRadialBlur::updatePostFX(Renderer::PostFX & fx, float a)
{
    if(_pTarget != null)
    {
        Core::Matrix4f view, proj, viewProj;
        _pCamera->getBaseCamera()->getViewMatrix(view);
        _pCamera->getBaseCamera()->getProjectionMatrix(proj);
        viewProj = proj * view;

        Core::Vector3f wpos = _pTarget->getWorldPosition();
        Core::Vector3f cpos = viewProj.apply(Core::Vector4f(wpos, 1.0f)).homogeneousDiv();

        cpos += Core::Vector3f(1.0f, 1.0f, 0.0f);
        cpos *= 0.5f;
        cpos.y = 1.0f - cpos.y;

        _center.x = cpos.x;
        _center.y = cpos.y;
    }

    fx.radialBlurCenter = _center;
    fx.radialBlurFactor = Core::lerp(_blur1, _blur2, a);
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
}
