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
#ifndef CHRONORAGE_PLAYERCTRLKEYBOARD_H_
#define CHRONORAGE_PLAYERCTRLKEYBOARD_H_

#include <Controls/IPlayerCtrl.h>
#include <Universe/NodeCamera.h>
#include <Window/InputManager.h>
#include <GameSettings.h>
namespace ChronoRage
{
class PlayerCtrlKeyboard : public Core::Object, public IPlayerCtrl
{
public:
    PlayerCtrlKeyboard(const Ptr<Window::InputManager> & pInput, const Ptr<GameSettings> & pGameSettings);

    virtual void setViewDimensions(int32 width, int32 height);
    virtual void setPlayerPos(const Core::Vector3f & pos);

    virtual bool showCursor() const { return false; }

    virtual void shakeDie() {};

    virtual void setDefaultRadialSpeed();

    virtual Core::Vector3f getMoveDir() const;
    virtual Core::Vector3f getShootDir() const;
    virtual void setShootDir(const Core::Vector3f & dir) { _dir = dir; };

    virtual bool fire() const;
    virtual bool timeWarp() const;

    virtual void update(double elapsed);

protected:
    Ptr<Window::InputManager>       _pInput;
    Ptr<GameSettings>               _pGameSettings;
    Core::Vector3f                  _dir;
};
}

#endif
