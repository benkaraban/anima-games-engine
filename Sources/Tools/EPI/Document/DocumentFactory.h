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
#ifndef DOCUMENTFACTORY_H_
#define DOCUMENTFACTORY_H_

#include <Core/Standard.h>

#include <EPI/Document/DocumentBase.moc.h>

namespace Core
{
class XMLDocument;
}

namespace EPI
{
/**
*  Les documents sont composés de différents donnees : les proprietes
*  Des document peuvent utiliser les memes types de proprietes independanmment de leur heritage.
*  afin de factoriser le code de creation de document on se propose de construire 
*  les documents par une factory.
*/
class LM_API_EPI DocumentFactory
{
public:
    static Ptr<DocumentBase> factoryCreateDocument(GuiDocument& guiGoc, const DocumentType type, const Ptr<DocumentRenderer>& pDocRdr, const Core::String& title=L"");
    static Ptr<DocumentBase> factoryLoadDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);

private:
    static Ptr<DocumentBase> loadTextureDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadModelDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadTerrainDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadParticlesDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadWorldDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadScriptDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadGroupDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadSplineDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    static Ptr<DocumentBase> loadPicBankDocument(GuiDocument& guiGoc, const Ptr<ImportInfo> & pImportInfo, const Ptr<DocumentRenderer>& pDocRdr);
    
};

} //EPI
#endif DOCUMENTFACTORY_H_
