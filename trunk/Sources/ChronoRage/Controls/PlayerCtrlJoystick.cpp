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
#include <Controls/PlayerCtrlJoystick.h>
#include <Core/Math/GeometryComputations.h>
#include <Core/Math/GeometryIntersect.h>

namespace ChronoRage
{

const float DEFAULT_RADIAL_SPEED = 20.0f;

//-----------------------------------------------------------------------------
PlayerCtrlJoystick::PlayerCtrlJoystick(const Ptr<Window::IJoystick> & pJoy, const JoySettings & settings) :
    _pJoy(pJoy),
    _settings(settings),
    _dir(0.0f, 1.0f, 0.0f),
    _fire(false)
{
    setDefaultRadialSpeed();
}
//-----------------------------------------------------------------------------
void PlayerCtrlJoystick::setDefaultRadialSpeed()
{
    setRadialSpeed(DEFAULT_RADIAL_SPEED);
}
//-----------------------------------------------------------------------------
Core::Vector3f PlayerCtrlJoystick::getMoveDir() const
{
    Core::Vector3f direction(_pJoy->xAxisL(), -_pJoy->yAxisL(), 0.0f);

    if(_settings.inverseSticks)
        direction.set(_pJoy->xAxisR(), -_pJoy->yAxisR(), 0.0f);

    if(direction.getLengthSafe() > 1.0f)
        direction.normalizeFast();

    return direction;
}
//-----------------------------------------------------------------------------
void PlayerCtrlJoystick::shakeDie()
{
    _pJoy->shake(1.0f, 0.0f, 0.5f);
}
//-----------------------------------------------------------------------------
void PlayerCtrlJoystick::update(double elapsed)
{
    Core::Vector3f joyDir(_pJoy->xAxisR(), -_pJoy->yAxisR(), 0.0f);

    if(_settings.inverseSticks)
        joyDir.set(_pJoy->xAxisL(), -_pJoy->yAxisL(), 0.0f);

    if(!joyDir.isZero())
    {
        float intensity = joyDir.getLengthSafe();
        
        bool firing = false;
        if(_settings.autofire)
        {
            firing = !_pJoy->isButtonDown(_settings.fireButton);
            _fire = firing && (intensity > 0.5f);
        }
        else
        {
            _fire = _pJoy->isButtonDown(_settings.fireButton);
        }

        joyDir *= 1.0f / intensity;

        float angle = Core::L_acosSafe(Core::dot(_dir, joyDir));
        angle = std::min(angle, intensity * float(elapsed) * getRadialSpeed());

        Core::Vector3f zVec(Core::cross(_dir, joyDir));
        
        if(zVec.isZero())
            zVec = Core::Vector3f(0.0f, 0.0f, 1.0f);

        if(!Core::isEquivalent(angle, 0.0f))
        {
            zVec.normalizeSafe();
            Core::Matrix4f matR(Core::ROTATION, angle, zVec);
            _dir = matR.apply3x3(_dir);
            _dir.normalizeSafe();
        }
    }
    else
    {
        _fire = false;
    }
}
//-----------------------------------------------------------------------------
Core::Vector3f PlayerCtrlJoystick::getShootDir() const
{
    return _dir;
}
//-----------------------------------------------------------------------------
void PlayerCtrlJoystick::setViewDimensions(int32 width, int32 height)
{
}
//-----------------------------------------------------------------------------
void PlayerCtrlJoystick::setPlayerPos(const Core::Vector3f & pos)
{
}
//-----------------------------------------------------------------------------
bool PlayerCtrlJoystick::fire() const
{
    return _fire;
}
//-----------------------------------------------------------------------------
bool PlayerCtrlJoystick::timeWarp() const
{
    return _pJoy->isButtonDown(_settings.timeWarpButton);
}
//-----------------------------------------------------------------------------
}
