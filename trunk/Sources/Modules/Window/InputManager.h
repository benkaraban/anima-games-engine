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
#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Math/Vector.h>

#include <Window/IJoystick.h>

namespace Window
{
class LM_API_WIN InputManager : public Core::Object
{
public:
    InputManager();

    bool isLMBDoubleClicked() const;
    bool isLMBClicked() const;
    bool isLMBDown() const;

    bool isMMBClicked() const;
    bool isMMBDown() const;

    bool isRMBClicked() const;
    bool isRMBDown() const;

    bool isKeyTyped(int32 keyCode) const;
    bool isKeyDown(int32 keyCode) const;
    bool isKeyReleased(int32 keyCode) const;

    int32 getKeyTyped() const;

    int32 getMouseXDelta() const;
    int32 getMouseYDelta() const;

    int32 getMouseXPos() const;
    int32 getMouseYPos() const;

    Core::Vector2f getMousePos() const;

    int32 getMouseWheelDelta() const;

    const String & getKeyboardBuffer() const;

    void update();

    //--------------------------------
    const Core::List<Ptr<IJoystick> > & getJoysticks() const;

    //--------------------------------

    void initialise(void * hwnd);
    void keyDown(int32 keyCode);
    void keyUp(int32 keyCode);
    void mouseMove(int32 x, int32 y);
    void mouseWheel(int32 wheel);
    void mouseBoutons(int32 flags);
    void mouseLBDoubleClick();
    void mouseLB(bool down);
    void mouseMB(bool down);
    void mouseRB(bool down);
    void character(wchar_t c);

protected:
    static const int32 KEY_COUNT = 256;

    bool    _isKeyTyped[KEY_COUNT];
    bool    _isKeyDown[KEY_COUNT];
    bool    _isKeyReleased[KEY_COUNT];
    bool    _isKeyReleasedObsolete[KEY_COUNT];
    bool    _isSystemKey[KEY_COUNT];

    String  _keyboardBuffer;
    String  _incomingBuffer;

    int32   _xMouse;
    int32   _yMouse;
    int32   _xMouseDelta;
    int32   _yMouseDelta;
    int32   _mouseWheel;

    bool    _isLMBDown;
    bool    _isMMBDown;
    bool    _isRMBDown;

    bool    _isLMBClicked;
    bool    _isMMBClicked;
    bool    _isRMBClicked;

    bool    _isLMBDoubleClicked;

    Core::List<Ptr<IJoystick> > _joysticks;
};
}
#endif /* INPUTMANAGER_H_ */
