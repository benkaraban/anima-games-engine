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
#include "PtyNodeInstance.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeInstance::PtyNodeInstance(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNInstance, const Core::String& title):
    PtyNode(pWorld, pNInstance, title)
{
   // Ptr<PtyNode> pPtyN (new PtyNode(pNInstance, L"Node"));

//    addChild(pPtyN);

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeInstance::~PtyNodeInstance()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeInstance::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodeInstance> pPW (new PtyWidgetNodeInstance(pDtaProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodeInstance::updateData()
{
    PtyNode::updateData();
}
//-----------------------------------------------------------------------------
void PtyNodeInstance::updateProperty()
{
    PtyNode::updateProperty();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeInstance::clone() const
{
    Ptr<PtyNodeInstance> pRet  (new PtyNodeInstance( *this ));
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyNodeInstance::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeInstance> pPty = LM_DEBUG_PTR_CAST<PtyNodeInstance>(pSrc);
    updateData();
}

//-----------------------------------------------------------------------------
bool PtyNodeInstance::isItMe(const Ptr<Universe::Node>& pNode)
{
    return (pNode == getUniverseNode());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeInstance::PtyWidgetNodeInstance(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeInstance::~PtyWidgetNodeInstance()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeInstance::setupUi()
{
    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = Ptr<PtyWidgetNode> (new PtyWidgetNode(pDataPNodeW, this));
    
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _layout->addWidget(_pPWNode.get());


    setLayout(_layout);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeInstance::readProperty()
{
    Ptr<PtyNodeInstance> pP = LM_DEBUG_PTR_CAST<PtyNodeInstance>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeInstance::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeInstance> pP = LM_DEBUG_PTR_CAST<PtyNodeInstance>(getDataProxy()->getProperty());  
}
//-----------------------------------------------------------------------------
} // namespace EPI