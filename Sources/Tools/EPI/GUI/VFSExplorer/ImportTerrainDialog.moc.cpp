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
#include "ImportTerrainDialog.moc.h"

#include <QVBoxLayout>
#include <Workflow/TerrainModelGen.h>
namespace EPI
{

ImportTerrainDialog::ImportTerrainDialog(QWidget * parent)
:   QDialog(parent),
    _layout(NULL),
    _terrainGenerationOptions(NULL)
{
    setupUi();
}

ImportTerrainDialog::~ImportTerrainDialog()
{
    if(_layout != NULL)
        delete _layout;
    if(_terrainGenerationOptions != NULL)
        delete _terrainGenerationOptions;
    
    _layout                     = NULL;
    _terrainGenerationOptions   = NULL;
}

void ImportTerrainDialog::setupUi()
{
    _layout = new QVBoxLayout(this);

    _terrainGenerationOptions = new TerrainGenerationOptions(this, true);
    _layout->addWidget(_terrainGenerationOptions);

    connect(_terrainGenerationOptions, SIGNAL(accept()), this, SLOT(accept()));
    connect(_terrainGenerationOptions, SIGNAL(reject()), this, SLOT(reject()));

    setMinimumSize(500, 300);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    
}

bool ImportTerrainDialog::fileOpen()
{
    return _terrainGenerationOptions->heightMapOpen();
}

Ptr<Workflow::TMGenInput> ImportTerrainDialog::getTerrainOptions() const
{
    return _terrainGenerationOptions->getTerrainOptions();
}
}//namespace EPI