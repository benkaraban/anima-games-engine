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
#ifndef WINDOW_JOYSTICK_DI_H_
#define WINDOW_JOYSTICK_DI_H_

#include <Core/List.h>
#include <Window/IJoystick.h>

namespace Window
{
class LM_API_WIN JoystickDI : public Core::Object, public IJoystick
{
public:
    JoystickDI(IDirectInputDevice8 * pDevice, const String & name);
    virtual ~JoystickDI();

    virtual float xAxisL() const;
    virtual float yAxisL() const;

    virtual float xAxisR() const;
    virtual float yAxisR() const;

    virtual int32 getButtonTyped() const;

    virtual bool isButtonTyped(int32 n) const;
    virtual bool isButtonDown(int32 n) const;

    virtual void startVibrations(float intensity);
    virtual void stopVibrations();

    virtual void shake(float intensity, float duration);
    virtual void shake(float intensity1, float intensity2, float duration);

    virtual const String & getName() const;
    virtual String getButtonName(int32 index) const;

    virtual void update();

    static void enumerateJoysticks(Core::List<Ptr<IJoystick> > & joysticks, void * hwnd);

protected:
    IDirectInputDevice8 *   _pDevice;
    IDirectInputEffect  *   _pShakeLoop;
    Core::List<IDirectInputEffect*> _shakes;
    int32                   _iShake;
    String                  _name;
    float                   _xAxis;
    float                   _yAxis;
    float                   _zAxis;
    float                   _zRoll;
    Core::List<bool>        _buttonDown;
    Core::List<bool>        _buttonTyped;
};
}

#endif
