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
#ifndef PTY_PART_EFFECT_ATTRACTOR_H_
#define PTY_PART_EFFECT_ATTRACTOR_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffect.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/PartEffectAttractor.h>

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
class LM_API_EPI PtyPartEffectAttractor : public PtyPartEffect
{
    friend class PtyWidgetPartEffectAttractor;

public:
    PtyPartEffectAttractor( const Ptr<Universe::NodeEmitter>& pNodeE,
                            const Ptr<Universe::PartEffectAttractor>& pEffect,
                            const Core::String& title = L"Attractor effect");
    PtyPartEffectAttractor( const Ptr<Universe::NodeEmitter>& pNodeE,
                            const Core::String& title = L"Attractor effect");
    virtual ~PtyPartEffectAttractor();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateProperty();
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_ATTRACTOR;}

private:
    Core::Vector3f   _position;
    float            _intensity;
    bool             _magnet;
    float            _magnetRadius;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectAttractor : public PropertyWidget
{
    friend class PtyPartEffectAttractor;
    Q_OBJECT
public:
    PtyWidgetPartEffectAttractor(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectAttractor();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void deleteWidget();

Q_SIGNALS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private:
    QGridLayout *                       _layout;

    QtToolbox::CollapsibleWidget *      _groupBox;
    QPushButton *                       _del;
    QtToolbox::SingleSlidingHDR *       _X;
    QtToolbox::SingleSlidingHDR *       _Y;
    QtToolbox::SingleSlidingHDR *       _Z;
    QtToolbox::SingleSlidingHDR *       _intensity;
    QCheckBox *                         _magnet;
    QtToolbox::SingleSlidingHDR *       _magnetRadius;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_PART_EFFECT_ATTRACTOR_H_*/