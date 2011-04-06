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
#include "ViewportLayout.moc.h"


#include <QMouseEvent>
#include <QMenu>
#include <QContextMenuEvent>
#include <QScrollArea>

#include <Core/List.h>

#include <EPI/ApplicationBase.moc.h>
#include <EPI/ControllerMain.moc.h>
#include <EPI/Document/Properties/PtyNodeCamera.h>
#include <EPI/GUI/GuiDocumentManager.h>
#include <EPI/GUI/Viewport/NavigationStyle.h>
#include <EPI/GUI/GuiContext.moc.h>

namespace EPI
{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewportLayout::ViewportLayout(const Ptr<ControllerMain>& pCtrl, QWidget * parent):
    QSplitter(Qt::Horizontal, parent),
    topSplitter(NULL),
    botSplitter(NULL)
{
    setOpaqueResize(false);
    setChildrenCollapsible(false);
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("padding: 2px; padding-bottom: 0;");

    setStyleSheet(
        "QSplitter                    {background-color: #6C6C6C;}"
        "QSplitter::handle:horizontal {width: 2px; background-color: #535353;}"
        "QSplitter::handle:vertical   {height: 2px; background-color: #535353;}"
        "QSplitter::handle            {image: url("");}"
        "QSplitter::handle:pressed    {url("");}"
        );




   // setStyleSheet("QSplitter::handle:horizontal {width: 20px;}");


    



  //  setHandleWidth(4);

    //connection au signal aucun guiDocument dans la liste
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalNoGuiDocument()),
            this, SLOT(noGuiDocument()));

    //signal un guiDocument est actif
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>&)),
            this, SLOT(selectActiveGuiDocument(const Ptr<EPI::GuiDocument>&)));

    //text information
    connect(this, SIGNAL(signalTextInfo(const String&)),
            const_cast<ControllerMain*>(pCtrl.get()), SLOT(setTextInfo(const String&)));

  //  timer = new QTimer(this);  
}
//-----------------------------------------------------------------------------
Core::List<ViewportDesc> getSizeDesc(EViewportLayoutMode _mode, Ptr<GuiDocument> pCurrentGDoc, QList<int> sizeParent, QSize sizeWidget, QSplitter *topSplitter, QSplitter *botSplitter, Core::List<Ptr<WorldViewportContainer> > _viewports)
{
    Core::List<ViewportDesc> desc = pCurrentGDoc->getGuiContext()->getViewportContext().getDescs(_mode);

    QList<int> sizeTop;
    QList<int> sizeBottom;

    switch(_mode)
    {
    case SINGLE:
        //desc[0]._pNavigation = _viewports[0]->getNavigationStyle();
        break;

    case QUADRUPLE:
       /* desc[0]._pNavigation = _viewports[0]->getNavigationStyle();
        desc[1]._pNavigation = _viewports[1]->getNavigationStyle();
        desc[2]._pNavigation = _viewports[2]->getNavigationStyle();
        desc[3]._pNavigation = _viewports[3]->getNavigationStyle();*/

        sizeTop = topSplitter->sizes();
        sizeBottom = botSplitter->sizes();
        desc[0]._size.x = sizeParent[0]/(float)sizeWidget.width();
        desc[0]._size.y = sizeTop[0]/(float)sizeWidget.height();
        desc[1]._size.x = 1 - desc[0]._size.x;
        desc[1]._size.y = sizeBottom[0]/(float)sizeWidget.height();
        desc[2]._size.x = desc[0]._size.x;
        desc[2]._size.y = 1 - desc[0]._size.y;
        desc[3]._size.x = desc[1]._size.x;
        desc[3]._size.y = 1 - desc[1]._size.y;
        break;

    case DOUBLE_VERTICAL:
       /* desc[0]._pNavigation = _viewports[0]->getNavigationStyle();
        desc[1]._pNavigation = _viewports[1]->getNavigationStyle();*/

        desc[0]._size.x = sizeParent[0]/(float)sizeWidget.width();
        desc[0]._size.y = 1;
        desc[1]._size.x = 1-desc[0]._size.x;
        desc[1]._size.y = 1;
        break;
    }

    return desc;
}
//-----------------------------------------------------------------------------
bool ViewportLayout::navigationViewportsIsInAction() const
{
    for (int32 ii=0; ii<_viewports.size(); ++ii)
    {
        if (_viewports[ii]->getView()->getNavigationStyle()->isInAction() == true)
        {
            return true;
        }
    } 

    return false;
}
//-----------------------------------------------------------------------------

QWidget* createViewport(Core::List<Ptr<WorldViewport> >& viewports, const Ptr<GuiDocument>& pGDoc, const ViewportDesc& desc, QWidget* parent)
{
    QWidget* widget = new QWidget(parent);

    viewports.push_back(Ptr<WorldViewport>  (new WorldViewport(pGDoc, desc, widget)));
    QLayout* layout;

    layout = new QGridLayout(widget);
    layout->addWidget(viewports.back().get());
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);

    return widget;
}
void ViewportLayout::initialize()
{
    

    QList<int> viewportSize;
    int32 tmp, width, height;
    width = this->width();
    height = this->height();



    EViewportLayoutMode newMode = _pCurrentGDoc->getGuiContext()->getViewportContext().getActivateMode();

    Core::List<ViewportDesc> desc = _pCurrentGDoc->getGuiContext()->getViewportContext().getDescs(newMode);
   

    if (newMode != _mode)
    {                                                                
        _pCurrentGDoc->getGuiContext()->getViewportContext().setDescs(_mode, getSizeDesc(_mode, _pCurrentGDoc, sizes(), size(), topSplitter, botSplitter, _viewports));
    }

    _pCurrentGDoc->getGuiContext()->setCurrentViewport(NULL, null);
    clearAllViewports();
        delete topSplitter;//-> gauche
        delete botSplitter;//->droit

    botSplitter = NULL;
    topSplitter = NULL;

    
    
    _mode = newMode;

     
    switch(_mode)
    {
    case SINGLE:
        _viewports.push_back(Ptr<WorldViewportContainer>  (new WorldViewportContainer(_pCurrentGDoc, desc[0], this)));
        connect(_pCurrentGDoc.get(), SIGNAL(reDrawMe()), _viewports[0].get(), SLOT(repaint()));
        break;

    case QUADRUPLE:
        topSplitter = new QSplitter(Qt::Vertical, this);
        botSplitter = new QSplitter(Qt::Vertical, this);

        topSplitter->setChildrenCollapsible(false);
        topSplitter->setContentsMargins(0, 0, 0, 0);
        topSplitter->setOpaqueResize(false);
        botSplitter->setChildrenCollapsible(false);
        botSplitter->setContentsMargins(0, 0, 0, 0);
        botSplitter->setOpaqueResize(false);


        this->insertWidget(0, topSplitter);
        this->insertWidget(1, botSplitter);

        _viewports.push_back(Ptr<WorldViewportContainer>  (new WorldViewportContainer(_pCurrentGDoc, desc[0], NULL)));
        _viewports.push_back(Ptr<WorldViewportContainer>  (new WorldViewportContainer(_pCurrentGDoc, desc[1], NULL)));
        _viewports.push_back(Ptr<WorldViewportContainer>  (new WorldViewportContainer(_pCurrentGDoc, desc[2], NULL)));
        _viewports.push_back(Ptr<WorldViewportContainer>  (new WorldViewportContainer(_pCurrentGDoc, desc[3], NULL)));

        connect(_pCurrentGDoc.get(), SIGNAL(reDrawMe()), _viewports[0].get(), SLOT(repaint()));
        connect(_pCurrentGDoc.get(), SIGNAL(reDrawMe()), _viewports[1].get(), SLOT(repaint()));
        connect(_pCurrentGDoc.get(), SIGNAL(reDrawMe()), _viewports[2].get(), SLOT(repaint()));
        connect(_pCurrentGDoc.get(), SIGNAL(reDrawMe()), _viewports[3].get(), SLOT(repaint()));

        topSplitter->insertWidget(0, _viewports[0].get());
        topSplitter->insertWidget(1, _viewports[1].get());
        botSplitter->insertWidget(0, _viewports[2].get());
        botSplitter->insertWidget(1, _viewports[3].get());

        viewportSize.clear();
        tmp = desc[0]._size.x*width;
        viewportSize.push_back(tmp);
        viewportSize.push_back(width-tmp);
        setSizes(viewportSize);

        viewportSize.clear();
        tmp = desc[0]._size.y*height;
        viewportSize.push_back(tmp);
        viewportSize.push_back(height - tmp);
        topSplitter->setSizes(viewportSize);

        viewportSize.clear();
        tmp = desc[1]._size.y*height;
        viewportSize.push_back(tmp);
        viewportSize.push_back(height - tmp);
        botSplitter->setSizes(viewportSize);
        break;

    case DOUBLE_VERTICAL:
        Ptr<WorldViewportContainer> viewport (new WorldViewportContainer(_pCurrentGDoc, desc[0], NULL));
        _viewports.push_back(viewport);
        Ptr<WorldViewportContainer> viewport2 (new WorldViewportContainer(_pCurrentGDoc, desc[1], NULL));
        _viewports.push_back(viewport2);

        connect(_pCurrentGDoc.get(), SIGNAL(reDrawMe()), _viewports[0].get(), SLOT(repaint()));
        connect(_pCurrentGDoc.get(), SIGNAL(reDrawMe()), _viewports[1].get(), SLOT(repaint()));

        insertWidget(0, _viewports[0].get());
        insertWidget(1, _viewports[1].get());

        viewportSize.clear();
        tmp = desc[0]._size.x*width;
        viewportSize.push_back(tmp);
        viewportSize.push_back(width-tmp);
        setSizes(viewportSize);
        break;
    }

    _viewports[0]->getView()->setFocus();
    _pCurrentGDoc->getGuiContext()->setCurrentViewport(_viewports[0].get(), _pCurrentGDoc->getDocument()->getWorld()->getNodeListener());
}
//-----------------------------------------------------------------------------
ViewportLayout::~ViewportLayout()
{
}
//-----------------------------------------------------------------------------
void ViewportLayout::clearAllViewports()
{
    _viewports.clear();
}
//-----------------------------------------------------------------------------
void ViewportLayout::noGuiDocument()
{
    clearAllViewports();
    _pCurrentGDoc = null;
}
//-----------------------------------------------------------------------------
void ViewportLayout::selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc)
{
    if (_pCurrentGDoc!=null)
    {
        _pCurrentGDoc->getGuiContext()->getViewportContext().setDescs(_mode, getSizeDesc(_mode, _pCurrentGDoc, sizes(), size(), topSplitter, botSplitter, _viewports));

        disconnect(_pCurrentGDoc->getGuiContext().get(), SIGNAL(currentViewportLayoutChanged()), this, SLOT(initialize()));
        removeAction (_pCurrentGDoc->getActionExtend());
    }

    clearAllViewports();
    _pCurrentGDoc = pGDoc;

    if (pGDoc!=null)
    {
        connect(_pCurrentGDoc->getGuiContext().get(), SIGNAL(currentViewportLayoutChanged()), this, SLOT(initialize()));
        _mode = _pCurrentGDoc->getGuiContext()->getViewportContext().getActivateMode();
        addAction(_pCurrentGDoc->getActionExtend());
        initialize();
    }
}
//-----------------------------------------------------------------------------
void ViewportLayout::myContextMenu(QMouseEvent* event)
{
    if (_pCurrentGDoc!=null)
    {
        WorldViewport* pTemp = _pCurrentGDoc->getGuiContext()->getCurrentViewport()->getView(); // TODO
        if (pTemp!=NULL)
        {
            pTemp->turnOffNavigation();
        }

        QWidget* pChildAtEvent = childAt (event->pos().x(), event->pos().y()); 
        EViewportLayoutMode old = _pCurrentGDoc->getGuiContext()->getViewportContext().getActivateMode();

        QMenu menu ("DocumentMenu", this);
            QMenu* layoutMenu = menu.addMenu("Layout");
                layoutMenu->addActions(_pCurrentGDoc->getGuiContext()->getActionsViewportLayoutMode());
            menu.addSeparator();
            QMenu* cameraNavigationMenu = menu.addMenu("Navigation");
                cameraNavigationMenu->addActions(_pCurrentGDoc->getGuiContext()->getActionsViewportCameraNavigation());
            menu.addSeparator();
            QMenu* shadowModeMenu = menu.addMenu("ShadowMode");
                shadowModeMenu->addActions(_pCurrentGDoc->getGuiContext()->getActionsViewportShadowMode());
            QMenu* renderStatsModeMenu = menu.addMenu("RenderStats");
                renderStatsModeMenu->addActions(_pCurrentGDoc->getGuiContext()->getActionsViewportRenderStatsMode());

            
        QAction * ret = menu.exec(event->globalPos());
    }

}
//-----------------------------------------------------------------------------
void ViewportLayout::mousePressEvent(QMouseEvent* event)
{/*
    if(_pCurrentGDoc != null)
    {
        QWidget* pChildAtEvent = childAt (event->pos().x(), event->pos().y()); 
        
        for (int32 ii=0; ii<_viewports.size(); ++ii)
        {
            if (pChildAtEvent==_viewports[ii]->getView())
            {
                _pCurrentGDoc->getGuiContext()->setCurrentViewport(_viewports[ii].get());
                break;
            }
        }
        
    }*/


    if (event->button() == Qt::RightButton &&
        !(event->buttons() & Qt::LeftButton) &&
        !(event->buttons() & Qt::MidButton) &&
        navigationViewportsIsInAction() == false)    
    {
        myContextMenu(event);
    }


    QSplitter::mousePressEvent(event);
}
//-----------------------------------------------------------------------------
void ViewportLayout::mouseReleaseEvent(QMouseEvent* event)
{
    QSplitter::mouseReleaseEvent(event);
}
} // namespace EPI
