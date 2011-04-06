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
#ifndef PROPERTY_GIZMO_H_
#define PROPERTY_GIZMO_H_

#include <Core/Math/Vector.h>

#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>
#include <EPI/Document/ContentInfo/IContentRepresentation.h>

namespace Universe
{
    class World;
    class Node;
}

namespace QtToolbox
{
    class CollapsibleWidget;
}//namespace QtToolbox

class QGridLayout;

namespace EPI
{
class PtyWidgetNodeGizmo;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyNodeGizmo : public PtyNode
{
    friend class PtyWidgetNodeGizmo;

public:
    PtyNodeGizmo(const Ptr<Universe::World>& pWorldForRepresentation,  const Ptr<Universe::World>& pWorld,  const Ptr<Universe::Node>& pNode,  const Core::String& title = L"NodeGizmo");
    PtyNodeGizmo(const PtyNodeGizmo& other);
    virtual EPropertyClassName getCalssName() const {return PTYNODE_GIZMO;}
    virtual ~PtyNodeGizmo();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual bool isItMe(const Ptr<Universe::Node>& pNode);
    virtual void setSelected(bool isSelected);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);


    virtual String getIconFileName() {return L":/icons/ptyGizmo.png";}
    virtual bool isRepresented() const  {return true;}
    virtual void setRepresentationVisible(bool flag) {_pRepresentation->setVisible(flag);}
private:
    Ptr<Universe::World>          _pWorldForRepresentation;
    Ptr<IContentRepresentation>   _pRepresentation;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetNodeGizmo : public PropertyWidget
{
    friend class PtyNodeGizmo;

public:
    PtyWidgetNodeGizmo(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetNodeGizmo();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    QGridLayout * _layout;
    QtToolbox::CollapsibleWidget *          _groupBox;

    Ptr<PropertyWidget> _pPWNode;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_GIZMO_H_
