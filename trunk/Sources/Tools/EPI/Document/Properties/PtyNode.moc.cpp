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
#include "PtyNode.moc.h"

#include <QCheckBox>

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <Universe/Node.h>
#include <Universe/World.h>
#include <Universe/Tools.h>

#include <EPI/Document/Properties/PtyTag.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/Document/Properties/PtyScript.moc.h>
#include <EPI/Document/PropertySelection.moc.h>

#include <EPI/Constants.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNode::PtyNode(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title):
    PropertyNode(pNode->getName(), true, true, CONTENT),
    _pNode(pNode),
    _pWorld(pWorld),
    _pXMLforResurrect(null),
    _showWidgetTransform(true),
    _showWidgetTag(true),
    _showWidgetVisible(true),
    _showWidgetLightmap(true),
    _isVisible(true),
    _isStatic(pNode->isStatic()),
    _lightmapRatio(pNode->getLightmapResolutionRatio())
{
    addFlag(FLAG_PTY_NODE);

    setVisibleWidgetLightmap(pNode->isInstance() || (pNode->getNodeType() == Universe::NODE_MESH));

    if (getName() == String::EMPTY)
    {
        if (title == String::EMPTY)
        {
            if (_pNode->getTags().size() > 0)
            {
                setName(*(_pNode->getTags().begin()));
            }
        }
        else setName(title);
    }

    _ressourceName = _pNode->getResourceName();

    _pPtyT = Ptr<Property> (new PtyTransformNode(pNode, L"Transform"));

   ((PtyTransformNode*)(_pPtyT.get()))->setPtyNode(this);

    _pPtyTag = Ptr<Property> (new PtyTag(pNode, L"Transform"));
    _pPtyScript = Ptr<Property> (new PtyScript(null, L"Script"));

    LM_DEBUG_PTR_CAST<PtyScript>(_pPtyScript)->setScript(getUniverseNode()->getNodeScript());
    updateData();
}
//-----------------------------------------------------------------------------
void PtyNode::internalRestart()
{
    getUniverseNode()->restart(true);
}
//-----------------------------------------------------------------------------
void PtyNode::internalStop()
{
    getUniverseNode()->stop(true);
}
//-----------------------------------------------------------------------------
PtyNode::PtyNode(const PtyNode& other):
    PropertyNode         (other),
    _pWorld              (other._pWorld),
    _isVisible           (other._isVisible),
    _isStatic            (other._isStatic),
    _lightmapRatio       (other._lightmapRatio),
    _showWidgetTransform (other._showWidgetTransform),
    _showWidgetTag       (other._showWidgetTag),
    _showWidgetVisible   (other._showWidgetVisible),
    _showWidgetLightmap  (other._showWidgetLightmap),
    _ressourceName       (other._ressourceName)
{
    addFlag(FLAG_PTY_NODE);

    _pPtyT               = other._pPtyT->clone();
    LM_DEBUG_PTR_CAST<PtyTransformNode> (_pPtyT)->updateMasterData( Core::Vector3f(Core::ZERO), Core::Matrix3f(Core::IDENTITY));
   // LM_DEBUG_PTR_CAST<PtyTransformNode> (_pPtyT)->setWorldPosition(LM_DEBUG_PTR_CAST<PtyTransformNode> (other._pPtyT)->getWorldPosition());

    ((PtyTransformNode*)(_pPtyT.get()))->setPtyNode(this);
    _pPtyTag             = other._pPtyTag->clone();
    _pPtyScript          = other._pPtyScript->clone();

    setUniverseNode(null);

    if (other.getUniverseNode() == null)
    {
        LM_ASSERT(other._pXMLforResurrect!=null);
        _pXMLforResurrect = other._pXMLforResurrect;
    }
    else
    {
        LM_ASSERT(other._pXMLforResurrect == null);
        _pXMLforResurrect = Ptr<Core::XMLNode> (new Core::XMLNode(L"xmlForResurrect"));
        _pWorld->exportXML(_pXMLforResurrect, other.getUniverseNode(), false);
    }
}
//-----------------------------------------------------------------------------
void PtyNode::setName(const Core::String& name)
{
    PropertyNode::setName(name);
    getUniverseNode()->setName(name);
}
//-----------------------------------------------------------------------------
void PtyNode::setVisibleWidgetTransform(bool flag)
{
    _showWidgetTransform = flag;
}
//-----------------------------------------------------------------------------
void PtyNode::setVisibleWidgetTag(bool flag)
{
    _showWidgetTag = flag;
}
//-----------------------------------------------------------------------------
void PtyNode::setVisibleWidgetShow(bool flag)
{
    _showWidgetVisible = flag;
}
//-----------------------------------------------------------------------------
void PtyNode::setVisibleWidgetLightmap(bool flag)
{
    _showWidgetLightmap = flag;
}
//-----------------------------------------------------------------------------
PtyNode::~PtyNode()
{
    if (getUniverseNode()!=null)
    {
        getUniverseNode()->kill();
    }
}
//-----------------------------------------------------------------------------
void PtyNode::setUniverseNode(const Ptr<Universe::Node>& pNode)
{
    _pNode = pNode;
    LM_DEBUG_PTR_CAST<PtyTransformNode>(_pPtyT)->setUniverseNode(_pNode);
    LM_DEBUG_PTR_CAST<PtyTag>(_pPtyTag)->setUniverseNode(_pNode);
    
}
//-----------------------------------------------------------------------------
const Ptr<Universe::Node>& PtyNode::getUniverseNode() const
{
    return _pNode;
}
//-----------------------------------------------------------------------------
Ptr<Universe::Node>& PtyNode::getUniverseNode()
{
    return _pNode;
}
//-----------------------------------------------------------------------------
void PtyNode::addChild(Ptr<Property> pChild, int32 indexPos)
{
    PropertyNode::addChild(pChild, indexPos);

    Ptr<PtyNode> pChildNode = Core::ptrDynamicCast<PtyNode> (pChild);

    if (getUniverseNode()!= null && pChildNode != null)
    {
        _pNode->addChild(pChildNode->getUniverseNode(), true, indexPos);
        pChildNode->updateProperty();
    }

    getPtyTransform()->updateProperty();
}
//-----------------------------------------------------------------------------
void PtyNode::removeChild(Ptr<Property> pChild)
{
    PropertyNode::removeChild(pChild);

    if (pChild->hasFlag(FLAG_PTY_NODE) == true)
    {
        Ptr<PtyNode> pPtyNodeChild = LM_DEBUG_PTR_CAST<PtyNode>  (pChild);

        Ptr<Universe::Node> pNode = getUniverseNode();
        if (pNode!= null && pPtyNodeChild->getUniverseNode() != null)
        {
            pNode->removeChild(pPtyNodeChild->getUniverseNode(), true);
            pPtyNodeChild->updateProperty();
        }
    }
}
//-----------------------------------------------------------------------------
void PtyNode::removeChild(const int32 index)
{
    Ptr<Property> pChild = getChild(index);

    removeChild(pChild);
}
//-----------------------------------------------------------------------------
void PtyNode::removeAllChildren()
{
    PropertyNode::removeAllChildren();

    Ptr<Universe::Node> pNode = getUniverseNode();
    if (pNode!= null)
    {
        for (int32 ii=0; ii<pNode->getChildCount(); ++ii)
        {
            pNode->removeChild(pNode->getChild(ii), true);
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNode::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetNode> pPW (new PtyWidgetNode(pDtaProxy, parent));


    QObject::connect(pPW.get(), SIGNAL(emitDocumentContentUpdated()), parent, SIGNAL(emitDocumentContentUpdated()));
    QObject::connect(pPW.get(), SIGNAL(propertyModified()), parent, SIGNAL(propertyModified()));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNode::updateProperty()
{
    Ptr<PtyScript> ptyScript = LM_DEBUG_PTR_CAST<PtyScript>(getPtyScript());

    getPtyTransform()->updateProperty();
    _isVisible = getUniverseNode()->isVisible();
    _isStatic = getUniverseNode()->isStatic();
    _lightmapRatio = getUniverseNode()->getLightmapResolutionRatio();

    ptyScript->setScript(getUniverseNode()->getNodeScript());
    
    _ressourceName = _pNode->getResourceName();
    //updateChildren();
}
//-----------------------------------------------------------------------------
void PtyNode::updateData()
{
    Ptr<PtyScript> ptyScript = LM_DEBUG_PTR_CAST<PtyScript>(getPtyScript());

    getPtyTransform()->updateData();
    getUniverseNode()->setVisible(_isVisible, true);
    getUniverseNode()->setStatic(_isStatic);
    getUniverseNode()->setLightmapResolutionRatio(_lightmapRatio);
    getUniverseNode()->setNodeScript(ptyScript->getScript());
}
//-----------------------------------------------------------------------------
void PtyNode::updateChildren()
{
    Ptr<PtyTransformNode> t = LM_DEBUG_PTR_CAST<PtyTransformNode>(getPtyTransform());

    for (int32 ii=0; ii<getNbChildren(); ++ii)
    {
        if(getChild(ii)->hasFlag(FLAG_PTY_NODE) == true)
        {
            Ptr<PtyNode> pPtyNChild = LM_DEBUG_PTR_CAST<PtyNode>(getChild(ii));

            Ptr<Property> pT = pPtyNChild->getPtyTransform();

            if (pT!=null)
            {
                if (pT->getCalssName() == PTY_TRANSFORM_NODE)
                {
                    LM_DEBUG_PTR_CAST<PtyTransformNode>(pT)->updateMasterData(t->getWorldPosition(), t->_masterAxisInWorldCoord*t->_axisParentCoord);
                }

               pT->updateProperty();

               pPtyNChild->updateChildren();
            }
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNode::clone() const
{
    Ptr<PtyNode> pRet (new PtyNode( *this ));

    return pRet;
}
//-----------------------------------------------------------------------------
void PtyNode::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<Universe::Node> tmpPNode = _pNode;

    Ptr<PtyNode> pPty = LM_DEBUG_PTR_CAST<PtyNode>(pSrc);

    
    _pWorld = pPty->_pWorld;
    _showWidgetTransform = pPty->_showWidgetTransform;
    _showWidgetTag       = pPty->_showWidgetTag;
    _showWidgetVisible     = pPty->_showWidgetVisible;
    _isVisible = pPty->_isVisible;
    _isStatic = pPty->_isStatic;
    _lightmapRatio = pPty->_lightmapRatio;
    _ressourceName = pPty->_ressourceName;

    _pPtyT->copy(pPty->_pPtyT);
    _pPtyTag->copy(pPty->_pPtyTag);
    _pPtyScript->copy(pPty->_pPtyScript);
    setUniverseNode(tmpPNode);
    

    updateData();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyNode::getPtyTransform() const
{
    return _pPtyT;
}
//-----------------------------------------------------------------------------
Ptr<Property>& PtyNode::getPtyTransform()
{
    return _pPtyT;
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyNode::getPtyTag() const
{
    return _pPtyTag;
}
//-----------------------------------------------------------------------------
Ptr<Property>& PtyNode::getPtyTag()
{
    return _pPtyTag;
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyNode::getPtyScript() const
{
    return _pPtyScript;
}
//-----------------------------------------------------------------------------
bool PtyNode::isItMe(const Ptr<Universe::Node>& pNode)
{
    return pNode==_pNode;
}
//-----------------------------------------------------------------------------
bool PtyNode::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    if (id == _pPtyT->getId())
    {
        pResult = _pPtyT;
        return true;
    }
    else if (id ==  _pPtyTag->getId())
    {
        pResult = _pPtyTag;
        return true;
    }
    else if (id == _pPtyScript->getId())
    {
        pResult = _pPtyScript;
        return true;
    }
    
    return PropertyNode::findPtyWithId(pResult, id);
}
//-----------------------------------------------------------------------------
void PtyNode::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(_pNode==null);
    LM_ASSERT(_pXMLforResurrect!=null);

    if (pWorld!=null)
    {
        setUniverseWorld(pWorld);
    }

    setUniverseNode(_pWorld->importXML(_pXMLforResurrect));

    _pXMLforResurrect = null;

    PtyNode* pParent = LM_DEBUG_CAST<PtyNode*>(getParent());
    if (pParent!= NULL)
    {
        pParent->getUniverseNode()->addChild(getUniverseNode());
    }

    //attention ! les classe filles on la resposabilite de faire l' updateData();
}
//-----------------------------------------------------------------------------
bool PtyNode::isTransformable() const
{
    return true;
}
//-----------------------------------------------------------------------------
void PtyNode::copyId(const Property& pSrc)
{
    const PtyNode& other = (const PtyNode&)pSrc;

    PropertyNode::copyId(pSrc);
        _pPtyT->setId(other._pPtyT->getId());
        _pPtyTag->setId(other._pPtyTag->getId());
        _pPtyScript->setId(other._pPtyScript->getId());
}
//-----------------------------------------------------------------------------
bool PtyNode::isIntersecting (const Core::Rayf& ray, float& distance)
{
    return getUniverseNode()->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
Core::AABoxf PtyNode::getBoundingBox()
{
    Core::AABoxf boxNode;
    getHierarchyBox(boxNode, *(getUniverseNode()));

    return boxNode;
}
//-----------------------------------------------------------------------------
void PtyNode::setVisible(bool flag)
{
    _isVisible = flag;
    getUniverseNode()->setVisible(_isVisible, true);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeGroupMemoCam::PtyNodeGroupMemoCam(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title) :
    PtyNode(pWorld, pNode, title)
{
    
}
//-----------------------------------------------------------------------------
PtyNodeGroupMemoCam::~PtyNodeGroupMemoCam()
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeEditableChildren::PtyNodeEditableChildren(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title) :
    PtyNode(pWorld, pNode, title)
{
    _pPtySelection = Ptr<PropertySelection>( new PropertySelectionDefault(L"Selection_"+title, this, pStateRecorder) );

    connect (_pPtySelection.get(), SIGNAL(selectionChanged()),
        this, SLOT(editingSelectionChanged()));
}
//-----------------------------------------------------------------------------
PtyNodeEditableChildren::~PtyNodeEditableChildren()
{
}
//-----------------------------------------------------------------------------
void PtyNodeEditableChildren::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeEditableChildren> pPty = LM_DEBUG_PTR_CAST<PtyNodeEditableChildren>(pSrc);

    _pPtySelection->copy(pPty->_pPtySelection);
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyNodeEditableChildren::getPtySelection() const
{
    return _pPtySelection;
}
//-----------------------------------------------------------------------------
Ptr<Property>& PtyNodeEditableChildren::getPtySelection()
{
    return _pPtySelection;
}
//-----------------------------------------------------------------------------
bool PtyNodeEditableChildren::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    if (PtyNode::findPtyWithId(pResult, id) == true)
    {
        return true;
    }

    if (id == _pPtySelection->getId())
    {
        pResult = _pPtySelection;
        return true;
    }
    
    return PropertyNode::findPtyWithId(pResult, id);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeEditableContent::PtyNodeEditableContent(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title):
    PtyNode(pWorld, pNode, title)
{
    addFlag(FLAG_PTY_EDITABLE_CONTENT);
    _pPtySelection = Ptr<PropertySelection>( new PropertySelectionContent(L"Selection_"+title, &ptiesContentSelectable, pStateRecorder) );

    connect (_pPtySelection.get(), SIGNAL(selectionChanged()),
        this, SLOT(editingSelectionChanged()));
}
//-----------------------------------------------------------------------------
PtyNodeEditableContent::PtyNodeEditableContent(const PtyNodeEditableContent& other):
    PtyNode(other)
{
    addFlag(FLAG_PTY_EDITABLE_CONTENT);
    _pPtySelection = other._pPtySelection->clone();

    ptiesContentSelectable.clear();

    connect (_pPtySelection.get(), SIGNAL(selectionChanged()),
        this, SLOT(editingSelectionChanged()));
}
//-----------------------------------------------------------------------------
PtyNodeEditableContent::~PtyNodeEditableContent()
{
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyNodeEditableContent::getPtySelection() const
{
    return _pPtySelection;
}
//-----------------------------------------------------------------------------
Ptr<Property>& PtyNodeEditableContent::getPtySelection()
{
    return _pPtySelection;
}
//-----------------------------------------------------------------------------
void PtyNodeEditableContent::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeEditableContent> pPty = LM_DEBUG_PTR_CAST<PtyNodeEditableContent>(pSrc);

    _pPtySelection->copy(pPty->_pPtySelection);
}
//-----------------------------------------------------------------------------
bool PtyNodeEditableContent::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    if (PtyNode::findPtyWithId(pResult, id) == true)
    {
        return true;
    }

    if (id == _pPtySelection->getId())
    {
        pResult = _pPtySelection;
        return true;
    }

    for (int32 iPty=0; iPty<getPtiesContentSelectable().size(); iPty++)
    {
        if(getPtiesContentSelectable()[iPty]->getId() == id)
        {
            pResult = getPtiesContentSelectable()[iPty];
            return true;
        }
        else if (getPtiesContentSelectable()[iPty]->findPtyWithId(pResult, id))
        {
            return true;
        }
    }
    
    return PropertyNode::findPtyWithId(pResult, id);
}
//-----------------------------------------------------------------------------
int32 PtyNodeEditableContent::findIndexWithId(int32 id)
{
    int32 ret = -1;

    for (int32 ii=0; ii<getPtiesContentSelectable().size(); ii++)
    {
        if (getPtiesContentSelectable()[ii]->getId() == id)
        {
            ret = ii;
            break;
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >&  PtyNodeEditableContent::getPtiesContentSelectable() const
{
    return ptiesContentSelectable;
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Property> >&  PtyNodeEditableContent::getPtiesContentSelectable()
{
    return ptiesContentSelectable;
}
//-----------------------------------------------------------------------------
void PtyNodeEditableContent::removePtyContent(int32 index)
{
    getPtiesContentSelectable().erase(getPtiesContentSelectable().begin()+index);
}
//-----------------------------------------------------------------------------
void PtyNodeEditableContent::removePtyContent(const Ptr<Property>& pPty)
{
    const Core::List<Ptr<Property> >& ptiesContent = getPtiesContentSelectable();

    int32 ii=0;
    for (; ii<ptiesContent.size(); ++ii)
    {
        if (ptiesContent[ii] ==pPty)
        {
            break;
        }
    }

    LM_ASSERT(ii<ptiesContent.size());

    removePtyContent(ii);
}
//-----------------------------------------------------------------------------
void PtyNodeEditableContent::insertPtyContent(const Ptr<Property>& pPty, int32 index)
{
    if (index<0)
    {
        getPtiesContentSelectable().push_back(pPty);
    }
    else
    {
        getPtiesContentSelectable().insert(getPtiesContentSelectable().begin()+index, pPty);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNode::PtyWidgetNode(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNode::~PtyWidgetNode()
{
    Ptr<PtyNode> pP = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());

    QWidget* pTextScript = (QWidget*)LM_DEBUG_PTR_CAST<PtyScript> (pP->getPtyScript())->getCodeEditor();
        _pGroupScript->getLayout()->removeWidget(pTextScript);
        pTextScript->setParent(NULL);
}
//-----------------------------------------------------------------------------
void PtyWidgetNode::setupUi()
{
    Ptr<PtyNode> pP = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());

    Ptr<PropertyWidgetDataProxy> pDataPW (new PropertyWidgetDataProxy(pP->getPtyTransform(), pP->getName(), getDataProxy()->getStateRecorder()));
    _pPWTransfrom = pP->getPtyTransform()->createPropertyWidget(pDataPW, this);

    Ptr<PropertyWidgetDataProxy> pDataPWTag (new PropertyWidgetDataProxy(pP->getPtyTag(), pP->getName(), getDataProxy()->getStateRecorder()));
    _pPWTag = pP->getPtyTag()->createPropertyWidget(pDataPWTag, this);

    QWidget* pTextScript = (QWidget*)LM_DEBUG_PTR_CAST<PtyScript> (pP->getPtyScript())->getCodeEditor();

    _pShow = new QCheckBox("Show", this);
    _pStatic = new QCheckBox("Static lighting", this);
    _pLightmapRatio = new QtToolbox::ComboBox(this, "Lightmap ratio");
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);


    _pGroupInfo = new QtToolbox::CollapsibleWidget(this, "Information", true);
    _pGroupInfo->collapse(true);
    _ressourceName = new QtToolbox::LineEdit(this, "Ressource", false, "", 250);
    _ressourceName->setReadOnly(true);
    _pGroupInfo->getLayout()->addWidget(_ressourceName);
    _pGroupInfo->getLayout()->setContentsMargins(0,0,0,0);
    _pGroupInfo->getLayout()->setSpacing(0);

    _pGroupScript = new QtToolbox::CollapsibleWidget(this, "Script");
    _pGroupScript->getLayout()->addWidget(pTextScript);
    _pGroupScript->getLayout()->setContentsMargins(0,0,0,0);
    _pGroupScript->getLayout()->setSpacing(0);

    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(pP->getPtyScript());
    _pGroupScript->collapse(pPtyScript->getScript()=="");

    _layout->addWidget(_pShow);
    _layout->addWidget(_pGroupInfo);
    _layout->addWidget(_pPWTransfrom.get());
    _layout->addWidget(_pPWTag.get());
    _layout->addWidget(_pGroupScript);
    
    _layout->addWidget(_pStatic);
    _layout->addWidget(_pLightmapRatio);

    _pLightmapRatio->addItem("1:1");
    _pLightmapRatio->addItem("1:2");
    _pLightmapRatio->addItem("1:4");
    _pLightmapRatio->addItem("1:8");
      

    int32 height = 0;

    if (pP->_showWidgetTransform==false)    _pPWTransfrom->setVisible(false);
    else                                    height += _pPWTransfrom->height();
    if (pP->_showWidgetTag==false)          _pPWTag->setVisible(false);
    else                                    height += _pPWTag->height();
    if (pP->_showWidgetVisible==false)    _pShow->setVisible(false);
    else                                    height += _pShow->height();

    if(!pP->_showWidgetLightmap)
    {
        _pStatic->setVisible(false);
        _pLightmapRatio->setVisible(false);
    }
    else
    {
        height += _pStatic->height();
        height += _pLightmapRatio->height();
    }

    setLayout(_layout);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    getWidgetsForUndoRedo().push_back(_pShow);
    getWidgetsForUndoRedo().push_back(_pStatic);
    getWidgetsForUndoRedo().push_back(_pLightmapRatio);

    connect(pTextScript, SIGNAL(textChanged ()), this, SLOT(textScriptChanged()));
    connect(pTextScript, SIGNAL(lostFocus ()), this, SLOT(textScriptLostFocus()));

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetNode::collapse(bool flag)
{
    _pPWTag->collapse(flag);
    _pPWTransfrom->collapse(flag);
}
//-----------------------------------------------------------------------------
void PtyWidgetNode::readProperty()
{
    Ptr<PtyNode> pP = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
    _pPWTransfrom->updatePropertyWidget();
    _pPWTag->updatePropertyWidget();
    _pShow->setChecked(pP->_isVisible);
    _pStatic->setChecked(pP->_isStatic);

    if(pP->_lightmapRatio == 1)
        _pLightmapRatio->selectIndex(0);
    else if(pP->_lightmapRatio == 2)
        _pLightmapRatio->selectIndex(1);
    else if(pP->_lightmapRatio == 4)
        _pLightmapRatio->selectIndex(2);
    else
        _pLightmapRatio->selectIndex(3);

    _ressourceName->setText(StrToQStr(pP->_ressourceName));
}
//-----------------------------------------------------------------------------
void PtyWidgetNode::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNode> pP = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());  
    pP->setVisible(_pShow->isChecked());
    pP->_isStatic = _pStatic->isChecked();
    pP->_lightmapRatio = (1 << _pLightmapRatio->selectedIndex());
}
//-----------------------------------------------------------------------------
void PtyWidgetNode::textScriptChanged()
{
    Ptr<PtyNode> pP = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());  
    pP->updateData();
    emit propertyModified();

}
//-----------------------------------------------------------------------------
void PtyWidgetNode::textScriptLostFocus()
{
    emit emitDocumentContentUpdated();
}
//-----------------------------------------------------------------------------
} // namespace EPI