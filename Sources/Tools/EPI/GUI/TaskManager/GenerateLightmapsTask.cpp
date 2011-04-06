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
#include "GenerateLightmapsTask.h"
#include <EPI/ImportInfo/ImportWorldInfo.h>
#include <Packing/LightmapGen.h>
#include <Packing/WorldPak.h>

namespace EPI
{

GenerateLightmapsTask::GenerateLightmapsTask(const Ptr<LightmapsGenerationInfo> & pLMInfo)
:   _pLMInfo(pLMInfo),
    _progress(0)
{
}

GenerateLightmapsTask::~GenerateLightmapsTask()
{
}

void GenerateLightmapsTask::run()
{
    try
    {
        Ptr<Core::PtrProgressObs> pObs(new Core::PtrProgressObs(this));
        {
            Ptr<Core::IProgressObs> pSubObs(new Core::SubProgressObs(pObs, 0.0f, 0.5f));
            Packing::LightmapsGenOptions options;

            options.pWorld = _pLMInfo->pWorld;
            options.path = L"/Temp/";
            options.prefix = L"lm-";
            options.compressTextures = false;
            options.debugCopy = false;
            options.tag = _pLMInfo->tagFilter;
            options.pObserver = pSubObs;
            options.defaultResolution = _pLMInfo->defaultResolution;
            
            Packing::generateLightmaps(options);

            Ptr<Core::VirtualFileSystem> pVFS(_pLMInfo->pWorld->getRessourcesPool()->getVFS());

            String name(L"/Temp/test.world");

            Ptr<Core::OutputStream> pWorldOutput(pVFS->writeFile(name));
            Ptr<Core::OutputStream> pMetaOutput(pVFS->writeFile(name + L".xml"));

            if(pWorldOutput == null) throw Core::Exception(L"Unable to write open target world file");
            if(pMetaOutput == null) throw Core::Exception(L"Unable to write open target world meta file");

            // Fichier infos .xml
            {
                Ptr<Core::XMLDocument> pDoc(new Core::XMLDocument());
                _pLMInfo->exportXML(pDoc);
                pDoc->saveDocument(*pMetaOutput);
            }
            // Fichier world
            {
                Core::XMLDocument doc;
                _pLMInfo->pWorld->exportWorldXML(doc);
                doc.saveDocument(*pWorldOutput);
            }

        }
        {
            Ptr<Core::IProgressObs> pSubObs(new Core::SubProgressObs(pObs, 0.5f, 1.0f));
            Packing::WorldPackingOptions options;

            options.pWorld = _pLMInfo->pWorld;
            options.path = _pLMInfo->lmPath;
            options.pObserver = pSubObs;
            options.textureAtlasSize = _pLMInfo->textureAtlasSize;
            options.batchSize = _pLMInfo->batchSize;
            options.border = _pLMInfo->border;

            Packing::packWorld(options);
        }
    }
    catch(Core::Exception & e)
    {
        ERR << e.getMessage() << L"\n";
    }


    emit taskTerminated();
}

void GenerateLightmapsTask::progress(float p)
{
    _progress = p * 100;
    emit progressState(_progress);
}

}//namespace EPI