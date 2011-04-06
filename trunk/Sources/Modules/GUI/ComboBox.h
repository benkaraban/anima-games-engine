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
#ifndef GUI_COMBOBOX_H_
#define GUI_COMBOBOX_H_

#include <GUI/Widget.h>
#include <GUI/Label.h>
#include <GUI/Button.h>
#include <GUI/Manager.h>

namespace GUI
{

class LM_API_GUI ComboBox : public Button, public IEventHandler
{
public:
    ComboBox(const Ptr<ResourcePool> & pResPool,
             const Ptr<Renderer::IHUD> & pHUD,
             const Ptr<Window::InputManager> & pInput,
             const Ptr<Manager> & pManager);

    void setIndex(int32 index);
    int32 getIndex() const { return _index; }

    void setMaxLines(int32 m) { _maxLines = m; }
    int32 getMaxLines() const { return _maxLines; }

    void setItems(const Core::List<String> & items) { _items = items; }
    const Core::List<String> getItems() const { return _items; }

    void setItemsStyle(const ButtonStyle & style)  { _itemsStyle = style; }
    const ButtonStyle & getItemsStyle() const { return _itemsStyle; }

    void onEvent(EEvent ev, Widget * pWidget);

    void setPadding(float p) { _padding = p; }
    float getPadding() const { return _padding; }

protected:
    void setRenderState(EButtonRenderState rs);
    void updateRenderState();

    virtual void internalNotify(EEvent ev);

    Ptr<Window::InputManager>   _pInput;
    Ptr<Manager>                _pManager;
    ButtonStyle                 _itemsStyle;
    Core::List<String>          _items;
    int32                       _index;
    float                       _padding;
    int32                       _maxLines;
    Ptr<Widget>                 _pDialog;
};

}

#endif

