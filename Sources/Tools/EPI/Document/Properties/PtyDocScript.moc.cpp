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
#include "PtyDocScript.moc.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <EPI/Document/Properties/PtyScript.moc.h>

#include <QTextEdit>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocScript::PtyDocScript(GuiDocument& guiDoc, const Ptr<DocumentRenderer>& pDocR, const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _pWorldA(Ptr<WorldScriptableForDocScript>(new WorldScriptableForDocScript(pDocR))),
    _guiDoc(guiDoc)
{
    Ptr<PtyScript>    pPtyScript (new PtyScript(_pWorldA, L"Script"));
    addChild(pPtyScript);
    pPtyScript->setSelectable(false);

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyDocScript::~PtyDocScript()
{

}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyDocScript::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetDocScript> pPW (new PtyWidgetDocScript(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyDocScript::updateData()
{
    _pWorldA->setWorldXMLDataPath(_worldXMLPath, _guiDoc);
    emit worldChanged();
}
//-----------------------------------------------------------------------------
void PtyDocScript::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocScript::clone() const
{
    return Ptr<Property>(new PtyDocScript( *this ));
}
//-----------------------------------------------------------------------------
void PtyDocScript::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyDocScript> pPty = LM_DEBUG_PTR_CAST<PtyDocScript>(pSrc);

    _pWorldA      = pPty->_pWorldA;
    _worldXMLPath = pPty->_worldXMLPath;
    _tagGameplay  = pPty->_tagGameplay;
    updateData();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyDocScript::getPtyScript()
{
    return getChild(0);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetDocScript::PtyWidgetDocScript(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetDocScript::~PtyWidgetDocScript()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetDocScript::setupUi()
{
    Ptr<PtyDocScript> pPtyDocScript = LM_DEBUG_PTR_CAST<PtyDocScript>(getDataProxy()->getProperty());

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _worldXML = new QtToolbox::LineEdit(this, "Drop world here");
        _worldXML->pushAuthorizedDropMimeData("asset/world");
        _worldXML->setReadOnly(true);

    _tagGameplay = new QtToolbox::LineEdit(this, "GamePlay Tags", false, "", 600);

    Ptr<PropertyWidgetDataProxy> pDataPWScript (new PropertyWidgetDataProxy(pPtyDocScript->getPtyScript(), pPtyDocScript->getName(), getDataProxy()->getStateRecorder()));
        _pPtyWidgetScript = pPtyDocScript->getPtyScript()->createPropertyWidget(pDataPWScript, this);


    _layout->addWidget(_worldXML);
    _layout->addWidget(_tagGameplay);
    _layout->addWidget(_pPtyWidgetScript.get(), 10);

    getWidgetsForUndoRedo().push_back(_tagGameplay);

    setLayout(_layout);

    connect(_worldXML, SIGNAL(valueUpdated()),
            this, SLOT (worldXMLChange()));

    connect(_pPtyWidgetScript.get(), SIGNAL(propertyModified()), this, SIGNAL(propertyModified()));
    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocScript::readProperty()
{
    Ptr<PtyDocScript> pPtyDocScript = LM_DEBUG_PTR_CAST<PtyDocScript>(getDataProxy()->getProperty());

    _worldXML->setText(Core::String8(pPtyDocScript->_worldXMLPath).c_str());
    _tagGameplay->setText(Core::String8(pPtyDocScript->_tagGameplay).c_str());
    _pPtyWidgetScript->updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocScript::writeProperty(QWidget* pWidget)
{
    Ptr<PtyDocScript> pPtyDocScript = LM_DEBUG_PTR_CAST<PtyDocScript>(getDataProxy()->getProperty());

    if (pWidget==NULL || pWidget == _worldXML)
    {
        pPtyDocScript->_worldXMLPath = Core::String(_worldXML->text().toStdString().c_str());
    }

    if (pWidget==NULL || pWidget == _tagGameplay)
    {
        pPtyDocScript->_tagGameplay = Core::String(_tagGameplay->text().toStdString().c_str());
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetDocScript::worldXMLChange()
{
    Ptr<PtyDocScript> pPtyDocScript = LM_DEBUG_PTR_CAST<PtyDocScript>(getDataProxy()->getProperty());

    pPtyDocScript->_worldXMLPath = Core::String(_worldXML->text().toStdString().c_str());
    pPtyDocScript->updateData();
    //updatePropertyWidget();
/*
    makeConnectionToProperties(false);
        pPtyDocScript->launchSignalProeprtyChanged();
    makeConnectionToProperties(true);

    emit propertyModified();*/
}
//-----------------------------------------------------------------------------
} // namespace EPI