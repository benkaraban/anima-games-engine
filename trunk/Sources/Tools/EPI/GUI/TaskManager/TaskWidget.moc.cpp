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
#include "TaskWidget.moc.h"

#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QMessageBox>

#include <EPI/Constants.h>
#include <EPI/GUI/TaskManager/ImportTextureTask.h>
#include <EPI/GUI/TaskManager/ImportTerrainTask.h>
#include <EPI/GUI/TaskManager/ImportTerrainLayersTask.h>
#include <EPI/GUI/TaskManager/ImportModelTask.h>
#include <EPI/GUI/TaskManager/ImportPicBankTask.h>
#include <EPI/GUI/TaskManager/GenerateLightmapsTask.h>
#include <EPI/GUI/TaskManager/GenerateOcclusionmapsTask.h>

namespace EPI
{

TaskWidget::TaskWidget(QWidget * parent)
:   QFrame(parent),
    _pTask(NULL)
{
    setupUi();
}

TaskWidget::~TaskWidget()
{
    if(_layout != NULL)
        delete _layout;
    if(_name != NULL)
        delete _name;
    if(_status != NULL)
        delete _status;
    if(_progressBar != NULL)
        delete _progressBar;
    if(_cancel != NULL)
        delete _cancel;
    _layout = NULL;
    _name = NULL;
    _status = NULL;
    _progressBar = NULL;
    _cancel = NULL;


    if(_pTask != NULL)
        delete _pTask;
    _pTask = NULL;
}

void TaskWidget::setupUi()
{
    setFrameStyle(QFrame::StyledPanel);

    _layout = new QGridLayout();
    _layout->setContentsMargins(1, 1, 1, 1);
    _layout->setSpacing(1);

    _name = new QLabel("Name", this);
    _layout->addWidget(_name, 0, 0, 1, 1);

    _status = new QLabel("Waiting", this);
    _layout->addWidget(_status, 0, 1, 1, 1);

    _taskStatus = TASK_WAITING;

    _progressBar = new QProgressBar(this);
    _progressBar->setMinimum(0);
    _progressBar->setMaximum(100);
    _progressBar->setValue(0);
    _layout->addWidget(_progressBar, 1, 0, 1, 1);

    _cancel = new QPushButton("Cancel", this);
    _cancel->setEnabled(true);
    _layout->addWidget(_cancel, 1, 1, 1, 1);

    setLayout(_layout);

    setFixedHeight(40);

    connect(_cancel, SIGNAL(clicked()), this, SLOT(cancelTask()));
}

void TaskWidget::start()
{
    _cancel->setEnabled(false);
    _status->setText("Running");
    _taskStatus = TASK_RUNNING;
    _pTask->start();
}

void TaskWidget::taskTerminated()
{
    bool isOK = true;

    if(_pTask != NULL)
    {
        if(!_pTask->getErrors().isEmpty())
        {
            QMessageBox::critical(this, _name->text(), _pTask->getErrors());
            isOK = false;
        }
        else if(!_pTask->getWarnings().isEmpty())
        {
            QMessageBox::warning(this, _name->text(), _pTask->getWarnings());
        }
    }

    if(isOK)
        emit taskTerminated(this);
    else
        emit taskCancelled(this);
}

void TaskWidget::cancelTask()
{
    emit taskCancelled(this);
}

void TaskWidget::importTexture(const Ptr<ImportTextureInfo> & pTextureInfo)
{
    if(_pTask == NULL)
    {
        _taskType = TASK_TEXTURE_COMPRESSION;

        Core::String textureLabel; 
        textureLabel << pTextureInfo->textureName << L" (";
        textureLabel << textureFormatToString(pTextureInfo->pTextureOptions->format) << L", ";
        textureLabel << textureTypeToString(pTextureInfo->pTextureOptions->texType) << L", ";

        if(pTextureInfo->pTextureOptions->generateMipmap)
            textureLabel << L"MIP)...";
        else
            textureLabel << L"NOMIP)...";

        _name->setText(String8(textureLabel).c_str());

        _status->setText("Waiting");
        _taskStatus = TASK_WAITING;

        _pTask = new ImportTextureTask(pTextureInfo);

        connect(_pTask, SIGNAL(taskTerminated()), this, SLOT(taskTerminated()));
        connect(_pTask, SIGNAL(progressState(int)), _progressBar, SLOT(setValue(int)));
    }
}

Ptr<ImportTextureInfo> TaskWidget::getImportTextureInfo()
{
    Ptr<ImportTextureInfo> pImportTextureInfo = Core::null;

    if(_pTask != NULL && _taskType == TASK_TEXTURE_COMPRESSION)
    {
        ImportTextureTask * pImportTextureTask = LM_DEBUG_CAST<ImportTextureTask *>(_pTask);
        if(pImportTextureTask != NULL)
        {
            pImportTextureInfo = pImportTextureTask->getImportTextureInfo();
        }
    }

    return pImportTextureInfo;
}

Ptr<ImportPicBankInfo> TaskWidget::getImportPicBankInfo()
{
    Ptr<ImportPicBankInfo> pImportPicBankInfo;

    if(_pTask != NULL && _taskType == TASK_PIC_BANK_GENERATION)
    {
        ImportPicBankTask  * pImportPicBankTask = LM_DEBUG_CAST<ImportPicBankTask *>(_pTask);
        if(pImportPicBankTask != NULL)
        {
            pImportPicBankInfo = pImportPicBankTask->getImportPicBankInfo();
        }
    }

    return pImportPicBankInfo;
}

Ptr<LightmapsGenerationInfo> TaskWidget::getLightmapsGenerationInfo()
{
    Ptr<LightmapsGenerationInfo> pLMInfo;

    if(_pTask != NULL && _taskType == TASK_LIGHTMAPS_GENERATION)
    {
        GenerateLightmapsTask * pLMTask = LM_DEBUG_CAST<GenerateLightmapsTask*>(_pTask);
        if(pLMTask != NULL)
            pLMInfo = pLMTask->getInfo();
    }

    return pLMInfo;
}

Ptr<OcclusionmapsGenerationInfo> TaskWidget::getOcclusionmapsGenerationInfo()
{
    Ptr<OcclusionmapsGenerationInfo> pOMInfo;

    if(_pTask != NULL && _taskType == TASK_OCCLUSIONMAPS_GENERATION)
    {
        GenerateOcclusionmapsTask * pOMTask = LM_DEBUG_CAST<GenerateOcclusionmapsTask*>(_pTask);
        if(pOMTask != NULL)
            pOMInfo = pOMTask->getInfo();
    }

    return pOMInfo;
}

void TaskWidget::importModel(const Ptr<ImportModelInfo> & pModelInfo)
{
    if(_pTask == NULL)
    {
        _taskType = TASK_MODEL_CONVERT;

        Core::String modelLabel; 
        modelLabel << pModelInfo->sourceName;

        _name->setText(String8(modelLabel).c_str());

        _status->setText("Waiting");
        _taskStatus = TASK_WAITING;

        _pTask = new ImportModelTask(pModelInfo);

        connect(_pTask, SIGNAL(taskTerminated()), this, SLOT(taskTerminated()));
        connect(_pTask, SIGNAL(progressState(int)), _progressBar, SLOT(setValue(int)));
        connect(_pTask, SIGNAL(error(const QString & , const QString & )), this, SLOT(error(const QString & , const QString & )));
        connect(_pTask, SIGNAL(warning(const QString & , const QString & )), this, SLOT(warning(const QString & , const QString & )));
    }
}

Ptr<ImportModelInfo> TaskWidget::getImportModelInfo()
{
    Ptr<ImportModelInfo> pImportModelInfo = Core::null;

    if(_pTask != NULL && _taskType == TASK_MODEL_CONVERT)
    {
        ImportModelTask * pImportModelTask = LM_DEBUG_CAST<ImportModelTask *>(_pTask);
        if(pImportModelTask != NULL)
        {
            pImportModelInfo = pImportModelTask->getImportModelInfo();
        }
    }

    return pImportModelInfo;
}


void TaskWidget::importTerrain(const Ptr<ImportTerrainInfo> & pTerrainInfo)
{
    if(_pTask == NULL)
    {
        _taskType = TASK_TERRAIN_GENERATION;

        Core::String terrainLabel; 
        terrainLabel << pTerrainInfo->terrainModelName;

        _name->setText(String8(terrainLabel).c_str());

        _status->setText("Waiting");
        _taskStatus = TASK_WAITING;

        _pTask = new ImportTerrainTask(pTerrainInfo);

        connect(_pTask, SIGNAL(taskTerminated()), this, SLOT(taskTerminated()));
        connect(_pTask, SIGNAL(progressState(int)), _progressBar, SLOT(setValue(int)));
    }
}

Ptr<ImportTerrainInfo> TaskWidget::getImportTerrainInfo()
{
    Ptr<ImportTerrainInfo> pImportTerrainInfo = Core::null;

    if(_pTask != NULL && _taskType == TASK_TERRAIN_GENERATION)
    {
        ImportTerrainTask * pImportTerrainTask = LM_DEBUG_CAST<ImportTerrainTask *>(_pTask);
        if(pImportTerrainTask != NULL)
        {
            pImportTerrainInfo = pImportTerrainTask->getImportTerrainInfo();
        }
    }

    return pImportTerrainInfo;
}

void TaskWidget::importPicBank(const Ptr<ImportPicBankInfo> & pPicBankInfo)
{
    if(_pTask == NULL)
    {
        _taskType = TASK_PIC_BANK_GENERATION;

        _name->setText(String8(pPicBankInfo->dirName).c_str());

        _status->setText("Waiting");
        _taskStatus = TASK_WAITING;

        _pTask = new ImportPicBankTask(pPicBankInfo);

        connect(_pTask, SIGNAL(taskTerminated()), this, SLOT(taskTerminated()));
        connect(_pTask, SIGNAL(progressState(int)), _progressBar, SLOT(setValue(int)));
    }
}


void TaskWidget::generateTerrainTextures(const Ptr<ImportTerrainLayersInfo> & pTerrainLayersInfo)
{
    if(_pTask == NULL)
    {
        _taskType = TASK_TERRAIN_TEXTURE_GENERATION;

        Core::String terrainLabel = L"Terrain textures Generation";

        _name->setText(String8(terrainLabel).c_str());

        _status->setText("Waiting");
        _taskStatus = TASK_WAITING;

        _pTask = new ImportTerrainLayersTask(pTerrainLayersInfo);

        connect(_pTask, SIGNAL(taskTerminated()), this, SLOT(taskTerminated()));
        connect(_pTask, SIGNAL(progressState(int)), _progressBar, SLOT(setValue(int)));
    }
}

void TaskWidget::generateLightmaps(const Ptr<LightmapsGenerationInfo> & pLMInfo)
{
    if(_pTask == NULL)
    {
        _taskType = TASK_LIGHTMAPS_GENERATION;

        Core::String terrainLabel = L"Lightmaps Generation";

        _name->setText(String8(terrainLabel).c_str());

        _status->setText("Waiting");
        _taskStatus = TASK_WAITING;

        _pTask = new GenerateLightmapsTask(pLMInfo);

        connect(_pTask, SIGNAL(taskTerminated()), this, SLOT(taskTerminated()));
        connect(_pTask, SIGNAL(progressState(int)), _progressBar, SLOT(setValue(int)));
    }
}

void TaskWidget::generateOcclusionmaps(const Ptr<OcclusionmapsGenerationInfo> & pOMInfo)
{
    if(_pTask == NULL)
    {
        _taskType = TASK_OCCLUSIONMAPS_GENERATION;

        Core::String terrainLabel = L"Occlusionmaps Generation";

        _name->setText(String8(terrainLabel).c_str());

        _status->setText("Waiting");
        _taskStatus = TASK_WAITING;

        _pTask = new GenerateOcclusionmapsTask(pOMInfo);

        connect(_pTask, SIGNAL(taskTerminated()), this, SLOT(taskTerminated()));
        connect(_pTask, SIGNAL(progressState(int)), _progressBar, SLOT(setValue(int)));
    }
}

}//namespace EPI