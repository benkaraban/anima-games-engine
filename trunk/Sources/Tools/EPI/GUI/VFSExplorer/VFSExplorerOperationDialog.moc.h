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
#ifndef VFSEXPLORER_OPERATION_DIALOG_H_
#define VFSEXPLORER_OPERATION_DIALOG_H_

#include <QDialog>

#include <Core/Standard.h>
#include <Core/List.h>

class QVBoxLayout;
class QLineEdit;
class QDialogButtonBox;

namespace EPI
{

LM_ENUM_3(EVFSE_OPERATION_TYPE, EVFSE_NEW_DIR, EVFSE_RENAME_FILE, EVFSE_RENAME_DIR);

class LM_API_EPI VFSExplorerOperationDialog : public QDialog
{
    Q_OBJECT
public:
    VFSExplorerOperationDialog(const Core::List<Core::String> & others, EVFSE_OPERATION_TYPE type, const Core::String & oldName, QWidget *parent = 0);
    virtual ~VFSExplorerOperationDialog();

    void setupUi();

    inline const Core::String & getNewName() const { return _newName; }
    inline const Core::String & getOldName() const { return _oldName; }

protected Q_SLOTS:
    void textChanged();

private:
    Core::List<Core::String>    _others;
    EVFSE_OPERATION_TYPE        _type;
    Core::String                _oldName;
    QVBoxLayout *               _layout;
    QLineEdit *                 _lineEdit;
    QDialogButtonBox *          _buttonBox;
    Core::String                _newName;
};

}//namespace EPI

#endif/*VFSEXPLORER_OPERATION_DIALOG_H_*/
