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
#include "ImportTerrainTask.h"

#include <Core/IProgressObs.h>

#include <Images/Operator.h>

#include <Workflow/ModelOptimize.h>

namespace EPI
{

class ImportTerrainTaskObs : public Core::IProgressObs
{
public:
    ImportTerrainTaskObs(ImportTerrainTask * pImportTerrainTask)
    :   _pImportTerrainTask(pImportTerrainTask)
    {}

    ~ImportTerrainTaskObs(){}

    virtual void progress(float p)
    {
        _pImportTerrainTask->progress(p);
    }
private:
    ImportTerrainTask * _pImportTerrainTask;
};

ImportTerrainTask::ImportTerrainTask(const Ptr<ImportTerrainInfo> & pTerrainInfo)
:   _pTerrainInfo(pTerrainInfo),
    _progress(0)
{
}

ImportTerrainTask::~ImportTerrainTask()
{
}

void ImportTerrainTask::run()
{
    Ptr<Images::Image> pHeightMapRGBF32 (new Images::Image);
    pHeightMapRGBF32->load2(*_pTerrainInfo->pStreamHeightMap);
    _pTerrainInfo->pTerrainOptions->pHeightMap = Ptr<Images::Image>(new Images::Image(Core::GRAY_F32, pHeightMapRGBF32->getWidth(), pHeightMapRGBF32->getHeight()));

    if(_pTerrainInfo->normalMapName != Core::String::EMPTY)
    {
        _pTerrainInfo->pTerrainOptions->pNormalMap = Ptr<Images::Image>(new Images::Image);
        _pTerrainInfo->pTerrainOptions->pNormalMap->load2(*_pTerrainInfo->pStreamNormalMap);
    }

    Images::convert(pHeightMapRGBF32, _pTerrainInfo->pTerrainOptions->pHeightMap);

    _pTerrainInfo->pTerrainModel = Workflow::generateTerrainModel(*_pTerrainInfo->pTerrainOptions);
    _pTerrainInfo->pStreamHeightMap = null;
    _pTerrainInfo->pStreamNormalMap = null;

    Workflow::ModelPackingOptions modelPackingOptions;
    modelPackingOptions.pObs = Ptr<ImportTerrainTaskObs>(new ImportTerrainTaskObs(this));
    Workflow::packModel(modelPackingOptions, *_pTerrainInfo->pTerrainModel);

    emit taskTerminated();
}

void ImportTerrainTask::progress(float p)
{
    _progress = p * 100;
    emit progressState(_progress);
}

}//namespace EPI