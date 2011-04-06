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
#include "GenerateOcclusionmapsDialog.moc.h"

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


namespace EPI
{
GenerateOcclusionmapsDialog::GenerateOcclusionmapsDialog(QWidget * parent)
:   QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    _layout(NULL),
    _occlusionmapsGenerationOptions(NULL)
{
    setupUi();
}

GenerateOcclusionmapsDialog::~GenerateOcclusionmapsDialog()
{
    LM_DELETE(_layout);
    LM_DELETE(_occlusionmapsGenerationOptions);
}

void GenerateOcclusionmapsDialog::setupUi()
{
    _layout = new QVBoxLayout(this);

    _occlusionmapsGenerationOptions = new OcclusionmapsGenerationOptions(this, true);
    _layout->addWidget(_occlusionmapsGenerationOptions);

    resize(_occlusionmapsGenerationOptions->width(), _occlusionmapsGenerationOptions->height());

    connect(_occlusionmapsGenerationOptions, SIGNAL(sizeChanged(int , int)), this, SLOT(resize(int, int)));
    connect(_occlusionmapsGenerationOptions, SIGNAL(accept()), this, SLOT(accept()));
    connect(_occlusionmapsGenerationOptions, SIGNAL(reject()), this, SLOT(reject()));
}

void GenerateOcclusionmapsDialog::resize(int width, int height)
{
    this->setFixedWidth(width + _layout->contentsMargins().left() + _layout->contentsMargins().right());
    this->setFixedHeight(height + _layout->contentsMargins().top() + _layout->contentsMargins().bottom());
}

}//namespace EPI
