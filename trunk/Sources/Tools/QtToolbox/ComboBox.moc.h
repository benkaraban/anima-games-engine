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
#ifndef COMBO_BOX_H_
#define COMBO_BOX_H_

#include <QWidget>

#include <Core/Standard.h>

class QGridLayout;
class QLabel;
class QComboBox;
class QAction;

#define ADD_ITEM_COMBOBOX_LM_ENUM_COUNT(typeEnum) typeEnum##_COUNT
#define ADD_ITEM_COMBOBOX_LM_ENUM(combo, typeEnum, sliceStart) \
    for (int32 ii=0; ii<ADD_ITEM_COMBOBOX_LM_ENUM_COUNT(typeEnum); ++ii)\
    {\
        String str = toString((typeEnum)(ii));\
        String goodStr = str.slice(sliceStart);\
        goodStr.toLower();\
        combo->addItem(goodStr);\
    }

#define ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE(combo, nspace, typeEnum, sliceStart) \
    for (int32 ii=0; ii<ADD_ITEM_COMBOBOX_LM_ENUM_COUNT(typeEnum); ++ii)\
    {\
    String str = nspace::toString((typeEnum)(ii));\
        String goodStr = str.slice(sliceStart);\
        goodStr.toLower();\
        combo->addItem(goodStr);\
    }

namespace QtToolbox
{

class LM_API_QTB ComboBox : public QWidget
{
    Q_OBJECT
public:
    ComboBox(   QWidget *parent = 0,
                const QString & name = "");
    virtual ~ComboBox();

    void setupUi();

    void addItem(const QString & item);
    void addItem(const String & item);

    void selectItem(const QString & item);
    void selectItem(const String & item);
    void selectIndex(int32 index);

    void clear();

    void setMaxVisibleItems(int32 maxVisibles);

    Core::String selectedText() const;
    int32 selectedIndex() const;

protected Q_SLOTS:
    virtual void textChanged();

Q_SIGNALS:
    void valueUpdated();
   
protected:
    QGridLayout *   _gridLayout;
    QLabel *        _nameLabel;
    QComboBox *     _value;
    QString         _name;
};


class LM_API_QTB ComboBoxActions : public QWidget
{
    Q_OBJECT

public:
    ComboBoxActions(   QWidget *parent = 0, const Core::String & name = L"");
    virtual ~ComboBoxActions();

    void setupUi();

    void initialiseActions(QList<QAction*>* pActions);

    void selectAction(const QString & item);
    QAction* selectedAction() const;

protected Q_SLOTS:
    virtual void indexChanged(int index);

protected:
    QGridLayout *   _gridLayout;
    QList<QAction*>* _pActions;
    QComboBox *     _pCBox;
};
}//namespace QtToolbox

#endif/*COMBO_BOX_H_*/
