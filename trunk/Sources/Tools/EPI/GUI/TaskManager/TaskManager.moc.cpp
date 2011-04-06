/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include "TaskManager.moc.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QSpacerItem>

#include <EPI/Constants.h>
#include <EPI/ImportInfo/ImportTextureInfo.h>
#include <EPI/ImportInfo/ImportTerrainInfo.h>
#include <EPI/ImportInfo/ImportTerrainLayersInfo.h>
#include <EPI/ImportInfo/ImportModelInfo.h>
#include <EPI/ImportInfo/ImportPicBankInfo.h>
#include <EPI/ImportInfo/ImportWorldInfo.h>
#include <EPI/GUI/TaskManager/TaskWidget.moc.h>

namespace EPI
{

TaskManager::TaskManager(QWidget * parent, int32 maxParallelTasks)
:   QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    _maxParallelTasks(maxParallelTasks)
{
    setupUi();
}

TaskManager::~TaskManager()
{
    if(_layout != NULL)
        delete _layout;
    if(_scrollLayout != NULL)
        delete _scrollLayout;
    if(_scrollArea != NULL)
        delete _scrollArea;
    

    _layout = NULL;
    _scrollLayout = NULL;
    _scrollArea = NULL;
}

void TaskManager::setupUi()
{
    _layout     = new QVBoxLayout();
    _scrollArea = new QScrollArea(this);
    _scrollLayout = new QVBoxLayout();
    _scrollLayout->setContentsMargins(0, 0, 0, 0);

    _scrollArea->setLayout(_scrollLayout);
    _layout->addWidget(_scrollArea);

    setMinimumWidth(400);
    setMinimumHeight(300);

    _verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _scrollLayout->addItem(_verticalSpacer);

    setLayout(_layout);
    
}

void TaskManager::taskTerminated(TaskWidget * taskWidget)
{
    Core::ScopedLock  lock(_mutex);

    disconnect(taskWidget, SIGNAL(taskTerminated(TaskWidget *)), this, SLOT(taskTerminated(TaskWidget *)));
    disconnect(taskWidget, SIGNAL(taskCancelled(TaskWidget *)), this, SLOT(taskCancelled(TaskWidget *)));

    if(taskWidget->getTaskType() == TASK_TEXTURE_COMPRESSION)
    {
        Ptr<ImportTextureInfo> pTextureInfo = taskWidget->getImportTextureInfo();
        emit generationDone(pTextureInfo);
    }
    else if(taskWidget->getTaskType() == TASK_MODEL_CONVERT)
    {
        Ptr<ImportModelInfo> pModelInfo = taskWidget->getImportModelInfo();
        emit generationDone(pModelInfo);
    }
    else if(taskWidget->getTaskType() == TASK_TERRAIN_GENERATION)
    {
        Ptr<ImportTerrainInfo> pTerrainInfo = taskWidget->getImportTerrainInfo();
        emit generationDone(pTerrainInfo);
    }
    else if(taskWidget->getTaskType() == TASK_PIC_BANK_GENERATION)
    {
        Ptr<ImportPicBankInfo> pPicBankInfo = taskWidget->getImportPicBankInfo();
        emit generationDone(pPicBankInfo);
    }
    else if(taskWidget->getTaskType() == TASK_LIGHTMAPS_GENERATION)
    {
        Ptr<ImportWorldInfo> pWorldInfo(new ImportWorldInfo());
        pWorldInfo->lightmaps = *taskWidget->getLightmapsGenerationInfo();
        pWorldInfo->worldFileName = pWorldInfo->lightmaps.lmPath + L".world";
        emit generationDone(pWorldInfo);
    }
    else if(taskWidget->getTaskType() == TASK_OCCLUSIONMAPS_GENERATION)
    {
        Ptr<ImportWorldInfo> pWorldInfo(new ImportWorldInfo());
        pWorldInfo->occlusionmaps = *taskWidget->getOcclusionmapsGenerationInfo();
        pWorldInfo->worldFileName = pWorldInfo->occlusionmaps.omPath + L".world";
        emit generationDone(pWorldInfo);
    }

    removeTask(taskWidget);
}

void TaskManager::taskCancelled(TaskWidget * taskWidget)
{
    Core::ScopedLock  lock(_mutex);

    disconnect(taskWidget, SIGNAL(taskTerminated(TaskWidget *)), this, SLOT(taskTerminated(TaskWidget *)));
    disconnect(taskWidget, SIGNAL(taskCancelled(TaskWidget *)), this, SLOT(taskCancelled(TaskWidget *)));

    removeTask(taskWidget);
}

void TaskManager::removeTask(TaskWidget * taskWidget)
{
    _scrollLayout->removeWidget(taskWidget);

    int32 runningTasks = 0;
    Core::List<TaskWidget *>::iterator iTasks = _tasks.begin();
    while(iTasks != _tasks.end())
    {
        if(*iTasks == taskWidget)
            iTasks = _tasks.erase(iTasks);
        else
        {
            if((*iTasks)->getTaskStatus() == TASK_WAITING && runningTasks < _maxParallelTasks)
            {
                (*iTasks)->start();
                runningTasks++;
            }            

            ++iTasks;
        }
    }
    
    delete taskWidget;
    taskWidget = NULL;

    if(_tasks.size() == 0)
        hide();
}

void TaskManager::generate(const Ptr<ImportInfo> & pImportInfo)
{
    Core::ScopedLock  lock(_mutex);

    switch(pImportInfo->assetType)
    {
    case TEXTURE_ASSET:
        importTexture(LM_DEBUG_PTR_CAST<ImportTextureInfo>(pImportInfo));
        break;
    case TERRAIN_ASSET:
        importTerrain(LM_DEBUG_PTR_CAST<ImportTerrainInfo>(pImportInfo));
        break;
    case TERRAIN_LAYERS:
        generateTerrainTextures(LM_DEBUG_PTR_CAST<ImportTerrainLayersInfo>(pImportInfo));
        break;
    case MODEL_ASSET:
        importModel(LM_DEBUG_PTR_CAST<ImportModelInfo>(pImportInfo));
        break;
    case PIC_BANK_ASSET:
        importPicBank(LM_DEBUG_PTR_CAST<ImportPicBankInfo>(pImportInfo));
        break;
    case WORLD_ASSET: 
    {
        // En fait si on "génère" un world c'est qu'on génère les lightmaps d'un world
        // mais comme le fichier obtenu est un world, on passe par le type WORLD_ASSET
        Ptr<ImportWorldInfo> pWorldInfo = LM_DEBUG_PTR_CAST<ImportWorldInfo>(pImportInfo);
        if(pWorldInfo->lightmaps.sourceWorld != String::EMPTY)
            generateLightmaps(pWorldInfo);
        else if(pWorldInfo->occlusionmaps.sourceWorld != String::EMPTY)
            generateOcclusionmaps(pWorldInfo);
        break;
    }
    default:
        Core::String message;
        message << L"Unknown asset type : " << pImportInfo->assetType << L". Import cancelled!";
        throw Core::Exception(message);
        break;
    }
}

void TaskManager::importTexture(const Ptr<ImportTextureInfo> & pTextureInfo)
{
    if(pTextureInfo->pStreamImage == null)
    {
        emit sourceNotOpened(pTextureInfo);
        return;
    }

    TaskWidget * taskWidget = new TaskWidget(this);

    taskWidget->importTexture(pTextureInfo);

    connectAndEngueueTask(taskWidget);
}

void TaskManager::importPicBank(const Ptr<ImportPicBankInfo> & pPicBankInfo)
{
    TaskWidget * taskWidget = new TaskWidget(this);

    taskWidget->importPicBank(pPicBankInfo);

    connectAndEngueueTask(taskWidget);
}

void TaskManager::importTerrain(const Ptr<ImportTerrainInfo> & pTerrainInfo)
{
    if(pTerrainInfo->pStreamHeightMap == null || pTerrainInfo->pStreamNormalMap == null)
    {
        emit sourceNotOpened(pTerrainInfo);
        return;
    }

    TaskWidget * taskWidget = new TaskWidget(this);

    taskWidget->importTerrain(pTerrainInfo);

    connectAndEngueueTask(taskWidget);
}

void TaskManager::generateLightmaps(const Ptr<ImportWorldInfo> &pImportInfo)
{
    TaskWidget * taskWidget = new TaskWidget(this);
    taskWidget->generateLightmaps(Ptr<LightmapsGenerationInfo>(new LightmapsGenerationInfo(pImportInfo->lightmaps)));
    connectAndEngueueTask(taskWidget);
}

void TaskManager::generateOcclusionmaps(const Ptr<ImportWorldInfo> &pImportInfo)
{
    TaskWidget * taskWidget = new TaskWidget(this);
    taskWidget->generateOcclusionmaps(Ptr<OcclusionmapsGenerationInfo>(new OcclusionmapsGenerationInfo(pImportInfo->occlusionmaps)));
    connectAndEngueueTask(taskWidget);
}

void TaskManager::generateTerrainTextures(const Ptr<ImportTerrainLayersInfo> & pTerrainLayerInfo)
{
    TaskWidget * taskWidget = new TaskWidget(this);

    taskWidget->generateTerrainTextures(pTerrainLayerInfo);

    connectAndEngueueTask(taskWidget);
}

void TaskManager::importModel(const Ptr<ImportModelInfo> & pModelInfo)
{
    if(pModelInfo->pStreamModel == null && pModelInfo->import == IMPORT_MODEL_INIT)
    {
        emit sourceNotOpened(pModelInfo);
        return;
    }

    TaskWidget * taskWidget = new TaskWidget(this);
    taskWidget->importModel(pModelInfo);

    connectAndEngueueTask(taskWidget);
}

void TaskManager::connectAndEngueueTask(TaskWidget * taskWidget)
{
    _scrollLayout->removeItem(_verticalSpacer);
    _scrollLayout->addWidget(taskWidget, Qt::AlignTop);
    _scrollLayout->addItem(_verticalSpacer);

    connect(taskWidget, SIGNAL(taskTerminated(TaskWidget *)), this, SLOT(taskTerminated(TaskWidget *)));
    connect(taskWidget, SIGNAL(taskCancelled(TaskWidget *)), this, SLOT(taskCancelled(TaskWidget *)));

    show();
    
    _tasks.push_back(taskWidget);

    int32 runningTasks = 0;

    for(int32 ii=0; ii<_tasks.size(); ++ii)
    {
        if(_tasks[ii]->getTaskStatus() == TASK_RUNNING)
            runningTasks++;
    }

    if(runningTasks < _maxParallelTasks)
        taskWidget->start();
}

}//namespace EPI