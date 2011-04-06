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
#include "DocumentScript.moc.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>

#include <QtToolbox/VFSDialog.moc.h>

#include <EPI/Constants.h>
#include <EPI/Document/PropertySelection.moc.h>
#include <EPI/Document/Properties/PtyDocScript.moc.h>
#include <EPI/Document/Properties/PtyScript.moc.h>
#include <EPI/Document/IDocumentWidget.h>
#include <EPI/Document/Properties/PtyDocInformation.moc.h>
#include <EPI/Document/Properties/PtyDocEdition.moc.h>
#include <EPI/DocumentRenderer.h>


#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Gui/GuiContext.moc.h>
#include <EPI/Gui/Viewport/CameraManager.h>

namespace EPI
{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DocumentScriptWidget::DocumentScriptWidget(const DocumentScript& doc, QWidget* parent) :
    IDocumentWidget(parent),
    _doc(const_cast<DocumentScript&>(doc))
{
    setupUi();

    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (_doc.DocumentBase::getPropertyDocumentContent());
    Ptr<PtyDocScript> pPty = LM_DEBUG_PTR_CAST<PtyDocScript> (pPtyContent->getChild(0));

    if (pPty->getWorldXMLPath() != L"")
    {
        _editWorld->setEnabled(true);
        _saveEditWorld->setEnabled(true);
    }
    else
    {
        _editWorld->setEnabled(false);
        _saveEditWorld->setEnabled(false);
    }
}
//-----------------------------------------------------------------------------
DocumentScriptWidget::~DocumentScriptWidget()
{
}
//-----------------------------------------------------------------------------
void DocumentScriptWidget::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _editWorld = new QPushButton("Edit World", this);
    _editWorld->setCheckable(true);
    connect(_editWorld, SIGNAL(toggled(bool)), this, SLOT(swapEditWorld()));

    _saveEditWorld = new QPushButton("Save World", this);
    connect(_saveEditWorld, SIGNAL(released()), this, SLOT(saveEditWorld()));

    _layout->addWidget(_editWorld, 0, 0, Qt::AlignTop );
    _layout->addWidget(_saveEditWorld, 0, 1, Qt::AlignTop );

    setLayout(_layout);
    setFixedHeight(30);
}
//-----------------------------------------------------------------------------
void DocumentScriptWidget::swapEditWorld()
{
    _doc.swapEditWorld();
}
//-----------------------------------------------------------------------------
void DocumentScriptWidget::saveEditWorld()
{
    _doc.saveEditWorld();
}
//-----------------------------------------------------------------------------
void DocumentScriptWidget::enableWorldEdit()
{
    _editWorld->setEnabled(true);
    _saveEditWorld->setEnabled(true);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DocumentScript::DocumentScript(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr) :
    DocumentBase(guiGoc, DOC_SCRIPT, pDocRdr),
    _isInEditWorld(false)
{
    initialize(); 
}
//-----------------------------------------------------------------------------
DocumentScript::DocumentScript(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Core::String & fileName) :
    DocumentBase(guiGoc, DOC_SCRIPT, pDocRdr),
    _isInEditWorld(false)
{
    initialize();
    loadFile(fileName);
}
//-----------------------------------------------------------------------------
DocumentScript::~DocumentScript()
{
}
//-----------------------------------------------------------------------------
void DocumentScript::initialize()
{
    Ptr<PropertySelectionRoot> pPtySelectScript (new PropertySelectionDefault(L"ScriptCurrentSelection", DocumentBase::getPropertyDocumentContent().get(), getStateRecorder()));
    Ptr<PropertySelectionDual> pPtySelectDual (new PropertySelectionDual(L"SelectionDocScript", pPtySelectScript, null));
 
    Ptr<PtyDocEdition> pPtyDocEdit = getPropertyDocumentEdition();
    pPtyDocEdit->changePropertySelection(pPtySelectDual);

    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());
    Ptr<PtyDocScript> pPtyDocScript (new PtyDocScript(getGuiDocument(), getDocumentRenderer(), L"Script"));
        pPtyContent->addChild(pPtyDocScript);

    connect(pPtyContent->getChild(0).get(), SIGNAL(worldChanged()),
        this, SIGNAL(worldEditIsEnable()));

    pPtyDocScript->setWorldXMLPath(MiscConst::pathWorldDefault);
    pPtyDocScript->setTagGamePlay(MiscConst::defaultTagGameplay);
    pPtyDocScript->updateData();
}
//-----------------------------------------------------------------------------
const Ptr<Property> DocumentScript::getPropertyScript() const
{
    Ptr<PtyDocScript> pPtyDocScript = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    return pPtyDocScript->getPtyScript(); 
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentScript::getPropertyScript()
{
    Ptr<PtyDocScript> pPtyDocScript = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    return pPtyDocScript->getPtyScript(); 
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& DocumentScript::getWorld() const
{
    Ptr<PtyDocScript>   pPtyDocScript   = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>      pPtyScript      = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 

    Ptr<IWorldScriptable>   pWorldA = pPtyScript->getWorldScriptable();
    return pWorldA->getWorldForAnimation();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentScript::getPropertyDocumentContentStart() const
{
    Ptr<Property> pRet;

    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>   pPtyDocScript   = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>      pPtyScript      = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();
        

        if (pADoc!= null)
        {
            const Ptr<Property>& pPty = pADoc->getPropertyDocumentContentStart();
            if (pPty!=null) return pPty;
        }
    }

    return DocumentBase::getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
void DocumentScript::runStopScript()
{
    Ptr<PtyDocScript>   pPtyDocScript   = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>      pPtyScript      = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
    Ptr<IWorldScriptable>   pWorldA = pPtyScript->getWorldScriptable();

    
    if (isEditing() == true)
    {
        setEditing(false);
        connect(pWorldA->getLuaOutput().get(), SIGNAL(newErrorMsg(const String & )), this, SLOT(errorInScript()));
        pWorldA->runScript(pPtyScript->getScript(), pPtyScript->getName(), pPtyDocScript->getTagGamePlay());
        emit runStopScriptChanged();
    }
    else
    {
        setEditing(true);
        pWorldA->stopScript();
        disconnect(pWorldA->getLuaOutput().get(), SIGNAL(newErrorMsg(const String & )), this, SLOT(errorInScript()));
        emit runStopScriptChanged();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::errorInScript()
{
    runStopScript();
}
//-----------------------------------------------------------------------------
void DocumentScript::escape()
{
    if (isEditing() == false)
    {
        runStopScript();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::save()
{

    if (_isInEditWorld==false)
    {
        saveScript();
    }
    else
    {
        int ret = QMessageBox::information(NULL, "Question :", "World editing mode",
                    "Save Script", "Save World", "Cancel");

        if (ret==0)             saveScript();
        else if (ret == 1)      saveEditWorld();
        
        return;
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::saveAs()
{
    QtToolbox::VFSDialog dlg (*getDocumentRenderer()->getVFS(), QtToolbox::VFSD_SAVE, DOCUMENT_LUA_SCRIPT_EXT, NULL);
    if(dlg.exec())
    {
        Core::String fileName = dlg.getSelectedFile();
        saveFile(fileName);
        Ptr<PyDocumentInformation> pPinfo = LM_DEBUG_PTR_CAST<PyDocumentInformation>(DocumentBase::getPyDocumentInformation());
        pPinfo->setTitle(fileName);
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::saveScript()
{
    if (getTitle().startsWith(L"/") == true)
    {
        saveFile(getTitle());
    }
    else
    {
        saveAs();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::saveFile(const Core::String & fileName)
{
    Ptr<PtyDocScript>   pPtyDocScript   = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>      pPtyScript      = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
    Core::String8       script          = pPtyScript->getScript();

    Ptr<Core::OutputStream> pOutput = DocumentBase::getDocumentRenderer()->getVFS()->writeFile(fileName);
    pOutput->writeData(script.c_str(), script.size());
    pOutput->close();

    DocumentBase::setModified(false);
}
//-----------------------------------------------------------------------------
void DocumentScript::loadFile(const Core::String & fileName)
{
    Ptr<Core::InputStream> pInput = getDocumentRenderer()->getVFS()->readFile(fileName);

    try
    {
        Core::String8   str;
        Core::readAllStream(str, *pInput);


        Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getPropertyScript());
        pPtyScript->setScript(str);
    }
    catch(Core::Exception & e)
    {
        String message;
        message << L"An exception has been caught while extracting data from the lua file :\n" << e.getMessage();
        QMessageBox::critical( NULL, "Nyx", Core::String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
Ptr<IDocumentWidget> DocumentScript::createDocumentWidget(QWidget* parent) const
{
    Ptr<IDocumentWidget> pRet = Ptr<IDocumentWidget>(new DocumentScriptWidget(*this, parent));

    connect(this, SIGNAL(worldEditIsEnable()),
        pRet.get(), SLOT(enableWorldEdit()));


    return pRet;
}
//-----------------------------------------------------------------------------
void DocumentScript::swapEditWorld()
{
    if (_isInEditWorld==false)
    {
        _isInEditWorld = true;

        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            connect(const_cast<DocumentBase*>(pADoc.get()), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                    this, SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)));

            Ptr<PtyDocEdition> pPtyADocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (pADoc->getPropertyDocumentEdition());
            Ptr<PropertySelection> pAPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyADocEdit->getPtySelection());
    
            Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (DocumentBase::getPropertyDocumentEdition());
            Ptr<PropertySelectionDual> pPtySelectionDual = LM_DEBUG_PTR_CAST<PropertySelectionDual> (pPtyDocEdit->getPtySelection());
            pPtySelectionDual->setPty2(pAPtySelection);
            pPtySelectionDual->setUsePty1(false);
        }
    }
    else
    {
        _isInEditWorld = false;

        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            connect(const_cast<DocumentBase*>(pADoc.get()), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                    this, SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)));
        }

        Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (DocumentBase::getPropertyDocumentEdition());
        Ptr<PropertySelectionDual> pPtySelectionDual = LM_DEBUG_PTR_CAST<PropertySelectionDual> (pPtyDocEdit->getPtySelection());
        pPtySelectionDual->setPty2(null);
        pPtySelectionDual->setUsePty1(true);
    }

    emit documentContentUpdated();
    emit currentSelectionChanged(getSelectedProperties());
}
//-----------------------------------------------------------------------------
void DocumentScript::saveEditWorld()
{
    Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
    Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

    Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

    if (pADoc!=null)
    {
        pADoc->save();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::deleteSelectedProperties()
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->deleteSelectedProperties();
        }
    }
    else
    {
        DocumentBase::deleteSelectedProperties();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::duplicateSelectedProperties()
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->duplicateSelectedProperties();
        }
    }
    else
    {
        DocumentBase::duplicateSelectedProperties();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::addChildrenToCurrentProperty(Core::List<Ptr<Property> > pties)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->addChildrenToCurrentProperty(pties);
        }
    }
    else
    {
        DocumentBase::addChildrenToCurrentProperty(pties);
    }
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentScript::getPropertyDocumentContent() const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getPropertyDocumentContent();
        }
    }

    return DocumentBase::getPropertyDocumentContent();
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentScript::getPtyForEdition() const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getPtyForEdition();
        }
    }

    return DocumentBase::getPtyForEdition();
}
//-----------------------------------------------------------------------------
void DocumentScript::selectProperty(Ptr<Property> p)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->selectProperty(p);
        }
    }
    else
    {
        DocumentBase::selectProperty(p);
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::selectProperty(int32 id)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->selectProperty(id);
        }
    }
    else
    {
        DocumentBase::selectProperty(id);
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::selectProperties(const Core::List<int32>& id)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->selectProperties(id);
        }
    }
    else
    {
        DocumentBase::selectProperties(id);
    }
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& DocumentScript::getSelectedProperties() const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getSelectedProperties();
        }
    }

    return DocumentBase::getSelectedProperties();
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentScript::getProperty(int32 id)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getProperty(id);
        }
    }

    return DocumentBase::getProperty(id);
}
//-----------------------------------------------------------------------------
void DocumentScript::getProperty(const Core::List<int32>& ids, Core::List<Ptr<Property> >& pties)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->getProperty(ids, pties);
        }
    }
    else
    {
        DocumentBase::getProperty(ids, pties);
    }
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentScript::getDefaultProperty() const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getDefaultProperty();
        }
    }

    return DocumentBase::getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
bool DocumentScript::isEditing()
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->isEditing();
        }
    }

    return DocumentBase::isEditing();
}
//-----------------------------------------------------------------------------
void DocumentScript::setEditing(bool edit)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->setEditing(edit);
        }
    }

    return DocumentBase::setEditing(edit);
}
//-----------------------------------------------------------------------------
int32 DocumentScript::getPropertyIdFromUniverseNode(const Ptr<Universe::Node>& pNode)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getPropertyIdFromUniverseNode(pNode);
        }
    }

    return DocumentBase::getPropertyIdFromUniverseNode(pNode);
}
//-----------------------------------------------------------------------------
QList<QAction*>& DocumentScript::getActionsCreateProperty()
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getActionsCreateProperty();
        }
    }

    return DocumentBase::getActionsCreateProperty();
}
//-----------------------------------------------------------------------------
void DocumentScript::activeProperty(int32 id)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->activeProperty(id);
        }
    }
    else
    {
        DocumentBase::activeProperty(id);
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::contentIsModified()
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->contentIsModified();
        }
    }
    else
    {
        DocumentBase::contentIsModified();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::undo()
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->undo();
        }
    }
    else
    {
        DocumentBase::undo();
    }
}
//-----------------------------------------------------------------------------
void DocumentScript::redo()
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->redo();
        }
    }
    else
    {
        DocumentBase::redo();
    }
}
//-----------------------------------------------------------------------------
const Ptr<StateRecorder>& DocumentScript::getStateRecorder() const
{
   if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getStateRecorder();
        }
    }

    return DocumentBase::getStateRecorder();
}
//-----------------------------------------------------------------------------
const Ptr<DocumentRenderer>& DocumentScript::getDocumentRenderer() const
{
   if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getDocumentRenderer();
        }
    }

    return DocumentBase::getDocumentRenderer();
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> DocumentScript::getWorldDeco()
{
   if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getWorldDeco();
        }
    }

    return DocumentBase::getWorldDeco();
}
//-----------------------------------------------------------------------------
void DocumentScript::setWorldDeco(const Ptr<Universe::World>& pWorld)
{
   if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->setWorldDeco(pWorld);
        }
    }
   else
   {
        DocumentBase::setWorldDeco(pWorld);
   }
}
//-----------------------------------------------------------------------------
const Ptr<GuiDescription>& DocumentScript::getGuiDescription() const
{
   if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getGuiDescription();
        }
    }

    return DocumentBase::getGuiDescription();
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& DocumentScript::getWorldInfoContent() const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getWorldInfoContent();
        }
    }

    return DocumentBase::getWorldInfoContent();
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<PtyNodeCamera> >& DocumentScript::getAllCameraContent() const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getAllCameraContent();
        }
    }

    return DocumentBase::getAllCameraContent();
}
//-----------------------------------------------------------------------------
void DocumentScript::setClosable(bool closable)
{
   if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->setClosable(closable);
        }
    }
   else
   {
        DocumentBase::setClosable(closable);
   }
}
//-----------------------------------------------------------------------------
void DocumentScript::setModified(bool modified)
{
   if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            pADoc->setModified(modified);
        }
    }
   else
   {
        DocumentBase::setModified(modified);
   }
}
//-----------------------------------------------------------------------------
bool DocumentScript::canClose(bool userInteraction) const
{
    Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
    Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

    Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

    if (pADoc!=null)
    {
        bool ADocCanClose = pADoc->canClose(false);

        if (ADocCanClose==false)
        {
            if (userInteraction==true)
            {
                int ret = QMessageBox::warning(NULL, "Close Document Without saving World Edit?","Close Document Without saving World Edit?",
                    QMessageBox::Ok | QMessageBox::No, QMessageBox::No);

                if (ret==QMessageBox::No) return false;
            }
            else
            {
                return false;
            }
        }
    }

    return DocumentBase::canClose(userInteraction);
}
//-----------------------------------------------------------------------------
bool DocumentScript::dropInViewportIsPossible() const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->dropInViewportIsPossible();
        }
    }

    return DocumentBase::dropInViewportIsPossible();
}
//-----------------------------------------------------------------------------
bool DocumentScript::dropThisFormatIsAuthorized(const QMimeData* mimeData, QString& format) const
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->dropThisFormatIsAuthorized(mimeData, format);
        }
    }

    return DocumentBase::dropThisFormatIsAuthorized(mimeData, format);
}
//-----------------------------------------------------------------------------
void DocumentScript::dropThisFile(const QMimeData* mimeData, QString& format)
{
    if (_isInEditWorld==true)
    {
        Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
        Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
        Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->dropThisFile(mimeData, format);
        }
    }

    return DocumentBase::dropThisFile(mimeData, format);
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> DocumentScript::getCameraEdition() const
{
    Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
    Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());


    if (pWorldA->getCameraScript() != null)
    {
        return pWorldA->getCameraScript();
    }

    return this->getGuiDocument().getGuiContext()->getCameraManager()->getDefaultCameraUser();

}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentScript::getActivePlayerPosition()
{
    Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
    Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

    if (pWorldA!=null)
    {
        Ptr<DocumentBase> pADoc = pWorldA->getDocumentForAnimation();

        if (pADoc!=null)
        {
            return pADoc->getActivePlayerPosition();
        }
    }

    return null;
}
//-----------------------------------------------------------------------------
Ptr<IWorldScriptable>  DocumentScript::getWorldScriptable() const
{
    Ptr<PtyDocScript>                  pPtyDocScript  = LM_DEBUG_PTR_CAST<PtyDocScript> (DocumentBase::getPropertyDocumentContent()->getChild(0));
    Ptr<PtyScript>                     pPtyScript     = LM_DEBUG_PTR_CAST<PtyScript> (pPtyDocScript->getPtyScript()); 
    Ptr<WorldScriptableForDocScript>   pWorldA        = LM_DEBUG_PTR_CAST<WorldScriptableForDocScript>(pPtyScript->getWorldScriptable());

    return pWorldA;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}
