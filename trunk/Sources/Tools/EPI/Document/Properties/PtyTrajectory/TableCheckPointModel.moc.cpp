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
#include "TableCheckPointModel.moc.h"

#include <EPI/Document/Properties/PtyTrajectory/PtyCheckPoint.moc.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyTrajectory.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_ENUM_3   (ECPInfoType,
             CP_INFO_TIME,
             CP_INFO_POSITION,
             CP_INFO_ORIENTATION)
//-----------------------------------------------------------------------------
void columnIndexToCPInfo(int32 column, ECPInfoType& type, int32& index)
{
    switch(column)
    {
    case 0: type=CP_INFO_TIME; index=0; break;
    case 1: type=CP_INFO_POSITION; index=0; break;
    case 2: type=CP_INFO_POSITION; index=1; break;
    case 3: type=CP_INFO_POSITION; index=2; break;
    case 4: type=CP_INFO_ORIENTATION; index=0; break;
    case 5: type=CP_INFO_ORIENTATION; index=1; break;
    case 6: type=CP_INFO_ORIENTATION; index=2; break;
    }
}
//-----------------------------------------------------------------------------
int32 CPInfoToColumnIndex(ECPInfoType type, int32 index)
{
    if (type==CP_INFO_TIME) return 0;

    if (type==CP_INFO_POSITION) return 1+index;

    return 4+index;
}
//-----------------------------------------------------------------------------
TableCheckPointModel::TableCheckPointModel(PtyTrajectory& pty, QObject *parent) : 
    QAbstractTableModel(parent),
    _pty(pty)
{
/*
    setHeaderData(0, Qt::Horizontal, QObject::tr("time"), Qt::DisplayRole);
    setHeaderData(1, Qt::Horizontal, QObject::tr("Pos X"), Qt::DisplayRole);
    setHeaderData(2, Qt::Horizontal, QObject::tr("Pos Y"), Qt::DisplayRole);
    setHeaderData(3, Qt::Horizontal, QObject::tr("Pos Z"), Qt::DisplayRole);
    setHeaderData(4, Qt::Horizontal, QObject::tr("Rot X"), Qt::DisplayRole);
    setHeaderData(5, Qt::Horizontal, QObject::tr("Rot Y"), Qt::DisplayRole);
    setHeaderData(6, Qt::Horizontal, QObject::tr("Rot Z"), Qt::DisplayRole);*/
}
//-----------------------------------------------------------------------------
int TableCheckPointModel::rowCount(const QModelIndex &parent) const
{
    return _pty.getPtiesContentSelectable().size();
}
//-----------------------------------------------------------------------------
int TableCheckPointModel::columnCount(const QModelIndex &) const
{
    return 7;
}
//-----------------------------------------------------------------------------
QVariant TableCheckPointModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole))
         return QVariant();

    QVariant ret;

    Ptr<PtyCheckPoint> pPtyTra = LM_DEBUG_PTR_CAST<PtyCheckPoint>(_pty.getPtiesContentSelectable()[index.row()]);
    Ptr<PtyTransform> pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyTra->getPtyTransform());

    const Core::Vector3f    position    = pPtyT->getLocalPosition();
   // const Core::Quaternionf orient = pPtyT->getLocalOrientation();
    const Core::Orientationf or = pPtyT->getLocalOrientationEuler();
    const float time = pPtyTra->getTime();

    switch(index.column())
    {
        case 0: ret = time; break;
        case 1: ret = position.x; break;
        case 2: ret = position.y; break;
        case 3: ret = position.z; break;
        case 4: ret = or._pitch; break;
        case 5: ret = or._yaw; break;
        case 6: ret = or._roll; break;
    }

    float test = ret.toFloat();
    return ret;
}
//-----------------------------------------------------------------------------
QVariant TableCheckPointModel::headerData(int section, Qt::Orientation orientation,
                        int role) const
{
    if (orientation == Qt::Horizontal)
    {
        //if (section == 0) return QVariant("Time"); 
        if (section == 1) return QVariant("Pos X"); 
        if (section == 2) return QVariant("Pos Y"); 
        if (section == 3) return QVariant("Pos Z"); 
        if (section == 4) return QVariant("Rot X");
        if (section == 5) return QVariant("Rot Y"); 
        if (section == 6) return QVariant("Rot Z"); 
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}
//-----------------------------------------------------------------------------
Qt::ItemFlags TableCheckPointModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;

    return flags;
}
//-----------------------------------------------------------------------------
bool TableCheckPointModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {

        ECPInfoType type;
        int32 coord;
        columnIndexToCPInfo(index.column(), type, coord);
    
        Ptr<PtyCheckPoint> pPtyCP = LM_DEBUG_PTR_CAST<PtyCheckPoint>(_pty.getPtiesContentSelectable()[index.row()]);
        Ptr<PtyTransform> pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyCP->getPtyTransform());

        Core::Vector3f    position    = pPtyT->getLocalPosition();


        Ptr<IState> pUndoState;
        if (_pty.getStateRecorder()->isRecording() == false)
            pUndoState = Ptr<IState> ((IState*) new StatePropertyChanged (*pPtyCP));

        switch(type)
        {
            case CP_INFO_TIME:  pPtyCP->setTime(value.toFloat()); break;
            case CP_INFO_POSITION:  position[coord] = value.toFloat(); pPtyT->setLocalPosition(position); break;
//            case CP_INFO_ORIENTATION: cp[idPt].orient[coord] = value.toFloat(); break;
        }

        if (_pty.getStateRecorder()->isRecording() == false)
        {
            Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*pPtyCP));
            _pty.getStateRecorder()->recording(pUndoState, pRedoState);
        }

        emit dataChanged(index, index);

        pPtyCP->updateData();
        _pty.updateData();
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void TableCheckPointModel::insertCheckPoint(int32 rowIndex)
{
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    _pty.createCheckPoint(rowIndex);
    _pty.updateData();
    endInsertRows();
}
//-----------------------------------------------------------------------------
void TableCheckPointModel::emitAllDataChanged()
{
    //QModelIndex bidon ;
    //emit dataChanged(index(0,0), index(rowCount(bidon)-1, columnCount(bidon)-1));
    //beginInsertRows(QModelIndex(), 0, rowCount());
    //endInsertRows();
    reset();
}
//-----------------------------------------------------------------------------
void TableCheckPointModel::deleteCheckPoint(int32 indexStart, int32 indexEnd)
{
    beginRemoveRows(QModelIndex(), indexStart, indexEnd); 
    _pty.deleteCheckPoint(indexStart, indexEnd);
    endRemoveRows();

    _pty.updateData();
}
//-----------------------------------------------------------------------------
void TableCheckPointModel::insertLocalisationCamera(int32 index)
{
    beginInsertRows(QModelIndex(), index, index);
    _pty.insertLocalisationCamera(index);
    endInsertRows();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace EPI