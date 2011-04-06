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
#include "PtyDocNodeEmitters.h"

#include <QGridLayout>

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <EPI/Document/Properties/PtyNodeEmitter/PtyNodeEmitter.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocNodeEmitters::PtyDocNodeEmitters(const Ptr<Universe::World>& pWorld, bool loadingFile, const Core::String& title)
:   PropertyNode(title, true, false, CONTENT),
    _pWorld(pWorld)
{
    if(!loadingFile)
    {
        Ptr<Universe::NodeEmitter> pNodeEmitter = _pWorld->createBillboardEmitter();
        pNodeEmitter->beginMatrixUpdate();
        pNodeEmitter->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
        pNodeEmitter->endMatrixUpdate();
        Ptr<Property> pPtyNodeEmitter(new PtyNodeEmitter(pWorld, pNodeEmitter));
        
        addChild(pPtyNodeEmitter);
    }
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyDocNodeEmitters::~PtyDocNodeEmitters()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyDocNodeEmitters::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetDocNodeEmitters> pPW (new PtyWidgetDocNodeEmitters(pDataProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyDocNodeEmitters::updateData()
{
}
//-----------------------------------------------------------------------------
void PtyDocNodeEmitters::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocNodeEmitters::clone() const
{
    return Ptr<Property>(new PtyDocNodeEmitters( *this ));
}
//-----------------------------------------------------------------------------
void PtyDocNodeEmitters::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyDocNodeEmitters> pPtyDocNodeEmitters = LM_DEBUG_PTR_CAST<PtyDocNodeEmitters>(pSrc);

    _worldDeco  = pPtyDocNodeEmitters->_worldDeco;
    _pWorld     = pPtyDocNodeEmitters->_pWorld;

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocNodeEmitters::createNewNodeEmitter()
{
    Ptr<Universe::NodeEmitter> pNodeEmitter = _pWorld->createBillboardEmitter();
    pNodeEmitter->beginMatrixUpdate();
    pNodeEmitter->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    pNodeEmitter->endMatrixUpdate();

    Ptr<Property> pPtyNodeEmitter(new PtyNodeEmitter(_pWorld, pNodeEmitter));
    
    updateProperty();

    return pPtyNodeEmitter;
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocNodeEmitters::addNodeEmitter(Ptr<Universe::NodeEmitter> & pNodeEmitter)
{
    Ptr<Property> pPtyNodeEmitter(new PtyNodeEmitter(_pWorld, pNodeEmitter));
    
    updateProperty();

    return pPtyNodeEmitter;
}
//-----------------------------------------------------------------------------
const Core::String & PtyDocNodeEmitters::getWorldDecoPath()
{
    return _worldDeco;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetDocNodeEmitters::PtyWidgetDocNodeEmitters(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetDocNodeEmitters::~PtyWidgetDocNodeEmitters()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetDocNodeEmitters::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::SmartGroupBox(this, "World");

    _worldDeco = new QtToolbox::LineEdit(this, "Drop world here");
    _worldDeco->pushAuthorizedDropMimeData("asset/world");
    _worldDeco->setReadOnly(true);

    _groupBox->addWidget(_worldDeco);

    _layout->addWidget(_groupBox);

    getWidgetsForUndoRedo().push_back(_worldDeco);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocNodeEmitters::readProperty()
{
    Ptr<PtyDocNodeEmitters> pPtyDocNodeEmitters = LM_DEBUG_PTR_CAST<PtyDocNodeEmitters>(getDataProxy()->getProperty());
    _worldDeco->setText(Core::String8(pPtyDocNodeEmitters->_worldDeco).c_str());
}
//-----------------------------------------------------------------------------
void PtyWidgetDocNodeEmitters::writeProperty(QWidget* pWidget)
{
    Ptr<PtyDocNodeEmitters> pPtyDocNodeEmitters = LM_DEBUG_PTR_CAST<PtyDocNodeEmitters>(getDataProxy()->getProperty());
    pPtyDocNodeEmitters->_worldDeco = Core::String(_worldDeco->text().toStdString().c_str());
}
//-----------------------------------------------------------------------------
} // namespace EPI