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
#include "DialogChooseVfsMountPoint.moc.h"

#include <QFileDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>

namespace EPI
{
//-----------------------------------------------------------------------------
DialogChooseVfsMountPoint::DialogChooseVfsMountPoint(const Core::List<Core::String>& listPath, int32 listPathIndex, QWidget *parent) :
    QDialog(parent),
    _browseButton(NULL),
    _dirComboBox(NULL),
    _layout(NULL),
    _cancelButton(NULL),
    _currentIndex(listPathIndex),
    _strs(listPath),
    _IsShowAgain(true)
{
    setupUi();

    this->setWindowTitle("Select Working Directory");
}
//-----------------------------------------------------------------------------
void DialogChooseVfsMountPoint::setupUi()
{
    _browseButton = new QPushButton(QIcon(":/icons/add.png"), "", this);
    connect(_browseButton, SIGNAL(clicked()), this, SLOT(launchBrowseDir()));

    _cancelButton = new QPushButton("Cancel", this);
    connect(_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    _okButton = new QPushButton("Ok", this);
    connect(_okButton, SIGNAL(clicked()), this, SLOT(ok()));

    _dirComboBox = new QComboBox(this);
        for (int32 ii=0; ii<_strs.size(); ++ii)
        {
            _dirComboBox->addItem(String8(_strs[ii]).c_str());
        }
        _dirComboBox->setCurrentIndex(_currentIndex);
    _dirComboBox->setEditable(false);

    _labelAddPath = new QLabel("Add Path", this);
    _labelselectPath = new QLabel("Select Path", this);

    _showAgain = new QCheckBox("Show dialog at startup", this);
    _showAgain->setChecked(_IsShowAgain);
    _layout = new QGridLayout(this);
    
    _layout->addWidget(_labelselectPath, 0, 0, 1, 1);
    _layout->addWidget(_dirComboBox,     0, 1, 1, 8);
    _layout->addWidget(_labelAddPath,    1, 7, Qt::AlignRight);
    _layout->addWidget(_browseButton,    1, 8, Qt::AlignRight);
    _layout->addWidget(_showAgain,2,1);
    _layout->addWidget(_okButton,        2, 7, Qt::AlignRight);
    _layout->addWidget(_cancelButton,    2, 8, Qt::AlignRight);

    _browseButton->setFixedSize(50, 20);

    setFixedSize(600, 120);
    setLayout(_layout);
}
//-----------------------------------------------------------------------------
void DialogChooseVfsMountPoint::launchBrowseDir()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                tr("Find Files"), QDir::currentPath());

     if (!directory.isEmpty())
     {
         if (_dirComboBox->findText(directory) == -1)
             _dirComboBox->addItem(directory);
         _dirComboBox->setCurrentIndex(_dirComboBox->findText(directory));
     }

}
//-----------------------------------------------------------------------------
void DialogChooseVfsMountPoint::ok()
{
    if (_dirComboBox->count() == 0) reject();

    dirComboBoxToDirList(_strs, _currentIndex);
    _IsShowAgain = _showAgain->isChecked();
    accept();
}
//-----------------------------------------------------------------------------
void DialogChooseVfsMountPoint::dirComboBoxToDirList(Core::List<Core::String>& strs, int32& currentIndex)
{
    strs.clear();
    currentIndex = _dirComboBox->currentIndex();

    for (int32 ii=0; ii<_dirComboBox->count(); ++ii)
    {
        Core::String str (Core::String8(_dirComboBox->itemText(ii).toStdString().c_str()));
        strs.push_back(str);
    }
}
//-----------------------------------------------------------------------------
} //namespace EPI