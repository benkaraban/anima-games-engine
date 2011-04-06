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
#ifndef EPI_TASKWIDGET_H_
#define EPI_TASKWIDGET_H_

#include <QFrame>

#include <EPI/ImportInfo/ImportTextureInfo.h>
#include <EPI/ImportInfo/ImportModelInfo.h>
#include <EPI/ImportInfo/ImportTerrainInfo.h>
#include <EPI/ImportInfo/ImportTerrainLayersInfo.h>
#include <EPI/ImportInfo/ImportPicBankInfo.h>
#include <EPI/ImportInfo/LightmapsGenerationInfo.h>
#include <EPI/ImportInfo/OcclusionmapsGenerationInfo.h>

class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;

namespace EPI
{

class ITask;

LM_ENUM_7(ETaskType,    TASK_TEXTURE_COMPRESSION, 
                        TASK_MODEL_CONVERT, 
                        TASK_TERRAIN_GENERATION, 
                        TASK_TERRAIN_TEXTURE_GENERATION,
                        TASK_PIC_BANK_GENERATION,
                        TASK_LIGHTMAPS_GENERATION,
                        TASK_OCCLUSIONMAPS_GENERATION);

LM_ENUM_2(ETaskStatus, TASK_WAITING, TASK_RUNNING);

class LM_API_EPI TaskWidget : public QFrame
{
    Q_OBJECT
public:
    TaskWidget(QWidget * parent = 0);
    virtual ~TaskWidget();

    void setupUi();

    void start();

    void importTexture(const Ptr<ImportTextureInfo> & pTextureInfo);
    Ptr<ImportTextureInfo> getImportTextureInfo();

    void importModel(const Ptr<ImportModelInfo> & pModelInfo);
    Ptr<ImportModelInfo> getImportModelInfo();

    void importTerrain(const Ptr<ImportTerrainInfo> & pTerrainInfo);
    Ptr<ImportTerrainInfo> getImportTerrainInfo();

    void generateTerrainTextures(const Ptr<ImportTerrainLayersInfo> & pTerrainLayerInfo);

    void generateLightmaps(const Ptr<LightmapsGenerationInfo> & pLMInfo);
    Ptr<LightmapsGenerationInfo> getLightmapsGenerationInfo();

    void generateOcclusionmaps(const Ptr<OcclusionmapsGenerationInfo> & pOMInfo);
    Ptr<OcclusionmapsGenerationInfo> getOcclusionmapsGenerationInfo();

    void importPicBank(const Ptr<ImportPicBankInfo> & pPicBankInfo);
    Ptr<ImportPicBankInfo> getImportPicBankInfo();

    inline ETaskType getTaskType() const { return _taskType; }
    inline ETaskStatus getTaskStatus() const { return _taskStatus; }

protected Q_SLOTS:
    void taskTerminated();
    void cancelTask();

Q_SIGNALS:
    void taskTerminated(TaskWidget * taskWidget);
    void taskCancelled(TaskWidget * taskWidget);

private:
    ITask *         _pTask;

    QGridLayout *   _layout;
    QLabel *        _name;
    QLabel *        _status;
    QProgressBar *  _progressBar;
    QPushButton *   _cancel;

    ETaskType       _taskType;
    ETaskStatus     _taskStatus;
};


}//namespace EPI

#endif /*EPI_TASKWIDGET_H_*/