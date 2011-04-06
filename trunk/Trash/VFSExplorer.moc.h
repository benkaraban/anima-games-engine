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
#ifndef VFS_EXPLORER_H_
#define VFS_EXPLORER_H_

#include <QDialog>
#include <Core/VFS/VirtualFileSystem.h>

class QVBoxLayout;
class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QDialogButtonBox;

namespace QtToolbox
{

LM_ENUM_2(EVFSExplorerMode, VFSE_LOAD, VFSE_SAVE);

class LM_API_QTB VFSExplorer : public QDialog
{
    Q_OBJECT
public:
    VFSExplorer(Core::VirtualFileSystem & vfs,
                EVFSExplorerMode loadFile,
                QWidget * parent = 0);
    VFSExplorer(Core::VirtualFileSystem & vfs,
                EVFSExplorerMode loadFile,
                const Core::String & extension,
                QWidget * parent = 0);
    virtual ~VFSExplorer();

    virtual void setupUi();

    virtual inline const Core::String & getSelectedFile(){ return _selectedString; }

protected slots:
    void exploreVFS(QTreeWidgetItem * item);
    void chooseFile(QTreeWidgetItem * item, int column);
    void chooseFileAndAccept(QTreeWidgetItem * item, int column);
    void textChanged(const QString & text);

private:
    Core::String locate(QTreeWidgetItem * item);

private:
    QVBoxLayout *       _layout;
    QTreeWidget *       _treeView;
    QLineEdit *         _lineEdit;
    QDialogButtonBox *  _buttonBox;

    Core::VirtualFileSystem &   _vfs;
    EVFSExplorerMode            _loadFile;
    Core::String                _extension;
    Core::String                _selectedString;
};

}//namespace QtToolbox

#endif/*VFS_EXPLORER_H_*/
