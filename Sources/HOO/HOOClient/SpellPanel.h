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
#ifndef HOOCLIENT_SPELLPANEL_H_
#define HOOCLIENT_SPELLPANEL_H_

#include <GUI/Widget.h>
#include <GUI/BitmapLabel.h>
#include <GUI/Label.h>
#include <GUI/Button.h>
#include <GUI/Picture.h>
#include <GUI/RichTextLabel.h>

#include <HOOUserProtocol/GameMessage/GameData.h>

namespace Core
{
    class Dictionary;
}

namespace GameEngine
{
    class Spell;
}

class WidgetFactory;

class SpellPanel : public GUI::Widget
{
public:
    SpellPanel(const GameEngine::Spell & spell, WidgetFactory & factory);

    // Pass panel
    SpellPanel(WidgetFactory & factory);

    int32 getId() const { return _spellId; };

    void setSpellEnabled(bool enabled);
    void setStatus(const HOOUserProtocol::PlayerStatus & status);

protected:
    Ptr<GUI::Picture>       _pPanel;

    bool                    _isSpellEnabled;
    bool                    _isRetribution;
    int32                   _spellId;
    int32                   _playerMana;
    int32                   _sphereId;

    Ptr<GUI::Label>         _pTitle;
    Ptr<GUI::Label>         _pWhy;
    Ptr<GUI::Label>         _pEnergyControl;
    Ptr<GUI::Label>         _pLifeControl;
    Ptr<GUI::Label>         _pSpiritControl;
    Ptr<GUI::Picture>       _pPicture;
    Ptr<GUI::Picture>       _pSpheresCircle;

    Ptr<GUI::Widget>        _pEffectsPanel;
    Core::List<Ptr<GUI::Label> > _effects;

    Ptr<GUI::RichTextLabel> _pDetailsLabel;
};

#endif
