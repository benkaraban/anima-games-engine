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
#include "GenerateOcclusionmapsTask.h"
#include <EPI/ImportInfo/ImportWorldInfo.h>
#include <Packing/OcclusionmapGen.h>

namespace EPI
{

GenerateOcclusionmapsTask::GenerateOcclusionmapsTask(const Ptr<OcclusionmapsGenerationInfo> & pOMInfo)
:   _pOMInfo(pOMInfo),
    _progress(0)
{
}

GenerateOcclusionmapsTask::~GenerateOcclusionmapsTask()
{
}

void GenerateOcclusionmapsTask::run()
{
    try
    {
        Ptr<Core::PtrProgressObs> pObs(new Core::PtrProgressObs(this));

        Packing::OcclusionMapsGenOptions options;

        options.pObserver = pObs;
        options.prefix = L"om-";
        options.pWorld = _pOMInfo->pWorld;
        options.path = _pOMInfo->omPath;
        options.compressTextures = _pOMInfo->compressedTextures;
        options.tag = _pOMInfo->tagFilter;
        options.defaultResolution = _pOMInfo->defaultResolution;
        options.samplesCount = _pOMInfo->samplesCount;
        options.triangleMinRes = _pOMInfo->triangleMinRes;
        
        Packing::generateOcclusionMaps(options);



        //String targetFileName = _pLMInfo->lmPath + L".world";
        //String metaFileName = targetFileName + L".xml";

        //ImportWorldInfo worldInfo;
        //worldInfo.worldFileName = targetFileName;
        //worldInfo.lightmaps = *_pLMInfo;

        //Ptr<Core::VirtualFileSystem> pVFS(_pLMInfo->pWorld->getRessourcesPool()->getVFS());

        //Ptr<Core::OutputStream> pWorldOutput(pVFS->writeFile(targetFileName));
        //Ptr<Core::OutputStream> pMetaOutput(pVFS->writeFile(metaFileName));

        //if(pWorldOutput == null) throw Core::Exception(L"Unable to write open target world file");
        //if(pMetaOutput == null) throw Core::Exception(L"Unable to write open target world meta file");
        //{
        //    Core::XMLDocument doc;
        //    _pLMInfo->pWorld->exportWorldXML(doc);
        //    doc.saveDocument(*pWorldOutput);
        //}
        ////{
        ////    Core::XMLDocument doc;
        ////    worldInfo.exportXML();
        ////    doc.saveDocument(*pMetaOutput);
        ////}
    }
    catch(Core::Exception & e)
    {
        ERR << e.getMessage() << L"\n";
    }


    emit taskTerminated();
}

void GenerateOcclusionmapsTask::progress(float p)
{
    _progress = p * 100;
    emit progressState(_progress);
}

}//namespace EPI
