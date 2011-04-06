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
#include "PtyDocModel.moc.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <EPI/Document/Properties/PtyModel.moc.h>
#include <EPI/Document/Properties/PtySun.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocModel::PtyDocModel(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorld,  const Ptr<StateRecorder>& pStateRecorder, const Ptr<ImportModelInfo>& pModelInfo, const Core::String& title):
    _pWorld(pWorld),
    _pModelInfo(pModelInfo),
    PropertyNode(title, true, false, CONTENT)
{
    Core::Ptr<Universe::Node> pNodeInstance = _pWorld->createInstance(pModelInfo->modelName);
        if(pNodeInstance->getNodeType() != Universe::NODE         && 
           pNodeInstance->getNodeType() != Universe::NODE_SKELETON)
        {
            throw Core::Exception(String(L"Unknown Model type : ") << pNodeInstance->getNodeType());
        }

    Ptr<Property> pPtyModel (new PtyModel(_pWorld, pStateRecorder, pNodeInstance, _pModelInfo, L"Model"));
    pPtyModel->setDeletable(false);
    Ptr<Property> pPtySun   (new PtySun(guiDoc, _pWorld));

    addChild(pPtyModel);
    addChild(pPtySun);

    updateProperty();

    connect(pPtyModel.get(), SIGNAL(save(const Ptr<Assets::Model>&)),
            this, SLOT(saveModel(const Ptr<Assets::Model>&)));
}
//-----------------------------------------------------------------------------
PtyDocModel::~PtyDocModel()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyDocModel::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetDocModel> pPW (new PtyWidgetDocModel(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyDocModel::updateData()
{
}
//-----------------------------------------------------------------------------
void PtyDocModel::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocModel::clone() const
{
    return Ptr<Property>(new PtyDocModel( *this ));
}
//-----------------------------------------------------------------------------
void PtyDocModel::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyDocModel> pPty = LM_DEBUG_PTR_CAST<PtyDocModel>(pSrc);

    _worldDeco = pPty->_worldDeco;

    updateData();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyDocModel::getPtyModel()
{
    return getChild(0);
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyDocModel::getPtySun()
{
    return getChild(1);
}
//-----------------------------------------------------------------------------
void PtyDocModel::save(const Core::String & fileName)
{
    _pModelInfo->modelName = fileName;
    LM_DEBUG_PTR_CAST<PtyModel>(getPtyModel())->saveModel();;
}
//-----------------------------------------------------------------------------
void PtyDocModel::saveModel(const Ptr<Assets::Model>& pModel)
{
    _pModelInfo->import = IMPORT_MODEL_SAVE;
    _pModelInfo->pModel = pModel;

    emit generate(_pModelInfo);
}
//-----------------------------------------------------------------------------
const Core::String & PtyDocModel::getWorldDecoPath()
{
    return _worldDeco;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetDocModel::PtyWidgetDocModel(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetDocModel::~PtyWidgetDocModel()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetDocModel::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::SmartGroupBox(this, "World", true);
    
    _worldDeco = new QtToolbox::LineEdit(this, "Drop world here");
    _worldDeco->pushAuthorizedDropMimeData("asset/world");
    _worldDeco->setReadOnly(true);

    _groupBox->addWidget(_worldDeco);

    _layout->addWidget(_groupBox);

    getWidgetsForUndoRedo().push_back(_worldDeco);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocModel::readProperty()
{
    Ptr<PtyDocModel> pPtyDocModel = LM_DEBUG_PTR_CAST<PtyDocModel>(getDataProxy()->getProperty());
    _worldDeco->setText(Core::String8(pPtyDocModel->_worldDeco).c_str());
}
//-----------------------------------------------------------------------------
void PtyWidgetDocModel::writeProperty(QWidget* pWidget)
{
    Ptr<PtyDocModel> pPtyDocModel = LM_DEBUG_PTR_CAST<PtyDocModel>(getDataProxy()->getProperty());
    pPtyDocModel->_worldDeco = Core::String(_worldDeco->text().toStdString().c_str());
}
//-----------------------------------------------------------------------------

} // namespace EPI