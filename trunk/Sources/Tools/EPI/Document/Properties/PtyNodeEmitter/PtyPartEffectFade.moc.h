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
#ifndef PTY_PART_EFFECT_FADE_H_
#define PTY_PART_EFFECT_FADE_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffect.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/PartEffectFade.h>

class QGridLayout;
class QCheckBox;
class QPushButton;

namespace QtToolbox
{
    class QuickColorPicker;
    class SingleSlidingHDR;
    class CollapsibleWidget;
}//namespace QtToolbox

namespace EPI
{
//-----------------------------------------------------------------------------
class LM_API_EPI PtyPartEffectFade : public PtyPartEffect
{
    friend class PtyWidgetPartEffectFade;

public:
    PtyPartEffectFade(    const Ptr<Universe::NodeEmitter>& pNodeE,
                          const Ptr<Universe::PartEffectFade>& pEffect,
                          const Core::String& title = L"Fade effect");
    PtyPartEffectFade(    const Ptr<Universe::NodeEmitter>& pNodeE,
                          const Core::String& title = L"Fade effect");
    virtual ~PtyPartEffectFade();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateProperty();
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_FADE;}

private:
    Core::Vector4f  _color0;
    Core::Vector4f  _color1;
    float           _time1;
    bool            _killPartsAtEnd;
    bool            _onlyAlpha;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectFade : public PropertyWidget
{
    friend class PtyPartEffectFade;
    Q_OBJECT
public:
    PtyWidgetPartEffectFade(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectFade();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void deleteWidget();

Q_SIGNALS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *      _groupBox;
    QPushButton *                       _del;
    QtToolbox::QuickColorPicker *       _color0;
    QtToolbox::QuickColorPicker *       _color1;
    QtToolbox::SingleSlidingHDR *       _time1;
    QCheckBox *                         _killAtEnd;
    QCheckBox *                         _onlyAlpha;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PtyPartEffectFade2 : public PtyPartEffect
{
    friend class PtyWidgetPartEffectFade2;

public:
    PtyPartEffectFade2(    const Ptr<Universe::NodeEmitter>& pNodeE,
                           const Ptr<Universe::PartEffectFade2>& pFade2,
                           const Core::String& title = L"Fade2 effect");
    PtyPartEffectFade2(    const Ptr<Universe::NodeEmitter>& pNodeE,
                           const Core::String& title = L"Fade2 effect");
    virtual ~PtyPartEffectFade2();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateProperty();
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_FADE2;}

private:
    Core::Vector4f  _color0;
    Core::Vector4f  _color1;
    Core::Vector4f  _color2;
    float           _time1;
    float           _time2;
    bool            _killPartsAtEnd;
    bool            _loop;
    bool            _onlyAlpha;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectFade2 : public PropertyWidget
{
    friend class PtyPartEffectFade2;
    Q_OBJECT
public:
    PtyWidgetPartEffectFade2(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectFade2();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void deleteWidget();

Q_SIGNALS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *      _groupBox;
    QPushButton *                       _del;
    QtToolbox::QuickColorPicker *       _color0;
    QtToolbox::QuickColorPicker *       _color1;
    QtToolbox::QuickColorPicker *       _color2;
    QtToolbox::SingleSlidingHDR *       _time1;
    QtToolbox::SingleSlidingHDR *       _time2;
    QCheckBox *                         _killAtEnd;
    QCheckBox *                         _loop;
    QCheckBox *                         _onlyAlpha;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PtyPartEffectFade3 : public PtyPartEffect
{
    friend class PtyWidgetPartEffectFade3;

public:
    PtyPartEffectFade3(    const Ptr<Universe::NodeEmitter>& pNodeE,
                           const Ptr<Universe::PartEffectFade3>& pFade3,
                           const Core::String& title = L"Fade3 effect");
    PtyPartEffectFade3(    const Ptr<Universe::NodeEmitter>& pNodeE,
                           const Core::String& title = L"Fade3 effect");
    virtual ~PtyPartEffectFade3();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateProperty();
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_FADE3;}

private:
    Core::Vector4f  _color0;
    Core::Vector4f  _color1;
    Core::Vector4f  _color2;
    Core::Vector4f  _color3;
    float           _time1;
    float           _time2;
    float           _time3;
    bool            _killPartsAtEnd;
    bool            _loop;
    bool            _onlyAlpha;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectFade3 : public PropertyWidget
{
    friend class PtyPartEffectFade3;
    Q_OBJECT
public:
    PtyWidgetPartEffectFade3(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectFade3();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void deleteWidget();

Q_SIGNALS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *      _groupBox;
    QPushButton *                       _del;
    QtToolbox::QuickColorPicker *       _color0;
    QtToolbox::QuickColorPicker *       _color1;
    QtToolbox::QuickColorPicker *       _color2;
    QtToolbox::QuickColorPicker *       _color3;
    QtToolbox::SingleSlidingHDR *       _time1;
    QtToolbox::SingleSlidingHDR *       _time2;
    QtToolbox::SingleSlidingHDR *       _time3;
    QCheckBox *                         _killAtEnd;
    QCheckBox *                         _loop;
    QCheckBox *                         _onlyAlpha;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_PART_EFFECT_ANIMATE_H_*/
