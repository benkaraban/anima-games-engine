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
#include <Core/Strings/Tools.h>
#include <Window/Window.h>
#include <Window/InputManager.h>
#include <Window/JoystickDI.h>
#include <Window/JoystickXI.h>

namespace Window
{
//-----------------------------------------------------------------------------
InputManager::InputManager() :
    _xMouse(0),
    _yMouse(0),
    _mouseWheel(0),
    _isLMBDown(false),
    _isMMBDown(false),
    _isRMBDown(false),
    _isLMBClicked(false),
    _isMMBClicked(false),
    _isRMBClicked(false),
    _isLMBDoubleClicked(false)
{
    for(int32 ii=0; ii < KEY_COUNT; ii++)
    {
        _isKeyTyped[ii] = false;
        _isKeyDown[ii] = false;
        _isSystemKey[ii] = false;
        _isKeyReleased[ii] = false;
        _isKeyReleasedObsolete[ii] = true;
    }

    _isSystemKey[VK_LCONTROL] = true;
    _isSystemKey[VK_RCONTROL] = true;
    _isSystemKey[VK_CONTROL] = true;
    _isSystemKey[VK_LSHIFT] = true;
    _isSystemKey[VK_RSHIFT] = true;
    _isSystemKey[VK_SHIFT] = true;
    _isSystemKey[VK_LMENU] = true;
    _isSystemKey[VK_RMENU] = true;
    _isSystemKey[VK_RETURN] = true;
}
//-----------------------------------------------------------------------------
bool InputManager::isLMBClicked() const
{
    return _isLMBClicked;
}
//-----------------------------------------------------------------------------
bool InputManager::isLMBDown() const
{
    return _isLMBDown;
}
//-----------------------------------------------------------------------------
bool InputManager::isMMBClicked() const
{
    return _isMMBClicked;
}
//-----------------------------------------------------------------------------
bool InputManager::isMMBDown() const
{
    return _isMMBDown;
}
//-----------------------------------------------------------------------------
bool InputManager::isRMBClicked() const
{
    return _isRMBClicked;
}
//-----------------------------------------------------------------------------
bool InputManager::isRMBDown() const
{
    return _isRMBDown;
}
//-----------------------------------------------------------------------------
bool InputManager::isLMBDoubleClicked() const
{
    return _isLMBDoubleClicked;
}
//-----------------------------------------------------------------------------
bool InputManager::isKeyTyped(int32 keyCode) const
{
    LM_ASSERT(keyCode >=0 && keyCode < KEY_COUNT);
    return _isKeyTyped[keyCode];
}
//-----------------------------------------------------------------------------
int32 InputManager::getKeyTyped() const
{
    int32 keyCode = 0;

    while(keyCode < KEY_COUNT && (!isKeyTyped(keyCode) || keyCode == VK_SHIFT || keyCode == VK_MENU || keyCode == VK_CONTROL))
        keyCode++;

    if(keyCode >= KEY_COUNT)
        return -1;
    else
    {
        if(keyCode == VK_LCONTROL && isKeyTyped(VK_RMENU))
            return VK_RMENU;
        else
            return keyCode;
    }
}
//-----------------------------------------------------------------------------
bool InputManager::isKeyDown(int32 keyCode) const
{
    LM_ASSERT(keyCode >=0 && keyCode < KEY_COUNT);
    return _isKeyDown[keyCode];
}
//-----------------------------------------------------------------------------
bool InputManager::isKeyReleased(int32 keyCode) const
{
    LM_ASSERT(keyCode >=0 && keyCode < KEY_COUNT);
    return _isKeyReleased[keyCode];
}
//-----------------------------------------------------------------------------
int32 InputManager::getMouseXDelta() const
{
    return _xMouseDelta;
}
//-----------------------------------------------------------------------------
int32 InputManager::getMouseYDelta() const
{
    return _yMouseDelta;
}
//-----------------------------------------------------------------------------
int32 InputManager::getMouseXPos() const
{
    return _xMouse;
}
//-----------------------------------------------------------------------------
int32 InputManager::getMouseYPos() const
{
    return _yMouse;
}
//-----------------------------------------------------------------------------
Core::Vector2f InputManager::getMousePos() const
{
    return Core::Vector2f(float(_xMouse), float(_yMouse));
}
//-----------------------------------------------------------------------------
int32 InputManager::getMouseWheelDelta() const
{
    return _mouseWheel;
}
//-----------------------------------------------------------------------------
void InputManager::update()
{
    BYTE keyState[KEY_COUNT];

    if(GetKeyboardState(keyState))
    {
        for(int32 ii=0; ii < KEY_COUNT; ii++)
        {
            if(_isSystemKey[ii])
            {
                if((keyState[ii] & 0x80) != 0)
                {
                    if(_isKeyDown[ii])
                    {
                        _isKeyTyped[ii] = false;
                    }
                    else
                    {
                        _isKeyDown[ii] = true;
                        _isKeyTyped[ii] = true;
                    }
                }
                else
                {
                    _isKeyDown[ii] = false;
                    _isKeyTyped[ii] = false;
                }
            }
            else
            {
                _isKeyDown[ii] = ((keyState[ii] & 0x80) != 0);
                _isKeyTyped[ii] = false;
                if(_isKeyReleasedObsolete[ii])
                    _isKeyReleased[ii] = false;
                else
                    _isKeyReleasedObsolete[ii] = true;
            }
        }
    }

    _xMouseDelta = false;
    _yMouseDelta = false;
    _mouseWheel = false;
    _isLMBClicked = false;
    _isMMBClicked = false;
    _isRMBClicked = false;
    _isLMBDoubleClicked = false;

    Core::removeControlChars(_incomingBuffer, _keyboardBuffer);
    _incomingBuffer.clear();

    for(int32 ii=0; ii < _joysticks.size(); ii++)
        _joysticks[ii]->update();
}
//-----------------------------------------------------------------------------
void InputManager::keyDown(int32 keyCode)
{
    if(!_isSystemKey[keyCode])
        _isKeyTyped[keyCode] = true;
}
//-----------------------------------------------------------------------------
void InputManager::keyUp(int32 keyCode)
{
    if(!_isSystemKey[keyCode])
    {
        _isKeyTyped[keyCode] = false;
        _isKeyReleased[keyCode] = true;
        _isKeyReleasedObsolete[keyCode] = false;
    }
}
//-----------------------------------------------------------------------------
void InputManager::mouseMove(int32 x, int32 y)
{
    _xMouseDelta += (x - _xMouse);
    _yMouseDelta += (y - _yMouse);
    _xMouse = x;
    _yMouse = y;
}
//-----------------------------------------------------------------------------
void InputManager::mouseWheel(int32 wheel)
{
    _mouseWheel += wheel;
}
//-----------------------------------------------------------------------------
void InputManager::mouseLB(bool down)
{
    if(down)
    {
        _isLMBClicked = (!_isLMBDown);
        _isLMBDown = true;
    }
    else
    {
        _isLMBDown = false;
    }
}
//-----------------------------------------------------------------------------
void InputManager::mouseRB(bool down)
{
    if(down)
    {
        _isRMBClicked = (!_isRMBDown);
        _isRMBDown = true;
    }
    else
    {
        _isRMBDown = false;
    }
}
//-----------------------------------------------------------------------------
void InputManager::mouseMB(bool down)
{
    if(down)
    {
        _isMMBClicked = (!_isMMBDown);
        _isMMBDown = true;
    }
    else
    {
        _isMMBDown = false;
    }
}
//-----------------------------------------------------------------------------
void InputManager::mouseLBDoubleClick()
{
    _isLMBDoubleClicked = true;
}
//-----------------------------------------------------------------------------
void InputManager::mouseBoutons(int32 flags)
{
    mouseLB((flags & FLAG_LBUTTON) != 0);
    mouseMB((flags & FLAG_MBUTTON) != 0);
    mouseRB((flags & FLAG_RBUTTON) != 0);
}
//-----------------------------------------------------------------------------
const String & InputManager::getKeyboardBuffer() const
{
    return _keyboardBuffer;
}
//-----------------------------------------------------------------------------
void InputManager::character(wchar_t c)
{
    _incomingBuffer.push_back(c);
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<IJoystick> > & InputManager::getJoysticks() const
{
    return _joysticks;
}
//-----------------------------------------------------------------------------
void InputManager::initialise(void * hwnd)
{
    JoystickXI::enumerateJoysticks(_joysticks);
    JoystickDI::enumerateJoysticks(_joysticks, hwnd);
}
//-----------------------------------------------------------------------------
}
