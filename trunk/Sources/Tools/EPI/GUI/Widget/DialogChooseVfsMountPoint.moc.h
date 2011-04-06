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
#ifndef EPI_DIALOG_CHOOSE_VFS_MOUNT_POINT_H
#define EPI_DIALOG_CHOOSE_VFS_MOUNT_POINT_H

#include <QDialog>
//#include <QDir>

class QPushButton;
class QComboBox;
class QGridLayout;
class QLabel;
class QCheckBox;


#include <Core/Standard.h>
#include <Core/List.h>
namespace EPI
{
//-----------------------------------------------------------------------------
class LM_API_EPI DialogChooseVfsMountPoint : public QDialog
{
    Q_OBJECT

public:
    DialogChooseVfsMountPoint(const Core::List<Core::String>& listPath, int32 listPathIndex, QWidget *parent = 0);
    void getDialogOption(Core::List<Core::String>& strs, int32& currentIndex, bool& show) {currentIndex= _currentIndex; strs = _strs; show = _IsShowAgain;}

private:
    void setupUi();
    void dirComboBoxToDirList(Core::List<Core::String>& strs, int32& currentIndex);
    
private Q_SLOTS:
    void launchBrowseDir();
    void ok();

private:
    QPushButton *               _browseButton;
    QPushButton *               _okButton;
    QPushButton *               _cancelButton;
    QComboBox   *               _dirComboBox;
    QGridLayout *               _layout;
    QLabel*                     _labelAddPath;
    QLabel*                     _labelselectPath;
    QCheckBox*                  _showAgain;
    Core::List<Core::String>    _strs;
    int32                       _currentIndex;
    bool                        _IsShowAgain;
};
//-----------------------------------------------------------------------------
}//namespace EPI

#endif/*EPI_DIALOG_CHOOSE_VFS_MOUNT_POINT_H*/
