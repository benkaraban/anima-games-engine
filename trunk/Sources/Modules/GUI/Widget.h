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
#ifndef GUI_GUIELEMENT_H_
#define GUI_GUIELEMENT_H_

#include <Core/List.h>
#include <Core/Dictionary.h>
#include <Core/Math/Vector.h>

#include <Window/InputManager.h>

namespace GUI
{
LM_ENUM_4(EEvent, 
          EV_COMMAND,
          EV_SCROLLBAR,
          EV_OVER_BEGIN,
          EV_OVER_END
          );

class Widget;

class LM_API_GUI IEventHandler
{
public:
    virtual ~IEventHandler() {};
    virtual void onEvent(EEvent ev, Widget * pWidget) = 0;
};

class Focus
{
public:
    Focus();
    void setOwner(Widget * pOwner);
    bool hasFocus(Widget * pOwner) const { return pOwner == _pOwner; }
    Widget * getOwner() const { return _pOwner; }
protected:
    Widget * _pOwner;
};

class LM_API_GUI IKeyController
{
public:
    virtual ~IKeyController() {};

    virtual bool up() = 0;
    virtual bool down() = 0;
    virtual bool left() = 0;
    virtual bool right() = 0;

    virtual bool enter() = 0;
    virtual bool escape() = 0;
};

struct LM_API_GUI KeyNavigation : public Core::Object
{
    bool                ignoreMouse;
    bool                focusJustChanged;
    Widget *            pKeyFocus;
    Ptr<IKeyController> pCtrl;
};

class LM_API_GUI Widget : public Core::Object
{
public:
    Widget();

    virtual ~Widget(){}

    void setDictionary(const Ptr<Core::Dictionary> & pDico);

    int32 getId() const;
    void setId(int32 id);

    void setVisible(bool visible);
    bool isVisible() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void kill();
    bool isAlive() const;

    void setPosition(const Core::Vector2f & pos);
    const Core::Vector2f & getPosition() const;

    const Core::Vector2f & getScreenPos() const { return _screenPos; }

    void setSize(const Core::Vector2f & size);
    const Core::Vector2f & getSize() const;

    void setClipRect(const Core::Vector2f & size);
    const Core::Vector2f & getClipRect() const;

    void setOwnColor(const Core::Vector4f & color, float changeTime = 0.0f);
    const Core::Vector4f & getOwnColor() const { return _ownColor; };
    const Core::Vector4f & getColor() const;

    void addEventHandler(IEventHandler * pEvHandler);
    void removeEventHandler(IEventHandler * pEvHandler);

    void addChild(const Ptr<Widget> & pWidget);
    void removeChild(const Ptr<Widget> & pWidget);
    const Core::List<Ptr<Widget> > & getChildren() const { return _children; }

    // Cuisine interne, à ne pas utiliser en temps normal
    // (mais public quand même parce que ça peut servir)

    int32 getDeltaPriority() const;
    void setDeltaPriority(int32 p);

    int32 getPriority() const;

    // A ne pas confondre avec enabled, à ne pas utiliser en temps normal
    void setActive(bool active);
    bool isActive() const;

    virtual void update(double elapsed);

    virtual void        focus() {};
    virtual void        gainFocus() {};
    virtual void        loseFocus() {};
    virtual Widget *    getPrevWidget() { return _pPrevWidget; }
    virtual Widget *    getNextWidget() { return _pNextWidget; }
    virtual void        setPrevWidget(Widget * pWidget) { _pPrevWidget = pWidget; };
    virtual void        setNextWidget(Widget * pWidget) { _pNextWidget = pWidget; };

    virtual Widget * getKeyWidgetL() const { return _pKeyWidgetL; }
    virtual Widget * getKeyWidgetR() const { return _pKeyWidgetR; }
    virtual Widget * getKeyWidgetU() const { return _pKeyWidgetU; }
    virtual Widget * getKeyWidgetD() const { return _pKeyWidgetD; }

    virtual void setKeyWidgetL(Widget * pWidget) { _pKeyWidgetL = pWidget; }
    virtual void setKeyWidgetR(Widget * pWidget) { _pKeyWidgetR = pWidget; }
    virtual void setKeyWidgetU(Widget * pWidget) { _pKeyWidgetU = pWidget; }
    virtual void setKeyWidgetD(Widget * pWidget) { _pKeyWidgetD = pWidget; }

    virtual void pack();

    void setKeyNavigation(KeyNavigation * pKeyNav) { _pKeyNav = pKeyNav; }

    virtual void startAsDialog() {};

protected:
    void setParentPosition(const Core::Vector2f & pos);
    void setParentPriority(int32 p);
    void setParentVisible(bool visible);
    void setParentEnabled(bool enabled);
    void setParentActive(bool active);
    void setParentColor(const Core::Vector4f & color);

    void updateColor();

    virtual void updateKeyNavigation();

    virtual void onVisibilityChanged();
    virtual void onEnableChanged();
    virtual void onActiveChanged();
    virtual void onPositionChanged();
    virtual void onPriorityChanged(int32 p);
    virtual void onSizeChanged();
    virtual void onClipRectChanged();
    virtual void onColorChanged();
    virtual void onKill();

    virtual void notify(EEvent ev);

    Ptr<Core::Dictionary>   _pDico;
    KeyNavigation *         _pKeyNav;

    Widget *            _pPrevWidget;
    Widget *            _pNextWidget;

    Widget *            _pKeyWidgetL;
    Widget *            _pKeyWidgetR;
    Widget *            _pKeyWidgetU;
    Widget *            _pKeyWidgetD;

    int32               _id;
    int32               _parentPriority;
    int32               _deltaPriority;
    Core::Vector2f      _parentPos;
    Core::Vector2f      _localPos;
    Core::Vector2f      _screenPos;
    Core::Vector2f      _size;
    Core::Vector2f      _clipRect;
    Core::Vector4f      _parentColor;
    Core::Vector4f      _ownColor;
    Core::Vector4f      _color;

    Core::Vector4f      _color1;
    Core::Vector4f      _color2;
    float               _colorChangeDuration;
    float               _colorChangeElapsed;

    bool                _isAlive;

    bool                _isVisible;
    bool                _isEnabled;
    bool                _isActive;

    bool                _isParentVisible;
    bool                _isParentEnabled;
    bool                _isParentActive;

    Core::List<Ptr<Widget> >     _children;
    Core::List<IEventHandler * > _evHandlers;

    static int32        sUID;

    friend class Manager;
};

}

#endif
