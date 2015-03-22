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
#ifndef PROPERTY_WORLD_H_
#define PROPERTY_WORLD_H_


#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/World.h>

namespace QtToolbox
{
    class QuickColorPicker;
    class CollapsibleWidget;
    class SingleSlidingDouble;
}//namespace QtToolbox

class QGridLayout;

namespace EPI
{

class PtyWidgetWorld;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWorld : public PropertyNode
{
    friend class PtyWidgetWorld;

public:
    PtyWorld(const Ptr<Universe::World>& pWorld, const Core::String& title = L"Misc");
    virtual ~PtyWorld();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual void updateData();
    virtual void updateProperty();

    virtual bool            isLeaf () const {return true;}

    virtual String getIconFileName() {return L":/icons/ptyMisc.png";}

private:
    Ptr<Universe::World>    _pWorld;
    double                  _waterLevel;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetWorld : public PropertyWidget
{
    friend class PtyWorld;

public:
    PtyWidgetWorld(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetWorld();

    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);

    void setupUi();

private:
    QGridLayout *                       _layout;
    QtToolbox::CollapsibleWidget *      _groupBox;

    QtToolbox::SingleSlidingDouble *    _waterLevel;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_WORLD_H_