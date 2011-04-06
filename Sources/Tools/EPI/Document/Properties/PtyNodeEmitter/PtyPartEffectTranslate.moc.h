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
#ifndef PTY_PART_EFFECT_TRANSLATE_H_
#define PTY_PART_EFFECT_TRANSLATE_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffect.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/PartEffectTranslate.h>

class QGridLayout;
class QPushButton;

namespace QtToolbox
{
    class SingleSlidingHDR;
    class CollapsibleWidget;
}//namespace QtToolbox

namespace EPI
{
//-----------------------------------------------------------------------------
class LM_API_EPI PtyPartEffectTranslate : public PtyPartEffect
{
    friend class PtyWidgetPartEffectTranslate;

public:
    PtyPartEffectTranslate( const Ptr<Universe::NodeEmitter>& pNodeE,
                            const Ptr<Universe::PartEffectTranslate>& pEffect,
                            const Core::String& title = L"Translate effect");
    PtyPartEffectTranslate( const Ptr<Universe::NodeEmitter>& pNodeE,
                            const Core::String& title = L"Translate effect");

    virtual ~PtyPartEffectTranslate();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    //universeData <= property
    virtual void updateData();

    //property <= universeData
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual Universe::EEffect           getPartEffectType() const {return Universe::EFFECT_TRANSLATE;}

private:
    Core::Vector3f   _constSpeed;
    Core::Vector3f   _randSpeed;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetPartEffectTranslate : public PropertyWidget
{
    friend class PtyPartEffectTranslate;
    Q_OBJECT
public:
    PtyWidgetPartEffectTranslate(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetPartEffectTranslate();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void deleteWidget();

Q_SIGNALS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private:
    QGridLayout *                    _layout;

    QtToolbox::CollapsibleWidget *   _groupBox;
    QPushButton *                    _del;

    QtToolbox::SingleSlidingHDR *    _constSpeedX;
    QtToolbox::SingleSlidingHDR *    _constSpeedY;
    QtToolbox::SingleSlidingHDR *    _constSpeedZ;

    QtToolbox::SingleSlidingHDR *    _randSpeedX;
    QtToolbox::SingleSlidingHDR *    _randSpeedY;
    QtToolbox::SingleSlidingHDR *    _randSpeedZ;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_PART_EFFECT_Translate_H_*/
