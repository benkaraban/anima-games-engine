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
#ifndef EPI_TABLE_CHECK_POINT_MODEL_H_
#define EPI_TABLE_CHECK_POINT_MODEL_H_

#include <QAbstractTableModel>


class QAction;

namespace EPI
{
class PtyTrajectory;
//-----------------------------------------------------------------------------
class TableCheckPointModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableCheckPointModel(PtyTrajectory& pty, QObject *parent = 0);
    ~TableCheckPointModel() {}
/*
    void setTrajectory(const Core::Trajectory& traj) {_traj = traj; 
        reset ();}*/
    const Core::Trajectory& getTrajectory() const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;


    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    void insertCheckPoint(int32 rowIndex);
    void deleteCheckPoint(int32 indexStart, int32 indexEnd);

    void emitAllDataChanged();
    void insertLocalisationCamera(int32 index);
    
    PtyTrajectory&  getPtyTrajectory() {return _pty;}
private:
    PtyTrajectory& _pty;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif /*EPI_TABLE_CHECK_POINT_MODEL_H_*/
