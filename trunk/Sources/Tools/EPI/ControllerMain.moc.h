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
#ifndef CONTROLLER_MAIN_H_
#define CONTROLLER_MAIN_H_

#include <QObject>

#include <Core/Standard.h>
#include <Core/List.h>

#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/DocumentRenderer.h>
#include <EPI/Gui/GuiDocumentManager.h>

namespace EPI
{
//-----------------------------------------------------------------------------
class GuiDocumentManager;
class RendererManager;
class DocumentRenderer;
class GuiDocument;
class ApplicationWindowBase;
enum EMemoCameraID;
class OutputWidget;
//-----------------------------------------------------------------------------
class LM_API_EPI ControllerMain : public QObject, public Core::Object
{
    Q_OBJECT

public:
    ControllerMain(const Ptr<GuiDocumentManager>& pGDocMng, const Ptr<RendererManager>& pRdrMng);
    virtual ~ControllerMain();
    void launchApplication();

    inline Ptr<RendererManager> & getRendererManager() { return _pRdrMng; }

    OutputWidget*  getOutputWidget() const {return _pOutputWidget;}

    bool canCloseAllDocument() const;

    void saveCurrentDocument() const;
    void saveAsCurrentDocument() const;

    QAction*    getActionDeleteCurrentPties() const;
    QAction*    getActionCopyCurrentPties() const;
    QAction*    getActionPastePtiesCopy() const;
    QAction*    getActionRenameCurrentPty() const;
    QAction*    getActionSelectGuiDocumentDialogNext() const;
    QAction*    getActionSelectGuiDocumentDialogPrev() const;
    QAction*    getActionUndoCurrentGuiDocument() const;
    QAction*    getActionRedoCurrentGuiDocument() const;
    QAction*    getActionPlayStopCurrentGuiDocument() const;
    QAction*    getActionRestartCurrentGuiDocument() const {return _pActionRestartCurrentGuiDocument;}
    QAction*    getActionStopCurrentGuiDocument() const {return _pActionStopCurrentGuiDocument;}
    QAction*    getActionExecuteScriptsCurrentGuiDocument() const;
    QAction*    getActionLogDebugInfo() const;
    QAction*    getActionSelectMultiTransformVTool() const;
    QAction*    getActionSelectMoveVTool() const;
    QAction*    getActionSelectRotateVTool() const;
    QAction*    getActionSelectScaleVTool() const;
    QAction*    getActionSelectCopyLocationVTool() const {return _pActionSelectCopyLocationVTool;}
    QAction*    getActionSelectCopyCameraLocationVTool() const {return _pActionSelectCopyCameraLocationVTool;}
    QAction*    getActionCopyCameraLocationToCurrentSelection() const {return _pActionCopyCameraLocationToCurrentSelection;}
    QAction*    getActionEngageCopyCameraLocationAttach() const {return _pActionEngageCopyCameraLocationAttach;}
    QAction*    getActionEscape() const;
    QAction*    getActionGetMemoCam1() const;
    QAction*    getActionGetMemoCam2() const;
    QAction*    getActionGetMemoCam3() const;
    QAction*    getActionGetMemoCam4() const;
    QAction*    getActionGetMemoCam5() const;
    QAction*    getActionSetMemoCam1() const;
    QAction*    getActionSetMemoCam2() const;
    QAction*    getActionSetMemoCam3() const;
    QAction*    getActionSetMemoCam4() const;
    QAction*    getActionSetMemoCam5() const;
    QAction*    getActionFreeAspectRatio() const {return _pActionFreeAspectRatio;}
    QAction*    getAction169AspectRatio() const  {return _pAction169AspectRatio;}
    QAction*    getAction43AspectRatio() const   {return _pAction43AspectRatio;}
    QAction*    getActionViewUser() const   {return _pActionViewUser;}
    QAction*    getActionViewTop() const   {return _pActionViewTop;}
    QAction*    getActionViewBottom() const   {return _pActionViewBottom;}
    QAction*    getActionViewLeft() const   {return _pActionViewLeft;}
    QAction*    getActionViewRight() const   {return _pActionViewRight;}
    QAction*    getActionViewFront() const   {return _pActionViewFront;}
    QAction*    getActionViewBack() const   {return _pActionViewBack;}
    QAction*    getActionViewSolid() const          {return _pActionViewSolid;}
    QAction*    getActionViewWireframe() const      {return _pActionViewWireframe;}
    QAction*    getActionViewWireframeSolid() const {return _pActionViewWireframeSolid;}
    
    QAction*    getActionViewDefault() const        {return _pActionViewDefault;}
    QAction*    getActionViewNormal() const         {return _pActionViewNormal;}
    QAction*    getActionViewUV() const             {return _pActionViewUV;}
    QAction*    getActionSelectAll() const             {return _pActionSelectAll;}
    QAction*    getActionDeselectAll() const             {return _pActionDeselectAll;}
    QAction*    getActionToggleShowIHM() const             {return _pActionCurrentViewportActiveDesactiveShowIHM;}
    

    Ptr<GuiDocumentManager> getDocumentManager() const {return _pGDocMng;}
    void setMakeSleepForGDoc(bool make);
private:
    void setMemoCamera(const EMemoCameraID& id);
    void getMemoCamera(const EMemoCameraID& id);

public Q_SLOTS:
    void newGuiDocument(const DocumentType type, const Core::String& title=L"", const Ptr<DocumentRenderer>& docRdr=null);
    void loadGuiDocument(const Ptr<ImportInfo> & pImportInfo);
    void deleteCurrentGuiDocument();
    void deleteGuiDocument(int32 id);
    void activeGuiDocument(int32 id);
    void setTextInfo(const String& str);

    void generateInfo(const Ptr<ImportInfo> & pImportInfo);
    void generationDone(const Ptr<ImportInfo> & pImportInfo);

Q_SIGNALS:
    ///
    void signalNewGuiDocument(const Ptr<GuiDocument>& pGDoc);
    void signalDeleteGuiDocument(const Ptr<GuiDocument>& pGDoc);

    void signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc);

    void signalNoGuiDocument();
    void signalFirstGuiDocument();

    void signalSetTextInfo(const String& str);

    void generate(const Ptr<ImportInfo> & pImportInfo);

    void documentSaved() const;
    

private Q_SLOTS:
    void deleteSelectedPropertiesOnCurrentDocument();
    void copySelectedPropertiesOnCurrentDocument();
    void pasteOnCurrentDocument();
    void undoCurrentGuiDocument();
    void redoCurrentGuiDocument();
    void playStopCurrentGuiDocument();
    void selectGuiDocumentDialogNext();
    void selectGuiDocumentDialogPrev();
    void runStopScriptspCurrentGuiDocument();
    void logDebugInfo();
    void selectMultiTransformVTool();
    void selectMoveVTool();
    void selectRotateVTool();
    void selectScaleVTool();
    void selectCopyLocationVTool();
    void selectCopyCameraLocationVTool();
    void copyCameraLocationToCurrentSelection();
    void engageCameraLocationAttach();
    void escape();
    void renameCurrentPty();
    void setMemoCam1();
    void setMemoCam2();
    void setMemoCam3();
    void setMemoCam4();
    void setMemoCam5();
    void getMemoCam1();
    void getMemoCam2();
    void getMemoCam3();
    void getMemoCam4();
    void getMemoCam5();

    void setFreeAspectRatio();
    void set169AspectRatio();
    void set43AspectRatio();

    void setViewUser();
    void setViewTop();
    void setViewBottom();
    void setViewLeft();
    void setViewRight();
    void setViewFront();
    void setViewBack();

    void setViewSolid();
    void setViewWireframe();
    void setViewWireframeSolid();
    void setViewDefault();
    void setViewNormal();
    void setViewUV();

    void restartCurrentGuiDocument();
    void stopCurrentGuiDocument();

    void deselectAllPty();
    void selectAllPty();

    void setCurrentViewportToggleShowIHM();

private:
    void reloadTexture(const Ptr<ImportInfo> & pImportInfo);
    void reloadModel(const Ptr<ImportInfo> & pImportInfo);
    void reloadTerrain(const Ptr<ImportInfo> & pImportInfo);

private:
    Ptr<GuiDocumentManager>         _pGDocMng;
    Ptr<RendererManager>            _pRdrMng;

    QAction*                        _pActionDeleteCurrentPtiesSelected;
    QAction*                        _pActionCopyCurrentPtiesSelected;
    QAction*                        _pActionPasteCurrentPtiesSelected;
    QAction*                        _pActionSelectGuiDocumentDialogNext;
    QAction*                        _pActionSelectGuiDocumentDialogPrev;
    QAction*                        _pActionUndoCurrentGuiDocument;
    QAction*                        _pActionRedoCurrentGuiDocument;
    QAction*                        _pActionPlayStopCurrentGuiDocument;
    QAction*                        _pActionExecuteScriptsCurrentGuiDocument;
    QAction*                        _pActionRestartCurrentGuiDocument;
    QAction*                        _pActionStopCurrentGuiDocument;
    QAction*                        _pActionRenameCurrentPty;

    QAction*                        _pActionSelectMultiTransformVTool;
    QAction*                        _pActionSelectMoveVTool;
    QAction*                        _pActionSelectRotateVTool;
    QAction*                        _pActionSelectScaleVTool;
    QAction*                        _pActionSelectCopyLocationVTool;
    QAction*                        _pActionSelectCopyCameraLocationVTool;
    QAction*                        _pActionCopyCameraLocationToCurrentSelection;
    QAction*                        _pActionEngageCopyCameraLocationAttach;
    QAction*                        _pActionEscape;

    QAction*                        _pActionSetMemoCam1;
    QAction*                        _pActionSetMemoCam2;
    QAction*                        _pActionSetMemoCam3;
    QAction*                        _pActionSetMemoCam4;
    QAction*                        _pActionSetMemoCam5;
    QAction*                        _pActionGetMemoCam1;
    QAction*                        _pActionGetMemoCam2;
    QAction*                        _pActionGetMemoCam3;
    QAction*                        _pActionGetMemoCam4;
    QAction*                        _pActionGetMemoCam5;

    QAction*                        _pActionFreeAspectRatio;
    QAction*                        _pAction169AspectRatio;
    QAction*                        _pAction43AspectRatio;

    QAction*                        _pActionLogDebugInfo;

    Core::List<Ptr<Property> >      _ptiesCopy;

    QAction*                        _pActionViewUser;
    QAction*                        _pActionViewTop;
    QAction*                        _pActionViewBottom;
    QAction*                        _pActionViewLeft;
    QAction*                        _pActionViewRight;
    QAction*                        _pActionViewFront;
    QAction*                        _pActionViewBack;

    QAction*                        _pActionViewSolid;
    QAction*                        _pActionViewWireframe;
    QAction*                        _pActionViewWireframeSolid;

    QAction*                        _pActionViewDefault;
    QAction*                        _pActionViewNormal;
    QAction*                        _pActionViewUV;

    QAction*                        _pActionSelectAll;
    QAction*                        _pActionDeselectAll;

    QAction*                        _pActionCurrentViewportActiveDesactiveShowIHM;

    OutputWidget*                   _pOutputWidget;

    bool _makeSleep;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif // CONTROLLER_MAIN_H_