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
#include "ScreenAchievements.h"

namespace ChronoRage
{

const float ENABLE_CONTINUE_TIMER = 20.0f;
const float SWITCH_PICTURE_TIMER = 7.0f;
const float SWITCH_PICTURE_FADE_OUT = 0.2f;
const float SWITCH_PICTURE_FADE_IN = 0.2f;

ScreenAchievements::ScreenAchievements(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();
    _pPanel = _appContext.pWidgetFactory->createLargePanel();

    _pButtonBack = _appContext.pWidgetFactory->createMediumButton(L"back", 750 - (368 / 2), 610);
    _pButtonBack->setHotKey(VK_ESCAPE);

    _pButtonBack->addEventHandler(this);

    _pPanel->addChild(_pButtonBack);

    for(int32 ii=0; ii < EAchievementType_COUNT; ++ii)
    {
        Core::String picture = GameTools::getAchievementPicture( (EAchievementType) ii);
        Core::String labelDictKey = GameTools::getAchievementDictKey( (EAchievementType) ii);
        Core::String hintDictKey = GameTools::getAchievementHintDictKey( (EAchievementType) ii);
        if(ii < 7)
        {   
            _pAchievementPics[ii] = _appContext.pWidgetFactory->createPicture(picture, 20, (ii+1) * 72);
            _pAchievementLabels[ii] = createLabel(labelDictKey, 90, 10 + ((ii+1) * 72));
            _pAchievementHints[ii] = createHint(hintDictKey, 90, 20 + ((ii+1) * 72));
        }
        else if(ii < 14)
        {
            _pAchievementPics[ii] = _appContext.pWidgetFactory->createPicture(picture, 353, (ii-7+1) * 72);
            _pAchievementLabels[ii] = createLabel(labelDictKey, 423, 10 + ((ii-7+1) * 72));
            _pAchievementHints[ii] = createHint(hintDictKey, 423, 20 + ((ii-7+1) * 72));
        }
        else
        {
            _pAchievementPics[ii] = _appContext.pWidgetFactory->createPicture(picture, 686, (ii-14+1) * 72);
            _pAchievementLabels[ii] = createLabel(labelDictKey, 756, 10 + ((ii-14+1) * 72));
            _pAchievementHints[ii] = createHint(hintDictKey, 756, 20 + ((ii-14+1) * 72));
        }

        _pPanel->addChild(_pAchievementPics[ii]);
        _pPanel->addChild(_pAchievementLabels[ii]);
        _pPanel->addChild(_pAchievementHints[ii]);
    }

    _pPanel->pack();

    _pDialog->addChild(_pPanel);
    _pDialog->pack();


}

Ptr<GUI::Label> ScreenAchievements::createKeyLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_LEFT, GUI::VL_UP);
    pResult->setBold(false);
    pResult->setFontSize(14);
    return pResult;
}

Ptr<GUI::Label> ScreenAchievements::createLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_LEFT, GUI::VL_CENTER);
    pResult->setBold(false);
    pResult->setFontSize(14);
    return pResult;
}

Ptr<GUI::RichTextLabel> ScreenAchievements::createHint(const String & text, int32 x, int32 y)
{
    Ptr<GUI::RichTextLabel> pResult = _appContext.pWidgetFactory->createRichTextHint(text, x, y);
    return pResult;
}

void ScreenAchievements::resetLabels()
{
    _pButtonBack->setDictionary(_appContext.pDico);

    _pButtonBack->setText(L"back");

    for(int32 ii=0; ii < EAchievementType_COUNT; ++ii)
    {
        Core::String labelDictKey = GameTools::getAchievementDictKey( (EAchievementType) ii);
        _pAchievementLabels[ii]->setDictionary(_appContext.pDico);
        _pAchievementLabels[ii]->setText(labelDictKey);

        Core::String hintDictKey = GameTools::getAchievementHintDictKey( (EAchievementType) ii);
        _pAchievementHints[ii]->setDictionary(_appContext.pDico);
        _pAchievementHints[ii]->setText(hintDictKey);
    }
}


void ScreenAchievements::startScreen()
{
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    for(int32 ii=0; ii < EAchievementType_COUNT; ++ii)
    {
        if(_appContext.pHighScores->isAchievementUnlocked( (EAchievementType)ii))
        {
            _pAchievementPics[ii]->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        }
        else
        {
            _pAchievementPics[ii]->setOwnColor(Core::Vector4f(0.5f, 0.5f, 0.5f, 1.0f));
        }
    }
}
    
void ScreenAchievements::endScreen()
{
}

void ScreenAchievements::update(double elapsed)
{
}

void ScreenAchievements::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            _pLastKeyFocus = _pButtonBack.get();

            if(_pScreenNext != null)
            {
                _pManager->setScreen(_pScreenNext.get());
            }
        }
    }
}

}//namespace ChronoRage
