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
#ifndef HOOCLIENT_SPELLICON_H_
#define HOOCLIENT_SPELLICON_H_

#include <GUI/Label.h>
#include <GUI/Picture.h>

#include <HOOUserProtocol/GameMessage/GameData.h>

namespace GUI
{
    class BitmapLabel;
}

namespace Core
{
    class Dictionary;
}

namespace GameEngine
{
    class Spell;
}

class WidgetFactory;

LM_ENUM_4(ESpellIconInternalState,
          SIIS_DEFAULT,
          SIIS_MOUSE_OVER,
          SIIS_PUSHED_OVER,
          SIIS_PUSHED_NOT_OVER);

class SpellIcon : public GUI::Widget
{
public:
    SpellIcon(const GameEngine::Spell & spell, WidgetFactory & factory);

    // Icon pass
    SpellIcon(WidgetFactory & factory);

    int32 getSpellId() const { return _spellId; }

    void setSpellEnabled(bool enabled);

    void setStatus(const HOOUserProtocol::PlayerStatus & status);

    virtual void update(double elapsed);

protected:
    //void setCooldown(int32 cooldown);
    virtual void onSizeChanged();

    Ptr<Window::InputManager>   _pInput;
    ESpellIconInternalState _state;

    bool                    _isSpellEnabled;
    int32                   _spellId;
    int32                   _baseCost;
    int32                   _sphereId;

    Core::Vector2f          _picNaturalSize;
    Core::Vector2f          _naturalSize;

    Ptr<GUI::Picture>       _pPicture;
    //Ptr<GUI::Picture>       _pManaIcon;
    //Ptr<GUI::BitmapLabel>   _pManaText;

    Ptr<GUI::Picture>       _pCooldownIcon;
    Ptr<GUI::BitmapLabel>   _pCooldownText;
};

#endif
