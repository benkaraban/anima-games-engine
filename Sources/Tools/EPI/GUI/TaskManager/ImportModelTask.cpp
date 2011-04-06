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
#include "ImportModelTask.h"

#include <Core/System/Files.h>

#include <Workflow/ModelOptimize.h>
#include <Workflow/ModelImporterX.h>
#include <Workflow/ModelImporterOBJ.h>
#include <Workflow/COLLADA/ModelImporterCOLLADA.h>

#include <QMessageBox>

namespace EPI
{

class ImportModelTaskObs : public Core::IProgressObs
{
public:
    ImportModelTaskObs(ImportModelTask * pImportModelTask)
    :   _pImportModelTask(pImportModelTask)
    {}

    ~ImportModelTaskObs(){}

    virtual void progress(float p)
    {
        _pImportModelTask->progress(p);
    }
private:
    ImportModelTask * _pImportModelTask;
};

ImportModelTask::ImportModelTask(const Ptr<ImportModelInfo> & pModelInfo)
:   _pModelInfo(pModelInfo),
    _progress(0)
{
}

ImportModelTask::~ImportModelTask()
{
}

void ImportModelTask::run()
{
    try
    {
        String warnings;

        if(_pModelInfo->import == IMPORT_MODEL_INIT)
        {
            Core::String extension = Core::System::getFileExt(_pModelInfo->sourceName);
            extension.toUpper();

            Ptr<Workflow::ModelImporter> pImporter(null);

            if( extension == L"X")
            {
                pImporter = Ptr<Workflow::ModelImporter>(new Workflow::ModelImporterX());
            }
            else if(extension == L"DAE")
            {
                pImporter = Ptr<Workflow::ModelImporter>(new Workflow::ModelImporterCOLLADA());
            }
            else if(extension == L"OBJ")
            {
                pImporter = Ptr<Workflow::ModelImporter>(new Workflow::ModelImporterOBJ());
            }

            _pModelInfo->pModel = pImporter->importModel(*_pModelInfo->pStreamModel);
            _pModelInfo->pStreamModel = null;

            Workflow::setupTextures(*_pModelInfo->pModel, _pModelInfo->pVFS, _pModelInfo->modelName);

            warnings = pImporter->getWarnings();
        }
        if(_pModelInfo->import == IMPORT_MODEL_INIT || _pModelInfo->import == IMPORT_MODEL_GENERATE)
        {
            _pModelInfo->modelPackingOptions.pObs = Ptr<ImportModelTaskObs>(new ImportModelTaskObs(this));
            Workflow::packModel(_pModelInfo->modelPackingOptions, *_pModelInfo->pModel);
        }

        emit progressState(100);

        if(!warnings.empty())
            _warnings = QString(String8(warnings).c_str());

        emit taskTerminated();
    }
    catch(Core::Exception & exception)
    {
        _errors = QString(String8(exception.getMessage()).c_str());
        emit taskTerminated();
    }
}

void ImportModelTask::progress(float p)
{
    _progress = p * 100;
    emit progressState(_progress);
}

}//namespace EPI