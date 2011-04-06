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
#ifndef PROPERTY_FOG_H_
#define PROPERTY_FOG_H_

#include <QWidget>
#include <QGridLayout>

#include <Core/Standard.h>
#include <Core/Math/Vector.h>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/World.h>

namespace QtToolbox
{
    class QuickColorPicker;
    class SingleSlidingDouble;
    class RangedSlidingDouble;
    class RangedSlidingHDR;
    class CollapsibleWidget;
}//namespace QtToolbox

namespace EPI
{
class PtyWidgetFog;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyFog : public PropertyNode
{
    Q_OBJECT

    friend class PtyWidgetFog;

public:
    PtyFog(const Ptr<Universe::World>& pWorld, const Core::String& title = L"Fog");
    virtual ~PtyFog();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual bool            isLeaf () const {return true;}

    virtual String getIconFileName() {return L":/icons/ptyFog.png";}

private:
    Core::Vector4f _color;
    double _start;
    double _end;
    double _opacity;

    Ptr<Universe::World>    _pWorld;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetFog : public PropertyWidget
{
    Q_OBJECT

    friend class PtyFog;

public:
    PtyWidgetFog(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetFog();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *   _groupBox;
    QtToolbox::QuickColorPicker *    _color;
    QtToolbox::RangedSlidingHDR *   _range;
    QtToolbox::SingleSlidingDouble * _opacity;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_FOG_H_
