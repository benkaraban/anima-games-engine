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
#include "LightmapsGenerationOptions.moc.h"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>

#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <Core/System/Files.h>

#include <EPI/Constants.h>
#include <EPI/ImportInfo/LightmapsGenerationInfo.h>

#include <EPI/Gui/Widget/FileDialog.moc.h>

namespace EPI
{

LightmapsGenerationOptions::LightmapsGenerationOptions(QWidget * parent, bool import)
:   QWidget(parent),
    _layout(NULL),
    _suffix(NULL),
    _tagFilter(NULL),
    _defaultRes(NULL),
    _textureAtlasSize(NULL),
    _batchSize(NULL),
    _border(NULL)
{
    setupUi(import);
}

LightmapsGenerationOptions::~LightmapsGenerationOptions()
{
    LM_DELETE(_layout);
    LM_DELETE(_suffix);
    LM_DELETE(_tagFilter);
    LM_DELETE(_defaultRes);
    LM_DELETE(_textureAtlasSize);
    LM_DELETE(_batchSize);
    LM_DELETE(_border);
}

void LightmapsGenerationOptions::setupUi(bool import)
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _suffix = new QtToolbox::LineEdit(this, "Suffix", false, "-pak");
    _suffix->setFixedHeight(16);
    _layout->addWidget(_suffix, 0, 0, 1, 1);

    _tagFilter = new QtToolbox::LineEdit(this, "Filter tag", false, "");
    _tagFilter->setFixedHeight(16);
    _layout->addWidget(_tagFilter, 1, 0, 1, 1);

    _defaultRes = new QtToolbox::ComboBox(this, "Default res.");
    _defaultRes->setFixedHeight(16);
    _defaultRes->addItem("16");
    _defaultRes->addItem("32");
    _defaultRes->addItem("64");
    _defaultRes->addItem("128");
    _defaultRes->addItem("256");
    _defaultRes->addItem("512");
    _defaultRes->addItem("1024");
    _defaultRes->selectIndex(4);
    _layout->addWidget(_defaultRes, 3, 0, 1, 1);

    _textureAtlasSize = new QtToolbox::ComboBox(this, "Atlas size");
    _textureAtlasSize->setFixedHeight(16);
    _textureAtlasSize->addItem("1024");
    _textureAtlasSize->addItem("2048");
    _textureAtlasSize->addItem("4096");
    _textureAtlasSize->selectIndex(2);
    _layout->addWidget(_textureAtlasSize, 4, 0, 1, 1);

    _border = new QtToolbox::LineEdit(this, "Atlas border", false, "2");
    _border->setFixedHeight(16);
    _layout->addWidget(_border, 5, 0, 1, 1);

    _batchSize = new QtToolbox::LineEdit(this, "Batch size", false, "3000");
    _batchSize->setFixedHeight(16);
    _layout->addWidget(_batchSize, 6, 0, 1, 1);

    

    if(import)
    {
        _buttonBox = new QDialogButtonBox(this);
        _buttonBox->setOrientation(Qt::Horizontal);
        _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        
        connect(_buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
        connect(_buttonBox, SIGNAL(accepted()), this, SLOT(checkBeforeValidation()));

        _layout->addWidget(_buttonBox, 7, 0, 1, 3);
    }
    else
    {
        _generate = new QPushButton(QIcon(":/icons/gears.png"), "Generate", this);
        _suffix->setReadOnly(true);

        connect(_generate, SIGNAL(clicked()), this, SLOT(checkBeforeValidation()));
        _layout->addWidget(_generate, 7, 0, 1, 3);
    }
    

    setLayout(_layout);
    this->setFixedWidth(267);
    this->setFixedHeight(153);

    //connect(_dirOpen, SIGNAL(clicked()), this, SLOT(dirOpen()));
}

void LightmapsGenerationOptions::checkBeforeValidation()
{
    Core::String userSuffix(_suffix->text().toStdString().c_str());

    if(!userSuffix.empty())
        emit accept();
}

Ptr<LightmapsGenerationInfo> LightmapsGenerationOptions::getInfo() const
{
    Ptr<LightmapsGenerationInfo> pResult(new LightmapsGenerationInfo());

    pResult->suffix = String(_suffix->text().toStdString().c_str());
    pResult->tagFilter = String(_tagFilter->text().toStdString().c_str());
    pResult->defaultResolution = Core::toInteger(_defaultRes->selectedText());
    pResult->textureAtlasSize = Core::toInteger(_textureAtlasSize->selectedText());
    pResult->batchSize = _batchSize->text().toInt();
    pResult->border = _border->text().toInt();

    return pResult;
}

void LightmapsGenerationOptions::rejected()
{
    emit reject();
}


}//namespace EPI