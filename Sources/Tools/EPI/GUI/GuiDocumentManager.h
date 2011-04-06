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
#ifndef DOCUMENT_MANAGER_H_
#define DOCUMENT_MANAGER_H_

#include <Core/Standard.h>

#include <EPI/GUI/GuiDocument.moc.h>

namespace EPI
{
class DocumentRenderer;
enum DocumentType;

//-----------------------------------------------------------------------------
class LM_API_EPI GuiDocumentManager : public QObject, public Core::Object
{
public:
    GuiDocumentManager();
    virtual ~GuiDocumentManager();

    const Ptr<GuiDocument>& createGuiDocument(const DocumentType type, const Core::String& title=L"", const Ptr<DocumentRenderer>& pDocRdr=null);
    const Ptr<GuiDocument>& loadGuiDocument(const Ptr<ImportInfo> & pImportInfo, bool & docAlreadyOpened, const Ptr<DocumentRenderer>& pDocRdr=null);
    Ptr<GuiDocument> destroyGuiDocument(int32 id);

    Ptr<GuiDocument> getGuiDocument(int32 id) const;
    Ptr<GuiDocument> getCurrentGuiDocument() const {return getGuiDocument(_idCurrentGDoc);}
    int32 getCurrentGuiDocumentId() const {return _idCurrentGDoc;}

    Ptr<GuiDocument> selectGuiDocument(int32 id);

    /**
    *  permet d'acceder a la liste de documents
    */
    const Core::List<Ptr<GuiDocument> >& getAllGuiDocument() const {return _gDocs;}
    Core::List<Ptr<GuiDocument> >& getAllGuiDocument() {return _gDocs;}

    bool isEmpty() const;
private:
    int32 _idCurrentGDoc;
    Core::List<Ptr<GuiDocument> > _gDocs;
};
//-----------------------------------------------------------------------------
} // namespace EPI
#endif // DOCUMENT_MANAGER_H_