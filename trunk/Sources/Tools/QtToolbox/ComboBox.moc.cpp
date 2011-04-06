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
#include "ComboBox.moc.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QAction>

namespace QtToolbox
{

ComboBox::ComboBox(QWidget *parent,
                   const QString & name)
:   QWidget(parent),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _value(NULL),
    _name(name)
{
    setupUi();
}

ComboBox::~ComboBox()
{
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_nameLabel != NULL)
        delete _nameLabel;
    if(_value != NULL)
        delete _value;

    _gridLayout     = NULL;
    _nameLabel      = NULL;
    _value          = NULL;
}

void ComboBox::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("ComboBox"));
    this->setFixedWidth(205);
    this->setFixedHeight(16);

    _gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName(QString::fromUtf8("label"));
    _nameLabel->setFixedWidth(70);
    _nameLabel->setText(_name);

    _gridLayout->addWidget(_nameLabel, 0, 0, 1, 1);

    _value = new QComboBox(this);
    _value->setObjectName(QString::fromUtf8("value"));
    _value->setFixedWidth(130);

    _gridLayout->addWidget(_value, 0, 1, 1, 1);

    setLayout(_gridLayout);

    
    connect(_value,    SIGNAL(currentIndexChanged(const QString &)),
            this,      SLOT(textChanged()));
}

void ComboBox::selectIndex(int32 index)
{
    _value->setCurrentIndex(index);
}

void ComboBox::selectItem(const QString & item)
{
    int index = _value->findText(item);
    if(index >= 0)
    {
        _value->setCurrentIndex(index);
    }
}

Core::String ComboBox::selectedText() const
{
    Core::String text(Core::String8(_value->currentText().toStdString().c_str()));

    return text;
}

void ComboBox::selectItem(const String & item)
{
    selectItem(QString(String8(item).c_str()));
}

int32 ComboBox::selectedIndex() const
{
    return _value->currentIndex();
}

void ComboBox::clear()
{
    _value->clear();
}

void ComboBox::setMaxVisibleItems(int32 maxVisibles)
{ 
    _value->setMaxVisibleItems(maxVisibles); 
}

void ComboBox::textChanged()
{
    emit valueUpdated();
}

void ComboBox::addItem(const QString & item)
{
    _value->addItem(item);
}

void ComboBox::addItem(const String & item)
{
    _value->addItem(Core::String8(item).c_str());
}

ComboBoxActions::ComboBoxActions(QWidget *parent, const Core::String & name) :
    QWidget(parent),
    _pActions(NULL)
{
    this->setObjectName(Core::String8((name)).c_str());
    setupUi();
}

ComboBoxActions::~ComboBoxActions()
{
    _pActions = NULL;
}

void ComboBoxActions::setupUi()
{
 
    this->setFixedWidth(205);
    this->setFixedHeight(16);

    _gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);



    _pCBox = new QComboBox(this);
    _pCBox->setObjectName(QString::fromUtf8("value"));
    _pCBox->setFixedWidth(130);

    _gridLayout->addWidget(_pCBox, 0, 0);

    setLayout(_gridLayout);

    
    connect(_pCBox,    SIGNAL(currentIndexChanged(int)),
            this,      SLOT(indexChanged(int)));
}

void ComboBoxActions::initialiseActions(QList<QAction*>* pActions)
{
    _pActions = pActions;

    _pCBox->clear();

    for (int32 ii=0; ii<_pActions->size(); ++ii)
    {
        _pCBox->addItem((*_pActions)[ii]->text());
    }


}

void ComboBoxActions::selectAction(const QString & item)
{
    int index = _pCBox->findText(item);
    if(index >= 0)
    {
        _pCBox->setCurrentIndex(index);
    }
}

QAction* ComboBoxActions::selectedAction() const
{
    return NULL;
}

void ComboBoxActions::indexChanged(int index)
{
    if (_pActions!=NULL)
    {
        (*_pActions)[index]->trigger();
    }
}
}//namespace QtToolbox
