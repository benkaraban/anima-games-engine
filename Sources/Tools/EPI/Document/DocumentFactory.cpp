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
#include <EPI/Document/DocumentFactory.h>
#include <EPI/Document/DocumentParticles.moc.h>
#include <EPI/Document/DocumentMaterial.h>
#include <EPI/Document/DocumentTexture.h>
#include <EPI/Document/DocumentTerrainMesh.h>
#include <EPI/Document/DocumentWorld.moc.h>
#include <EPI/Document/DocumentScript.moc.h>
#include <EPI/Document/DocumentTest.h>
#include <EPI/Document/DocumentModel.h>
#include <EPI/Document/DocumentScript.moc.h>
#include <EPI/Document/DocumentGroup.h>
#include <EPI/Document/DocumentSpline.h>
#include <EPI/Document/DocumentPicBank.h>
#include <EPI/Document/StateRecorder.h>

#include <EPI/Document/Properties/PtyDocInformation.moc.h>

#include <EPI/Constants.h>

#include <EPI/ImportInfo/ImportTextureInfo.h>
#include <EPI/ImportInfo/ImportTerrainInfo.h>
#include <EPI/ImportInfo/ImportParticlesInfo.h>
#include <EPI/ImportInfo/ImportModelInfo.h>
#include <EPI/ImportInfo/ImportWorldInfo.h>
#include <EPI/ImportInfo/ImportScriptInfo.h>
#include <EPI/ImportInfo/ImportGroupInfo.h>
#include <EPI/ImportInfo/ImportSplineInfo.h>
#include <EPI/ImportInfo/ImportPicBankInfo.h>

#include <Core/XML/XMLDocument.h>

namespace EPI
{
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::factoryCreateDocument(GuiDocument& guiGoc, const DocumentType type, const Ptr<DocumentRenderer>& pDocRdr, const Core::String& title)
{
    Core::String newTitle = title;

    
    Ptr<DocumentBase> pDoc;

    switch(type)
    {
    case DOC_UNKNOW:
        pDoc = null;
        break;
    case DOC_PARTICLES:
        pDoc = Ptr<DocumentBase>(new DocumentParticles(guiGoc, pDocRdr));
        break;
    case DOC_MATERIAL:
        pDoc = Ptr<DocumentBase>(new DocumentMaterial(guiGoc, pDocRdr));
        break;
    case DOC_WORLD:
        pDoc = Ptr<DocumentBase>(new DocumentWorld(guiGoc, pDocRdr));
    break;
    case DOC_SCRIPT:
        pDoc = Ptr<DocumentBase>(new DocumentScript(guiGoc, pDocRdr));
        break;
    case DOC_GROUP:
        pDoc = Ptr<DocumentBase>(new DocumentGroup(guiGoc, pDocRdr));
        break;
    case DOC_SPLINE:
        pDoc = Ptr<DocumentBase>(new DocumentSpline(guiGoc, pDocRdr));
        break;
    case DOC_TEST:
        pDoc = Ptr<DocumentBase>(new DocumentTest(guiGoc, pDocRdr));
    break;
    }
    
    if (pDoc!=null)
    {
        if (newTitle==L"")
        {
            newTitle = L"noName_" + Core::toString(pDoc->getID());
        }

        LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(newTitle);
        pDoc->getStateRecorder()->enable();
    }

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::factoryLoadDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<DocumentBase> pDoc;

    if(pImportInfo->assetType == TEXTURE_ASSET) pDoc = loadTextureDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == MODEL_ASSET) pDoc = loadModelDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == TERRAIN_ASSET) pDoc = loadTerrainDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == PARTICLES_ASSET) pDoc = loadParticlesDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == WORLD_ASSET) pDoc = loadWorldDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == SCRIPT_ASSET) pDoc = loadScriptDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == GROUP_ASSET) pDoc = loadGroupDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == SPLINE_ASSET) pDoc = loadSplineDocument(guiGoc, pImportInfo, pDocRdr);
    else if(pImportInfo->assetType == PIC_BANK_ASSET) pDoc = loadPicBankDocument(guiGoc, pImportInfo, pDocRdr);
    else
    {
        LM_ASSERT(false);
        Core::String message;
        message << L"Unknown asset type. Loading cancelled!";
        throw Core::Exception(message);
    }

    pDoc->getStateRecorder()->enable();

    

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadTextureDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportTextureInfo> pTextureInfo = LM_DEBUG_PTR_CAST<ImportTextureInfo>(pImportInfo);

    Ptr<DocumentBase> pDoc(new DocumentTexture(guiGoc, pDocRdr, pTextureInfo));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pTextureInfo->textureName);

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadModelDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportModelInfo> pModelInfo = LM_DEBUG_PTR_CAST<ImportModelInfo>(pImportInfo);

    Ptr<DocumentBase> pDoc(new DocumentModel(guiGoc, pDocRdr, pModelInfo));

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadTerrainDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportTerrainInfo> pTerrainInfo = LM_DEBUG_PTR_CAST<ImportTerrainInfo>(pImportInfo);

    Ptr<DocumentBase> pDoc(new DocumentTerrainMesh(guiGoc, pDocRdr, pTerrainInfo));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pTerrainInfo->terrainModelName);

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadPicBankDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportPicBankInfo> pPicBankInfo = LM_DEBUG_PTR_CAST<ImportPicBankInfo>(pImportInfo);
        
    Ptr<DocumentBase> pDoc(new DocumentPicBank(guiGoc, pDocRdr, pPicBankInfo));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pPicBankInfo->picBankName);

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadParticlesDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportParticlesInfo> pParticlesInfo = LM_DEBUG_PTR_CAST<ImportParticlesInfo>(pImportInfo);

    Ptr<DocumentBase> pDoc(new DocumentParticles(guiGoc, pDocRdr, pParticlesInfo->particlesFileName));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pParticlesInfo->particlesFileName);

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadWorldDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportWorldInfo> pWorldInfo = LM_DEBUG_PTR_CAST<ImportWorldInfo>(pImportInfo);

    Ptr<DocumentBase> pDoc(new DocumentWorld(guiGoc, pDocRdr, pWorldInfo->worldFileName));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pWorldInfo->worldFileName);

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase>  DocumentFactory::loadScriptDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportScriptInfo> pScriptInfo = LM_DEBUG_PTR_CAST<ImportScriptInfo>(pImportInfo);
    Ptr<DocumentBase> pDoc(new DocumentScript(guiGoc, pDocRdr, pScriptInfo->fileName));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pScriptInfo->fileName);

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadGroupDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportGroupInfo> pInfo = LM_DEBUG_PTR_CAST<ImportGroupInfo>(pImportInfo);
    Ptr<DocumentBase> pDoc(new DocumentGroup(guiGoc, pDocRdr, pInfo->fileName));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pInfo->fileName);

    return pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> DocumentFactory::loadSplineDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<ImportSplineInfo> pInfo = LM_DEBUG_PTR_CAST<ImportSplineInfo>(pImportInfo);
    Ptr<DocumentBase> pDoc(new DocumentSpline(guiGoc, pDocRdr, pInfo->fileName));

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->setTitle(pInfo->fileName);

    return pDoc;
}
//-----------------------------------------------------------------------------
} //EPI