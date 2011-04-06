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
#include "VFSExplorer.moc.h"

#include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>

#include <Core/List.h>
#include <Core/String.h>
#include <Core/System/Files.h>

namespace QtToolbox
{

VFSExplorer::VFSExplorer(   Core::VirtualFileSystem & vfs,
                            EVFSExplorerMode loadFile,
                            QWidget * parent)
:   QDialog(parent),
    _layout(NULL),
    _treeView(NULL),
    _lineEdit(NULL),
    _buttonBox(NULL),
    _vfs(vfs),
    _loadFile(loadFile)
{
    setupUi();
}

VFSExplorer::VFSExplorer(   Core::VirtualFileSystem & vfs,
                            EVFSExplorerMode loadFile,
                            const Core::String & extension,
                            QWidget * parent)
:   QDialog(parent),
    _layout(NULL),
    _treeView(NULL),
    _lineEdit(NULL),
    _buttonBox(NULL),
    _vfs(vfs),
    _loadFile(loadFile),
    _extension(extension)
{
    setupUi();
}

VFSExplorer::~VFSExplorer()
{
    if(_layout != NULL)
        delete _layout;
    if(_treeView != NULL)
        delete _treeView;
    if(_lineEdit != NULL)
        delete _lineEdit;
    if(_buttonBox != NULL)
        delete _buttonBox;

    _layout = NULL;
    _treeView = NULL;
    _lineEdit = NULL;
    _buttonBox = NULL;
}

void VFSExplorer::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("VFSExplorer"));
    this->setWindowTitle("VFS Explorer");
    this->setMinimumWidth(512);
    this->setMinimumHeight(512);

    _layout = new QVBoxLayout();
    _layout->setObjectName(QString::fromUtf8("VBoxLayout"));
    _layout->setContentsMargins(0, 0, 0, 0);

    _treeView = new QTreeWidget(this);
    _treeView->header()->setVisible(false);

    QTreeWidgetItem * root = new QTreeWidgetItem();
    root->setText(0, "/");
    QTreeWidgetItem * dummy = new QTreeWidgetItem();
    dummy->setText(0, "dummy");
    dummy->setDisabled(true);
    root->addChild(dummy);

    _treeView->addTopLevelItem( root );
    _layout->addWidget(_treeView);

    _lineEdit = new QLineEdit(this);
    _lineEdit->setReadOnly(_loadFile == VFSE_LOAD);
    _layout->addWidget(_lineEdit);

    _buttonBox = new QDialogButtonBox ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                        Qt::Horizontal,
                                        this );
    _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(false);

    _layout->addWidget(_buttonBox);

    setLayout(_layout);

    connect(_treeView,  SIGNAL(itemExpanded(QTreeWidgetItem *)),
            this,       SLOT(exploreVFS(QTreeWidgetItem *)));
    connect(_treeView,  SIGNAL(itemClicked(QTreeWidgetItem *, int)),
            this,       SLOT(chooseFile(QTreeWidgetItem *, int)));
    connect(_treeView,  SIGNAL(itemActivated(QTreeWidgetItem *, int)),
            this,       SLOT(chooseFile(QTreeWidgetItem *, int)));
    connect(_treeView,  SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this,       SLOT(chooseFileAndAccept(QTreeWidgetItem *, int)));

    connect(_lineEdit, SIGNAL(textChanged(const QString &)),
            this,      SLOT(textChanged(const QString &)));

    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void VFSExplorer::exploreVFS(QTreeWidgetItem * item)
{
    Core::String location = locate(item);

    //Si le dummy est présent c'est la première fois que ce noeud est ouvert
    if(item->child(0)->isDisabled())
    {
        item->removeChild(item->child(0));

        Core::List<Core::String> directories;
        Core::List<Core::String> files;
        _vfs.listDirs(location, directories);
        _vfs.listFiles(location, files);

        int32 ii = 0;
        //On ajoute les répertoires
        while(ii < directories.size())
        {
            Core::String8 directory(directories[ii]);
            QTreeWidgetItem * newItem = new QTreeWidgetItem();
            newItem->setText(0, directory.c_str());
            QTreeWidgetItem * dummy = new QTreeWidgetItem();
            dummy->setText(0, "dummy");
            dummy->setDisabled(true);
            newItem->addChild(dummy);

            item->addChild(newItem);

            ++ii;
        }

        ii = 0;
        //On ajoute les fichier
        while(ii < files.size())
        {
            if(_extension.length() == 0
            || Core::System::getFileExt(files[ii]) == _extension)
            {
                Core::String8 file(files[ii]);
                QTreeWidgetItem * newItem = new QTreeWidgetItem();
                newItem->setText(0, file.c_str());

                item->addChild(newItem);
            }
            ++ii;
        }
    }
}

void VFSExplorer::chooseFile(QTreeWidgetItem * item, int column)
{
    Core::String location = locate(item);

    if(_loadFile == VFSE_LOAD)
    {
        if(item->childCount() == 0)//Peut-être un fichier
        {
            if(_vfs.fileExists(location))
            {
                _lineEdit->setText(Core::String8(location).c_str());
                _selectedString = location;
            }
        }
    }
    else
    {
        _lineEdit->setText(Core::String8(location).c_str());
        _selectedString = location;
    }
}

void VFSExplorer::chooseFileAndAccept(QTreeWidgetItem * item, int column)
{
    Core::String location = locate(item);

    if(item->childCount() == 0)//Peut-être un fichier
    {
        if(_vfs.fileExists(location))
        {
            _lineEdit->setText(Core::String8(location).c_str());
            _selectedString = location;

            if(_loadFile == VFSE_SAVE)
            {
                if( QMessageBox::question(  this,
                                            "VFSExplorer",
                                            "Are you sure you want to overwrite this file?",
                                            QMessageBox::Ok|QMessageBox::Cancel,
                                            QMessageBox::Cancel ) == QMessageBox::Cancel)
                {
                    return;
                }
            }
            accept();
        }
    }
}

Core::String VFSExplorer::locate(QTreeWidgetItem * item)
{
    Core::String location;
    location << Core::String8(item->text(0).toStdString().c_str());
    while(item->parent() != NULL)
    {
        item = item->parent();
        Core::String temp;
        temp << Core::String8(item->text(0).toStdString().c_str());
        if(temp != L"/")
            temp << L"/";
        temp << location;
        location = temp;
    }

    return location;
}

void VFSExplorer::textChanged(const QString & text)
{
    String lineEditText(_lineEdit->text().toStdString().c_str());

    bool enableOkButton = false;
    enableOkButton = (_lineEdit->text().length() > 0) && !_vfs.dirExists(lineEditText);

    _buttonBox->button( QDialogButtonBox::Ok )->setEnabled( enableOkButton );
    if(_loadFile == VFSE_SAVE)
    {
        String8 selectedString8(text.toStdString().c_str());
        _selectedString = String(selectedString8);
    }
}

}//namespace QtToolbox
