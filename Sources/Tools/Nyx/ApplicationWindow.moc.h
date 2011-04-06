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
#ifndef ApplicationWindow_H_
#define ApplicationWindow_H_

#include <QDockWidget>
#include <QScrollArea>

#include <Core/Standard.h>

#include <EPI/GUI/ApplicationWindowBase.h>
#include <EPI/GUI/VFSExplorer/VFSExplorer.moc.h>
#include <EPI/GUI/Widget/PropertiesPanel.moc.h>
#include <EPI/GUI/Widget/PropertiesExplorer.moc.h>
#include <EPI/GUI/Widget/ViewportLayout.moc.h>
#include <EPI/GUI/Widget/WidgetInfo.moc.h>
#include <EPI/GUI/Widget/DocumentToolBar.moc.h>
#include <EPI/GUI/Widget/ToolsPanel.moc.h>
#include <EPI/GUI/TaskManager/TaskManager.moc.h>
#include <EPI/GUI/GuiDocument.moc.h>
#include "NyxSetting.h"

class QToolButton;
namespace QtToolbox
{
    class ComboBox;
}


namespace Nyx
{
class ConfigDialog;
class NyxDockWidget;
//-----------------------------------------------------------------------------
class ApplicationWindow: public EPI::ApplicationWindowBase
{
    Q_OBJECT

public:
    virtual ~ApplicationWindow();
    ApplicationWindow(  const Ptr<Nyx::NyxSetting>& pSetting,
                        const Ptr<EPI::ControllerMain>& pCtrl,
                        QWidget * parent = 0, 
                        Qt::WindowFlags flags = 0);

    void openVFSFile(const Core::String & fileName);
protected:
    void closeEvent(QCloseEvent * closeEvent);

private Q_SLOTS:
    void newDocumentMaterial();
    void newDocumentParticule();
    void newDocumentWorld();
    void newDocumentGroup();
    void newDocumentSpline();
    void newDocumentTest();
    void newDocumentScript();
    void saveDocument();
    void saveAsDocument();
    void closeDocument();
    void noGuiDocument();
    void activatedGuiDocument(const Ptr<EPI::GuiDocument>& );
    void viewportLayoutSize();
    void propertiesPanelSize();
    void fullScreenToggle();
    void options();

    void changeRdrSetting();
    void closeConfigDialog();

    void showHideWindowTaskManager();

private:
    void initDisplay();
    void setupUi();

    void setupUiMenuFile();
    void setupUiMenuEdit();
    void setupUiMenuInterfaceLayout();
    void setupUiMenuTools();
    void setupUiMenuQuickRdrSettings();

    void createEditToolButton();
    void createFileToolButton();
    void createToolsToolButton();
    void createDocumentToolButton();

    
    void createInterfaceLayoutToolButton();
    void createToolBar();
    void makeTitle(const String& titleDoc=L"");
    void updateTitle();

public:
    QDockWidget* _pDockWidgetPropertiesPanel;
    QDockWidget* _pDockWidgetPropertiesExplorer;
    QDockWidget* _pDockWidgetVFSExplorer;
    QDockWidget* _pDockWidgetInfo;
    QDockWidget* _pDockToolsPanel;
    QDockWidget* _pDockWidgetToolBar;
    NyxDockWidget* _pDockWidgetOutput;

    QScrollArea* _pScrollPropertiesPanel;

    Ptr<Nyx::NyxSetting>            _pSetting;
    Ptr<EPI::VFSExplorer>           _pVFSExplorer;
    Ptr<EPI::PropertiesExplorer>    _pPropertiesExplorer;
    Ptr<EPI::ToolsPanel>            _pToolsPanel;

    Ptr<EPI::PropertiesPanel>       _pPropertiesPanel;

    Ptr<EPI::ViewportLayout>        _pViewsLayout;
    Ptr<EPI::WidgetInfo>            _pWidgetInfo;
    
    Ptr<EPI::TaskManager>           _pTaskManager;
    Ptr<EPI::DocumentToolBar>       _pDocToolBar;

    QWidget*        _pMenuToolBar;
    QToolButton*    _pFileToolButton;
    QToolButton*    _pEditToolButton;
    QToolButton*    _pToolsToolButton;
    QToolButton*    _pDocumentToolButton;
    QToolButton*    _pInterfaceLayoutToolButton;


    QMenu*      _pMenuFile;
    QMenu*      _pMenuTools;
    QMenu*      _pMenuEdit;
    QMenu*      _pMenuInterfaceLayout;
    QtToolbox::ComboBox*      _pMenuQuickRdrSettings;

    QAction*    _pActionSave;
    QAction*    _pActionSaveAs;
    QAction*    _pActionCloseCurrentDoc;
    QAction*    _pActionQuit;
    QAction*    _pActionOptions;

    QAction*    _pActionWindowTaskManager;
    QAction*    _pActionViewportLayoutSize;
    bool        _isViewportLayoutMaximized;

    QAction*    _pActionPropertiesPanelSize;
    bool        _isPropertiesPanelSize;

    QAction*    _pActionFullscreen;
    bool        _isFullscreen;

    ConfigDialog*   _configDlg;
};
//-----------------------------------------------------------------------------
} // namespace Nyx
#endif ApplicationWindow_H_
