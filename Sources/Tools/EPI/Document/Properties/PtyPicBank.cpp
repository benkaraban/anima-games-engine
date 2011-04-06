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
#include "PtyPicBank.h"

#include <QGridLayout>

#include <Universe/World.h>

#include <QtToolbox/CollapsibleWidget.moc.h>

#include <EPI/ImportInfo/ImportPicBankInfo.h>
#include <EPI/GUI/Widget/PicBankGenerationOptions.moc.h>

#include <Workflow/TexCompression.h>

namespace EPI
{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyPicBank::PtyPicBank( const Ptr<Universe::World> pWorld,
                        const Ptr<ImportPicBankInfo> & pImportPicBankInfo, 
                        const Core::String& title)
:   PropertyNode(title, true, false, CONTENT),
    _pImportPicBankInfo(pImportPicBankInfo),
    _pWorld(pWorld)
{
    updateProperty();
}

PtyPicBank::~PtyPicBank()
{}

Ptr<PropertyWidget> PtyPicBank::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPicBank> pPW (new PtyWidgetPicBank(pDataProxy, _pImportPicBankInfo, parent));
    return pPW;
}

void PtyPicBank::updateData()
{
    _pWorld->setFogColor(_bcgColor);
}

void PtyPicBank::updateProperty()
{
    _bcgColor = _pWorld->getFogColor();  
}

Ptr<Property> PtyPicBank::clone() const
{
    return Ptr<Property>(new PtyPicBank( *this ));
}

void PtyPicBank::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyPicBank> pPty = LM_DEBUG_PTR_CAST<PtyPicBank>(pSrc);

    _pWorld   = pPty->_pWorld;
    _bcgColor = pPty->_bcgColor;

    updateData();
}

void PtyPicBank::generatePicBank(int32 maxSize)
{
    _pImportPicBankInfo->maxSize = maxSize;

    emit generate(_pImportPicBankInfo);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

PtyWidgetPicBank::PtyWidgetPicBank( const Ptr<PropertyWidgetDataProxy>& data, 
                                    const Ptr<ImportPicBankInfo> & pImportPicBankInfo, 
                                    QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
    _picBankGenerationOptions->setPicBankInfo(pImportPicBankInfo);
}

PtyWidgetPicBank::~PtyWidgetPicBank()
{}

void PtyWidgetPicBank::readProperty()
{
    Ptr<PtyPicBank> pPty = LM_DEBUG_PTR_CAST<PtyPicBank>(getDataProxy()->getProperty());

    _backGroundColor->setColorLinear(pPty->_bcgColor);
}

void PtyWidgetPicBank::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPicBank> pPty = LM_DEBUG_PTR_CAST<PtyPicBank>(getDataProxy()->getProperty());

    _backGroundColor->getColorLinear(pPty->_bcgColor);
}

void PtyWidgetPicBank::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBoxDoc   = new QtToolbox::CollapsibleWidget(this, "World", false);
    _groupBox      = new QtToolbox::CollapsibleWidget(this, "PicBank", false);

    _backGroundColor          = new QtToolbox::QuickColorPicker(this, "BackGround Color");
    _picBankGenerationOptions = new PicBankGenerationOptions(this, false);

    
    _groupBoxDoc->getLayout()->addWidget(_backGroundColor);
    _groupBox->getLayout()->addWidget(_picBankGenerationOptions);

    _layout->addWidget(_groupBoxDoc);
    _layout->addWidget(_groupBox);

    getWidgetsForUndoRedo().push_back(_backGroundColor);

    PropertyWidget::setupUi();

    connect(_picBankGenerationOptions, SIGNAL(accept()), this, SLOT(launchGeneration()));
    _groupBoxDoc->setFixedHeight(150);
}

void PtyWidgetPicBank::launchGeneration()
{
    LM_DEBUG_PTR_CAST<PtyPicBank>(getDataProxy()->getProperty())->generatePicBank(_picBankGenerationOptions->getMaxSize());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}//namespace EPI