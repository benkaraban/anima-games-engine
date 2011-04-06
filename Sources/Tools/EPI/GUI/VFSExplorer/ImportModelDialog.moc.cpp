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
#include "ImportModelDialog.moc.h"

#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QSpacerItem>

#include <QtToolbox/LineEdit.moc.h>

#include <EPI/Constants.h>
#include <EPI/ImportInfo/ImportModelInfo.h>

#include <Core/System/Files.h>
#include <Workflow/ModelOptimize.h>

#include <EPI/Gui/Widget/FileDialog.moc.h>

namespace EPI
{
ImportModelDialog::ImportModelDialog(QWidget * parent)
:   QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    _layout(NULL),
    _selectAll(NULL),
    _importModelLabel(NULL),
    _fileOpen(NULL),
    _listWidget(NULL),
    _generateNormals(NULL),
    _generateTangentBasis(NULL),
    _optimizeForCache(NULL),
    _maxBonesPerVertex(NULL),
    _maxBonesPerBlendingGroup(NULL),
    _buttonBox(NULL),
    _horizontalSpacer(NULL),
    _path(IMPORT_FROM_SHARED_FOLDER)
{
    setupUi();
}

ImportModelDialog::ImportModelDialog(QWidget * parent, const Ptr<ImportModelInfo> & pImportModelInfo)
:   QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    _layout(NULL),
    _selectAll(NULL),
    _importModelLabel(NULL),
    _fileOpen(NULL),
    _listWidget(NULL),
    _generateNormals(NULL),
    _generateTangentBasis(NULL),
    _optimizeForCache(NULL),
    _maxBonesPerVertex(NULL),
    _maxBonesPerBlendingGroup(NULL),
    _buttonBox(NULL),
    _horizontalSpacer(NULL),
    _path(IMPORT_FROM_SHARED_FOLDER)
{
    setupUi();

    initializeFromImportModelInfo(pImportModelInfo);
}

ImportModelDialog::~ImportModelDialog()
{
    if(_layout != NULL)
        delete _layout;
    if(_selectAll != NULL)
        delete _selectAll;
    if(_importModelLabel != NULL)
        delete _importModelLabel;
    if(_fileOpen != NULL)
        delete _fileOpen;
    if(_listWidget != NULL)
        delete _listWidget;
    if(_generateNormals != NULL)
        delete _generateNormals;
    if(_generateTangentBasis != NULL)
        delete _generateTangentBasis;
    if(_optimizeForCache != NULL)
        delete _optimizeForCache;
    if(_maximumBonesPer != NULL)
        delete _maximumBonesPer;
    if(_maxBonesPerVertex != NULL)
        delete _maxBonesPerVertex;
    if(_maxBonesPerBlendingGroup != NULL)
        delete _maxBonesPerBlendingGroup;
    if(_buttonBox != NULL)
        delete _buttonBox;


    _layout = NULL;
    _selectAll = NULL;
    _importModelLabel = NULL;
    _fileOpen = NULL;
    _listWidget = NULL;
    _generateNormals = NULL;
    _generateTangentBasis = NULL;
    _optimizeForCache = NULL;
    _maximumBonesPer = NULL;
    _maxBonesPerVertex = NULL;
    _maxBonesPerBlendingGroup = NULL;
    _buttonBox = NULL;
}

void ImportModelDialog::setupUi()
{
    _layout = new QGridLayout(this);

    _importModelLabel = new QLabel("Import models :", this);
    _importModelLabel->setFixedHeight(16);

    _fileOpen = new QPushButton(QIcon(":/icons/folder.png"), "...", this);
    _fileOpen->setFixedWidth(35);
    _fileOpen->setFixedHeight(16);

    _listWidget = new QListWidget(this);
    _listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    _selectAll = new QCheckBox("Select all", this);
    _selectAll->setFixedHeight(16);
    _selectAll->setChecked(true);

    _buttonBox = new QDialogButtonBox(this);
    _buttonBox->setOrientation(Qt::Horizontal);
    _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(false);

    _horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    _generateNormals = new QCheckBox("Generate normals", this);
    _generateNormals->setChecked(false);
    _generateTangentBasis = new QCheckBox("Generate tangent basis", this);
    _generateTangentBasis->setChecked(true);
    _optimizeForCache = new QCheckBox("Optimize for cache", this);
    _optimizeForCache->setChecked(true);
    _maximumBonesPer = new QLabel(this);
    _maximumBonesPer->setText("Maximum bones per :");
    _maxBonesPerVertex = new QtToolbox::LineEdit(this, "vertex", false, "4", 70);
    _maxBonesPerVertex->setValidator(1, 4);
    _maxBonesPerBlendingGroup = new QtToolbox::LineEdit(this, "blending group", false, "70", 70);
    _maxBonesPerBlendingGroup->setValidator(1, 70);

    _layout->addWidget(_selectAll, 0, 0, 1, 1);
    _layout->addWidget(_importModelLabel, 0, 1, 1, 1, Qt::AlignRight);
    _layout->addWidget(_fileOpen, 0, 2, 1, 1);
    _layout->addWidget(_listWidget, 1, 0, 1, 3);
    _layout->addWidget(_generateNormals, 2, 0, 1, 3);
    _layout->addWidget(_generateTangentBasis, 3, 0, 1, 3);
    _layout->addWidget(_optimizeForCache, 4, 0, 1, 3);
    _layout->addWidget(_maximumBonesPer, 5, 0, 1, 3);
    _layout->addWidget(_maxBonesPerVertex, 6, 0, 1, 3);
    _layout->addWidget(_maxBonesPerBlendingGroup, 7, 0, 1, 3);
    _layout->addItem  (_horizontalSpacer, 8, 0, 1, 1);
    _layout->addWidget(_buttonBox, 8, 1, 1, 2);
    
    setLayout(_layout);
    this->setFixedWidth(259);
    this->setFixedHeight(300);

    connect(_fileOpen,  SIGNAL(clicked()), this, SLOT(fileOpen()));
    connect(_selectAll, SIGNAL(stateChanged (int)), this, SLOT(onSelectAll(int)));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(checkBeforeValidation()));
}

const Core::List<Core::String> & ImportModelDialog::getModelNames() const
{
    return _modelNames;
}

void ImportModelDialog::getModelPackingOptions(Workflow::ModelPackingOptions & modelPackingOptions) const
{
    int32 maxBonesPerVertex = 1;
    int32 maxBonesPerBlendingGroup = 1;

    Core::String userMaxBonesPerVertex(_maxBonesPerVertex->text().toStdString().c_str());
    Core::String userMaxBonesPerBlendingGroup(_maxBonesPerBlendingGroup->text().toStdString().c_str());

    maxBonesPerVertex = Core::toInteger(userMaxBonesPerVertex);
    maxBonesPerBlendingGroup = Core::toInteger(userMaxBonesPerBlendingGroup);

    modelPackingOptions.maxBonesPerVertex = maxBonesPerVertex;
    modelPackingOptions.maxBonesPerBlendingGroup = maxBonesPerBlendingGroup;
    modelPackingOptions.generateNormals = (_generateNormals->checkState() == Qt::Checked);
    modelPackingOptions.generateTangentBasis = (_generateTangentBasis->checkState() == Qt::Checked);
    modelPackingOptions.optimizeForCache = (_optimizeForCache->checkState() == Qt::Checked);
}

bool ImportModelDialog::fileOpen()
{
    Core::List<String> files = FileDialog::getOpenFileNames(MiscConst::memoPathShared, IMPORT_FROM_SHARED_FOLDER,  MiscConst::sharedFolderShortcuts, NULL, L"Open File", AUTHORISED_MODEL_EXT);
    _listWidget->clear();
    if(files.size() > 0 )
    {
        for(int32 ii = 0; ii<files.size(); ii++)
        {
            Core::String fileName(files[ii]);
            if(Core::System::getPath(fileName).startsWith(IMPORT_FROM_SHARED_FOLDER))
            {
                String path = Core::System::getPath(fileName);
                MiscConst::addPathToMemoPath(MiscConst::memoPathShared, path);
                QListWidgetItem * item = new QListWidgetItem(StrToQStr(files[ii]), _listWidget);
                item->setCheckState(Qt::Checked);
                _listWidget->insertItem(ii, item);
            }
        }

        if(_listWidget->count() == 0)
        {
            Core::String message = L"Your files don't seem to be in the right folder : ";
            message << IMPORT_FROM_SHARED_FOLDER;
            QMessageBox::critical(this, "Nyx", Core::String8(message).c_str());
            emit reject();
        }
        else
        {
            _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(true);
        }
    }
    return (files.size() > 0);
}

void ImportModelDialog::onSelectAll(int checked)
{
    Qt::CheckState checkState =(Qt::CheckState)checked;

    for(int ii =0; ii<_listWidget->count(); ii++)
    {
        _listWidget->item(ii)->setCheckState(checkState);
    }
}

void ImportModelDialog::checkBeforeValidation()
{
    for(int32 ii =0; ii<_listWidget->count(); ii++)
    {
        if(_listWidget->item(ii)->checkState() == Qt::Checked)
        {    
            Core::String imageName(_listWidget->item(ii)->text().toStdString().c_str());
            _modelNames.push_back(imageName);
        }
    }

    if(_modelNames.size() > 0)
    {
        int32 maxBonesPerVertex = 1;
        int32 maxBonesPerBlendingGroup = 1;

        Core::String userMaxBonesPerVertex(_maxBonesPerVertex->text().toStdString().c_str());
        Core::String userMaxBonesPerBlendingGroup(_maxBonesPerBlendingGroup->text().toStdString().c_str());
    
        maxBonesPerVertex = Core::toInteger(userMaxBonesPerVertex);
        maxBonesPerBlendingGroup = Core::toInteger(userMaxBonesPerBlendingGroup);

        if(maxBonesPerVertex > maxBonesPerBlendingGroup)
            QMessageBox::critical(this, "Nyx", "'Max Bones per blending group' must be greater than 'Max bones per vertex'.");
        else
            emit accept();
    }
    else
        QMessageBox::critical(this, "Nyx", "You must select files before hitting 'OK'.");
}

void ImportModelDialog::initializeFromImportModelInfo(const Ptr<ImportModelInfo> & pImportModelInfo)
{
    QListWidgetItem * item = new QListWidgetItem(Core::String8(pImportModelInfo->sourceName).c_str(), _listWidget);
    item->setCheckState(Qt::Checked);
    _listWidget->insertItem(0, item);

    _selectAll->setEnabled(false);
    _importModelLabel->setEnabled(false);
    _fileOpen->setEnabled(false);
    _listWidget->setEnabled(false);

    _generateNormals->setChecked(pImportModelInfo->modelPackingOptions.generateNormals);
    _generateNormals->setEnabled(!pImportModelInfo->modelPackingOptions.generateNormals);
    _generateTangentBasis->setChecked(pImportModelInfo->modelPackingOptions.generateTangentBasis);
    _generateTangentBasis->setEnabled(!pImportModelInfo->modelPackingOptions.generateTangentBasis);
    _optimizeForCache->setChecked(pImportModelInfo->modelPackingOptions.optimizeForCache);
    _optimizeForCache->setEnabled(!pImportModelInfo->modelPackingOptions.optimizeForCache);

    Core::String maxBonesPerVertex = Core::toString(pImportModelInfo->modelPackingOptions.maxBonesPerVertex);
    Core::String maxBonesPerBlendingGroup = Core::toString(pImportModelInfo->modelPackingOptions.maxBonesPerBlendingGroup);

    _maxBonesPerVertex->setText(Core::String8(maxBonesPerVertex).c_str());
    _maxBonesPerBlendingGroup->setText(Core::String8(maxBonesPerBlendingGroup).c_str());

    connect(_generateNormals, SIGNAL(stateChanged(int)), this, SLOT(enableOK()));
    connect(_generateTangentBasis, SIGNAL(stateChanged(int)), this, SLOT(enableOK()));
    connect(_optimizeForCache, SIGNAL(stateChanged(int)), this, SLOT(enableOK()));
    connect(_maxBonesPerVertex, SIGNAL(valueUpdated()), this, SLOT(enableOK()));
    connect(_maxBonesPerBlendingGroup, SIGNAL(valueUpdated()), this, SLOT(enableOK()));
}

void ImportModelDialog::enableOK()
{
    _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(true);
}

}//namespace EPI