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
#include "GuiDocumentManager.h"

#include <EPI/Document/DocumentFactory.h>
#include <EPI/Document/Properties/PtyDocInformation.moc.h>

#include <EPI/DocumentRenderer.h>
#include <EPI/RendererManager.h>
#include <EPI/ApplicationBase.moc.h>
#include <EPI/Gui/GuiContext.moc.h>

namespace EPI
{
const int32 NO_GDOC_ID = -1;

//-----------------------------------------------------------------------------
GuiDocumentManager::GuiDocumentManager():
_idCurrentGDoc(NO_GDOC_ID)
{
}
//-----------------------------------------------------------------------------
GuiDocumentManager::~GuiDocumentManager()
{
}
//-----------------------------------------------------------------------------
const Ptr<GuiDocument>& GuiDocumentManager::createGuiDocument(const DocumentType type, const Core::String& title, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<DocumentBase> pDoc;

    Ptr<GuiDocument> pGuiDoc (new GuiDocument ());

    pDoc = DocumentFactory::factoryCreateDocument(*pGuiDoc.get(), type, pDocRdr, title);
    pGuiDoc->setDocument(pDoc);
    
    getAllGuiDocument().push_back(pGuiDoc);

    connect(pDoc.get(), SIGNAL(generate(const Ptr<ImportInfo> &)), pGuiDoc.get(), SLOT(generateInfo(const Ptr<ImportInfo> &)));

    return getAllGuiDocument().back();
}
//-----------------------------------------------------------------------------
const Ptr<GuiDocument>& GuiDocumentManager::loadGuiDocument(const Ptr<ImportInfo> & pImportInfo, bool & docAlreadyOpened, const Ptr<DocumentRenderer>& pDocRdr)
{
    Ptr<GuiDocument> pGuiDoc (new GuiDocument ());
    Ptr<DocumentBase> pDoc = DocumentFactory::factoryLoadDocument(*pGuiDoc.get(), pImportInfo, pDocRdr);
    pGuiDoc->setDocument(pDoc);
    Core::String docTitle = LM_DEBUG_PTR_CAST<PyDocumentInformation> (pDoc->getPyDocumentInformation())->getTitle();

    Core::List<Ptr<GuiDocument> >::const_iterator iDocument = getAllGuiDocument().begin();
    while(iDocument != getAllGuiDocument().end() 
    && LM_DEBUG_PTR_CAST<PyDocumentInformation> ((*iDocument)->getDocument()->getPyDocumentInformation())->getTitle() != docTitle)
    {
        ++iDocument;
    }

    if(iDocument == getAllGuiDocument().end())
    {
        docAlreadyOpened = false;

        connect(pDoc.get(), SIGNAL(generate(const Ptr<ImportInfo> &)), pGuiDoc.get(), SLOT(generateInfo(const Ptr<ImportInfo> &)));
        getAllGuiDocument().push_back(pGuiDoc);

        return getAllGuiDocument().back();
    }
    else
    {
        docAlreadyOpened = true;
        return *iDocument;
    }
    
}
//-----------------------------------------------------------------------------
Ptr<GuiDocument> GuiDocumentManager::destroyGuiDocument(int32 id)
{
    Ptr<GuiDocument> pGDoc = getGuiDocument(id);

    if (pGDoc!=null)
    {
        bool docIsFind = false;

        Core::List<Ptr<GuiDocument> >::iterator iGDoc = getAllGuiDocument().begin();
        Core::List<Ptr<GuiDocument> >::iterator iGDocEnd = getAllGuiDocument().end();

        for (; (iGDoc!=iGDocEnd); ++iGDoc)
        {
            if ((*iGDoc) == pGDoc)
            {
                getAllGuiDocument().erase(iGDoc);
                break;
            }
        }

        if (_idCurrentGDoc == pGDoc->getID())
        {
            if (getAllGuiDocument().size() == 0)
            {
                _idCurrentGDoc = -1;
            }
            else
            {
                _idCurrentGDoc = getAllGuiDocument().back()->getID();
            }
        }
    }

    return pGDoc;
}
//-----------------------------------------------------------------------------
Ptr<GuiDocument> GuiDocumentManager::getGuiDocument(int32 id) const
{
    Ptr<GuiDocument> ret;

    if (id != NO_GDOC_ID)
    {
	    bool docIsFind = false;

	    Core::List<Ptr<GuiDocument> >::const_iterator iDocGui = getAllGuiDocument().begin();
	    Core::List<Ptr<GuiDocument> >::const_iterator iDocGuiEnd = getAllGuiDocument().end();

	    for (; (iDocGui!=iDocGuiEnd) && (docIsFind==false); ++iDocGui)
	    {
		    if ((*iDocGui)->getID() == id)
		    {
			    docIsFind = true;
			    break;
		    }
	    }

	    if (docIsFind == false)
	    {
		    throw Core::Exception(L"ApplicationBase::getDocument(const int32 id) : Document does'nt exist");
	    }

        ret = *iDocGui;
    }

    return ret;
}
//-----------------------------------------------------------------------------
Ptr<GuiDocument> GuiDocumentManager::selectGuiDocument(int32 id)
{
    Ptr<GuiDocument> pOldGDoc = getCurrentGuiDocument();
    if (pOldGDoc != null)
    {
        pOldGDoc->stopAnimate();
        pOldGDoc->getGuiContext()->setCurrentViewport(NULL, null);
    }

    _idCurrentGDoc = id;

    Ptr<GuiDocument> pGDoc = getCurrentGuiDocument();
    if(pGDoc != null)
    {
        pGDoc->startAnimate();
    }
    return pGDoc;
}
//-----------------------------------------------------------------------------
bool GuiDocumentManager::isEmpty() const
{
    return getAllGuiDocument().size() == 0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI
