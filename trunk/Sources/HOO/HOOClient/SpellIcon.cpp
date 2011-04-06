/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include <GUI/BitmapLabel.h>

#include <HOOClient/SpellIcon.h>
#include <HOOClient/GUI/WidgetFactory.h>

#include <GameEngine/Spell.h>

static const float PADDING = 5.0f;
static const float APPEAR_TIME = 0.25f;

SpellIcon::SpellIcon(const GameEngine::Spell & spell, WidgetFactory & factory) :
    _pInput(factory.getManager()->getInput()),
    _spellId(spell.getId()),
    //_baseCost(spell.getCost()),
    _sphereId(int32(spell.getSphere())),
    _state(SIIS_DEFAULT)
{
    _pPicture = factory.createPicture(L"/HOO/GUI/Spells/" + Core::toString(spell.getId(), 4) + L".tga", 0, 0);
    _picNaturalSize = _pPicture->getSize();
    _naturalSize = _picNaturalSize + 2.0f * Core::Vector2f(PADDING, PADDING);
    _size = _naturalSize;

    String spellIcon(L"/HOO/GUI/Widgets/icon-");

    switch(spell.getType())
    {
    case GameEngine::SPELLTYPE_SPELL:           spellIcon << L"spell-"; break;
    case GameEngine::SPELLTYPE_COUNTER:         spellIcon << L"counter-"; break;
    case GameEngine::SPELLTYPE_RETRIBUTION:     spellIcon << L"retribution-"; break;
    case GameEngine::SPELLTYPE_TRICK:           spellIcon << L"trick-"; break;
    }

    switch(spell.getSphere())
    {
    case GameEngine::SPHERE_ENERGY: spellIcon << L"r.tga"; break;
    case GameEngine::SPHERE_SPIRIT: spellIcon << L"b.tga"; break;
    case GameEngine::SPHERE_LIFE:   spellIcon << L"g.tga"; break;
    }

    //_pManaIcon = factory.createPicture(spellIcon, 0, 0);
    /*_pManaText = factory.createSmallDigitsLabel(Core::toString(spell.getCooldown()), 15, 0);*/
    //_pManaText = factory.createSmallDigitsLabel(L"0", 15, 0);
    //_pManaText->setHLayout(GUI::HL_LEFT);
    //_pManaText->setVLayout(GUI::VL_CENTER);

    //_pCooldownIcon = factory.createPicture(L"/HOO/GUI/Widgets/icon-cooldown.tga", 40, 40);
    //_pCooldownText = factory.createSmallDigitsLabel(L"0", 60, 50);
    //_pCooldownText->setHLayout(GUI::HL_LEFT);
    //_pCooldownText->setVLayout(GUI::VL_CENTER);

    addChild(_pPicture);
    //_pPicture->addChild(_pManaIcon);
    //_pPicture->addChild(_pManaText);
    //_pPicture->addChild(_pCooldownIcon);
    //_pPicture->addChild(_pCooldownText);

    setId(spell.getId());
    setSize(_naturalSize);
    //setCooldown(0);
    setSpellEnabled(false);
}

SpellIcon::SpellIcon( WidgetFactory & factory) :
    _pInput(factory.getManager()->getInput()),
    _spellId(0),
    _baseCost(0),
    _sphereId(-1),
    _state(SIIS_DEFAULT)
{
    _pPicture = factory.createPicture(L"/HOO/GUI/Spells/0000.tga", 0, 0);
    _picNaturalSize = _pPicture->getSize();
    _naturalSize = _picNaturalSize + 2.0f * Core::Vector2f(PADDING, PADDING);
    _size = _naturalSize;

    addChild(_pPicture);

    setId(0);
    setSize(_naturalSize);
    //setCooldown(0);
    setSpellEnabled(false);
}

//void SpellIcon::setCooldown(int32 cooldown)
//{
//    if(_pCooldownText != null && _pCooldownIcon != null)
//    {
//        _pCooldownText->setText(Core::toString(cooldown));
//        _pCooldownIcon->setVisible(cooldown > 0);
//        _pCooldownText->setVisible(cooldown > 0);
//    }
//}

void SpellIcon::setSpellEnabled(bool enabled)
{
    static const float GREY = 0.25f;

    _isSpellEnabled = enabled;

    if(enabled)
        _pPicture->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    else
        _pPicture->setOwnColor(Core::Vector4f(GREY, GREY, GREY, 1.0f));
}

void SpellIcon::onSizeChanged()
{
    Widget::onSizeChanged();

    float rawRatio = _size.x / _naturalSize.x;
    float ratio = 1.0f - (0.25f * (1.0f - rawRatio));

    Core::Vector2f manaIconSize = ratio * Core::Vector2f(22.0f, 22.0f);

    Core::Vector2f basePos(rawRatio * Core::Vector2f(PADDING, PADDING));

    _pPicture->setSize(rawRatio * _picNaturalSize);
    _pPicture->setPosition(basePos);
    
    /*if(_pManaIcon != null && _pManaText != null && _pCooldownIcon != null && _pCooldownText != null)
    {
        _pManaIcon->setSize(manaIconSize);
        _pManaIcon->setPosition(Core::Vector2f(0.0f, 0.0f));

        _pManaText->setSize(Core::Vector2f(0.0f, manaIconSize.y));
        _pManaText->setZoomFactor(ratio);
        _pManaText->setPosition(Core::Vector2f(manaIconSize.x, 0.0f));

        Core::Vector2f cooldownSize(rawRatio * Core::Vector2f(30.0f, 30.0f));
        _pCooldownIcon->setSize(cooldownSize);
        _pCooldownText->setSize(Core::Vector2f(0.0f, 0.0f));

        _pCooldownIcon->setPosition(0.5f * _size - 0.5f * cooldownSize - (2.0f * (rawRatio - 0.5f)) * Core::Vector2f(10.0f, 0.0f));

        _pCooldownText->setPosition(0.5f * _size + rawRatio * Core::Vector2f(5.0f, 0.0f));
        _pCooldownText->setZoomFactor(ratio);
    }*/
}

/*
 * Le système actuel n'a plus d'information de cooldown ni de coût à afficher. Mais cette méthode peut
 * éventuellement être amené à modifier d'autres informations de statut.
 */
void SpellIcon::setStatus(const HOOUserProtocol::PlayerStatus & status)
{
//    bool cooldown0 = true;
//
//    for(int32 ii=0; ii < status.cooldowns.size(); ii++)
//    {
//        if(status.cooldowns[ii].spellId == _spellId)
//        {
//            setCooldown(status.cooldowns[ii].cooldown);
//            cooldown0 = false;
//        }
//    }
//
//    if(cooldown0)
//        setCooldown(0);
//
//    if(_pManaText != null)
//    {
//        int32 cost = std::max(0, _baseCost - status.spheres[_sphereId].chaining + status.spheres[0].impair);
//        _pManaText->setText(Core::toString(cost));
//
//        if(cost < _baseCost)
//            _pManaText->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);
//        else if(cost > _baseCost)
//            _pManaText->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);
//        else
//            _pManaText->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
//    }
}

void SpellIcon::update(double elapsed)
{
    Widget::update(elapsed);

    if(!isActive())
        return;

    Core::Vector2f lm = _pInput->getMousePos() - _screenPos;
    bool mouseOver = (lm.x >= 0.0f) && (lm.y >= 0.0f) && (lm.x < _size.x) && (lm.y < _size.y);
    bool lmbDown = _pInput->isLMBDown() && _isSpellEnabled;

    switch(_state)
    {
    case SIIS_DEFAULT:
        if(mouseOver)
        {
            if(!lmbDown)
            {
                _state = SIIS_MOUSE_OVER;
                notify(GUI::EV_OVER_BEGIN);
            }
        }
        break;

    case SIIS_MOUSE_OVER:
        if(mouseOver)
        {
            if(lmbDown)
            {
                _state = SIIS_PUSHED_OVER;
            }
        }
        else
        {
            _state = SIIS_DEFAULT;
            notify(GUI::EV_OVER_END);
        }
        break;

    case SIIS_PUSHED_OVER:
        if(mouseOver)
        {
            if(!lmbDown)
            {
                notify(GUI::EV_COMMAND);
                _state = SIIS_MOUSE_OVER;
            }
        }
        else
        {
            _state = SIIS_PUSHED_NOT_OVER;
        }
        break;

    case SIIS_PUSHED_NOT_OVER:
        if(lmbDown)
        {
            if(mouseOver)
            {
                _state = SIIS_PUSHED_OVER;
            }
        }
        else
        {
            _state = SIIS_DEFAULT;
            notify(GUI::EV_OVER_END);
        }
        break;
    }
}

