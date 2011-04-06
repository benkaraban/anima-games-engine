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
#ifndef PTY_PART_EFFECT_SIZE_H_
#define PTY_PART_EFFECT_SIZE_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffect.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/PartEffectSize.h>

class QGridLayout;
class QCheckBox;
class QPushButton;

namespace QtToolbox
{
    class SingleSlidingHDR;
    class CollapsibleWidget;
}//namespace QtToolbox

namespace EPI
{
//-----------------------------------------------------------------------------
class LM_API_EPI PtyPartEffectSize : public PtyPartEffect
{
    friend class PtyWidgetPartEffectSize;

public:
    PtyPartEffectSize(    const Ptr<Universe::NodeEmitter>& pNodeE,
                          const Ptr<Universe::PartEffectSize>& pEffect,
                          const Core::String& title = L"Size effect");
    PtyPartEffectSize(    const Ptr<Universe::NodeEmitter>& pNodeE,
                          const Core::String& title = L"Size effect");
    virtual ~PtyPartEffectSize();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateProperty();
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_SIZE;}

private:
    float   _width0;
    float   _height0;
    float   _width1;
    float   _height1;
    float   _time1;
    bool    _squareParts;
    bool    _killPartsAtEnd;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectSize : public PropertyWidget
{
    friend class PtyPartEffectSize;
    Q_OBJECT
public:
    PtyWidgetPartEffectSize(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectSize();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void deleteWidget();

Q_SIGNALS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *   _groupBox;
    QPushButton *                    _del;
    QtToolbox::SingleSlidingHDR *    _width0;
    QtToolbox::SingleSlidingHDR *    _height0;
    QtToolbox::SingleSlidingHDR *    _width1;
    QtToolbox::SingleSlidingHDR *    _height1;
    QtToolbox::SingleSlidingHDR *    _time1;
    QCheckBox *                      _squareParts;
    QCheckBox *                      _killAtEnd;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PtyPartEffectSize2 : public PtyPartEffect
{
    friend class PtyWidgetPartEffectSize2;

public:
    PtyPartEffectSize2(    const Ptr<Universe::NodeEmitter>& pNodeE,
                           const Ptr<Universe::PartEffectSize2>& pFade2,
                           const Core::String& title = L"Size2 effect");
    PtyPartEffectSize2(    const Ptr<Universe::NodeEmitter>& pNodeE,
                           const Core::String& title = L"Size2 effect");
    virtual ~PtyPartEffectSize2();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateProperty();
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_SIZE2;}

private:
    float   _width0;
    float   _height0;
    float   _width1;
    float   _height1;
    float   _width2;
    float   _height2;
    float   _time1;
    float   _time2;
    bool    _squareParts;
    bool    _killPartsAtEnd;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectSize2 : public PropertyWidget
{
    friend class PtyPartEffectSize2;
    Q_OBJECT
public:
    PtyWidgetPartEffectSize2(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectSize2();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void deleteWidget();

Q_SIGNALS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *   _groupBox;
    QPushButton *                    _del;
    QtToolbox::SingleSlidingHDR *    _width0;
    QtToolbox::SingleSlidingHDR *    _height0;
    QtToolbox::SingleSlidingHDR *    _width1;
    QtToolbox::SingleSlidingHDR *    _height1;
    QtToolbox::SingleSlidingHDR *    _width2;
    QtToolbox::SingleSlidingHDR *    _height2;
    QtToolbox::SingleSlidingHDR *    _time1;
    QtToolbox::SingleSlidingHDR *    _time2;
    QCheckBox *                      _squareParts;
    QCheckBox *                      _killAtEnd;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_PART_EFFECT_SIZE_H_*/