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
#ifndef EPI_TASK_MANAGER_H_
#define EPI_TASK_MANAGER_H_

#include <QDialog>

#include <Core/List.h>
#include <Core/RT/Mutex.h>

class QScrollArea;
class QVBoxLayout;
class QSpacerItem;

namespace EPI
{
struct ImportInfo;
struct ImportTextureInfo;
struct ImportTerrainInfo;
struct ImportTerrainLayersInfo;
struct ImportModelInfo;
struct ImportPicBankInfo;
struct ImportWorldInfo;

class TaskWidget;

class LM_API_EPI TaskManager : public QDialog
{
    Q_OBJECT
public:
    TaskManager(QWidget * parent = 0, int32 maxParallelTasks = 2);
    virtual ~TaskManager();

    void setupUi();

Q_SIGNALS:
    void sourceNotOpened(const Ptr<ImportInfo> & pImportInfo);
    void generationDone(const Ptr<ImportInfo> & pImportInfo);

public Q_SLOTS:
    void taskTerminated(TaskWidget * taskWidget);
    void taskCancelled(TaskWidget * taskWidget);
    void importTexture(const Ptr<ImportTextureInfo> & pTextureInfo);
    void importTerrain(const Ptr<ImportTerrainInfo> & pTerrainInfo);
    void generateTerrainTextures(const Ptr<ImportTerrainLayersInfo>& pTerrainLayerInfo);
    void generateLightmaps(const Ptr<ImportWorldInfo> &pImportInfo);
    void generateOcclusionmaps(const Ptr<ImportWorldInfo> &pImportInfo);
    void importModel(const Ptr<ImportModelInfo> & pModelInfo);
    void importPicBank(const Ptr<ImportPicBankInfo> & pPicBankInfo);

    void generate(const Ptr<ImportInfo> & pInfo);

private:
    void removeTask(TaskWidget * taskWidget);
    void connectAndEngueueTask(TaskWidget * taskWidget);

private:
    QVBoxLayout * _layout;
    QScrollArea * _scrollArea;
    QVBoxLayout * _scrollLayout;
    QSpacerItem * _verticalSpacer;

    Core::List<TaskWidget *> _tasks;
    int32 _maxParallelTasks;

    Core::Mutex _mutex;
};


}//namespace EPI

#endif /*EPI_TASK_MANAGER_H_*/