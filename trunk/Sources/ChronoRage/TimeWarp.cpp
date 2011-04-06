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
#include "TimeWarp.h"

#include <Universe/Tasks/PostFX.h>

namespace ChronoRage
{

TimeWarp::TimeWarp(const Ptr<Universe::World> & pWorld, const Ptr<Universe::NodeCamera> & pCamera, const Ptr<Player> & pPlayer)
:   _pWorld(pWorld),
    _pCamera(pCamera),
    _pPlayer(pPlayer),
    _timeWarpStarted(false),
    _timeWarpElapsed(0.0f)
{}

TimeWarp::~TimeWarp()
{
    if(_pTimeWarpSound != null)
    {
        _pTimeWarpSound->kill();
        _pTimeWarpSound = null;
    }
}

void TimeWarp::start(Ptr<Universe::NodeSoundSource> pSound)
{
    Ptr<Universe::ITask> pTaskFlash(new Universe::TaskFXFade(   _pCamera,
                                                                Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                                                                Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f),
                                                                0.2,
                                                                Core::INTER_LINEAR));
    _pWorld->registerTask(pTaskFlash);

    _timeWarpStarted = true;
    _timeWarpElapsed = 0.0f;
    _pTimeWarpSound = pSound;
    _doFadeOut = true;
}

void TimeWarp::end()
{
    Renderer::PostFX fx = _pCamera->getBaseCamera()->getPostFX();
    fx.saturation = 1.0f;
    fx.radialBlurFactor = 0.0f;
    _pCamera->getBaseCamera()->setPostFX(fx);


    _timeWarpStarted = false;
    _timeWarpElapsed = 0.0f;

    if(_pTimeWarpSound != null)
    {
        _pTimeWarpSound->kill();
        _pTimeWarpSound = null;
    }
}

void TimeWarp::update(float elapsed)
{
    _timeWarpElapsed = std::min(TIME_WARP_DURATION, _timeWarpElapsed + elapsed);

    if((_timeWarpElapsed >= (TIME_WARP_DURATION - TIME_WARP_TRANSITION)) && _doFadeOut)
    {
        if(_pTimeWarpSound != null)
        {
            _pTimeWarpSound->fadeOut(0.8f);
        }
        _doFadeOut = false;
    }

    manageFX();

    if(_timeWarpElapsed >= TIME_WARP_DURATION)
    {
        end();
    }
}

float TimeWarp::getTimeWarpFactor() const
{
    if(!_timeWarpStarted)
        return 1.0f;

    float timeWarpFactor = 1.0f;
    if(_timeWarpElapsed <= TIME_WARP_TRANSITION)
    {
        timeWarpFactor = Core::lerp(0.0f, TIME_WARP_MIN_FACTOR, _timeWarpElapsed / TIME_WARP_TRANSITION);
    }
    else if(_timeWarpElapsed >= (TIME_WARP_DURATION - TIME_WARP_TRANSITION) )
    {
        float timeWarpTransition = _timeWarpElapsed - (TIME_WARP_DURATION - TIME_WARP_TRANSITION);
        timeWarpFactor = Core::lerp(TIME_WARP_MIN_FACTOR, 1.0f, timeWarpTransition / TIME_WARP_TRANSITION);
    }
    else
        timeWarpFactor = TIME_WARP_MIN_FACTOR;
    
    return timeWarpFactor;
}

void TimeWarp::manageFX()
{
    if(_timeWarpStarted)
    {
        Renderer::PostFX fx = _pCamera->getBaseCamera()->getPostFX();
        fx.saturation = Core::lerp(0.25f, 1.0f, getTimeWarpFactor());

        if(_timeWarpElapsed >= (TIME_WARP_DURATION - TIME_WARP_TRANSITION) )
        {
            Core::Matrix4f view, proj, viewProj;
            _pCamera->getBaseCamera()->getViewMatrix(view);
            _pCamera->getBaseCamera()->getProjectionMatrix(proj);
            viewProj = proj * view;

            Core::Vector3f wpos = _pPlayer->getPosition();
            Core::Vector3f cpos = viewProj.apply(Core::Vector4f(wpos, 1.0f)).homogeneousDiv();

            cpos += Core::Vector3f(1.0f, 1.0f, 0.0f);
            cpos *= 0.5f;
            cpos.y = 1.0f - cpos.y;

            Core::Vector2f center(0.0f, 0.0f);
            center.x = cpos.x;
            center.y = cpos.y;
            

            fx.radialBlurCenter = center;
            fx.radialBlurFactor = Core::lerp(1.0f, 0.0f, getTimeWarpFactor());
        }

        _pCamera->getBaseCamera()->setPostFX(fx);
    }
}

}//namespace ChronoRage
