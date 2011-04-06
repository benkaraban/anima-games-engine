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
#ifndef EPI_IMPORT_MODEL_DIALOG_H_
#define EPI_IMPORT_MODEL_DIALOG_H_

#include <QDialog>

#include <Core/List.h>

class QGridLayout;
class QLabel;
class QPushButton;
class QListWidget;
class QCheckBox;
class QDialogButtonBox;
class QSpacerItem;

namespace QtToolbox
{
class LineEdit;
}

namespace Workflow
{
struct ModelPackingOptions;
}

namespace EPI
{

struct ImportModelInfo;

class ImportModelDialog : public QDialog
{
    Q_OBJECT
public:
    ImportModelDialog(QWidget * parent = 0);
    ImportModelDialog(QWidget * parent, const Ptr<ImportModelInfo> & pImportModelInfo);
    ~ImportModelDialog();

    void setupUi();

    const Core::List<Core::String> & getModelNames() const;
    void getModelPackingOptions(Workflow::ModelPackingOptions & modelPackingOptions) const;

public Q_SLOTS:
    bool fileOpen();
protected Q_SLOTS:
    void onSelectAll(int);
    void checkBeforeValidation();

    void enableOK();

private:
    void initializeFromImportModelInfo(const Ptr<ImportModelInfo> & pImportModelInfo);

private:
    QGridLayout *           _layout;
    QCheckBox *             _selectAll;
    QLabel *                _importModelLabel;
    QPushButton *           _fileOpen;
    QListWidget *           _listWidget;
    QCheckBox *             _generateNormals;
    QCheckBox *             _generateTangentBasis;
    QCheckBox *             _optimizeForCache;
    QLabel *                _maximumBonesPer;
    QtToolbox::LineEdit *   _maxBonesPerVertex;
    QtToolbox::LineEdit *   _maxBonesPerBlendingGroup;
    QDialogButtonBox *      _buttonBox;
    QSpacerItem *           _horizontalSpacer;

    Core::String                _path;

    Core::List<Core::String>    _modelNames;
};

}//namespace EPI

#endif/*EPI_IMPORT_MODEL_DIALOG_H_*/
