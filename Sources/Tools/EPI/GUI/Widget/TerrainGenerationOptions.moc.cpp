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
#include "TerrainGenerationOptions.moc.h"

#include <Workflow/TerrainModelGen.h>

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>

#include <QtToolbox/LineEdit.moc.h>

#include <Core/System/Files.h>

#include <EPI/Constants.h>
#include <EPI/ImportInfo/ImportTerrainInfo.h>

#include <EPI/Gui/Widget/FileDialog.moc.h>

namespace EPI
{

const int32 LOD_MIN = 1;
const int32 LOD_MAX = 10;
const int32 BLOCK_W_MIN = 10;
const int32 BLOCK_W_MAX = 100;
const double BLEND_SCALE_MIN = 0.0;
const double BLEND_SCALE_MAX = 1.0;
const int32 MAX_HEIGHT_MIN = 0; 
const int32 MAX_HEIGHT_MAX = 1000;
const int32 TERRAIN_SIZE_MIN = 1;
const int32 TERRAIN_SIZE_MAX = 10000;

TerrainGenerationOptions::TerrainGenerationOptions(QWidget * parent, bool import)
:   QWidget(parent),
    _layout(NULL),
    _heightMapLabel(NULL),
    _heightMapLine(NULL),
    _heightMapOpen(NULL),
    _normalMapLabel(NULL),
    _normalMapLine(NULL),
    _normalMapOpen(NULL),
    _levelOfDetailEdit(NULL),
    _blockWidthEdit(NULL),
    _blendScaleFactorEdit(NULL),
    _maxHeightEdit(NULL),
    _terrainSizeEdit(NULL),
    _buttonBox(NULL),
    _generate(NULL),
    _heightMap(L""),
    _normalMap(L""),
    _path(IMPORT_FROM_SHARED_FOLDER)
{
    setupUi(import);
}

TerrainGenerationOptions::~TerrainGenerationOptions()
{
}

void TerrainGenerationOptions::setupUi(bool import)
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _terrainNameLabel = new QLabel("Terrain Name :", this);
    _terrainNameLabel->setFixedHeight(16);
    _layout->addWidget(_heightMapLabel, 0, 0, 1, 1);

    _terrainNameLine = new QLineEdit(this);
    _terrainNameLine->setFixedHeight(16);
    _layout->addWidget(_terrainNameLine, 0, 1, 1, 1);

    _heightMapLabel = new QLabel("Height map :", this);
    _heightMapLabel->setFixedHeight(16);
    _layout->addWidget(_heightMapLabel, 1, 0, 1, 1);

    _heightMapLine = new QLineEdit(this);
    _heightMapLine->setFixedHeight(16);
    _heightMapLine->setReadOnly(true);
    _layout->addWidget(_heightMapLine, 1, 1, 1, 1);

    _heightMapOpen = new QPushButton(QIcon(":/icons/folder.png"), "", this);
    _heightMapOpen->setFixedWidth(35);
    _heightMapOpen->setFixedHeight(16);
    _layout->addWidget(_heightMapOpen, 1, 2, 1, 1);

    _normalMapLabel = new QLabel("Normal map :", this);
    _normalMapLabel->setFixedHeight(16);
    _layout->addWidget(_normalMapLabel, 2, 0, 1, 1);

    _normalMapLine = new QLineEdit(this);
    _normalMapLine->setFixedHeight(16);
    _normalMapLine->setReadOnly(true);
    _layout->addWidget(_normalMapLine, 2, 1, 1, 1);

    _normalMapOpen = new QPushButton(QIcon(":/icons/folder.png"), "", this);
    _normalMapOpen->setFixedWidth(35);
    _normalMapOpen->setFixedHeight(16);
    _layout->addWidget(_normalMapOpen, 2, 2, 1, 1);

    _levelOfDetailEdit = new QtToolbox::LineEdit(this, "Level of detail", false, "4");
    _levelOfDetailEdit->setValidator(LOD_MIN, LOD_MAX);
    _layout->addWidget(_levelOfDetailEdit, 3, 0, 1, 3);

    _blockWidthEdit = new QtToolbox::LineEdit(this, "Block width", false, "21");
    _blockWidthEdit->setValidator(BLOCK_W_MIN, BLOCK_W_MAX);
    _layout->addWidget(_blockWidthEdit, 4, 0, 1, 3);

    _blendScaleFactorEdit = new QtToolbox::LineEdit(this, "Blend scale", false, "0.7");
    _blendScaleFactorEdit->setValidator(BLEND_SCALE_MIN, BLEND_SCALE_MAX, 6);
    _layout->addWidget(_blendScaleFactorEdit, 5, 0, 1, 3);

    _maxHeightEdit = new QtToolbox::LineEdit(this, "Max height", false, "180");
    _maxHeightEdit->setValidator(MAX_HEIGHT_MIN, MAX_HEIGHT_MAX);
    _layout->addWidget(_maxHeightEdit, 6, 0, 1, 3);

    _terrainSizeEdit = new QtToolbox::LineEdit(this, "Terrain size", false, "500");
    _terrainSizeEdit->setValidator(TERRAIN_SIZE_MIN, TERRAIN_SIZE_MAX);
    _layout->addWidget(_terrainSizeEdit, 7, 0, 1, 3);

    if(import)
    {
        _buttonBox = new QDialogButtonBox(this);
        _buttonBox->setOrientation(Qt::Horizontal);
        _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(false);
        
        connect(_buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
        connect(_buttonBox, SIGNAL(accepted()), this, SLOT(checkBeforeValidation()));

        _layout->addWidget(_buttonBox, 8, 0, 1, 3);
    }
    else
    {
        _generate = new QPushButton(QIcon(":/icons/gears.png"), "Generate", this);
        _generate->setEnabled(false);

        _terrainNameLine->setReadOnly(true);
        _heightMapLine->setReadOnly(true);
        _heightMapOpen->setEnabled(false);
        _normalMapLine->setReadOnly(true);
        _normalMapOpen->setEnabled(false);

        connect(_generate, SIGNAL(clicked()), this, SLOT(checkBeforeValidation()));
        _layout->addWidget(_generate, 8, 0, 1, 3);
    }   

    setLayout(_layout);
/*
    this->setFixedWidth(261);
    this->setFixedHeight(205);
*/
    connect(_heightMapOpen, SIGNAL(clicked()), this, SLOT(heightMapOpen()));
    connect(_normalMapOpen, SIGNAL(clicked()), this, SLOT(normalMapOpen()));

    if(!import)
    {
        connect(_levelOfDetailEdit,     SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_blockWidthEdit,        SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_blendScaleFactorEdit,  SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_maxHeightEdit,         SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_terrainSizeEdit,       SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
    }
}

Core::String TerrainGenerationOptions::getTerrainName() const
{
    Core::String terrainName(_terrainNameLine->text().toStdString().c_str());

    if(terrainName == Core::String::EMPTY)
    {
        terrainName = Core::System::getFileBaseName(_heightMap);
    }

    return terrainName;
}

Ptr<Workflow::TMGenInput> TerrainGenerationOptions::getTerrainOptions() const
{
    Ptr<Workflow::TMGenInput>   pTerrainOptions(new Workflow::TMGenInput());

    Core::String userLOD(_levelOfDetailEdit->text().toStdString().c_str());
    Core::String userBlockWidth(_blockWidthEdit->text().toStdString().c_str());
    Core::String userMaxHeight(_maxHeightEdit->text().toStdString().c_str());
    Core::String userTerrainSize(_terrainSizeEdit->text().toStdString().c_str());
    Core::String userBlendScaleFactor(_blendScaleFactorEdit->text().toStdString().c_str());

    pTerrainOptions->LODCount = Core::toInteger(userLOD);
    pTerrainOptions->blockWidth = Core::toInteger(userBlockWidth);
    pTerrainOptions->maxHeight = Core::toInteger(userMaxHeight);
    pTerrainOptions->terrainSize.x = Core::toInteger(userTerrainSize);
    pTerrainOptions->terrainSize.y = Core::toInteger(userTerrainSize);
    pTerrainOptions->blendScaleFactor = Core::toFloat(userBlendScaleFactor);

    return pTerrainOptions;
}

void TerrainGenerationOptions::setTerrainInfo(const Ptr<ImportTerrainInfo> & pImportTerrainInfo)
{
    _heightMapLine->setText(Core::String8(pImportTerrainInfo->heightMapName).c_str());
    _normalMapLine->setText(Core::String8(pImportTerrainInfo->normalMapName).c_str());

    _levelOfDetailEdit->setText(Core::String8(Core::toString(pImportTerrainInfo->pTerrainOptions->LODCount)).c_str());
    _blockWidthEdit->setText(Core::String8(Core::toString(pImportTerrainInfo->pTerrainOptions->blockWidth)).c_str());
    _blendScaleFactorEdit->setText(Core::String8(Core::toString(pImportTerrainInfo->pTerrainOptions->blendScaleFactor)).c_str());
    int32 maxHeight = pImportTerrainInfo->pTerrainOptions->maxHeight;
    _maxHeightEdit->setText(Core::String8(Core::toString(maxHeight)).c_str());
    int32 terrainSize = pImportTerrainInfo->pTerrainOptions->terrainSize.x;
    _terrainSizeEdit->setText(Core::String8(Core::toString(terrainSize)).c_str());

    _generate->setEnabled(false);
}

bool TerrainGenerationOptions::heightMapOpen()
{
    //QString file = QFileDialog::getOpenFileName(NULL, tr("Open File"), String8(_path).c_str(), String8(AUTHORISED_TERRAIN_EXT).c_str());
    String file = FileDialog::getOpenFileName(MiscConst::memoPathShared, IMPORT_FROM_SHARED_FOLDER,  MiscConst::sharedFolderShortcuts, NULL, L"Open File", AUTHORISED_TERRAIN_EXT);

    if( file.length() > 0 )
    {
        Core::String fileName(file);

        if(Core::System::getPath(fileName).startsWith(IMPORT_FROM_SHARED_FOLDER))
        {
            String path = Core::System::getPath(fileName);
            MiscConst::addPathToMemoPath(MiscConst::memoPathShared, path);

            _terrainNameLine->setText(String8(Core::System::getFileBaseName(fileName)).c_str());
            _heightMapLine->setText(String8(fileName).c_str());
            _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(true);

            _path = Core::System::getPath(fileName);
            _heightMap = Core::String(_heightMapLine->text().toStdString().c_str());
        }
        else
        {
            Core::String message = L"Your file doesn't seem to be in the right folder : ";
            message << IMPORT_FROM_SHARED_FOLDER;
            QMessageBox::critical(this, "Nyx", Core::String8(message).c_str());
        }
    }
    return (file.length() > 0);
}

void TerrainGenerationOptions::normalMapOpen()
{
    //QString file = QFileDialog::getOpenFileName(this, tr("Open File"), String8(_path).c_str(), String8(AUTHORISED_IMAGE_EXT).c_str());
    String file = FileDialog::getOpenFileName(MiscConst::memoPathShared, IMPORT_FROM_SHARED_FOLDER,  MiscConst::sharedFolderShortcuts, NULL, L"Open File", AUTHORISED_IMAGE_EXT);

    if(file.length() > 0 )
    {
        Core::String fileName(file);
        if(Core::System::getPath(fileName).startsWith(IMPORT_FROM_SHARED_FOLDER))
        {
            String path = Core::System::getPath(fileName);
            MiscConst::addPathToMemoPath(MiscConst::memoPathShared, path);

            _normalMapLine->setText(String8(fileName).c_str());
            if(_heightMapLine->text().length() > 0)
                _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(true);

            _path = Core::System::getPath(fileName);
            _normalMap = Core::String(_normalMapLine->text().toStdString().c_str());
        }
        else
        {
            Core::String message = L"Your file doesn't seem to be in the right folder : ";
            message << IMPORT_FROM_SHARED_FOLDER;
            QMessageBox::critical(this, "Nyx", Core::String8(message).c_str());
        }
    }
}

void TerrainGenerationOptions::checkBeforeValidation()
{
    int32 levelOfDetail = 0;
    int32 blockWidth    = 0;
    int32 maxHeight     = 0;
    int32 terrainSize   = 0;
    double blendScaleFactor = 0.0;

    Core::String userLOD(_levelOfDetailEdit->text().toStdString().c_str());
    Core::String userBlockWidth(_blockWidthEdit->text().toStdString().c_str());
    Core::String userMaxHeight(_maxHeightEdit->text().toStdString().c_str());
    Core::String userTerrainSize(_terrainSizeEdit->text().toStdString().c_str());
    Core::String userBlendScaleFactor(_blendScaleFactorEdit->text().toStdString().c_str());

    levelOfDetail = Core::toInteger(userLOD);
    blockWidth    = Core::toInteger(userBlockWidth);
    maxHeight     = Core::toInteger(userMaxHeight);
    terrainSize   = Core::toInteger(userTerrainSize);
    blendScaleFactor = Core::toFloat(userBlendScaleFactor);

    bool errorDetected = false;

    QPalette error = _blendScaleFactorEdit->palette();
    QPalette noError = _blendScaleFactorEdit->palette();
    error.setColor(QPalette::Text, Qt::red);
    noError.setColor(QPalette::Text, Qt::black);

    if(levelOfDetail < LOD_MIN || levelOfDetail > LOD_MAX)
    {
        _levelOfDetailEdit->setPalette(error);
        errorDetected = true;
    }
    else
        _levelOfDetailEdit->setPalette(noError);

    if(blockWidth < BLOCK_W_MIN || blockWidth > BLOCK_W_MAX)
    {
        _blockWidthEdit->setPalette(error);
        errorDetected = true;
    }
    else
        _blockWidthEdit->setPalette(noError);

    if(maxHeight < MAX_HEIGHT_MIN || maxHeight > MAX_HEIGHT_MAX)
    {
        _maxHeightEdit->setPalette(error);
        errorDetected = true;
    }
    else
        _maxHeightEdit->setPalette(noError);

    if(terrainSize < TERRAIN_SIZE_MIN || terrainSize > TERRAIN_SIZE_MAX)
    {
        _terrainSizeEdit->setPalette(error);
        errorDetected = true;
    }
    else
        _terrainSizeEdit->setPalette(noError);

    if(blendScaleFactor < BLEND_SCALE_MIN || blendScaleFactor > BLEND_SCALE_MAX)
    {
        _blendScaleFactorEdit->setPalette(error);
        errorDetected = true;
    }
    else
        _blendScaleFactorEdit->setPalette(noError);

    if(!errorDetected)
    {
        if(_generate != NULL)
            _generate->setEnabled(false);

        emit accept();
    }

}

void TerrainGenerationOptions::rejected()
{
    emit reject();
}

void TerrainGenerationOptions::optionsChanged()
{
    if(_generate != NULL)
    {
        _generate->setEnabled(true);
    }
}

}//namespace EPI