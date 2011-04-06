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
#include "PtyTerrainMesh.h"

#include <QGridLayout>

#include <Universe/World.h>

#include <QtToolbox/CollapsibleWidget.moc.h>

#include <EPI/ImportInfo/ImportTerrainInfo.h>
#include <EPI/Document/Properties/PtySun.moc.h>
#include <EPI/GUI/Widget/TerrainGenerationOptions.moc.h>

namespace EPI
{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

PtyTerrainMesh::PtyTerrainMesh( GuiDocument& guiDoc, 
                                const Ptr<Universe::World>& pWorld, 
                                const Ptr<ImportTerrainInfo> & pImportTerrainInfo,
                                const Core::String& title)
:   PropertyNode(title, true, true, CONTENT),
    _pWorld(pWorld),
    _pImportTerrainInfo(pImportTerrainInfo)
{
    Ptr<Property>       pPtySun (new PtySun(guiDoc, _pWorld));

    pPtySun->setSelectable(false);

    addChild(pPtySun);

    updateData();
}

PtyTerrainMesh::~PtyTerrainMesh()
{}

Ptr<PropertyWidget> PtyTerrainMesh::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetTerrainMesh> pPW (new PtyWidgetTerrainMesh(pDataProxy, _pImportTerrainInfo, parent));
    return pPW;
}

void PtyTerrainMesh::updateData()
{
}

Ptr<Property> PtyTerrainMesh::clone() const
{
    return Ptr<Property>(new PtyTerrainMesh( *this ));
}

void PtyTerrainMesh::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyTerrainMesh> pPty = LM_DEBUG_PTR_CAST<PtyTerrainMesh>(pSrc);

    _pWorld = pPty->_pWorld;

    updateData();
}

const Ptr<Property>& PtyTerrainMesh::getPtySun()
{
    return getChild(0);
}

void PtyTerrainMesh::generateTerrain(const Ptr<Workflow::TMGenInput> & pTerrainOptions)
{
    _pImportTerrainInfo->pTerrainOptions = pTerrainOptions;

    emit generate(_pImportTerrainInfo);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

PtyWidgetTerrainMesh::PtyWidgetTerrainMesh( const Ptr<PropertyWidgetDataProxy>& data, 
                                    const Ptr<ImportTerrainInfo> & pImportTerrainInfo, 
                                    QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
    _terrainGenerationOptions->setTerrainInfo(pImportTerrainInfo);
}

PtyWidgetTerrainMesh::~PtyWidgetTerrainMesh()
{}

void PtyWidgetTerrainMesh::readProperty()
{
    _pPtyWidgetSun->updatePropertyWidget();
}

void PtyWidgetTerrainMesh::writeProperty(QWidget* pWidget)
{}

void PtyWidgetTerrainMesh::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Terrain", false);
    _groupBox->setStyle(QtToolbox::CW_TITLE_1);
    _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _terrainGenerationOptions = new TerrainGenerationOptions(this, false);
    _groupBox->getLayout()->addWidget(_terrainGenerationOptions);

    Ptr<PtyTerrainMesh> pP = LM_DEBUG_PTR_CAST<PtyTerrainMesh>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPWSun (new PropertyWidgetDataProxy(pP->getPtySun(), pP->getName(), getDataProxy()->getStateRecorder()));
    _pPtyWidgetSun = pP->getPtySun()->createPropertyWidget(pDataPWSun, this);

    _layout->addWidget(_groupBox);
    _layout->addWidget(_pPtyWidgetSun.get());
   

    PropertyWidget::setupUi();

    connect(_terrainGenerationOptions, SIGNAL(accept()), this, SLOT(launchGeneration()));
}

void PtyWidgetTerrainMesh::launchGeneration()
{
    Ptr<Workflow::TMGenInput> pTerrainOptions = _terrainGenerationOptions->getTerrainOptions();
    LM_DEBUG_PTR_CAST<PtyTerrainMesh>(getDataProxy()->getProperty())->generateTerrain(pTerrainOptions);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}//namespace EPI