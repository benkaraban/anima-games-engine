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
#include "PtyEmitterRepGroup.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>

#include <QCheckBox>

#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
PtyEmitterRepGroup::PtyEmitterRepGroup(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::PartRepresentationGroup>& pRep,
    const Core::String& title):
    PtyEmitterRep(pNodeE, pRep, title),
    _groupName(L"")
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterRepGroup::PtyEmitterRepGroup(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Core::String& title):
    PtyEmitterRep(pNodeE, null, title)
{
    _pRep = Ptr<Universe::PartRepresentationGroup>(new Universe::PartRepresentationGroup(pNodeE->getWorld(), L""));
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterRepGroup::~PtyEmitterRepGroup()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyEmitterRepGroup::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetEmitterRepGroup> pPW (new PtyWidgetEmitterRepGroup(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyEmitterRepGroup::updateData()
{
    Ptr<Universe::PartRepresentationGroup> pRepGroup = LM_DEBUG_PTR_CAST<Universe::PartRepresentationGroup>(getRepresentation());

    pRepGroup->setGroupName(_groupName);
    pRepGroup->setAlignOnDir(_alignOnDir);
    pRepGroup->setRestartGroups(_restartGroups);
    pRepGroup->setAffectSize(_affectSize);
    pRepGroup->setAffectRoll(_affectRoll);
    pRepGroup->setGlowAsColor(_glowAsColor);
}
//-----------------------------------------------------------------------------
void PtyEmitterRepGroup::updateProperty()
{
    Ptr<Universe::PartRepresentationGroup> pRepGroup = LM_DEBUG_PTR_CAST<Universe::PartRepresentationGroup>(getRepresentation());

    _groupName = pRepGroup->getGroupName();
    _alignOnDir = pRepGroup->getAlignOnDir();
    _restartGroups = pRepGroup->getRestartGroups();
    _affectSize = pRepGroup->getAffectSize();
    _affectRoll = pRepGroup->getAffectRoll();
    _glowAsColor = pRepGroup->getGlowAsColor();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyEmitterRepGroup::clone() const
{
    return Ptr<Property>(new PtyEmitterRepGroup( *this ));
}
//-----------------------------------------------------------------------------
void PtyEmitterRepGroup::internalCopy(const Ptr<Property>& pSrc)
{
    PtyEmitterRep::internalCopy(pSrc);
    Ptr<PtyEmitterRepGroup> pPty = LM_DEBUG_PTR_CAST<PtyEmitterRepGroup>(pSrc);

    _groupName      = pPty->_groupName;
    _alignOnDir     = pPty->_alignOnDir;
    _restartGroups  = pPty->_restartGroups;
    _affectSize     = pPty->_affectSize;
    _affectRoll     = pPty->_affectRoll;
    _glowAsColor    = pPty->_glowAsColor;
    
    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterRepGroup::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getRepresentation() == null);

    Ptr<Universe::PartRepresentationGroup> pRep = Ptr<Universe::PartRepresentationGroup>(new Universe::PartRepresentationGroup(pWorld.get(), L""));
    
    setRepresentation(pRep);
    getUniverseNodeEmitter()->setRepresentation(getRepresentation());

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetEmitterRepGroup::PtyWidgetEmitterRepGroup(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetEmitterRepGroup::~PtyWidgetEmitterRepGroup()
{}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterRepGroup::readProperty()
{
    Ptr<PtyEmitterRepGroup> pP = LM_DEBUG_PTR_CAST<PtyEmitterRepGroup>(getDataProxy()->getProperty());

    _groupName->setText(pP->_groupName);
    _alignOnDir->setChecked(pP->_alignOnDir);
    _restartGroups->setChecked(pP->_restartGroups);
    _affectSize->setChecked(pP->_affectSize);
    _affectRoll->setChecked(pP->_affectRoll);
    _glowAsColor->setChecked(pP->_glowAsColor);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterRepGroup::writeProperty(QWidget* pWidget)
{
    Ptr<PtyEmitterRepGroup> pP = LM_DEBUG_PTR_CAST<PtyEmitterRepGroup>(getDataProxy()->getProperty());

    _groupName->getText(pP->_groupName);
    pP->_alignOnDir = _alignOnDir->isChecked();
    pP->_restartGroups = _restartGroups->isChecked();
    pP->_affectSize = _affectSize->isChecked();
    pP->_affectRoll = _affectRoll->isChecked();
    pP->_glowAsColor = _glowAsColor->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterRepGroup::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupName    = new CustomLine(this, "Group name");
    _groupName->pushAuthorizedDropMimeData("asset/group");
    _groupName->setReadOnly(true);
    
    _alignOnDir     = new QCheckBox("Align on dir", this);
    _restartGroups  = new QCheckBox("Restart groups", this);
    _affectSize     = new QCheckBox("Affect size", this);
    _affectRoll     = new QCheckBox("Affect roll", this);
    _glowAsColor    = new QCheckBox("Glow as color", this);

    _layout->addWidget(_groupName);
    _layout->addWidget(_alignOnDir);
    _layout->addWidget(_restartGroups);
    _layout->addWidget(_affectSize);
    _layout->addWidget(_affectRoll);
    _layout->addWidget(_glowAsColor);

    setLayout(_layout);
    readProperty();

    getWidgetsForUndoRedo().push_back(_groupName);
    getWidgetsForUndoRedo().push_back(_alignOnDir);
    getWidgetsForUndoRedo().push_back(_restartGroups);
    getWidgetsForUndoRedo().push_back(_affectSize);
    getWidgetsForUndoRedo().push_back(_affectRoll);
    getWidgetsForUndoRedo().push_back(_glowAsColor);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI