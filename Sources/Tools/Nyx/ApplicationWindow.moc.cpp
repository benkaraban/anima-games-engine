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
#include <Nyx/ApplicationWindow.moc.h>

#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QToolButton>
#include <QComboBox>
#include <QPainter>
#include <QLabel>
#include <QBitmap>
#include <Core/Logger.h>
#include <QLinearGradient>
#include <EPI/Constants.h>
#include <EPI/ApplicationBase.moc.h>
#include <EPI/GUI/Widget/GuiDocumentsSelector.moc.h>

#include <QtToolbox/PushButtonMenu.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include "ConfigDialog.moc.h"

#include <EPI/RendererManager.h>
#include <QtToolbox/CollapsibleWidget.moc.h>

#include <EPI/Gui/Widget/OutputWidget.moc.h>
namespace Nyx
{

class NyxDockWidgetTitle : public QWidget
{
public:

    NyxDockWidgetTitle(const QString & title, QWidget* parent) :
      QWidget(parent),
      _collapsable(true),
      _collapsed(false),
      _pixOpen(8, 12),
      _pixCollapsed(8, 12),
      pointClicked(false)
    {
        _layout = new QVBoxLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        QWidget::setLayout(_layout);

        QWidget* widgetTitle = new QWidget(this);
        QHBoxLayout* layoutTitle = new QHBoxLayout(NULL);
            layoutTitle->setContentsMargins(0, 0, 0, 0);
            layoutTitle->setAlignment(Qt::AlignTop);
        QWidget* margeLeft = new QWidget(widgetTitle);
            margeLeft->setFixedSize(13,13);
        _widgetLabel = new QLabel(title, widgetTitle);
           // QString templateText = tr("<font color='%1'>%2</font>");
            //_widgetLabel->setText( templateText.arg( "lightgrey", title ) );
             QPalette palette = _widgetLabel->palette();
             //palette.setColor(ui->pLabel->backgroundRole(), Qt::yellow);
             palette.setColor(_widgetLabel->foregroundRole(), Qt::lightGray);
             _widgetLabel->setPalette(palette);


            _widgetLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
            
            widgetTitle->setLayout(layoutTitle);
        layoutTitle->addWidget(margeLeft);
        layoutTitle->addWidget(_widgetLabel);

        _layout->addWidget(widgetTitle);

        createPixMaps();
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    }

    ~NyxDockWidgetTitle()
    {
    }

    void colorBackGround(QPainter& painter, int32 bgWidth, int32 bgHeight)
    {
        QLinearGradient gradTitle(0, 0, 0, bgHeight);
            gradTitle.setColorAt(0, QColor::fromRgbF(0.2, 0.2, 0.2, 1));
            gradTitle.setColorAt(0.5, QColor::fromRgbF(0.3, 0.3, 0.3, 1));
            gradTitle.setColorAt(1, QColor::fromRgbF(0.4, 0.4, 0.4, 1));

        QBrush brushTitle (gradTitle);

        QRect backgroundTitle (0, 0, bgWidth, bgHeight);
        
        painter.fillRect(backgroundTitle, brushTitle);
    }

    void paintEvent(QPaintEvent * paintEvent)
    {
        QWidget::paintEvent(paintEvent);
        QPainter smartGBDPainter(this);

        colorBackGround(smartGBDPainter, width(), height());

        QRect frameGeo = rect();

        

        if(_collapsable)
        {
            
            if(_collapsed)
                smartGBDPainter.drawPixmap(rect().topLeft(), _pixCollapsed);
            else
                smartGBDPainter.drawPixmap(rect().topLeft(), _pixOpen);
        }
    }

    void collapse( bool collapse )
    {
        _collapsed = collapse;
    }

    bool isCollapsed() { return _collapsed;}

    virtual void mousePressEvent(QMouseEvent * mouseEvent )
    {
        if(_collapsable)
        {
            if( mouseEvent->button() == Qt::LeftButton )
            {
              //  QRect buttonArea( 0, 0, width(), 12);
              //  if( buttonArea.contains( mouseEvent->pos() ) )
              //  {
                    pointClicked = true;
               // }
            }
        }
        QWidget::mousePressEvent(mouseEvent);
    }

    virtual void mouseReleaseEvent(QMouseEvent * mouseEvent)
    {
        if(_collapsable)
        {
            if( mouseEvent->button() == Qt::LeftButton && pointClicked == true )
            {
                collapse( !_collapsed );
            }
        }
        QWidget::mouseReleaseEvent(mouseEvent);
        pointClicked = false;
    }

    virtual void focusOutEvent (QFocusEvent * event)
    {
        pointClicked = false;
        QWidget::focusOutEvent (event);
    }

private:
    void createPixMaps()
    {
        QColor maskColor = QColor(0,0,0,0);//QColor::fromRgbF(0.0, 0.0, 0.0, 0);
        QColor blackColor = QColor(102,102,102,255);
        QColor darkColor = QColor(202,202,202,202);

        _pixOpen.fill(Qt::transparent);
        QPainter openPainter(&_pixOpen);
        openPainter.setCompositionMode(QPainter::CompositionMode_Source);
        

        QPointF openPoints[3] = {   QPointF(_pixOpen.width()-1, _pixOpen.height()/4),
                                    QPointF(_pixOpen.width()-1, 3*_pixOpen.height()/4),
                                    QPointF(1, 3*_pixOpen.height()/4)};

        
        openPainter.setPen(blackColor);
        openPainter.setBrush(darkColor);
        openPainter.drawPolygon(openPoints, 3);
 //       _pixOpen.createMaskFromColor (maskColor);

        _pixCollapsed.fill(Qt::transparent);
        QPainter collapsedPainter(&_pixCollapsed);
        collapsedPainter.setCompositionMode(QPainter::CompositionMode_Source);
        
        
        QPointF collapsedPoints[3] = {  QPointF(1, _pixCollapsed.height()/4),
                                        QPointF(_pixCollapsed.width()-1, _pixCollapsed.height()/2),
                                        QPointF(1, 3*_pixCollapsed.height()/4)};

        
        collapsedPainter.setPen(blackColor);
        collapsedPainter.setBrush(darkColor);
        collapsedPainter.drawPolygon(collapsedPoints, 3);
   //     _pixCollapsed.createMaskFromColor (maskColor);
    }

    QColor   getBackgroundTitleColor() const
    {
        return QColor(170,170,170);
    }

    

    

    

private:
    QLabel*         _widgetLabel;
    QVBoxLayout *   _layout;
    QPixmap         _pixOpen;
    QPixmap         _pixCollapsed;
    bool            _collapsable;
    bool            _collapsed;
    bool            pointClicked;
};

class WidgetForNywDockWidget : public QWidget
{
public:
    WidgetForNywDockWidget(QWidget* parent) : QWidget(parent)
    {
        
    }

    ~WidgetForNywDockWidget() {}
    virtual QSize 	sizeHint () const
    {
        return _mySizeHint;
    }

    virtual void setSizeHint(QSize s)
    {
        _mySizeHint = s;
    }
private:
    QSize _mySizeHint;
};


class NyxDockWidget : public QDockWidget
{
public:
    NyxDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0) : 
        pasBeauMaisBon(NULL), _col(false), QDockWidget(title, parent, flags)
        {
            gre = new QWidget(); 
            gre->setFixedHeight(1);

            _titleMenu = new NyxDockWidgetTitle(title, NULL);
            _titleMenu->installEventFilter(this);

            setTitleBarWidget(_titleMenu);
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            _pContent = new WidgetForNywDockWidget(this);
            QGridLayout * l = new QGridLayout(_pContent);
            l->setContentsMargins(0,0,0,0);
            l->setSpacing(0);
            _pContent->setLayout(l);
            QDockWidget::setWidget(_pContent);
        }
    ~NyxDockWidget() {delete _pContent;}

    void setWidget(QWidget* w)
    {
        if (_pContent!=NULL && _pContent->layout() != NULL && w != NULL)
        {
           
            _pContent->layout()->addWidget(w);
        }
        //QDockWidget::setWidget(w);
        if (w!=NULL)
            _memoSize = w->size();

        _pContent->setSizeHint(_memoSize);
    }

    void collapse(bool flag)
    {
        if (_col==false && flag==true)
        {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            
            _memoSize = widget()->size();
            _pContent->setSizeHint(QSize(0,0));
            _col = true;
            _titleMenu->collapse(_col);
            pasBeauMaisBon = widget();
            QDockWidget::setWidget(gre);
            pasBeauMaisBon->setParent(NULL);
            if (pasBeauMaisBon!=NULL) pasBeauMaisBon->hide();
            
        }
        else if(_col==true && flag==false)
        {
            _pContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
            _pContent->setSizeHint(_memoSize);
            _pContent->setSizeHint(QSize(size().width(),_memoSize.height()));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            _col = false;
            _titleMenu->collapse(_col);
            //pasBeauMaisBon->resize(_memoSize);
            QDockWidget::setWidget(pasBeauMaisBon);
            if (pasBeauMaisBon!=NULL) pasBeauMaisBon->show();
            
            show();
            _pContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            
        }

        adjustSize();
    }

    virtual bool eventFilter(QObject* pWatched, QEvent* pEvent)
    {
        if (pEvent->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = (QMouseEvent*)(pEvent);
            NyxDockWidgetTitle* title = (NyxDockWidgetTitle*)pWatched;

            if (mouseEvent->button() == Qt::LeftButton)
            {
                title->mouseReleaseEvent(mouseEvent);
                collapse(title->isCollapsed());
                show();
                return true;
            }

           
        }

        return false;
    }


private:
    NyxDockWidgetTitle* _titleMenu;
    QWidget * pasBeauMaisBon;
    QWidget * gre;
    WidgetForNywDockWidget * _pContent;
    QSize   _memoSize;
    bool    _col;
};

void configToolBoutton(QToolButton*    pToolButton)
{
    pToolButton->setStyleSheet(
        "QToolButton {border: 0px solid #8f8f91; border-radius: 0px; background-color: transparent;}"
        "QToolButton::menu-arrow {image: url("");}"
        "QToolButton::menu-button {border: 0px solid gray; border-top-right-radius: 0px; border-bottom-right-radius: 0px;width: 0px;}"
        "QToolButton::menu-indicator {image: url(""); subcontrol-origin: padding; subcontrol-position: bottom right;}"
    );
}



class NyxDockWidgetTitleNormal : public QWidget
{
public:

    NyxDockWidgetTitleNormal(const QString & title, QWidget* parent) :
      QWidget(parent)
    {
        _layout = new QVBoxLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        //_layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
        QWidget::setLayout(_layout);

        QWidget* widgetTitle = new QWidget(this);
        QHBoxLayout* layoutTitle = new QHBoxLayout(NULL);
            layoutTitle->setContentsMargins(0, 0, 0, 0);
            layoutTitle->setAlignment(Qt::AlignCenter);
      /*  QWidget* margeLeft = new QWidget(widgetTitle);
            margeLeft->setFixedSize(13,13);*/
        _widgetLabel = new QLabel(title, widgetTitle);
           // QString templateText = tr("<font color='%1'>%2</font>");
            //_widgetLabel->setText( templateText.arg( "lightgrey", title ) );
             QPalette palette = _widgetLabel->palette();
             //palette.setColor(ui->pLabel->backgroundRole(), Qt::yellow);
             palette.setColor(_widgetLabel->foregroundRole(), Qt::black);
             _widgetLabel->setPalette(palette);


            _widgetLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
            
            widgetTitle->setLayout(layoutTitle);
       // layoutTitle->addWidget(margeLeft);
        layoutTitle->addWidget(_widgetLabel);

        _layout->addWidget(widgetTitle);
    }

    ~NyxDockWidgetTitleNormal()
    {
    }

    void colorBackGround(QPainter& painter, int32 bgWidth, int32 bgHeight)
    {
        QColor test = palette().color(QPalette::Active, QPalette::Background);
 
        QLinearGradient gradTitle(0, 0, 0, bgHeight);
            gradTitle.setColorAt(0,     QColor(test.red()*0.2, test.green()*0.2, test.blue()*0.2, 1));
            gradTitle.setColorAt(0.5,   QColor(test.red()*0.3, test.green()*0.3, test.blue()*0.3, 1));
            gradTitle.setColorAt(1,     QColor(test.red(), test.green(), test.blue(), 1));

          /*  gradTitle.setColorAt(0, QColor::fromRgbF(0.2, 0.2, 0.2, 1));
            gradTitle.setColorAt(0.5, QColor::fromRgbF(0.3, 0.3, 0.3, 1));
            gradTitle.setColorAt(1, QColor::fromRgbF(0.4, 0.4, 0.4, 1));*/

        QBrush brushTitle (gradTitle);

        QRect backgroundTitle (0, 0, bgWidth, bgHeight);
        
        painter.fillRect(backgroundTitle, brushTitle);
    }

    void paintEvent(QPaintEvent * paintEvent)
    {
        QWidget::paintEvent(paintEvent);
        QPainter smartGBDPainter(this);

        colorBackGround(smartGBDPainter, width(), height());
    }

    virtual void mousePressEvent(QMouseEvent * mouseEvent )
    {
        QWidget::mousePressEvent(mouseEvent);
    }

    virtual void mouseReleaseEvent(QMouseEvent * mouseEvent)
    {
        QWidget::mouseReleaseEvent(mouseEvent);
    }

    virtual void focusOutEvent (QFocusEvent * event)
    {
        QWidget::focusOutEvent (event);
    }

private:
    QLabel*         _widgetLabel;
    QVBoxLayout *   _layout;
};

class NyxDockWidgetNormal : public QDockWidget
{
public:
    NyxDockWidgetNormal(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0) : 
        QDockWidget(title, parent, flags)
    {
     /*   _titleMenu = new NyxDockWidgetTitleNormal(title, NULL);
        _titleMenu->installEventFilter(this);

        setTitleBarWidget(_titleMenu);*/
/*
            setStyleSheet(
                "QDockWidget::title {border: none; text-align: center; image: url(""); padding: 0px; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(199,199,199), stop: 1 rgb(201,201,201));}"
        );

        */

    }

private:
    NyxDockWidgetTitleNormal* _titleMenu;
};
//-----------------------------------------------------------------------------
ApplicationWindow::ApplicationWindow(   const Ptr<Nyx::NyxSetting>& pSetting,
                                        const Ptr<EPI::ControllerMain>& pCtrl,
                                        QWidget * parent, 
                                        Qt::WindowFlags flags) :
    ApplicationWindowBase(pCtrl, parent, flags),
    _pMenuFile(NULL),
    _pMenuEdit(NULL),
    _pMenuTools(NULL),
    _pMenuInterfaceLayout(NULL),
    _pActionSave(NULL),
    _pActionSaveAs(NULL),
    _pActionCloseCurrentDoc(NULL),
    _pActionQuit(NULL),
    _pActionViewportLayoutSize(NULL),
    _pActionPropertiesPanelSize(NULL),
    _pActionFullscreen(NULL),
    _pActionOptions(NULL),
    _pMenuToolBar(NULL),
    _pMenuQuickRdrSettings(NULL),
    _pFileToolButton(NULL),
    _pEditToolButton(NULL),
    _pDocumentToolButton(NULL),
    _pToolsToolButton(NULL),
    _pInterfaceLayoutToolButton(NULL),
    _isViewportLayoutMaximized(false),
    _isPropertiesPanelSize(false),
    _isFullscreen(false),
    _pSetting(pSetting)
{
    setupUi();
    initDisplay();
}
//-----------------------------------------------------------------------------
void ApplicationWindow::createFileToolButton()
{
    delete _pFileToolButton;

    setupUiMenuFile();

    _pFileToolButton = new QToolButton(this);
    _pFileToolButton->setMenu(_pMenuFile);
    _pFileToolButton->setPopupMode(QToolButton::InstantPopup);
    _pFileToolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pFileToolButton->setText("File");
    _pFileToolButton->setAutoRaise(false);
    _pFileToolButton->setArrowType(Qt::NoArrow);

    configToolBoutton(_pFileToolButton);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::createEditToolButton()
{
    delete _pEditToolButton;

    setupUiMenuEdit();

    _pEditToolButton = new QToolButton(this);
    _pEditToolButton->setMenu(_pMenuEdit);
    _pEditToolButton->setPopupMode(QToolButton::InstantPopup);
    _pEditToolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pEditToolButton->setText("Edit");
    _pEditToolButton->setAutoRaise(false);
    _pEditToolButton->setArrowType(Qt::NoArrow);

    configToolBoutton(_pEditToolButton);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::createDocumentToolButton()
{
    delete _pDocumentToolButton;

    setupUiMenuEdit();

    _pDocumentToolButton = new EPI::GuiDocumentsSelector (getControllerMain(), L"Documents", this);

    _pDocumentToolButton->setPopupMode(QToolButton::InstantPopup);
    _pDocumentToolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pDocumentToolButton->setText("Document");
    _pDocumentToolButton->setAutoRaise(false);
    _pDocumentToolButton->setArrowType(Qt::NoArrow);

    configToolBoutton(_pDocumentToolButton);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::createInterfaceLayoutToolButton()
{
    delete _pInterfaceLayoutToolButton;

    setupUiMenuInterfaceLayout();

    _pInterfaceLayoutToolButton = new QToolButton(this);
    _pInterfaceLayoutToolButton->setMenu(_pMenuInterfaceLayout);
    _pInterfaceLayoutToolButton->setPopupMode(QToolButton::InstantPopup);
    _pInterfaceLayoutToolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pInterfaceLayoutToolButton->setText("Layout");
    _pInterfaceLayoutToolButton->setAutoRaise(false);
    _pInterfaceLayoutToolButton->setArrowType(Qt::NoArrow);

    configToolBoutton(_pInterfaceLayoutToolButton);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::createToolsToolButton()
{
    delete _pToolsToolButton;

    setupUiMenuTools();

    _pToolsToolButton = new QToolButton(this);
    _pToolsToolButton->setMenu(_pMenuTools);
    _pToolsToolButton->setPopupMode(QToolButton::InstantPopup);
    _pToolsToolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pToolsToolButton->setText("Tools");
    _pToolsToolButton->setAutoRaise(false);
    _pToolsToolButton->setArrowType(Qt::NoArrow);

    configToolBoutton(_pToolsToolButton);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::createToolBar()
{
    delete _pMenuToolBar;

    createFileToolButton();
    createEditToolButton();
    createDocumentToolButton();
    createInterfaceLayoutToolButton();
    createToolsToolButton();
    setupUiMenuQuickRdrSettings();

    _pMenuToolBar = new QWidget(this);
    _pMenuToolBar->setContentsMargins(0,0,0,0);
    

    QGridLayout* tbLayout = new QGridLayout(_pMenuToolBar);


    tbLayout->setContentsMargins(0,0,0,0);
    tbLayout->addWidget(_pFileToolButton,            0,0,1,1, Qt::AlignLeft);
    tbLayout->addWidget(_pEditToolButton,            0,1,1,1, Qt::AlignLeft);
    tbLayout->addWidget(_pDocumentToolButton,        0,2,1,1, Qt::AlignLeft);
    tbLayout->addWidget(_pInterfaceLayoutToolButton, 0,3,1,1, Qt::AlignLeft);
    tbLayout->addWidget(_pToolsToolButton,           0,4,1,1, Qt::AlignLeft);
    

    tbLayout->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding), 0,5, 1,1);
    tbLayout->addWidget(_pMenuQuickRdrSettings, 0,6,1,1, Qt::AlignRight);

    tbLayout->setAlignment(Qt::AlignLeft);
    _pMenuToolBar->setLayout(tbLayout);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::setupUi()
{
    delete _pMenuFile;
    delete _pMenuEdit;
    delete _pMenuInterfaceLayout;
    delete _pActionSave;
    delete _pActionSaveAs;
    delete _pActionCloseCurrentDoc;
    delete _pActionViewportLayoutSize;
    delete _pActionPropertiesPanelSize;
    
  
    createToolBar();
    
    //addToolBar(Qt::TopToolBarArea, _pMenuToolBar);
    
    _pDockWidgetToolBar = new QDockWidget(tr("ToolBar"), this);
    _pDockWidgetToolBar->setAllowedAreas(Qt::TopDockWidgetArea);
    _pDockWidgetToolBar->setFeatures(0);
    _pDockWidgetToolBar->setContentsMargins(0,0,0,0);
    _pDockWidgetToolBar->setWindowTitle("");
    _pDockWidgetToolBar->setFixedHeight(21);
    _pDockWidgetToolBar->setWidget(_pMenuToolBar);

    _pDockWidgetToolBar->setStyleSheet(
        "QDockWidget {border: 1px solid lightgray; titlebar-close-icon: url(""); titlebar-normal-icon: url("");}"
        "QDockWidget::title {height: 2px; width: 2px; text-align: left; background: lightgray; padding: 0px; margin: 0px}"
        );
    QWidget* titleMenu = new QWidget(NULL);
    titleMenu->setFixedHeight(01);
    titleMenu->setStyleSheet("QWidget {background-color: transparent;}");

    _pDockWidgetToolBar->setTitleBarWidget(titleMenu);

/*
    setStyleSheet(
        "QSplitter                    {background-color: #6C6C6C;}"
        "QSplitter::handle:horizontal {width: 2px; background-color: #535353;}"
        "QSplitter::handle:vertical   {height: 2px; background-color: #535353;}"
        "QSplitter::handle            {image: url("");}"
        "QSplitter::handle:pressed    {url("");}"
        );
*/


    addDockWidget(Qt::TopDockWidgetArea,     _pDockWidgetToolBar);


       // setStyleSheet("QMainWindow {background-color: darkgray;} QMainWindow::separator {background: darkgray; width: 2px; height: 2px;} QMenuBar {background-color: darkgray;} QMenu {background-color: darkgray;}");
    
/*    QAction * undoAction = new QAction("Undo", this);
    undoAction->setShortcuts(QKeySequence::Undo);
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undoDocument()));
    QAction * redoAction = new QAction("Redo", this);
    redoAction->setShortcuts(QKeySequence::Redo);
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redoDocument()));*/
  /*  menuEdit->addAction(undoAction);
    menuEdit->addAction(redoAction);*/


    //PropertiesPanel
    _pDockWidgetPropertiesPanel = new NyxDockWidgetNormal(tr("Properties"), this);
    _pDockWidgetPropertiesPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _pDockWidgetPropertiesPanel->setFeatures(QDockWidget::DockWidgetMovable);
    _pPropertiesPanel = Ptr<EPI::PropertiesPanel> (new EPI::PropertiesPanel(getControllerMain(), _pDockWidgetPropertiesPanel) );
    _pScrollPropertiesPanel = new QScrollArea(this);
    _pScrollPropertiesPanel->setWidgetResizable(true);
    _pScrollPropertiesPanel->setWidget(_pPropertiesPanel.get());
    _pDockWidgetPropertiesPanel->setWidget(_pScrollPropertiesPanel);
 //   _pDockWidgetPropertiesPanel->setWidget(_pPropertiesPanel.get());

    //PropertiesExplorer
    _pDockWidgetPropertiesExplorer = new NyxDockWidgetNormal(tr("Properties Explorer"), this);
    _pDockWidgetPropertiesExplorer->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _pDockWidgetPropertiesExplorer->setFeatures(QDockWidget::DockWidgetMovable);
    _pPropertiesExplorer = Ptr<EPI::PropertiesExplorer> (new EPI::PropertiesExplorer(getControllerMain(), _pDockWidgetPropertiesPanel) );
    _pDockWidgetPropertiesExplorer->setWidget(_pPropertiesExplorer.get());

    //VFSExplorer
    _pDockWidgetVFSExplorer = new NyxDockWidgetNormal(tr("VFS Explorer"), this);
    _pDockWidgetVFSExplorer->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _pDockWidgetVFSExplorer->setFeatures(QDockWidget::DockWidgetMovable);
    _pVFSExplorer = Ptr<EPI::VFSExplorer> (new EPI::VFSExplorer(getControllerMain(), _pDockWidgetPropertiesPanel) );
    _pDockWidgetVFSExplorer->setWidget(_pVFSExplorer.get());

    //TaskManager
    _pTaskManager = Ptr<EPI::TaskManager>(new EPI::TaskManager(this));
    _pTaskManager->hide();
    
    //centralWidget
    QWidget* centralWidget = new QWidget(this);
    QGridLayout * centralLayout = new QGridLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    //ViewportLayout
    _pViewsLayout = Ptr<EPI::ViewportLayout> (new EPI::ViewportLayout(getControllerMain(), centralWidget));

    //DocumentToolBar
    _pDocToolBar = Ptr<EPI::DocumentToolBar> (new EPI::DocumentToolBar(getControllerMain()));
   
    centralLayout->addWidget(_pDocToolBar.get(),0,0);
    centralLayout->addWidget(_pViewsLayout.get(),1,0);

    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);
    setContentsMargins(0,0,0,0);
    //WidgetInfo
    _pDockWidgetInfo = new NyxDockWidgetNormal(tr("Info"), this);
    _pWidgetInfo = Ptr<EPI::WidgetInfo> (new EPI::WidgetInfo(getControllerMain(), this));
    _pDockWidgetInfo->setWidget(_pWidgetInfo.get());
   

    //OutputWidget
    _pDockWidgetOutput = new NyxDockWidget(tr("Output"), this);
    _pDockWidgetOutput->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    _pDockWidgetOutput->setFeatures(QDockWidget::AllDockWidgetFeatures);
    _pDockWidgetOutput->setContentsMargins(0,0,0,0);


    _pDockWidgetOutput->setWidget(getControllerMain()->getOutputWidget());
    _pDockWidgetOutput->collapse(true);

    //ToolsPanel
    _pDockToolsPanel = new NyxDockWidgetNormal(tr("ToolsPanel"), this);
    _pDockToolsPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _pDockToolsPanel->setFeatures(QDockWidget::DockWidgetMovable);
    _pToolsPanel = Ptr<EPI::ToolsPanel>(new EPI::ToolsPanel(getControllerMain(), this));
    _pDockToolsPanel->setWidget(_pToolsPanel.get());
    

    //On import un asset dans le VFS
    connect(_pVFSExplorer.get(), SIGNAL(importAsset(const Ptr<ImportInfo> &)),
            _pTaskManager.get(), SLOT(generate(const Ptr<ImportInfo> &)));
    //Le generation dans le TaskManager est terminée on informe le VFSExplorer et le ControllerMain
    connect(_pTaskManager.get(), SIGNAL(generationDone(const Ptr<ImportInfo> &)),
            _pVFSExplorer.get(), SLOT(importDone(const Ptr<ImportInfo> &)));
    connect(_pTaskManager.get(),        SIGNAL(generationDone(const Ptr<ImportInfo> &)),
            getControllerMain().get(),  SLOT(generationDone(const Ptr<ImportInfo> &)));
    //Ouverture d'une source par le VFSExplorer pour que le TaskManager puisse créer  sa tache
    connect(_pTaskManager.get(), SIGNAL(sourceNotOpened(const Ptr<ImportInfo> &)),
            _pVFSExplorer.get(), SLOT(openSource(const Ptr<ImportInfo> &)));
    //On a doubleclické dans le VFSExplorer pour regarder les propriété d'un asset
    connect(_pVFSExplorer.get(),        SIGNAL(openItemProperties(const Ptr<ImportInfo> &)),
            getControllerMain().get(),  SLOT(loadGuiDocument(const Ptr<ImportInfo> &)));
    //On a demandé une regénération d'un asset
    connect(getControllerMain().get(),  SIGNAL(generate(const Ptr<ImportInfo> &)),
            _pTaskManager.get(),        SLOT(generate(const Ptr<ImportInfo> &)));
    //On a sauvegardé un document on met à jour le VFS
    connect(getControllerMain().get(),  SIGNAL(documentSaved()),
            _pVFSExplorer.get(),        SLOT(refreshTree()));


    connect(getControllerMain().get(), SIGNAL(signalNoGuiDocument()),
            this, SLOT(noGuiDocument()));

    connect(getControllerMain().get(), SIGNAL(signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>& )),
        this, SLOT(activatedGuiDocument(const Ptr<EPI::GuiDocument>& )));


    addAction(_pActionViewportLayoutSize);
    addAction(_pActionFullscreen);
    addAction(_pActionPropertiesPanelSize);

    _configDlg = new ConfigDialog(_pSetting, this);
    _configDlg->setModal(false);
    _configDlg->setWindowModality (Qt::NonModal);
    connect(_configDlg, SIGNAL(accepted()),
        this, SLOT(closeConfigDialog()));

    _pDockWidgetInfo->setVisible(false);
    makeTitle();
}
//-----------------------------------------------------------------------------
void ApplicationWindow::updateTitle()
{
    Ptr<EPI::GuiDocument> pGDoc = getControllerMain()->getDocumentManager()->getCurrentGuiDocument();

    if (pGDoc!=null)
    {
        makeTitle(pGDoc->getDocument()->getTitle());
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::makeTitle(const String& titleDoc)
{
    QString title ("Nyx");
        title += " ";
        title += NYX_VERSION;
        title += "               ";
        title += String8(titleDoc).c_str();
    setWindowTitle(title);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::setupUiMenuFile()
{
    _pMenuFile = new QMenu("File", this);
  /*  _pMenuFile->setStyleSheet(
        "QMenu {background-color: transparent;}"
        "QMenu::item {background-color: rgba(100, 100, 100, 10);}"
     );*/

    QMenu* menuNew = new QMenu("New", this);
        menuNew->addAction("Material",  this, SLOT(newDocumentMaterial())   );
        //menuNew->addAction("Particle",  this, SLOT(newDocumentParticule())  );
        menuNew->addAction("World",     this, SLOT(newDocumentWorld())      );
        menuNew->addAction("Script",     this, SLOT(newDocumentScript())      );
        menuNew->addAction("Group",     this, SLOT(newDocumentGroup())      ); 
        menuNew->addAction("Spline",     this, SLOT(newDocumentSpline())      );
        //menuNew->addAction("Test",      this, SLOT(newDocumentTest())       );

    _pActionSave            = new QAction("Save", this);
    _pActionSaveAs          = new QAction("Save As ...", this);
    _pActionCloseCurrentDoc = new QAction("Close Current Doc", this);
    _pActionQuit            = new QAction("Quit", this);

    connect(_pActionSave, SIGNAL(triggered()),              this, SLOT(saveDocument()));
    connect(_pActionSaveAs, SIGNAL(triggered()),            this, SLOT(saveAsDocument()));
    connect(_pActionCloseCurrentDoc, SIGNAL(triggered()),   this, SLOT(closeDocument()));
    connect(_pActionQuit, SIGNAL(triggered()),              this, SLOT(close()));

    _pActionSave->setShortcuts(QKeySequence::Save);
    _pActionSaveAs->setShortcut(QKeySequence (Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    _pActionCloseCurrentDoc->setShortcuts(QKeySequence::Close);
    _pActionQuit->setShortcuts(QKeySequence::Quit);

    _pMenuFile->addMenu(menuNew);
    _pMenuFile->addSeparator();
    _pMenuFile->addAction(_pActionCloseCurrentDoc);
    _pMenuFile->addSeparator();
    _pMenuFile->addAction(_pActionSave);
    _pMenuFile->addAction(_pActionSaveAs);
    _pMenuFile->addSeparator();
    _pMenuFile->addAction(_pActionQuit);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::setupUiMenuEdit()
{
    _pMenuEdit = new QMenu("Edit", this);
        _pMenuEdit->addAction(getControllerMain()->getActionUndoCurrentGuiDocument());
        _pMenuEdit->addAction(getControllerMain()->getActionRedoCurrentGuiDocument());
}
//-----------------------------------------------------------------------------
void ApplicationWindow::setupUiMenuInterfaceLayout()
{
    _pMenuInterfaceLayout = new QMenu("InterfaceLayout", this);

    _pActionViewportLayoutSize = new QAction("Viewports (maximise/minimise)", this);
    _pActionPropertiesPanelSize =  new QAction("PropertiesPanel (maximise/minimise)", this);
    _pActionFullscreen = new QAction("Fullscreen", this);
    _pActionWindowTaskManager = new QAction("Windows - TaskManager", this);

    connect(_pActionViewportLayoutSize, SIGNAL(triggered()), this, SLOT(viewportLayoutSize()));
    connect(_pActionPropertiesPanelSize, SIGNAL(triggered()), this, SLOT(propertiesPanelSize()));
    connect(_pActionFullscreen, SIGNAL(triggered()), this, SLOT(fullScreenToggle()));
    connect(_pActionWindowTaskManager, SIGNAL(triggered()), this, SLOT(showHideWindowTaskManager()));

    _pActionViewportLayoutSize->setShortcut(QKeySequence(Qt::Key_F12));
    _pActionViewportLayoutSize->setShortcutContext(Qt::ApplicationShortcut);

    _pActionPropertiesPanelSize->setShortcut(QKeySequence(Qt::Key_F10));
    _pActionPropertiesPanelSize->setShortcutContext(Qt::ApplicationShortcut);

    _pActionFullscreen->setShortcut(QKeySequence(Qt::Key_F11));
    _pActionFullscreen->setShortcutContext(Qt::ApplicationShortcut);

    _pActionWindowTaskManager->setShortcut(QKeySequence(Qt::Key_M));
    _pActionWindowTaskManager->setShortcutContext(Qt::ApplicationShortcut);

    _pMenuInterfaceLayout->addAction(_pActionViewportLayoutSize);
    _pMenuInterfaceLayout->addAction(_pActionPropertiesPanelSize);
    _pMenuInterfaceLayout->addAction(_pActionFullscreen);
    _pMenuInterfaceLayout->addSeparator();
    _pMenuInterfaceLayout->addAction(_pActionWindowTaskManager);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::setupUiMenuTools()
{
    _pMenuTools = new QMenu("Tools", this);

    _pActionOptions = new QAction("Options", this);

    connect(_pActionOptions, SIGNAL(triggered()), this, SLOT(options()));

    _pMenuTools->addAction(_pActionOptions);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::setupUiMenuQuickRdrSettings()
{
    _pMenuQuickRdrSettings = new QtToolbox::ComboBox(NULL, "");

    _pMenuQuickRdrSettings->addItem("Nyx config");
    ADD_ITEM_COMBOBOX_LM_ENUM (_pMenuQuickRdrSettings,   Renderer::EGlobalLevel, 7)


    connect(_pMenuQuickRdrSettings, SIGNAL(valueUpdated()), this, SLOT(changeRdrSetting()));
/*
    QAction* pActionRdrSettingNyx       = new QAction("Nyx Config", this);
    QAction* pActionRdrSettingVLow      = new QAction("Very Low",   this);
    QAction* pActionRdrSettingLow       = new QAction("Low",        this);
    QAction* pActionRdrSettingMedium    = new QAction("Medium",     this);
    QAction* pActionRdrSettingHight     = new QAction("Hight",      this);
    QAction* pActionRdrSettingVHight    = new QAction("Very Hight", this);


    connect(pActionRdrSettingNyx,       SIGNAL(triggered()), this, SLOT(rdrSettingNyx()));
    connect(pActionRdrSettingVLow,      SIGNAL(triggered()), this, SLOT(rdrSettingVLow()));
    connect(pActionRdrSettingLow,       SIGNAL(triggered()), this, SLOT(rdrSettingLow()));
    connect(pActionRdrSettingMedium,    SIGNAL(triggered()), this, SLOT(rdrSettingMedium()));
    connect(pActionRdrSettingHight,     SIGNAL(triggered()), this, SLOT(rdrSettingHight()));
    connect(pActionRdrSettingVHight,    SIGNAL(triggered()), this, SLOT(rdrSettingVHight()));

    _pMenuQuickRdrSettings->addAction(pActionRdrSettingNyx);
    _pMenuQuickRdrSettings->addAction(pActionRdrSettingVLow);
    _pMenuQuickRdrSettings->addAction(pActionRdrSettingLow);
    _pMenuQuickRdrSettings->addAction(pActionRdrSettingMedium);
    _pMenuQuickRdrSettings->addAction(pActionRdrSettingHight);
    _pMenuQuickRdrSettings->addAction(pActionRdrSettingVHight);*/

/*
    _pMenuQuickRdrSettings->addItem("gre1");
    _pMenuQuickRdrSettings->addItem("gre2");
    _pMenuQuickRdrSettings->addItem("gre3");*/
}
//-----------------------------------------------------------------------------
ApplicationWindow::~ApplicationWindow()
{
    _pDockWidgetOutput->setWidget(NULL);
    getControllerMain()->getOutputWidget()->setParent(NULL);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::openVFSFile(const Core::String & fileName)
{
    _pVFSExplorer->openFile(fileName);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::newDocumentMaterial()
{
    getControllerMain()->newGuiDocument(EPI::DOC_MATERIAL);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::newDocumentParticule()
{
    getControllerMain()->newGuiDocument(EPI::DOC_PARTICLES);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::newDocumentWorld()
{
    getControllerMain()->newGuiDocument(EPI::DOC_WORLD);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::newDocumentGroup()
{
    getControllerMain()->newGuiDocument(EPI::DOC_GROUP);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::newDocumentSpline()
{
    getControllerMain()->newGuiDocument(EPI::DOC_SPLINE);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::newDocumentScript()
{
    getControllerMain()->newGuiDocument(EPI::DOC_SCRIPT);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::newDocumentTest()
{
    getControllerMain()->newGuiDocument(EPI::DOC_TEST);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::closeDocument()
{
    getControllerMain()->deleteCurrentGuiDocument();
}
//-----------------------------------------------------------------------------
void ApplicationWindow::saveDocument()
{
    getControllerMain()->saveCurrentDocument();
    updateTitle();
}
//-----------------------------------------------------------------------------
void ApplicationWindow::saveAsDocument()
{
    getControllerMain()->saveAsCurrentDocument();
    updateTitle();
}
//-----------------------------------------------------------------------------
void ApplicationWindow::closeEvent(QCloseEvent * event)
{
    bool close = true;

    if (getControllerMain()->canCloseAllDocument() == false)
    {
        int ret = QMessageBox::warning(this, "Close Nyx Without saving ?","Documents are modified, quit Nyx without saving ?",
                QMessageBox::Ok | QMessageBox::No, QMessageBox::No);

        if (ret==QMessageBox::No)
        {
            event->ignore ();
            close = false;
        }
    }
    
    if (close==true)
    {
        Ptr<WidgetSetting> pWSetting = LM_DEBUG_PTR_CAST<WidgetSetting>(_pSetting->getSetting(WIDGET_SETTING));

            pWSetting->geometryMainWindow   = saveGeometry();
            pWSetting->stateMainWindow      = saveState();
            pWSetting->propertiesPanel      = dockWidgetArea(_pDockWidgetPropertiesPanel);
            pWSetting->propertiesExplorer   = dockWidgetArea(_pDockWidgetPropertiesExplorer);
            pWSetting->vfsExplorer          = dockWidgetArea(_pDockWidgetVFSExplorer);
            pWSetting->widgetInfo           = dockWidgetArea(_pDockWidgetInfo);
            pWSetting->toolsPanel           = dockWidgetArea(_pDockToolsPanel);
            pWSetting->outputWidget         = dockWidgetArea(_pDockWidgetOutput);
        
            pWSetting->writeSetting();
        QMainWindow::closeEvent(event);
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::initDisplay()
{
    Ptr<WidgetSetting> pWSetting = LM_DEBUG_PTR_CAST<WidgetSetting>(_pSetting->getSetting(WIDGET_SETTING));
    
    bool restoreSuccessful = true;
    restoreSuccessful = restoreGeometry(pWSetting->geometryMainWindow);
    restoreSuccessful = restoreState(pWSetting->stateMainWindow);

    if(restoreSuccessful)
    {
        this->addDockWidget(pWSetting->propertiesPanel,     _pDockWidgetPropertiesPanel);
        this->addDockWidget(pWSetting->propertiesExplorer,  _pDockWidgetPropertiesExplorer);
        this->addDockWidget(pWSetting->vfsExplorer,         _pDockWidgetVFSExplorer);
        this->addDockWidget(pWSetting->widgetInfo,          _pDockWidgetInfo);
        this->addDockWidget(pWSetting->toolsPanel,          _pDockToolsPanel);
        this->addDockWidget(pWSetting->outputWidget,        _pDockWidgetOutput);
    }
    else
    {
        this->addDockWidget(Qt::RightDockWidgetArea,    _pDockWidgetPropertiesPanel);
        this->addDockWidget(Qt::LeftDockWidgetArea,     _pDockWidgetPropertiesExplorer);
        this->addDockWidget(Qt::LeftDockWidgetArea,     _pDockWidgetVFSExplorer);
        this->addDockWidget(Qt::RightDockWidgetArea,    _pDockWidgetInfo);
        this->addDockWidget(Qt::LeftDockWidgetArea,     _pDockToolsPanel);
        this->addDockWidget(Qt::RightDockWidgetArea,     _pDockWidgetOutput);
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::noGuiDocument()
{
    //menuFile
    _pActionCloseCurrentDoc->setDisabled(true);
    _pActionSave->setDisabled(true);
    _pActionSaveAs->setDisabled(true);
    //menuEdit
    _pMenuEdit->setDisabled(true);
}
//-----------------------------------------------------------------------------
void ApplicationWindow::activatedGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc)
{
    if (pGDoc!=null)
    {
        //menuFile
        _pActionCloseCurrentDoc->setDisabled(false);
        _pActionSave->setDisabled(false);
        _pActionSaveAs->setDisabled(false);
        //menuEdit
        _pMenuEdit->setDisabled(false);
        makeTitle(pGDoc->getDocument()->getTitle());
    }
    else
    {
        //menuFile
        _pActionCloseCurrentDoc->setDisabled(true);
        _pActionSave->setDisabled(true);
        _pActionSaveAs->setDisabled(true);
        //menuEdit
        _pMenuEdit->setDisabled(true);
         makeTitle();
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::viewportLayoutSize()
{
    if (_isViewportLayoutMaximized == false)
    {
        _isViewportLayoutMaximized = true;

        _pMenuToolBar->hide();
        _pDockWidgetPropertiesPanel->hide();
        _pDockWidgetPropertiesExplorer->hide();
        _pDockWidgetVFSExplorer->hide();
        _pDockWidgetInfo->hide();
        _pDockToolsPanel->hide();
        _pDocToolBar->hide();
        _pDockWidgetToolBar->hide();
        _pDockWidgetOutput->hide();
        
    }
    else
    {
        _isViewportLayoutMaximized = false;

        _pMenuToolBar->show();
        _pDockWidgetPropertiesPanel->show();
        _pDockWidgetPropertiesExplorer->show();
        _pDockWidgetVFSExplorer->show();
        _pDockWidgetInfo->show();
        _pDockToolsPanel->show();
        _pDocToolBar->show();
        _pDockWidgetToolBar->show();
        _pDockWidgetOutput->show();
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::propertiesPanelSize()
{
    if (_isPropertiesPanelSize == false)
    {
        _isPropertiesPanelSize = true;

        _pMenuToolBar->hide();
        centralWidget()->hide();
        _pDockWidgetPropertiesExplorer->hide();
        _pDockWidgetVFSExplorer->hide();
        _pDockWidgetInfo->hide();
        _pDockToolsPanel->hide();
        _pDocToolBar->hide();
        _pDockWidgetOutput->hide();
    }
    else
    {
        _isPropertiesPanelSize = false;

        _pMenuToolBar->show();
        centralWidget()->show();
        _pDockWidgetPropertiesExplorer->show();
        _pDockWidgetVFSExplorer->show();
        _pDockWidgetInfo->show();
        _pDockToolsPanel->show();
        _pDocToolBar->show();
        _pDockWidgetOutput->show();
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::fullScreenToggle()
{
    if (_isFullscreen == false)
    {
        _isFullscreen = true;
        showFullScreen();
    }
    else
    {
        _isFullscreen = false;
        showNormal();
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::showHideWindowTaskManager()
{
    _pTaskManager->setVisible(!_pTaskManager->isVisible());
}
//-----------------------------------------------------------------------------
void ApplicationWindow::options()
{
    _configDlg->show();
}
//-----------------------------------------------------------------------------
void ApplicationWindow::closeConfigDialog()
{
    _configDlg->hide();

    if (_configDlg->updateRenderer() == true)   
    {
        if (_pMenuQuickRdrSettings->selectedIndex() == 0)
        {
            Ptr<RendererSetting> pNyxRdrSet = LM_DEBUG_PTR_CAST<RendererSetting>(_pSetting->getSetting(RENDERER_SETTING));
            pNyxRdrSet->apply(getControllerMain()->getRendererManager()->getDefaultRenderer());
            if (getControllerMain()->getDocumentManager()->getAllGuiDocument().size() > 0)
            {
                _pViewsLayout->initialize();
            }
        }
    }

    if (_configDlg->updateGeneral() == true)   
    {
        Ptr<GeneralSetting> pGSet = LM_DEBUG_PTR_CAST<GeneralSetting>(_pSetting->getSetting(GENERAL_SETTING));
        if (pGSet->showConsol == true)  AllocConsole();
        else                            FreeConsole();
        getControllerMain()->setMakeSleepForGDoc(pGSet->sleepBetweenTwoUpdateWorld);
        EPI::MiscConst::pathWorldDefault = pGSet->paths.defaultWorld;
        EPI::MiscConst::defaultTagGameplay = pGSet->paths.defaultTagGameplay;

        if (pGSet->playSound == true)
        {
            EPI::MiscConst::sSoundGlobalGain = 1.f;
        }
        else
        {
            EPI::MiscConst::sSoundGlobalGain = 0.f;
        }
    }
}
//-----------------------------------------------------------------------------
void ApplicationWindow::changeRdrSetting()
{
    if ( _pMenuQuickRdrSettings->selectedIndex() == 0)
    {
        Ptr<RendererSetting> pNyxRdrSet = LM_DEBUG_PTR_CAST<RendererSetting>(_pSetting->getSetting(RENDERER_SETTING));
        pNyxRdrSet->apply(getControllerMain()->getRendererManager()->getDefaultRenderer());
    }
    else
    {
        Renderer::EGlobalLevel level = (Renderer::EGlobalLevel) (_pMenuQuickRdrSettings->selectedIndex()-1);
        RendererSetting pNyxRdrSet (L"", level);
        pNyxRdrSet.apply(getControllerMain()->getRendererManager()->getDefaultRenderer());
    }
    
    if (getControllerMain()->getDocumentManager()->getAllGuiDocument().size() > 0)
    {
        _pViewsLayout->initialize();
    }
}
//-----------------------------------------------------------------------------
} // namespace Nyx
