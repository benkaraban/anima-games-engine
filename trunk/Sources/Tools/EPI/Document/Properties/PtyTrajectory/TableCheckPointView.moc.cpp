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
#include "TableCheckPointView.moc.h"

#include <QAction>
#include <QHeaderView>

#include <EPI/Document/Properties/PtyTrajectory/TableCheckPointModel.moc.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyTrajectory.moc.h>
namespace EPI
{

//-----------------------------------------------------------------------------
SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
     : QItemDelegate(parent)
{
}
//-----------------------------------------------------------------------------
 QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem & option ,
     const QModelIndex & index ) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);


    if (index.column()==0)
    {
        editor->setRange(0, 3600);
        editor->setSingleStep(0.1);
        editor->setSuffix("s");
    }
    else if (index.column()>0 && index.column()<=3)
    {
        editor->setRange(-1000, 1000);
        editor->setSingleStep(0.1);
        editor->setSuffix("m");
    }
    else if (index.column()>3 && index.column()<=6)
    {
        editor->setRange(0, 360);
        editor->setSingleStep(0.1);
        editor->setSuffix("�");
    }

    connect(editor, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));

    return editor;
}
//-----------------------------------------------------------------------------
void SpinBoxDelegate::valueChanged(double v)
{
    int a = 9;
}
//-----------------------------------------------------------------------------
 void SpinBoxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}
//-----------------------------------------------------------------------------
 void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
//-----------------------------------------------------------------------------
 void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TableCheckPointView::TableCheckPointView(QWidget* parent) :
    QTableView(parent)
{
    _pActionShowPosition = new QAction("Show Position", horizontalHeader());
    _pActionShowRotation = new QAction("Show Rotation", horizontalHeader());

    _pActionShowPosition->setCheckable(true);
    _pActionShowRotation->setCheckable(true);

    connect(_pActionShowPosition, SIGNAL(triggered(bool)), this, SLOT(showPosition(bool)));
    connect(_pActionShowRotation, SIGNAL(triggered(bool)), this, SLOT(showRotation(bool)));

    horizontalHeader()->addAction(_pActionShowPosition);
    horizontalHeader()->addAction(_pActionShowRotation);

    horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    

    QAction* insertOneRow   = new QAction("Insert Check Point", this);
    QAction* deleteSelected = new QAction("Delete Selected Check Point", this);
    
    addAction(insertOneRow);
    addAction(deleteSelected);
    
    connect(insertOneRow, SIGNAL(triggered()), this, SLOT(insertRow()));
    connect(deleteSelected, SIGNAL(triggered()), this, SLOT(deleteSelectedCP()));
    
    deleteSelected->setShortcuts(QKeySequence::Delete);
    deleteSelected->setShortcutContext(Qt::ApplicationShortcut);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    SpinBoxDelegate* delegate = new  SpinBoxDelegate(this);
    setItemDelegate(delegate);
}
//-----------------------------------------------------------------------------
void TableCheckPointView::setVisiblePosition(bool flag)
{
    if (flag==true)
    {
        horizontalHeader()->showSection(1);
        horizontalHeader()->showSection(2);
        horizontalHeader()->showSection(3);
        _pActionShowPosition->setChecked(true);
    }
    else
    {
        horizontalHeader()->hideSection(1);
        horizontalHeader()->hideSection(2);
        horizontalHeader()->hideSection(3);
        _pActionShowPosition->setChecked(false);
    }
}
//-----------------------------------------------------------------------------
void TableCheckPointView::setVisibleRotation(bool flag)
{
    if (flag==true)
    {
        horizontalHeader()->showSection(4);
        horizontalHeader()->showSection(5);
        horizontalHeader()->showSection(6);
        _pActionShowRotation->setChecked(true);
    }
    else
    {
        horizontalHeader()->hideSection(4);
        horizontalHeader()->hideSection(5);
        horizontalHeader()->hideSection(6);
        _pActionShowRotation->setChecked(false);
    }
}
//-----------------------------------------------------------------------------
void TableCheckPointView::showPosition(bool checked)
{
    setVisiblePosition(checked);
}
//-----------------------------------------------------------------------------
void TableCheckPointView::showRotation(bool checked)
{
    setVisibleRotation(checked);
}
//-----------------------------------------------------------------------------
void TableCheckPointView::insertRow()
{
    QModelIndexList  selected = selectedIndexes();

    int32 rowIndex = 0;

    if (selected.size()>0)
    {
        rowIndex = selected.back().row()+1;
    }

    ((TableCheckPointModel*)model())->insertCheckPoint(rowIndex);
}
//-----------------------------------------------------------------------------
void TableCheckPointView::deleteSelectedCP()
{
    QItemSelectionModel  * selectModel = selectionModel ();

     QItemSelection selected = selectModel->selection();

     PtyTrajectory& ptyTraj = ((TableCheckPointModel*)model())->getPtyTrajectory();

    ptyTraj.getStateRecorder()->startRecording();
     Core::List<int32> tops;
     Core::List<int32> bottoms;
    for (int32 ii=0; ii<selected.size(); ++ii)
    {
        tops.push_back(selected[ii].top());
        bottoms.push_back(selected[ii].bottom());
    }

    LM_ASSERT(tops.size() == bottoms.size());

    std::sort( tops.begin( ), tops.end( ) );
    std::sort( bottoms.begin( ), bottoms.end( ) );

    for (int32 ii=0; ii<tops.size(); ++ii)
    {
        ((TableCheckPointModel*)model())->deleteCheckPoint(tops[ii]-ii, bottoms[ii]-ii);
    }

    ptyTraj.getStateRecorder()->stopRecording();

    ((TableCheckPointModel*)model())->emitAllDataChanged();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
