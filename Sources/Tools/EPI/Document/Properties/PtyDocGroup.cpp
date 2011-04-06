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
#include "PtyDocGroup.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <EPI/Document/Properties/PtyNode.moc.h>
namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocGroup::PtyDocGroup(   const Ptr<Universe::NodeGroup> pNodeG,
                            const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _pNodeG(pNodeG)
{
    
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyDocGroup::~PtyDocGroup()
{

}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyDocGroup::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetDocGroup> pPW (new PtyWidgetDocGroup(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyDocGroup::updateData()
{
}
//-----------------------------------------------------------------------------
void PtyDocGroup::updateProperty()
{
     
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocGroup::clone() const
{
    return Ptr<Property>(new PtyDocGroup( *this ));
}
//-----------------------------------------------------------------------------
void PtyDocGroup::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyDocGroup> pPty = LM_DEBUG_PTR_CAST<PtyDocGroup>(pSrc);

    updateData();
}
//-----------------------------------------------------------------------------
const Core::String& PtyDocGroup::getWorldDecoPath() const
{
    return _worldDeco;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> PtyDocGroup::getNodeGroup() const
{
    return _pNodeG;
}
//-----------------------------------------------------------------------------
void PtyDocGroup::setNodeGroup(const Ptr<Universe::NodeGroup>& node)
{
    _pNodeG = node;
    updateProperty();
}
//-----------------------------------------------------------------------------
void PtyDocGroup::addChild(Ptr<Property> pChild, int32 indexPos)
{
    PropertyNode::addChild(pChild, indexPos);

    Ptr<PtyNode> pChildNode = Core::ptrDynamicCast<PtyNode> (pChild);

    if (getNodeGroup()!= null && pChildNode != null)
    {
        getNodeGroup()->addChild(pChildNode->getUniverseNode(), true);
        pChildNode->updateProperty();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetDocGroup::PtyWidgetDocGroup(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetDocGroup::~PtyWidgetDocGroup()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetDocGroup::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::SmartGroupBox(this, "Material", true);


    _worldDeco = new QtToolbox::LineEdit(this, "Drop world here");
    _worldDeco->pushAuthorizedDropMimeData("asset/world");
    _worldDeco->setReadOnly(true);


    _groupBox->addWidget(_worldDeco, Qt::AlignTop | Qt::AlignLeft);

    _layout->addWidget(_groupBox);
    _layout->setAlignment(Qt::AlignTop);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_worldDeco); 

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocGroup::readProperty()
{
    Ptr<PtyDocGroup> pPtyDocGroup = LM_DEBUG_PTR_CAST<PtyDocGroup>(getDataProxy()->getProperty());

    _worldDeco->setText(Core::String8(pPtyDocGroup->_worldDeco).c_str());
}
//-----------------------------------------------------------------------------
void PtyWidgetDocGroup::writeProperty(QWidget* pWidget)
{
    Ptr<PtyDocGroup> pPtyDocGroup = LM_DEBUG_PTR_CAST<PtyDocGroup>(getDataProxy()->getProperty());

    if (pWidget==NULL || pWidget==_worldDeco)
        pPtyDocGroup->_worldDeco = Core::String(_worldDeco->text().toStdString().c_str());
}
//-----------------------------------------------------------------------------
} // namespace EPI