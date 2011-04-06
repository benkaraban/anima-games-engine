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
#include "CRComboBox.h"

#include <MenuScreens/GUI/CRButton.h>

namespace ChronoRage
{

CRComboBox::CRComboBox( const Ptr<GUI::ResourcePool> & pResPool,
                        const Ptr<Renderer::IHUD> & pHUD,
                        const Ptr<Window::InputManager> & pInput,
                        const Ptr<GUI::Manager> & pManager,
                        const Ptr<SoundLibrary> & pSoundLibrary)
:   GUI::ComboBox(pResPool, pHUD, pInput, pManager),
    _pSoundLibrary(pSoundLibrary)
{

}

void CRComboBox::internalNotify(GUI::EEvent ev)
{
    if(ev == GUI::EV_COMMAND)
    {
        _pSoundLibrary->playSound(SOUND_BUTTON_BLIP, false);
    }

    //Copier coller du internalNotify de GUI::ComboBox car on doit d�plier des CRButton et non des GUI::Button
    if(ev == GUI::EV_COMMAND)
    {
        _pDialog = Ptr<Widget>(new Widget());
        _pDialog->setPosition(_screenPos);

        float startY = getSize().y + _padding;

        GUI::ButtonStyle style(_style);
        style.state[GUI::BS_PUSHED].posDelta = Core::Vector2f(0.0f, 0.0f);

        Ptr<CRButton> pButtonCancel(new CRButton(_pManager->getResPool(), _pManager->getHUD(), _pManager->getInput(), _pSoundLibrary));
        pButtonCancel->setId(-1);
        pButtonCancel->setStyle(style);
        pButtonCancel->setText(getText());
        pButtonCancel->addEventHandler(this);
        pButtonCancel->setDictionary(_pDico);
        _pDialog->addChild(pButtonCancel);

        Core::List<Ptr<Button> > buttons;

        for(int32 ii=0; ii < _items.size(); ii++)
        {
            Ptr<CRButton> pItem(new CRButton(_pManager->getResPool(), _pManager->getHUD(), _pManager->getInput(), _pSoundLibrary));
            pItem->setId(ii);
            pItem->setStyle(_itemsStyle);
            pItem->setText(_items[ii]);
            pItem->addEventHandler(this);
            pItem->setDictionary(_pDico);

            if(ii == _index)
                pItem->setHotKey(VK_ESCAPE);

            Core::Vector2f size(pItem->getSize());
            Core::Vector2f pos((ii / _maxLines) * size.x, startY + (ii % _maxLines) * (size.y + _padding));

            pItem->setPosition(pos);
            _pDialog->addChild(pItem);
            buttons.push_back(pItem);
        }

        if(_pKeyNav != NULL)
        {
            for(int32 ii=0; ii < buttons.size(); ii++)
            {
                buttons[ii]->setKeyNavigation(_pKeyNav);

                if((ii - 1) >= 0)
                    buttons[ii]->setKeyWidgetU(buttons[ii - 1].get());
                if((ii + 1) < buttons.size())
                    buttons[ii]->setKeyWidgetD(buttons[ii + 1].get());
                if((ii + _maxLines) < buttons.size())
                    buttons[ii]->setKeyWidgetR(buttons[ii + _maxLines].get());
                if((ii - _maxLines) >= 0)
                    buttons[ii]->setKeyWidgetL(buttons[ii - _maxLines].get());
            }

            pButtonCancel->setKeyNavigation(_pKeyNav);
            pButtonCancel->setKeyWidgetD(buttons[0].get());
            buttons[0]->setKeyWidgetU(pButtonCancel.get());
        }

        buttons.clear();
        _pManager->pushDialog(_pDialog);
        _pKeyNav->pKeyFocus = pButtonCancel.get();
    }
}

}//namespace ChronoRage