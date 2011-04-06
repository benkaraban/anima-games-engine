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
#include "PtyDocWorld.h"

#include <QGridLayout>

#include <QtToolbox/SmartGroupBox.moc.h>

#include <EPI/Document/Properties/PtySun.moc.h>
#include <EPI/Document/Properties/PtySky.h>
#include <EPI/Document/Properties/PtyFog.moc.h>
#include <EPI/Document/Properties/PtyWorld.h>
#include <EPI/Document/Properties/PtyTerrain.moc.h>
#include <EPI/Document/Properties/PtyNodeDecal.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyNodeEmitter.moc.h>
#include <EPI/Document/Properties/PtyNodeInstance.h>
#include <EPI/Document/Properties/PtyWaterPatch.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocWorld::PtyDocWorld(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorld, const Ptr<DocumentRenderer> & pDocumentRenderer, const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _pWorld(pWorld),
    _pDocumentRenderer(pDocumentRenderer)
{
    Ptr<Property> pPtySun   (new PtySun(guiDoc, _pWorld));
    Ptr<Property> pPtySky   (new PtySky(_pWorld));
    Ptr<Property> pPtyFog   (new PtyFog(_pWorld));
    Ptr<Property> pPtyWorld (new PtyWorld(_pWorld));

    addChild(pPtySun);
    addChild(pPtySky);
    addChild(pPtyFog);
    addChild(pPtyWorld);

 /*   Core::List<Ptr<Universe::Node> > nodes;
    _pWorld->getAllNodesRoots(nodes);

    for(int32 iNode = 0; iNode < nodes.size(); ++iNode)
    {
        parseNodes(nodes[iNode], *this);
    }*/

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyDocWorld::~PtyDocWorld()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyDocWorld::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetDocWorld> pPW (new PtyWidgetDocWorld(pDataProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyDocWorld::updateData()
{
}
//-----------------------------------------------------------------------------
void PtyDocWorld::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocWorld::clone() const
{
    return Ptr<Property>(new PtyDocWorld( *this ));
}
//-----------------------------------------------------------------------------
void PtyDocWorld::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyDocWorld> pPtyDocWorld = LM_DEBUG_PTR_CAST<PtyDocWorld>(pSrc);

    _pWorld     = pPtyDocWorld->_pWorld;

    updateData();
}
//-----------------------------------------------------------------------------
#ifdef OLD_CODE
void PtyDocWorld::parseNodes(const Ptr<Universe::Node> & pNode, PropertyNode & ptyNode)
{
    Ptr<PropertyNode> pProperty = null;

    if(pNode->getNodeType() == Universe::NODE_TERRAIN)
    {
        pProperty = Ptr<PropertyNode>(new PtyTerrain(_pWorld, LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(pNode), _pDocumentRenderer));
        ptyNode.addChild(pProperty);
        _generationPtyNode.push_back(pProperty->getId());
    }
    else if(pNode->getNodeType() == Universe::NODE_EMITTER)
    {/*
        pProperty = Ptr<PropertyNode>(new PtyNodeEmitter(_pWorld, LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(pNode)));
        ptyNode.addChild(pProperty);*/
   /*     Ptr<Universe::NodeInstance> pNodeInstance = LM_DEBUG_PTR_CAST<Universe::NodeInstance>(pNode);
        pProperty = Ptr<PropertyNode>(new PtyNodeInstance(_pWorld, pNodeInstance, pNodeInstance->getResourceName()));
        ptyNode.addChild(pProperty);*/
    }
    else if(pNode->getNodeType() == Universe::NODE_DECAL)
    {
        pProperty = Ptr<PropertyNode>(new PtyNodeDecal(_pWorld, LM_DEBUG_PTR_CAST<Universe::NodeDecal>(pNode)));
        ptyNode.addChild(pProperty);
    }
    else if(pNode->getNodeType() == Universe::NODE_WATER_PATCH)
    {
        pProperty = Ptr<PropertyNode>(new PtyWaterPatch(_pWorld, LM_DEBUG_PTR_CAST<Universe::NodeWaterPatch>(pNode)));
        ptyNode.addChild(pProperty);
    }

    if(pProperty != null)
    {
        //recursion
        for(int32 iChild = 0; iChild < pNode->getChildCount(); ++iChild)
            parseNodes(pNode->getChild(iChild), *pProperty);
    }
}
#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetDocWorld::PtyWidgetDocWorld(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetDocWorld::~PtyWidgetDocWorld()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetDocWorld::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::SmartGroupBox(this, "DocWorld", true);

    _layout->addWidget(_groupBox, 0, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);
    this->setFixedWidth(_groupBox->width());

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocWorld::readProperty()
{
    Ptr<PtyDocWorld> pPtyDocWorld = LM_DEBUG_PTR_CAST<PtyDocWorld>(getDataProxy()->getProperty());
}
//-----------------------------------------------------------------------------
void PtyWidgetDocWorld::writeProperty(QWidget* pWidget)
{
    Ptr<PtyDocWorld> pPtyDocWorld = LM_DEBUG_PTR_CAST<PtyDocWorld>(getDataProxy()->getProperty());
}
//-----------------------------------------------------------------------------
} // namespace EPI