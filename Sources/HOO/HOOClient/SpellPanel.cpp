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
#include <Core/Logger.h>
#include <HOOClient/SpellPanel.h>
#include <HOOClient/GUI/WidgetFactory.h>
#include <GameEngine/Spell.h>

static const float GREY             = 0.5f;
static const int32 PANEL_WIDTH      = 256;
static const int32 PANEL_HEIGHT     = 380;
static const int32 X_BORDER         = 10;
static const int32 INTERNAL_WIDTH   = PANEL_WIDTH - (2 * X_BORDER);
//--------------------------------------------------------------------------------
SpellPanel::SpellPanel(const GameEngine::Spell & spell, WidgetFactory & factory) :
    _isRetribution(spell.getType() == GameEngine::SPELLTYPE_RETRIBUTION),
    _spellId(spell.getId()),
    _sphereId(int32(spell.getSphere()))
{
    _pPanel = factory.createPicture(L"/HOO/GUI/Widgets/panel-spell-details.tga", 0, 0);

    _pTitle = factory.createTitleLabel(L"", X_BORDER, 10);
    _pTitle->setSize(Core::Vector2f(float(INTERNAL_WIDTH), 45.0f));
    _pTitle->setDictionary(null);
    _pTitle->setBold(false);
    _pTitle->setShadow(false);
    _pTitle->setText(spell.getName());

    _pWhy = factory.createTitleLabel(L"", X_BORDER, 30);
    _pWhy->setSize(Core::Vector2f(float(INTERNAL_WIDTH), 45.0f));
    _pWhy->setFontSize(9);
    _pWhy->setOwnColor(Core::Vector4f(0.0f));
    _pWhy->setBold(false);
    _pWhy->setShadow(false);

    try
    {
        _pPicture = factory.createPicture(L"/HOO/GUI/Spells/" + Core::toString(spell.getId(), 4) + L".tga", 0, 0);
    }
    catch(Core::Exception & e)
    {
        ERR << L"Unable to load spell icon  : " << e.getMessage() << L"\n";
        _pPicture = factory.createPicture(L"/HOO/GUI/Spells/0000.tga", 0, 0);
    }

    _pPicture->setPosition(Core::Vector2f(0.5f * (float(PANEL_WIDTH) - _pPicture->getSize().x), 65.0f));

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

    //-----------------------------------------------------------------------------------
    // Panneau des effets
    //-----------------------------------------------------------------------------------
    _pEffectsPanel = Ptr<GUI::Widget>(new GUI::Widget());

    int32 y = 0;
    const Core::List<GameEngine::SpellEffect> & effectsOnCast = *spell.getEffectsOnCast();
    const Core::List<GameEngine::SpellEffect> & effectsOnHit = *spell.getEffectsOnHit();

    for(int32 ii=0; ii < effectsOnCast.size(); ii++)
    {
        Ptr<GUI::Label> pLabel = factory.createMediumLabel(L"", 0, y);

        pLabel->setDictionary(null);
        pLabel->setShadow(false);
        pLabel->setFontSize(9);
        pLabel->setTextColor(Core::Vector4f(0.25f,0.75f,1.0f,1.0f));
        pLabel->setText(L"- " + effectsOnCast[ii].getDescription(GameEngine::DETAILED));
        pLabel->fitText(true, true);

        _pEffectsPanel->addChild(pLabel);

        y += int32(pLabel->getSize().y);
    }
    for(int32 ii=0; ii < effectsOnHit.size(); ii++)
    {
        Ptr<GUI::Label> pLabel = factory.createMediumLabel(L"", 0, y);

        pLabel->setDictionary(null);
        pLabel->setShadow(false);
        pLabel->setFontSize(9);
        pLabel->setTextColor(Core::Vector4f(1.0f,1.0f,0.75f,1.0f));
        pLabel->setText(L"- " + effectsOnHit[ii].getDescription(GameEngine::DETAILED));
        pLabel->fitText(true, true);

        _pEffectsPanel->addChild(pLabel);

        y += int32(pLabel->getSize().y);
    }

    _pEffectsPanel->pack();
    _pEffectsPanel->setPosition(Core::Vector2f(0.4f * PANEL_WIDTH, 225.0f));

    _pDetailsLabel = factory.createLargeRTLabel(L"", X_BORDER + 10, y + 170, 0);
    _pDetailsLabel->setShadow(false);
    _pDetailsLabel->setFontSize(10);
    _pDetailsLabel->setDictionary(null);
    _pDetailsLabel->setSize(Core::Vector2f(float(INTERNAL_WIDTH - 20), float(PANEL_HEIGHT - (y + 195))));
    _pDetailsLabel->setText(spell.getDescription(GameEngine::DETAILED));

    float dy = 0.5f * (PANEL_HEIGHT - _pDetailsLabel->getPosition().y - _pDetailsLabel->getYLine());
    _pDetailsLabel->setPosition(Core::Vector2f(float(X_BORDER + 10), float(y + 170) + dy));

    _pSpheresCircle = factory.createPicture(L"/HOO/GUI/Widgets/icon-spheres-circle.tga", 0, 0);
    _pSpheresCircle->setPosition(Core::Vector2f(5.0f, 215.0f));

    _pEnergyControl = factory.createTitleLabel(L"", 40, 25);
    _pEnergyControl->setShadow(true);
    _pEnergyControl->setFontSize(15);
    _pEnergyControl->setDictionary(null);
    _pEnergyControl->setSize(Core::Vector2f(20.0f, 20.0f));
    _pEnergyControl->setText(Core::toString(spell.getSphereControl(GameEngine::SPHERE_ENERGY)));
    if(spell.getSphereControl(GameEngine::SPHERE_ENERGY) > 0)
        _pEnergyControl->setTextColor(Core::Vector4f(0.25f,1.0f,0.25f,1.0f));
    else if(spell.getSphereControl(GameEngine::SPHERE_ENERGY) < 0)
        _pEnergyControl->setTextColor(Core::Vector4f(1.0f,0.25f,0.25f,1.0f));
    else
        _pEnergyControl->setTextColor(Core::Vector4f(0.5f,0.5f,0.5f,1.0f));
    _pSpheresCircle->addChild(_pEnergyControl);

    _pLifeControl = factory.createTitleLabel(L"", 20, 60);
    _pLifeControl->setShadow(true);
    _pLifeControl->setFontSize(15);
    _pLifeControl->setDictionary(null);
    _pLifeControl->setSize(Core::Vector2f(20.0f, 20.0f));
    _pLifeControl->setText(Core::toString(spell.getSphereControl(GameEngine::SPHERE_LIFE)));
    if(spell.getSphereControl(GameEngine::SPHERE_LIFE) > 0)
        _pLifeControl->setTextColor(Core::Vector4f(0.25f,1.0f,0.25f,1.0f));
    else if(spell.getSphereControl(GameEngine::SPHERE_LIFE) < 0)
        _pLifeControl->setTextColor(Core::Vector4f(1.0f,0.25f,0.25f,1.0f));
    else
        _pLifeControl->setTextColor(Core::Vector4f(0.5f,0.5f,0.5f,1.0f));
    _pSpheresCircle->addChild(_pLifeControl);

    _pSpiritControl = factory.createTitleLabel(L"", 60, 60);
    _pSpiritControl->setShadow(true);
    _pSpiritControl->setFontSize(15);
    _pSpiritControl->setDictionary(null);
    _pSpiritControl->setSize(Core::Vector2f(20.0f, 20.0f));
    _pSpiritControl->setText(Core::toString(spell.getSphereControl(GameEngine::SPHERE_SPIRIT)));
    if(spell.getSphereControl(GameEngine::SPHERE_SPIRIT) > 0)
        _pSpiritControl->setTextColor(Core::Vector4f(0.25f,1.0f,0.25f,1.0f));
    else if(spell.getSphereControl(GameEngine::SPHERE_SPIRIT) < 0)
        _pSpiritControl->setTextColor(Core::Vector4f(1.0f,0.25f,0.25f,1.0f));
    else
        _pSpiritControl->setTextColor(Core::Vector4f(0.5f,0.5f,0.5f,1.0f));
    _pSpheresCircle->addChild(_pSpiritControl);

    _pPanel->addChild(_pTitle);
    _pPanel->addChild(_pWhy);
    _pPanel->addChild(_pPicture);
    _pPanel->addChild(_pEffectsPanel);
    _pPanel->addChild(_pDetailsLabel);
    _pPanel->addChild(_pSpheresCircle);

    _pDetailsLabel->setVisible(false);

    addChild(_pPanel);

    pack();
}
//--------------------------------------------------------------------------------
SpellPanel::SpellPanel(WidgetFactory & factory) : _spellId(0)
{
    const Core::Dictionary & dic = *factory.getDictionary();
    _pPanel = factory.createPicture(L"/HOO/GUI/Widgets/panel-spell-details.tga", 0, 0);

    _pTitle = factory.createTitleLabel(L"", X_BORDER, 10);
    _pTitle->setSize(Core::Vector2f(float(INTERNAL_WIDTH), 45.0f));
    _pTitle->setDictionary(null);
    _pTitle->setBold(false);
    _pTitle->setShadow(false);
    _pTitle->setText((*factory.getDictionary())[L"pass-turn"]);

    _pWhy = factory.createTitleLabel(L"", X_BORDER, 25);

    _pPicture = factory.createPicture(L"/HOO/GUI/Spells/0000.tga", 0, 0);
    _pPicture->setPosition(Core::Vector2f(0.5f * (float(PANEL_WIDTH) - _pPicture->getSize().x), 65.0f));

    String spellIcon(L"/HOO/GUI/Widgets/icon-");

    _pEffectsPanel = Ptr<GUI::Widget>(new GUI::Widget());

    int32 y = 21;

    _pEffectsPanel->pack();
    _pEffectsPanel->setPosition(Core::Vector2f(0.5f * (PANEL_WIDTH - _pEffectsPanel->getSize().x), 205.0f));

    _pDetailsLabel = factory.createLargeRTLabel(L"", X_BORDER + 10, y + 170, 0);
    _pDetailsLabel->setShadow(false);
    _pDetailsLabel->setFontSize(11);
    _pDetailsLabel->setDictionary(null);
    _pDetailsLabel->setSize(Core::Vector2f(float(INTERNAL_WIDTH - 20), float(PANEL_HEIGHT - (y + 195))));
    _pDetailsLabel->setText(dic[L"pass-details"]);

    float dy = 0.5f * (PANEL_HEIGHT - _pDetailsLabel->getPosition().y - _pDetailsLabel->getYLine());
    _pDetailsLabel->setPosition(Core::Vector2f(float(X_BORDER + 10), float(y + 170) + dy));

    _pPanel->addChild(_pTitle);
    _pPanel->addChild(_pPicture);
    _pPanel->addChild(_pEffectsPanel);
    _pPanel->addChild(_pDetailsLabel);

    addChild(_pPanel);

    pack();
}

void SpellPanel::setSpellEnabled(bool enabled)
{
    if(enabled || _spellId == 0)
    {
        _pTitle->setOwnColor(Core::Vector4f(1.0f));
        _pPicture->setPictureColor(Core::Vector4f(1.0f));
        _pWhy->setOwnColor(Core::Vector4f(0.0f));
    }
    else
    {
        _pTitle->setOwnColor(Core::Vector4f(GREY, GREY, GREY, 1.0f));
        _pPicture->setPictureColor(Core::Vector4f(GREY, GREY, GREY, 1.0f));
        _pWhy->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

        // TODO tester si le sort posséde des effets de lancement requiérant une
        // maitrise minimum
        /*if(_playerMana < _cost)
            _pWhy->setText(L"not-enough-mana");
        else if(_cooldown > 0)
            _pWhy->setText(L"spell-on-cooldown");
        else if(_isRetribution && !_hasVantage)
            _pWhy->setText(L"requires-vantage");
        else*/
        _pWhy->setText(L"spell-bad-turn");
    }
}
//--------------------------------------------------------------------------------
void SpellPanel::setStatus(const HOOUserProtocol::PlayerStatus & status)
{
    /*bool cooldown0 = true;

    for(int32 ii=0; ii < status.cooldowns.size(); ii++)
    {
        if(status.cooldowns[ii].spellId == _spellId)
        {
            setCooldown(status.cooldowns[ii].cooldown);
            cooldown0 = false;
        }
    }

    if(cooldown0)
        setCooldown(0);*/

    //if(_pManaText != null)
    //{
    //    /*_hasVantage = status.hasVantage;*/
    //    _playerMana = status.spheres[_sphereId].mana;
    //    /*_cost = std::max(0, _baseCost - status.spheres[_sphereId].chaining + status.spheres[0].impair);*/
    //    /*_pManaText->setText(Core::toString(_cost));*/

    //    /*if(_cost < _baseCost)
    //        _pManaText->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    //    else if(_cost > _baseCost)
    //        _pManaText->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    //    else*/
    //        _pManaText->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    //}
}


//--------------------------------------------------------------------------------