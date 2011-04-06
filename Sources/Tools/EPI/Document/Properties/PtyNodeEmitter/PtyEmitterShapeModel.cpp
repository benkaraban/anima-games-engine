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
#include "PtyEmitterShapeModel.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyEmitterShapeModel::PtyEmitterShapeModel(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::RessourcePool> & pResPool,
    const Ptr<Universe::EmitterShapeModel>& pShape,
    const Core::String& title):
    PtyEmitterShape(pNodeE, pShape, title),
    _pResPool(pResPool)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterShapeModel::PtyEmitterShapeModel(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::RessourcePool> & pResPool,
    const Core::String& title):
    PtyEmitterShape(pNodeE, Ptr<Universe::EmitterShapeModel> (new Universe::EmitterShapeModel(pResPool)), title),
    _pResPool(pResPool)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterShapeModel::~PtyEmitterShapeModel()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyEmitterShapeModel::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetEmitterShapeModel> pPW(new PtyWidgetEmitterShapeModel(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeModel::updateProperty()
{
    Ptr<Universe::EmitterShapeModel> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeModel>(getShape());

    _modelName = pShape->getModel();
    _meshName  = pShape->getMesh();
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeModel::updateData()
{
    Ptr<Universe::EmitterShapeModel> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeModel>(getShape());

    pShape->setModel(_modelName, _meshName);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyEmitterShapeModel::clone() const
{
    return Ptr<Property>(new PtyEmitterShapeModel( *this ));
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeModel::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyEmitterShapeModel> pPty = LM_DEBUG_PTR_CAST<PtyEmitterShapeModel>(pSrc);

    _modelName = pPty->_modelName;
    _meshName  = pPty->_meshName;
    
    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeModel::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getShape()==null);

    Ptr<Universe::EmitterShapeModel> pShape = Ptr<Universe::EmitterShapeModel> (new Universe::EmitterShapeModel(_pResPool));
    
    setShape(pShape);
    getUniverseNodeEmitter()->setShape(getShape());

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeModel::PtyWidgetEmitterShapeModel(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeModel::~PtyWidgetEmitterShapeModel()
{}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeModel::updateMeshList(const String & modelName)
{
    setConnectToCommitPropertyChange(false);

    Ptr<PtyEmitterShapeModel> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeModel>(getDataProxy()->getProperty());
    Ptr<Assets::Model> pModel = pP->_pResPool->getModel(modelName);
    _meshName->clear();

    for(int32 ii=0; ii < pModel->getMeshCount(); ii++)
        _meshName->addItem(String8(pModel->getMesh(ii).getName()).c_str());

    setConnectToCommitPropertyChange(true);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeModel::readProperty()
{
    String modelName;
    Ptr<PtyEmitterShapeModel> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeModel>(getDataProxy()->getProperty());
    _modelName->getText(modelName);

    if(pP->_modelName != modelName)
    {
        _modelName->setText(pP->_modelName);
        updateMeshList(pP->_modelName);
    }

    _meshName->selectItem(pP->_meshName);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeModel::writeProperty(QWidget* pWidget)
{
    String modelName;
    Ptr<PtyEmitterShapeModel> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeModel>(getDataProxy()->getProperty());
    _modelName->getText(modelName);

    if(modelName != pP->_modelName)
    {
        pP->_modelName = modelName;
        updateMeshList(modelName);
    }

    pP->_meshName  = _meshName->selectedText();
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeModel::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _modelName    = new CustomLine(this, "Model name");
    _modelName->pushAuthorizedDropMimeData("asset/model");
    _modelName->setReadOnly(true);

    _meshName     = new QtToolbox::ComboBox(this, "Mesh name");

    _layout->addWidget(_modelName);
    _layout->addWidget(_meshName);

    setLayout(_layout);

    readProperty();

    getWidgetsForUndoRedo().push_back(_modelName);
    getWidgetsForUndoRedo().push_back(_meshName);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI