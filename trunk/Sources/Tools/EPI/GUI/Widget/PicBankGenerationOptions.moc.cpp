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
#include "PicBankGenerationOptions.moc.h"

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
#include <EPI/ImportInfo/ImportPicBankInfo.h>

#include <EPI/Gui/Widget/FileDialog.moc.h>

namespace EPI
{

PicBankGenerationOptions::PicBankGenerationOptions(QWidget * parent, bool import)
:   QWidget(parent),
    _layout(NULL),
    _picBankNameLine(NULL),
    _dirNameLabel(NULL),
    _dirNameLine(NULL),
    _dirOpen(NULL),
    _maxSizeLine(NULL),
    _buttonBox(NULL),
    _horizontalSpacer(NULL),
    _path(IMPORT_FROM_SHARED_FOLDER)
{
    setupUi(import);
}

PicBankGenerationOptions::~PicBankGenerationOptions()
{
    LM_DELETE(_layout);
    LM_DELETE(_picBankNameLine);
    LM_DELETE(_dirNameLabel);
    LM_DELETE(_dirNameLine);
    LM_DELETE(_dirOpen);
    LM_DELETE(_maxSizeLine);
    LM_DELETE(_buttonBox);
    LM_DELETE(_horizontalSpacer);
}

void PicBankGenerationOptions::setupUi(bool import)
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _dirNameLabel = new QLabel("Source dir :", this);
    _dirNameLabel->setFixedHeight(16);
    _layout->addWidget(_dirNameLabel, 0, 0, 1, 1);

    _dirNameLine = new QLineEdit(this);
    _dirNameLine->setFixedHeight(16);
    _layout->addWidget(_dirNameLine, 0, 1, 1, 1);
    
    _dirOpen = new QPushButton(QIcon(":/icons/folder.png"), "", this);
    _dirOpen->setFixedWidth(35);
    _dirOpen->setFixedHeight(16);
    _layout->addWidget(_dirOpen, 0, 2, 1, 1);


    _picBankNameLine = new QtToolbox::LineEdit(this, "Bank name :");
    _picBankNameLine->setValidator("[A-Za-z0-9_]+");
    _layout->addWidget(_picBankNameLine, 1, 0, 1, 2);

    _maxSizeLine = new QtToolbox::LineEdit(this, "Pack size :", false, "1024");
    _maxSizeLine->setValidator("[0-9]+");
    _layout->addWidget(_maxSizeLine, 2, 0, 1, 2);


    if(import)
    {
        _buttonBox = new QDialogButtonBox(this);
        _buttonBox->setOrientation(Qt::Horizontal);
        _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        
        connect(_buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
        connect(_buttonBox, SIGNAL(accepted()), this, SLOT(checkBeforeValidation()));

        _layout->addWidget(_buttonBox, 3, 0, 1, 3);
    }
    else
    {
        _generate = new QPushButton(QIcon(":/icons/gears.png"), "Generate", this);

        _dirNameLine->setReadOnly(true);
        _dirOpen->setEnabled(false);
        _picBankNameLine->setEnabled(false);

        connect(_generate, SIGNAL(clicked()), this, SLOT(checkBeforeValidation()));
        _layout->addWidget(_generate, 3, 0, 1, 3);
    }
    

    setLayout(_layout);
    this->setFixedWidth(267);
    this->setFixedHeight(153);

    connect(_dirOpen, SIGNAL(clicked()), this, SLOT(dirOpen()));
}

bool PicBankGenerationOptions::dirOpen()
{
    //QString dir = QFileDialog::getExistingDirectory(NULL, tr("Open Directory"), String8(_path).c_str());

    String dir = FileDialog::getExistingDirectory(MiscConst::memoPathShared, IMPORT_FROM_SHARED_FOLDER,  MiscConst::sharedFolderShortcuts, NULL, L"Open Directory");

    if(dir.length() > 0)
    {
        String dirName(dir);

        if(dirName.startsWith(IMPORT_FROM_SHARED_FOLDER))
        {
            String lastDirName = dirName.slice(dirName.find_last_of(L"/") + 1);
            
            MiscConst::addPathToMemoPath(MiscConst::memoPathShared, dir);

            _dirNameLine->setText(StrToQStr(dir));
            _picBankNameLine->setText(String8(lastDirName).c_str());

            _path = dirName;
        }
        else
        {
            Core::String message = L"Your file doesn't seem to be in the right folder : ";
            message << IMPORT_FROM_SHARED_FOLDER;
            QMessageBox::critical(this, "Nyx", Core::String8(message).c_str());
        }
    }
    return (dir.length() > 0);
}

void PicBankGenerationOptions::checkBeforeValidation()
{
    Core::String userMaxSize(_maxSizeLine->text().toStdString().c_str());

    int32 maxSize = Core::toInteger(userMaxSize);

    bool errorDetected = false;

    QPalette error = _maxSizeLine->palette();
    QPalette noError = _maxSizeLine->palette();
    error.setColor(QPalette::Text, Qt::red);
    noError.setColor(QPalette::Text, Qt::black);

    if(!Core::isPowerOfTwo(maxSize))
    {
        _maxSizeLine->setPalette(error);
        errorDetected = true;
    }
    else
        _maxSizeLine->setPalette(noError);

    if(!errorDetected)
    {
        emit accept();
    }
}

void PicBankGenerationOptions::rejected()
{
    emit reject();
}

String PicBankGenerationOptions::getPicBankName() const
{
    return String(_picBankNameLine->text().toStdString().c_str());
}

String PicBankGenerationOptions::getDirName() const
{
    return String(_dirNameLine->text().toStdString().c_str());
}

int32 PicBankGenerationOptions::getMaxSize() const
{
    return Core::toInteger(String(_maxSizeLine->text().toStdString().c_str()));
}

void PicBankGenerationOptions::setPicBankInfo(const Ptr<ImportPicBankInfo> & pImportPicBankInfo)
{
    _dirNameLine->setText(String8(pImportPicBankInfo->dirName).c_str());
    _picBankNameLine->setText(String8(pImportPicBankInfo->picBankName).c_str());
    _maxSizeLine->setText(String8(Core::toString(pImportPicBankInfo->maxSize)).c_str());
}


}//namespace EPI