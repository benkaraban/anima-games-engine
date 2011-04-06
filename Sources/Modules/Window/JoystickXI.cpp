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
#include <Core/Logger.h>
#include <Core/Math/MathTools.h>
#include <Core/Math/Interpolate.h>
#include <Window/JoystickXI.h>
#include <XInput.h>

namespace Window
{
static const int32 BUTTONS_COUNT = 12;
//-----------------------------------------------------------------------------
static float toFloatInput(int32 x, int32 threshold)
{
    if(x > 0)
        return float(std::max(0, x - threshold)) / float(32767 - threshold);
    else
        return float(std::min(0, x + threshold)) / float(32768 - threshold);
}
//-----------------------------------------------------------------------------
void JoystickXI::enumerateJoysticks(Core::List<Ptr<IJoystick> > & joysticks)
{
    for(int32 ii=0; ii < 4; ii++)
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(state));

        if(XInputGetState(ii, &state) == ERROR_SUCCESS)
        {
            INF << L"XBox pad " << Core::toString(ii + 1) << L" detected\n";
            joysticks.push_back(Ptr<IJoystick>(new JoystickXI(ii)));
        }
    }
}
//-----------------------------------------------------------------------------
JoystickXI::JoystickXI(int32 index) : 
    _index(index),
    _name(L"XBox pad #" + Core::toString(index + 1)),
    _intensity1(0.0f),
    _intensity2(0.0f),
    _duration(0.0f)
{
    _buttonDownNow.resize(BUTTONS_COUNT, false);
    _buttonDown.resize(BUTTONS_COUNT, false);
    _buttonTyped.resize(BUTTONS_COUNT, false);
}
//-----------------------------------------------------------------------------
float JoystickXI::xAxisL() const
{
    return _xAxisL;
}
//-----------------------------------------------------------------------------
float JoystickXI::yAxisL() const
{
    return -_yAxisL;
}
//-----------------------------------------------------------------------------
float JoystickXI::xAxisR() const
{
    return _xAxisR;
}
//-----------------------------------------------------------------------------
float JoystickXI::yAxisR() const
{
    return -_yAxisR;
}
//-----------------------------------------------------------------------------
bool JoystickXI::isButtonTyped(int32 n) const
{
    return _buttonTyped[n];
}
//-----------------------------------------------------------------------------
bool JoystickXI::isButtonDown(int32 n) const
{
    return _buttonDown[n];
}
//-----------------------------------------------------------------------------
void JoystickXI::startVibrations(float intensity)
{
    internalSetVibration(intensity);
}
//-----------------------------------------------------------------------------
void JoystickXI::stopVibrations()
{
    internalSetVibration(0.0f);
}
//-----------------------------------------------------------------------------
void JoystickXI::shake(float intensity, float duration)
{
    shake(intensity, intensity, duration);
}
//-----------------------------------------------------------------------------
void JoystickXI::shake(float intensity1, float intensity2, float duration)
{
    _intensity1 = intensity1;
    _intensity2 = intensity2;
    _duration = duration;
    _timeStart = Core::Clock::localTime();
}
//-----------------------------------------------------------------------------
const String & JoystickXI::getName() const
{
    return _name;
}
//-----------------------------------------------------------------------------
String JoystickXI::getButtonName(int32 index) const
{
    switch(index)
    {
    case 0: return L"A"; break;
    case 1: return L"B"; break;
    case 2: return L"X"; break;
    case 3: return L"Y"; break;
    case 4: return L"LT"; break;
    case 5: return L"RT"; break;
    case 6: return L"LB"; break;
    case 7: return L"RB"; break;
    case 8: return L"BACK"; break;
    case 9: return L"START"; break;
    case 10: return L"LPAD"; break;
    case 11: return L"RPAD"; break;
    }

    return L"???";
}
//-----------------------------------------------------------------------------
void JoystickXI::internalSetVibration(float intensity)
{
    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(vibration));

    intensity = Core::clamp(0.0f, 1.0f, intensity);

    vibration.wLeftMotorSpeed = int32(65535.0f * intensity);
    vibration.wRightMotorSpeed = int32(65535.0f * 0.5f * intensity);

    XInputSetState(_index, &vibration);
}
//-----------------------------------------------------------------------------
void JoystickXI::update()
{
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(state));

    if(XInputGetState(_index, &state) == ERROR_SUCCESS)
    {
        const XINPUT_GAMEPAD & pad = state.Gamepad;

        _buttonDownNow[ 0] = (pad.wButtons & XINPUT_GAMEPAD_A) != 0;
        _buttonDownNow[ 1] = (pad.wButtons & XINPUT_GAMEPAD_B) != 0;
        _buttonDownNow[ 2] = (pad.wButtons & XINPUT_GAMEPAD_X) != 0;
        _buttonDownNow[ 3] = (pad.wButtons & XINPUT_GAMEPAD_Y) != 0;

        _buttonDownNow[ 4] = pad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
        _buttonDownNow[ 5] = pad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

        _buttonDownNow[ 6] = (pad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
        _buttonDownNow[ 7] = (pad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

        _buttonDownNow[ 8] = (pad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
        _buttonDownNow[ 9] = (pad.wButtons & XINPUT_GAMEPAD_START) != 0;

        _buttonDownNow[10] = (pad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
        _buttonDownNow[11] = (pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;

        _xAxisL = toFloatInput(pad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        _yAxisL = toFloatInput(pad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

        _xAxisR = toFloatInput(pad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        _yAxisR = toFloatInput(pad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }
    else
    {
        _xAxisL = 0.0f;
        _yAxisL = 0.0f;

        _xAxisR = 0.0f;
        _yAxisR = 0.0f;

        for(int32 ii=0; ii < _buttonDownNow.size(); ii++)
            _buttonDownNow[ii] = false;
    }

    for(int32 ii=0; ii < _buttonDownNow.size(); ii++)
    {
        if(_buttonDownNow[ii])
        {
            _buttonTyped[ii] = !_buttonDown[ii];
            _buttonDown[ii] = true;
        }
        else
        {
            _buttonTyped[ii] = false;
            _buttonDown[ii] = false;
        }
    }

    if(_duration > 0.0f)
    {
        Core::TimeDuration elapsed = Core::Clock::localTime() - _timeStart;
        float e = float(elapsed.ftotalSeconds()) / _duration;

        if(e > 1.0f)
        {
            internalSetVibration(0.0f);
            _duration = 0.0f;
        }
        else
        {
            internalSetVibration(Core::lerp(_intensity1, _intensity2, e));
        }
    }
}
//-----------------------------------------------------------------------------
int32 JoystickXI::getButtonTyped() const
{
    for(int32 ii=0; ii < _buttonTyped.size(); ii++)
        if(_buttonTyped[ii])
            return ii;
    return -1;
}
//-----------------------------------------------------------------------------
}