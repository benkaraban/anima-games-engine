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
#ifndef PTY_PART_EFFECT_ColorByPos_H_
#define PTY_PART_EFFECT_ColorByPos_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffect.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/PartEffectColorByPos.h>

class QGridLayout;
class QCheckBox;
class QPushButton;

namespace QtToolbox
{
    class QuickColorPicker;
    class SingleSlidingDouble;
    class RangedSlidingHDR;
    class CollapsibleWidget;
    class ComboBox;
}//namespace QtToolbox

namespace EPI
{
//-----------------------------------------------------------------------------
class LM_API_EPI PtyPartEffectColorByPos : public PtyPartEffect
{
    friend class PtyWidgetPartEffectColorByPos;

public:
    PtyPartEffectColorByPos( const Ptr<Universe::NodeEmitter>& pNodeE,
                             const Ptr<Universe::PartEffectColorByPos>& pEffect,
                             const Core::String& title = L"ColorByPos effect");
    PtyPartEffectColorByPos( const Ptr<Universe::NodeEmitter>& pNodeE,
                             const Core::String& title = L"ColorByPos effect");

    virtual ~PtyPartEffectColorByPos();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    //universeData <= property
    virtual void updateData();

    //property <= universeData
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_COLOR_BY_POS;}

private:
    Core::Vector4f       _color0;
    Core::Vector4f       _color1;
    float                _minPos;
    float                _maxPos;
    float                _randFactor;
    Universe::EColorAxis _axis;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectColorByPos : public PropertyWidget
{
    friend class PtyPartEffectColorByPos;
    Q_OBJECT
public:
    PtyWidgetPartEffectColorByPos(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectColorByPos();
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
    QtToolbox::RangedSlidingHDR *       _pos;
    QtToolbox::SingleSlidingDouble *    _randFactor;
    QtToolbox::ComboBox *               _axis;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_PART_EFFECT_ANIMATE_H_*/
