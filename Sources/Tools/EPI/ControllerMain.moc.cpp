/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include "ControllerMain.moc.h"

#include <EPI/RendererManager.h>
#include <EPI/DocumentRenderer.h>
#include <EPI/ImportInfo/ImportTextureInfo.h>
#include <EPI/ImportInfo/ImportModelInfo.h>
#include <EPI/ImportInfo/ImportTerrainInfo.h>
#include <EPI/ImportInfo/ImportPicBankInfo.h>
#include <EPI/GUI/GuiDocumentManager.h>
#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/Document/StateRecorder.h>
#include <EPI/Document/PropertySelection.moc.h>
#include <EPI/Gui/Widget/GuiDocumentsSelectorDialog.moc.h>

#include <QMessageBox>
#include <QAction>
#include <QInputDialog>

#include <EPI/Document/Property.moc.h>
#include <Universe/World.h>

#include <EPI/Gui/Viewport/Tools/TransformViewportTool.moc.h>
#include <EPI/Gui/GuiContext.moc.h>
#include <EPI/Document/DocumentWorld.moc.h>
#include <EPI/Gui/Viewport/WorldViewport.moc.h>
#include <EPI/Gui/Widget/OutputWidget.moc.h>
#include <EPI/Document/IWorldScriptable.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
void guiDocumentConfigureAfterCreation(const Ptr<GuiDocument>& pGDoc)
{
    if (pGDoc->getDocument()->getType() == DOC_SCRIPT)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewEdition();
    }
}
//-----------------------------------------------------------------------------
ControllerMain::ControllerMain(const Ptr<GuiDocumentManager>& pGDocMng, const Ptr<RendererManager>& pRdrMng):
    _pGDocMng(pGDocMng),
    _pRdrMng(pRdrMng),
    _makeSleep(true)
{
    _pActionDeleteCurrentPtiesSelected = new QAction(QIcon(":/icons/delete.png"), "Delete", this);
        connect(_pActionDeleteCurrentPtiesSelected, SIGNAL(triggered()), 
                this, SLOT(deleteSelectedPropertiesOnCurrentDocument()));
        _pActionDeleteCurrentPtiesSelected->setShortcuts(QKeySequence::Delete);

    _pActionCopyCurrentPtiesSelected = new QAction(QIcon(":/icons/copy.png"), "Copy", this);
        connect(_pActionCopyCurrentPtiesSelected, SIGNAL(triggered()), 
                this, SLOT(copySelectedPropertiesOnCurrentDocument()));
        _pActionCopyCurrentPtiesSelected->setShortcuts(QKeySequence::Copy);

    _pActionPasteCurrentPtiesSelected = new QAction(QIcon(":/icons/paste.png"), "Paste", this);
        connect(_pActionPasteCurrentPtiesSelected, SIGNAL(triggered()), 
                this, SLOT(pasteOnCurrentDocument()));
        _pActionPasteCurrentPtiesSelected->setShortcuts(QKeySequence::Paste);
        _pActionPasteCurrentPtiesSelected->setDisabled(true);

    _pActionSelectGuiDocumentDialogNext = new QAction("SelectDocument", this);
        connect(_pActionSelectGuiDocumentDialogNext, SIGNAL(triggered()), 
                this, SLOT(selectGuiDocumentDialogNext()));
        _pActionSelectGuiDocumentDialogNext->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_Tab));

    _pActionSelectGuiDocumentDialogPrev = new QAction("SelectDocument", this);
        connect(_pActionSelectGuiDocumentDialogPrev, SIGNAL(triggered()), 
                this, SLOT(selectGuiDocumentDialogPrev()));
        _pActionSelectGuiDocumentDialogPrev->setShortcut(QKeySequence (Qt::CTRL + Qt::SHIFT + Qt::Key_Tab));

    _pActionUndoCurrentGuiDocument = new QAction("Undo", this);
        connect(_pActionUndoCurrentGuiDocument, SIGNAL(triggered()), 
                this, SLOT(undoCurrentGuiDocument()));
        _pActionUndoCurrentGuiDocument->setShortcuts(QKeySequence::Undo);
        
    _pActionRedoCurrentGuiDocument = new QAction("Redo", this);
        connect(_pActionRedoCurrentGuiDocument, SIGNAL(triggered()), 
                this, SLOT(redoCurrentGuiDocument()));
        _pActionRedoCurrentGuiDocument->setShortcuts(QKeySequence::Redo);



    _pActionPlayStopCurrentGuiDocument = new QAction(QIcon(":/icons/play.png"), "Play", this);
        connect(_pActionPlayStopCurrentGuiDocument, SIGNAL(triggered()), 
                this, SLOT(playStopCurrentGuiDocument()));
        //_pActionPlayCurrentGuiDocument->setShortcuts(QKeySequence::Copy);


    _pActionRestartCurrentGuiDocument = new QAction(QIcon(":/icons/restart.png"), "Restart", this);
        connect(_pActionRestartCurrentGuiDocument, SIGNAL(triggered()), 
                this, SLOT(restartCurrentGuiDocument()));
        _pActionRestartCurrentGuiDocument->setShortcut(Qt::Key_F6);

    _pActionStopCurrentGuiDocument = new QAction("Stop", this);
        connect(_pActionStopCurrentGuiDocument, SIGNAL(triggered()), 
                this, SLOT(stopCurrentGuiDocument()));
        _pActionStopCurrentGuiDocument->setShortcut(Qt::Key_F7);

    _pActionExecuteScriptsCurrentGuiDocument = new QAction(QIcon(":/icons/play.png"), "Run", this);
        connect(_pActionExecuteScriptsCurrentGuiDocument, SIGNAL(triggered()), 
                this, SLOT(runStopScriptspCurrentGuiDocument()));
        _pActionExecuteScriptsCurrentGuiDocument->setShortcut(Qt::Key_F5);

    _pActionLogDebugInfo = new QAction("DebugInfo", this);
        connect(_pActionLogDebugInfo, SIGNAL(triggered()), 
                this, SLOT(logDebugInfo()));
        _pActionLogDebugInfo->setShortcut(Qt::Key_F1);
        
    _pActionRenameCurrentPty = new QAction("Rename", this);
        connect(_pActionRenameCurrentPty, SIGNAL(triggered()), 
                this, SLOT(renameCurrentPty()));
        _pActionRenameCurrentPty->setShortcut(Qt::Key_F2);

    _pActionSelectAll = new QAction("Select All", this);
        connect(_pActionSelectAll, SIGNAL(triggered()), 
                this, SLOT(selectAllPty()));
        _pActionSelectAll->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_A));
        

    _pActionDeselectAll = new QAction("Deselect All", this);
        connect(_pActionDeselectAll, SIGNAL(triggered()), 
                this, SLOT(deselectAllPty()));
        _pActionDeselectAll->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_D));

    _pActionSelectMultiTransformVTool = new QAction(QIcon(":/icons/moveRotateScale.png"), "MoveAndRotate", this);
        connect(_pActionSelectMultiTransformVTool, SIGNAL(triggered()), 
                this, SLOT(selectMultiTransformVTool()));
        _pActionSelectMultiTransformVTool->setShortcut(Qt::Key_T);

    _pActionSelectMoveVTool = new QAction(QIcon(":/icons/move.png"), "Move", this);
        connect(_pActionSelectMoveVTool, SIGNAL(triggered()), 
                this, SLOT(selectMoveVTool()));
        _pActionSelectMoveVTool->setShortcut(Qt::Key_Y);

    _pActionSelectRotateVTool = new QAction(QIcon(":/icons/rotate.png"), "Rotate", this);
        connect(_pActionSelectRotateVTool, SIGNAL(triggered()), 
                this, SLOT(selectRotateVTool()));
        _pActionSelectRotateVTool->setShortcut(Qt::Key_U);

    _pActionSelectScaleVTool = new QAction(QIcon(":/icons/scale.png"), "Scale", this);
        connect(_pActionSelectScaleVTool, SIGNAL(triggered()), 
                this, SLOT(selectScaleVTool()));
        _pActionSelectScaleVTool->setShortcut(Qt::Key_I);

    _pActionSelectCopyLocationVTool = new QAction(QIcon(":/icons/copyLocation.png"), "CopyLocation", this);
        connect(_pActionSelectCopyLocationVTool, SIGNAL(triggered()), 
                this, SLOT(selectCopyLocationVTool()));
        _pActionSelectCopyLocationVTool->setShortcut(Qt::Key_O);

    _pActionSelectCopyCameraLocationVTool = new QAction(QIcon(":/icons/copyCameraLocation.png"), "CopyCameraLocation", this);
        connect(_pActionSelectCopyCameraLocationVTool, SIGNAL(triggered()), 
                this, SLOT(selectCopyCameraLocationVTool()));
        _pActionSelectCopyCameraLocationVTool->setShortcut(Qt::Key_G);

    _pActionCopyCameraLocationToCurrentSelection = new QAction(QIcon(":/icons/copyCameraLocation.png"), "CopyCameraLocationToCurrentSelection", this);
        connect(_pActionCopyCameraLocationToCurrentSelection, SIGNAL(triggered()), 
                this, SLOT(copyCameraLocationToCurrentSelection()));
        _pActionCopyCameraLocationToCurrentSelection->setShortcut(Qt::CTRL + Qt::Key_G);

    _pActionEngageCopyCameraLocationAttach = new QAction(QIcon(":/icons/copyCameraLocation.png"), "engageCopyCameraLocationAttach", this);
        connect(_pActionEngageCopyCameraLocationAttach, SIGNAL(triggered()), 
                this, SLOT(engageCameraLocationAttach()));
        _pActionEngageCopyCameraLocationAttach->setShortcut(Qt::SHIFT + Qt::Key_G);

    _pActionEscape = new QAction("Escape", this);
        connect(_pActionEscape, SIGNAL(triggered()), 
                this, SLOT(escape()));
        _pActionEscape->setShortcut(Qt::Key_Escape);

    _pActionGetMemoCam1 = new QAction("GetMemoCam1", this);
        connect(_pActionGetMemoCam1, SIGNAL(triggered()), 
                this, SLOT(getMemoCam1()));

    _pActionGetMemoCam2 = new QAction("GetMemoCam2", this);
        connect(_pActionGetMemoCam2, SIGNAL(triggered()), 
                this, SLOT(getMemoCam2()));

    _pActionGetMemoCam3 = new QAction("GetMemoCam3", this);
        connect(_pActionGetMemoCam3, SIGNAL(triggered()), 
                this, SLOT(getMemoCam3()));

    _pActionGetMemoCam4 = new QAction("GetMemoCam4", this);
        connect(_pActionGetMemoCam4, SIGNAL(triggered()), 
                this, SLOT(getMemoCam4()));

    _pActionGetMemoCam5 = new QAction("GetMemoCam5", this);
        connect(_pActionGetMemoCam5, SIGNAL(triggered()), 
                this, SLOT(getMemoCam5()));

    _pActionSetMemoCam1 = new QAction("SetMemoCam1", this);
        connect(_pActionSetMemoCam1, SIGNAL(triggered()), 
                this, SLOT(setMemoCam1()));

    _pActionSetMemoCam2 = new QAction("SetMemoCam2", this);
        connect(_pActionSetMemoCam2, SIGNAL(triggered()), 
                this, SLOT(setMemoCam2()));

    _pActionSetMemoCam3 = new QAction("SetMemoCam3", this);
        connect(_pActionSetMemoCam3, SIGNAL(triggered()), 
                this, SLOT(setMemoCam3()));

    _pActionSetMemoCam4 = new QAction("SetMemoCam4", this);
        connect(_pActionSetMemoCam4, SIGNAL(triggered()), 
                this, SLOT(setMemoCam4()));

    _pActionSetMemoCam5 = new QAction("SetMemoCam5", this);
        connect(_pActionSetMemoCam5, SIGNAL(triggered()), 
                this, SLOT(setMemoCam5()));

    _pActionFreeAspectRatio = new QAction("FreeAspect", this);
        connect(_pActionFreeAspectRatio, SIGNAL(triggered()), 
                this, SLOT(setFreeAspectRatio()));
        _pActionFreeAspectRatio->setShortcut(Qt::Key_Minus);

    _pAction169AspectRatio = new QAction("16_9_Aspect", this);
        connect(_pAction169AspectRatio, SIGNAL(triggered()), 
                this, SLOT(set169AspectRatio()));
        _pAction169AspectRatio->setShortcut(Qt::Key_Asterisk);

    _pAction43AspectRatio = new QAction("4_3_Aspect", this);
        connect(_pAction43AspectRatio, SIGNAL(triggered()), 
                this, SLOT(set43AspectRatio()));
        _pAction43AspectRatio->setShortcut(Qt::Key_Slash);


    _pActionViewUser = new QAction("View User", this);
        connect(_pActionViewUser, SIGNAL(triggered()), 
                this, SLOT(setViewUser()));
        _pActionViewUser->setShortcut(QKeySequence("."));

    _pActionViewTop = new QAction("View Top", this);
        connect(_pActionViewTop, SIGNAL(triggered()), 
                this, SLOT(setViewTop()));

    _pActionViewBottom = new QAction("View Bottom", this);
        connect(_pActionViewBottom, SIGNAL(triggered()), 
                this, SLOT(setViewBottom()));

    _pActionViewLeft = new QAction("View Left", this);
        connect(_pActionViewLeft, SIGNAL(triggered()), 
                this, SLOT(setViewLeft()));

    _pActionViewRight = new QAction("View Right", this);
        connect(_pActionViewRight, SIGNAL(triggered()), 
                this, SLOT(setViewRight()));

    _pActionViewFront = new QAction("View Front", this);
        connect(_pActionViewFront, SIGNAL(triggered()), 
                this, SLOT(setViewFront()));

    _pActionViewBack = new QAction("View Back", this);
        connect(_pActionViewBack, SIGNAL(triggered()), 
                this, SLOT(setViewBack()));

    _pActionViewSolid = new QAction("View Solid", this);
        connect(_pActionViewSolid, SIGNAL(triggered()), 
                this, SLOT(setViewSolid()));

    _pActionViewWireframe = new QAction("View Wireframe", this);
        connect(_pActionViewWireframe, SIGNAL(triggered()), 
                this, SLOT(setViewWireframe()));

    _pActionViewWireframeSolid = new QAction("View WireframeSolid", this);
        connect(_pActionViewWireframeSolid, SIGNAL(triggered()), 
                this, SLOT(setViewWireframeSolid()));

    _pActionViewDefault = new QAction("View Default", this);
        connect(_pActionViewDefault, SIGNAL(triggered()), 
                this, SLOT(setViewDefault()));

    _pActionViewNormal = new QAction("View Normal", this);
        connect(_pActionViewNormal, SIGNAL(triggered()), 
                this, SLOT(setViewNormal()));

    _pActionViewUV = new QAction("View UV", this);
        connect(_pActionViewUV, SIGNAL(triggered()), 
                this, SLOT(setViewUV()));


    _pActionCurrentViewportActiveDesactiveShowIHM = new QAction("Show IHM", this);
        connect(_pActionCurrentViewportActiveDesactiveShowIHM, SIGNAL(triggered()), 
                this, SLOT(setCurrentViewportToggleShowIHM()));
        _pActionCurrentViewportActiveDesactiveShowIHM->setShortcut(Qt::Key_H);

    _pOutputWidget = new OutputWidget(NULL);
}
//-----------------------------------------------------------------------------
ControllerMain::~ControllerMain()
{
    delete _pActionDeleteCurrentPtiesSelected;
    delete _pActionCopyCurrentPtiesSelected;
    delete _pActionPasteCurrentPtiesSelected;
    delete _pActionSelectGuiDocumentDialogNext;
    delete _pActionUndoCurrentGuiDocument;
    delete _pActionRedoCurrentGuiDocument;
    delete _pActionPlayStopCurrentGuiDocument;


    if (_pOutputWidget!=NULL)
    {
        if (_pOutputWidget->parent()==NULL)
        {
            delete _pOutputWidget;
        }
    }


    _pGDocMng = null;
    _pRdrMng  = null;
}
//-----------------------------------------------------------------------------
void ControllerMain::launchApplication()
{
    if (_pGDocMng->isEmpty()==true)
    {
        emit signalNoGuiDocument();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::newGuiDocument(const DocumentType type, const Core::String& title, const Ptr<DocumentRenderer>& docRdr)
{
    if (_pGDocMng->isEmpty()==true)
    {
        emit signalFirstGuiDocument();
    }

    Ptr<DocumentRenderer> pDocumentRenderer;
    if (docRdr == null)
    {
        pDocumentRenderer = _pRdrMng->getDefaultRenderer();
    }
    else
    {
        pDocumentRenderer = docRdr;
    }
    const Ptr<GuiDocument>& pGDoc = _pGDocMng->createGuiDocument(type, title, pDocumentRenderer);
    pGDoc->setMakeSleep(_makeSleep);

    connect(pGDoc.get(), SIGNAL(generate(const Ptr<ImportInfo> &)), this, SLOT(generateInfo(const Ptr<ImportInfo> &)));
    emit signalNewGuiDocument(pGDoc);


    pGDoc->getDocument()->getStateRecorder()->disable();
    pGDoc->getDocument()->selectProperty(pGDoc->getDocument()->getDefaultProperty());
    activeGuiDocument(pGDoc->getID());
    pGDoc->zoomExtent();

    guiDocumentConfigureAfterCreation(pGDoc);

    Ptr<IWorldScriptable> pWorldScript = pGDoc->getDocument()->getWorldScriptable();
    if (pWorldScript != null)
    {
        getOutputWidget()->connectLuaOutput(*pWorldScript->getLuaOutput());
    }

    pGDoc->getDocument()->getStateRecorder()->enable();
}
//-----------------------------------------------------------------------------
void ControllerMain::loadGuiDocument(const Ptr<ImportInfo> & pImportInfo)
{
    try
    {
        bool alreadyOpened = false;

        


        const Ptr<GuiDocument>& pGDoc = _pGDocMng->loadGuiDocument(pImportInfo, alreadyOpened, _pRdrMng->getDefaultRenderer());
        pImportInfo->gDocId = pGDoc->getID();

        pGDoc->setMakeSleep(_makeSleep);

        //On a crÈÈ le premier document ?
        if (_pGDocMng->getAllGuiDocument().size() == 1)
        {
            emit signalFirstGuiDocument();
        }

        if(!alreadyOpened)
        {
            connect(pGDoc.get(), SIGNAL(generate(const Ptr<ImportInfo> &)), this, SLOT(generateInfo(const Ptr<ImportInfo> &)));
            emit signalNewGuiDocument(pGDoc);
        }

        pGDoc->getDocument()->getStateRecorder()->disable();
        pGDoc->getDocument()->selectProperty(pGDoc->getDocument()->getDefaultProperty());
        activeGuiDocument(pGDoc->getID());
        pGDoc->zoomExtent();
 
        guiDocumentConfigureAfterCreation(pGDoc);

        Ptr<IWorldScriptable> pWorldScript = pGDoc->getDocument()->getWorldScriptable();
        if (pWorldScript != null)
        {
            getOutputWidget()->connectLuaOutput(*pWorldScript->getLuaOutput());
        }

        pGDoc->getDocument()->getStateRecorder()->enable();

    }
    catch(Core::Exception & e)
    {
        Core::String message;
        message << L"An error occured while loading a document :\n" <<e.getMessage();
        QMessageBox::critical(NULL, "Nyx", Core::String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::deleteCurrentGuiDocument()
{
    deleteGuiDocument(_pGDocMng->getCurrentGuiDocumentId());
}
//-----------------------------------------------------------------------------
void ControllerMain::deleteGuiDocument(int id)
{
    Ptr<StateRecorder> test = _pGDocMng->getGuiDocument(id)->getDocument()->getStateRecorder();

    if (id!=-1)
    {
        int32             oldCurrentGDocId = _pGDocMng->getCurrentGuiDocumentId();
        Ptr<GuiDocument>  pGDoc            = _pGDocMng->getGuiDocument(id);

        if (pGDoc->canClose(true) == true)
        {
            _pGDocMng->destroyGuiDocument(id);

            if (pGDoc->getDocument()->isEditing() == false)
            {
                pGDoc->getDocument()->runStopScript();
            }

            
            emit signalDeleteGuiDocument(pGDoc);
            pGDoc = null;

            //si le document supprime etait le document courrant on signal que le document courant a change
            if (oldCurrentGDocId != _pGDocMng->getCurrentGuiDocumentId())
            {
                activeGuiDocument(_pGDocMng->getCurrentGuiDocumentId());
            }

            if(_pGDocMng->isEmpty()==true)
            {
                emit signalNoGuiDocument();
            }
            
        }
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::activeGuiDocument(int32 id)
{
    const Ptr<GuiDocument>& pGDoc = _pGDocMng->selectGuiDocument(id);
    emit signalActivatedGuiDocument(pGDoc);

    if (pGDoc!=null)
    {
        //pGDoc->getDocument()->selectProperty((pGDoc->getDocument()->getCurrentProperties())); //TODO

        pGDoc->setViewportTool(pGDoc->getViewportTool());
        if (pGDoc->getDocument()->getType() == DOC_SPLINE)
        {
            _pActionDeleteCurrentPtiesSelected->setShortcuts(QKeySequence::UnknownKey);
        }
        else
        {
            _pActionDeleteCurrentPtiesSelected->setShortcuts(QKeySequence::Delete);
        }
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::undoCurrentGuiDocument()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->undo();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::redoCurrentGuiDocument()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->redo();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setTextInfo(const String& str)
{
    emit signalSetTextInfo(str);
}
//-----------------------------------------------------------------------------
void ControllerMain::generateInfo(const Ptr<ImportInfo> & pImportInfo)
{
    emit generate(pImportInfo);
}
//-----------------------------------------------------------------------------
void ControllerMain::generationDone(const Ptr<ImportInfo> & pImportInfo)
{
    if(pImportInfo->assetType == TEXTURE_ASSET)
        reloadTexture(pImportInfo);
    else if(pImportInfo->assetType == MODEL_ASSET)
        reloadModel(pImportInfo);
    else if(pImportInfo->assetType == TERRAIN_ASSET)
        reloadTerrain(pImportInfo);
}
//-----------------------------------------------------------------------------
void ControllerMain::reloadTexture(const Ptr<ImportInfo> & pImportInfo)
{
    Ptr<ImportTextureInfo> pImportTextureInfo = LM_DEBUG_PTR_CAST<ImportTextureInfo>(pImportInfo);
    
    Core::List<Ptr<GuiDocument> > guiDocuments = _pGDocMng->getAllGuiDocument();

    Core::List<Ptr<GuiDocument> >::iterator iGuiDocument = guiDocuments.begin();
    while(iGuiDocument != guiDocuments.end())
    {
        Ptr<DocumentBase> pDocument = (*iGuiDocument)->getDocument();
        Ptr<Universe::World> pWorld = pDocument->getWorld();
        Ptr<Universe::RessourcePool> pRessourcePool = pWorld->getRessourcesPool();
        pRessourcePool->reloadTexture(pImportTextureInfo->textureName);

        ++iGuiDocument;
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::reloadModel(const Ptr<ImportInfo> & pImportInfo)
{
    Ptr<ImportModelInfo> pImportModelInfo = LM_DEBUG_PTR_CAST<ImportModelInfo>(pImportInfo);
    
    Core::List<Ptr<GuiDocument> > guiDocuments = _pGDocMng->getAllGuiDocument();

    Core::List<Ptr<GuiDocument> >::iterator iGuiDocument = guiDocuments.begin();
    while(iGuiDocument != guiDocuments.end())
    {
        Ptr<DocumentBase> pDocument = (*iGuiDocument)->getDocument();
        Ptr<Universe::World> pWorld = pDocument->getWorld();
        Ptr<Universe::RessourcePool> pRessourcePool = pWorld->getRessourcesPool();
        pRessourcePool->reloadModel(pImportModelInfo->modelName);

        ++iGuiDocument;
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::reloadTerrain(const Ptr<ImportInfo> & pImportInfo)
{
    Ptr<ImportTerrainInfo> pImportTerrainInfo = LM_DEBUG_PTR_CAST<ImportTerrainInfo>(pImportInfo);
    
    Core::List<Ptr<GuiDocument> > guiDocuments = _pGDocMng->getAllGuiDocument();

    Core::List<Ptr<GuiDocument> >::iterator iGuiDocument = guiDocuments.begin();
    while(iGuiDocument != guiDocuments.end())
    {
        Ptr<DocumentBase> pDocument = (*iGuiDocument)->getDocument();
        Ptr<Universe::World> pWorld = pDocument->getWorld();
        Ptr<Universe::RessourcePool> pRessourcePool = pWorld->getRessourcesPool();
        pRessourcePool->reloadTerrainModel(pImportTerrainInfo->terrainModelName);

        ++iGuiDocument;
    }
}
//-----------------------------------------------------------------------------
bool ControllerMain::canCloseAllDocument() const
{
    bool ret = true;

    Core::List<Ptr<GuiDocument> > guiDocuments = _pGDocMng->getAllGuiDocument();

    Core::List<Ptr<GuiDocument> >::iterator iGuiDocument = guiDocuments.begin();
    while(iGuiDocument != guiDocuments.end())
    {
        if ((*iGuiDocument)->canClose(false) == false)
        {
            ret = false;
            break;
        }
        ++iGuiDocument;
    }

    return ret;
}
//-----------------------------------------------------------------------------
void ControllerMain::saveCurrentDocument() const
{
    _pGDocMng->getCurrentGuiDocument()->getDocument()->save();
    emit documentSaved();
}
//-----------------------------------------------------------------------------
void ControllerMain::saveAsCurrentDocument() const
{
    _pGDocMng->getCurrentGuiDocument()->getDocument()->saveAs();
    emit documentSaved();
}
//-----------------------------------------------------------------------------
void ControllerMain::deleteSelectedPropertiesOnCurrentDocument()
{
    Ptr<GuiDocument> pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->deleteSelectedProperties();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectGuiDocumentDialogNext()
{
    const Core::List<Ptr<GuiDocument> >& docs = _pGDocMng->getAllGuiDocument();

    if (docs.size() > 1)
    {
        GuiDocumentsSelectorDialog  dlg (_pGDocMng, true);
            dlg.exec();

        int32 idDoc = dlg.getIdGDocSelected();
        if (idDoc>=0)
        {
            activeGuiDocument(idDoc);
        }
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectGuiDocumentDialogPrev()
{
    const Core::List<Ptr<GuiDocument> >& docs = _pGDocMng->getAllGuiDocument();

    if (docs.size() > 1)
    {
        GuiDocumentsSelectorDialog  dlg (_pGDocMng, false);
            dlg.exec();

        int32 idDoc = dlg.getIdGDocSelected();
        if (idDoc>=0)
        {
            activeGuiDocument(idDoc);
        }
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::copySelectedPropertiesOnCurrentDocument()
{
    _ptiesCopy.clear();
    

    Ptr<GuiDocument> pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        Core::List<Ptr<Property> > ptiesSelected = pGDoc->getDocument()->getSelectedProperties();
        ptiesSelected = removePropertyChildren(ptiesSelected);

        for(int32 ii=0; ii<ptiesSelected.size(); ++ii)
        {
            Ptr<Property> pPtyClone;
            cloneWithChild(pPtyClone, *ptiesSelected[ii], false);
            _ptiesCopy.push_back(pPtyClone);
        }

        if (_ptiesCopy.size() > 0)
            _pActionPasteCurrentPtiesSelected->setDisabled(false);
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::pasteOnCurrentDocument()
{
    Ptr<GuiDocument> pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null && _ptiesCopy.size()>0)
    {
        pGDoc->getDocument()->addChildrenToCurrentProperty(_ptiesCopy);
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::playStopCurrentGuiDocument()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        if (pGDoc->isAnimated() == true)
        {
            pGDoc->startAnimate();
        }
        else
        {
            pGDoc->stopAnimate();
        }
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::runStopScriptspCurrentGuiDocument()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->runStopScript();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::logDebugInfo()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        /*
        Ptr<StateRecorder> pStateRecord = pGDoc->getDocument()->getStateRecorder();

        const Core::List<Ptr<StateChangeSet> >&  states = pStateRecord->getStates();

        String str;
        for (int32 ii=0; ii<states.size(); ++ii)
        {
            str << states[ii]->toString() << L"\n";
        }
        signalSetTextInfo(str);
*/
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectMultiTransformVTool()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->selectMultiTransformVTool();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectMoveVTool()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->selectMoveVTool();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectRotateVTool()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->selectRotateVTool();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectScaleVTool()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->selectScaleVTool();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectCopyLocationVTool()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->selectCopyLocationVTool();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectCopyCameraLocationVTool()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->selectCopyCameraLocationVTool();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::copyCameraLocationToCurrentSelection()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->copyCameraLocationToCurrentSelection();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::engageCameraLocationAttach()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->engageCameraLocationAttach();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::escape()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->escape();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::renameCurrentPty()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->renameCurrentProperty();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::selectAllPty()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->selectAllProperties();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::deselectAllPty()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getDocument()->deselectAllProperties();
    }    
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewSolid()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportPolygonModeSolid();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewWireframe()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportPolygonModeWireframe();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewWireframeSolid()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportPolygonModeSolidWireframe();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewDefault()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportRenderModeNormal();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewNormal()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportRenderModeDebugNormal();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewUV()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportRenderModeUV();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setCurrentViewportToggleShowIHM()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->activeDesactiveIHM(); 
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setMemoCamera(const EMemoCameraID& id)
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null && pGDoc->getDocument()->getType() == DOC_WORLD)
    {
        Ptr<DocumentWorld> pDocWorld = LM_DEBUG_PTR_CAST<DocumentWorld>(pGDoc->getDocument());
        WorldViewport* pViewport = pGDoc->getGuiContext()->getCurrentViewport()->getView();

        if (pViewport!=NULL)
        {
            Ptr<Universe::NodeCamera> pCam = pViewport->getNodeCamera();
            if (pCam->getBaseCamera()->getCameraType() == Renderer::CAMERA_PERSPECTIVE)
                pDocWorld->setMemoCamera(pCam, id);
        }
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setMemoCam1()
{
    setMemoCamera(MEMO_CAM_1);
}
//-----------------------------------------------------------------------------
void ControllerMain::setMemoCam2()
{
    setMemoCamera(MEMO_CAM_2);
}
//-----------------------------------------------------------------------------
void ControllerMain::setMemoCam3()
{
    setMemoCamera(MEMO_CAM_3);
}
//-----------------------------------------------------------------------------
void ControllerMain::setMemoCam4()
{
    setMemoCamera(MEMO_CAM_4);
}
//-----------------------------------------------------------------------------
void ControllerMain::setMemoCam5()
{
    setMemoCamera(MEMO_CAM_5);
}
//-----------------------------------------------------------------------------
void ControllerMain::setFreeAspectRatio()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraAspectRatioFree();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::set169AspectRatio()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraAspectRatio_16_9();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::set43AspectRatio()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraAspectRatio_4_3();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewUser()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewUser();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewTop()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewTop();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewBottom()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewBottom();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewLeft()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewLeft();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewRight()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewRight();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewFront()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewFront();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::setViewBack()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->getGuiContext()->setCurrentViewportCameraViewBack();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::restartCurrentGuiDocument()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->restart();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::stopCurrentGuiDocument()
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        pGDoc->stop();
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::getMemoCamera(const EMemoCameraID& id)
{
    Ptr<GuiDocument>& pGDoc = _pGDocMng->getCurrentGuiDocument();

    if (pGDoc!=null && pGDoc->getDocument()->getType() == DOC_WORLD)
    {
        Ptr<DocumentWorld> pDocWorld = LM_DEBUG_PTR_CAST<DocumentWorld>(pGDoc->getDocument());
        WorldViewport* pViewport = pGDoc->getGuiContext()->getCurrentViewport()->getView();

        if (pViewport!=NULL)
        {
            Ptr<Universe::NodeCamera> pCam = pViewport->getNodeCamera();

            if (pCam->getBaseCamera()->getCameraType() == Renderer::CAMERA_PERSPECTIVE)
                pDocWorld->getMemoCamera(pCam, id);
        }
    }
}
//-----------------------------------------------------------------------------
void ControllerMain::getMemoCam1()
{
    getMemoCamera(MEMO_CAM_1);
}
//-----------------------------------------------------------------------------
void ControllerMain::getMemoCam2()
{
    getMemoCamera(MEMO_CAM_2);
}
//-----------------------------------------------------------------------------
void ControllerMain::getMemoCam3()
{
    getMemoCamera(MEMO_CAM_3);
}
//-----------------------------------------------------------------------------
void ControllerMain::getMemoCam4()
{
    getMemoCamera(MEMO_CAM_4);
}
//-----------------------------------------------------------------------------
void ControllerMain::getMemoCam5()
{
    getMemoCamera(MEMO_CAM_5);
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionDeleteCurrentPties() const
{
    return _pActionDeleteCurrentPtiesSelected;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionCopyCurrentPties() const
{
    return _pActionCopyCurrentPtiesSelected;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionPastePtiesCopy() const
{
    return _pActionPasteCurrentPtiesSelected;
}
//-----------------------------------------------------------------------------
QAction*    ControllerMain::getActionRenameCurrentPty() const
{
    return _pActionRenameCurrentPty;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSelectGuiDocumentDialogNext() const
{
    return _pActionSelectGuiDocumentDialogNext;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSelectGuiDocumentDialogPrev() const
{
    return _pActionSelectGuiDocumentDialogPrev;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionUndoCurrentGuiDocument() const
{
    return _pActionUndoCurrentGuiDocument;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionRedoCurrentGuiDocument() const
{
    return _pActionRedoCurrentGuiDocument;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionPlayStopCurrentGuiDocument() const
{
    return _pActionPlayStopCurrentGuiDocument;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionExecuteScriptsCurrentGuiDocument() const
{
    return _pActionExecuteScriptsCurrentGuiDocument;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionLogDebugInfo() const
{
    return _pActionLogDebugInfo;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSelectMultiTransformVTool() const
{
    return _pActionSelectMultiTransformVTool;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSelectMoveVTool() const
{
    return _pActionSelectMoveVTool;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSelectRotateVTool() const
{
    return _pActionSelectRotateVTool;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSelectScaleVTool() const
{
    return _pActionSelectScaleVTool;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionEscape() const
{
    return _pActionEscape;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionGetMemoCam1() const
{
    return _pActionGetMemoCam1;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionGetMemoCam2() const
{
    return _pActionGetMemoCam2;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionGetMemoCam3() const
{
    return _pActionGetMemoCam3;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionGetMemoCam4() const
{
    return _pActionGetMemoCam4;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionGetMemoCam5() const
{
    return _pActionGetMemoCam5;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSetMemoCam1() const
{
    return _pActionSetMemoCam1;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSetMemoCam2() const
{
    return _pActionSetMemoCam2;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSetMemoCam3() const
{
    return _pActionSetMemoCam3;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSetMemoCam4() const
{
    return _pActionSetMemoCam4;
}
//-----------------------------------------------------------------------------
QAction* ControllerMain::getActionSetMemoCam5() const
{
    return _pActionSetMemoCam5;
}
//-----------------------------------------------------------------------------
void ControllerMain::setMakeSleepForGDoc(bool make)
{
    _makeSleep = make;

    Core::List<Ptr<GuiDocument> >& gdocs = getDocumentManager()->getAllGuiDocument();

    for (int32 ii=0; ii<gdocs.size(); ++ii)
    {
        gdocs[ii]->setMakeSleep(_makeSleep);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
