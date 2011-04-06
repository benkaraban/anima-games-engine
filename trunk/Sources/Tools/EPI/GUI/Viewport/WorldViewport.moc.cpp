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
#include <EPI/GUI/Viewport/WorldViewport.moc.h>

#include <QMimeData>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QGridLayout>
#include <QMenu>

#include <EPI/ApplicationBase.moc.h>
#include <EPI/GUI/Viewport/NavigationStyle.h>
#include <EPI/GUI/Viewport/InteractionStyle.h>
#include <EPI/GUI/Viewport/ViewportToolInteraction.moc.h>
#include <EPI/GUI/WorldOverlay/NodeCameraEdition.h>

#include <EPI/Document/Properties/PtyDocEdition.moc.h>
#include <EPI/Document/Properties/PtyNodeCamera.h>
#include <EPI/Document/DocumentWorld.moc.h>
#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/Properties/ITransformable.h>

#include <EPI/GUI/Viewport/Tools/ViewportTool.moc.h>

#include <EPI/DocumentRenderer.h>
#include <EPI/Document/IEditable.h>

#include <EPI/Document/PropertySelection.moc.h>
#include <EPI/Gui/Viewport/CameraManager.h>

#include <EPI/Gui/Viewport/ViewportPicking.h>
#include <EPI/Gui/Viewport/SelectionManager.h>

#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/GUI/GuiContext.moc.h>

#include <Core/Debug/FPSCounter.h>


#include <Universe/World.h>
#include <EPI/GUI/WorldOverlay/WorldOverlay.h>
namespace EPI
{
struct IHMFlag
{
    IHMFlag()  {reset();}

    void reset()
    {
        viewTypePress    = false;
        renderModePress  = false;
        polygonModePress = false;
        AspectTypePress  = false;
    }

    bool viewTypePress;
    bool renderModePress;
    bool polygonModePress;
    bool AspectTypePress;
};

bool isInRectangle(const QPoint& pos, const Renderer::IRectangle& rect)
{
    if (pos.x() >= rect.getPosition().x && pos.x()<rect.getPosition().x + rect.getSize().x)
    {
        if (pos.y() >= rect.getPosition().y && pos.y()<rect.getPosition().y + rect.getSize().y)
        {
            return true;
        }
    }

    return false;
}

class WorldViewport::implementation
{
public:
    implementation(const Ptr<GuiDocument>& pGDoc, const ViewportDesc& desc) : _pGDoc(pGDoc), _desc(desc), deviceState(Gfx::EDeviceState(-1)) {}
    ~implementation() {}

public:
    Ptr<GuiDocument>            _pGDoc;

    Ptr<Renderer::IRenderView>  _pView;
    Ptr<IInteractionStyle>      _pInteraction;

    ViewportDesc                _desc;
    Ptr<Renderer::IHUD>         _pHUD;
    Ptr<Renderer::IText>        _pTextViewType;
    Ptr<Renderer::IText>        _pTextAspectType;
    Ptr<Renderer::IText>        _pTextRenderMode;
    Ptr<Renderer::IText>        _pTextPolygonMode;
    Ptr<Renderer::IRectangle>   _pBackgroundTextViewType;
    Ptr<Renderer::IRectangle>   _pBackgroundTextRenderMode;
    Ptr<Renderer::IRectangle>   _pBackgroundTextPolygonMode;
    Ptr<Renderer::IRectangle>   _pBackgroundTextAspectType;
    Ptr<Renderer::IRectangle>   _borders    [EBorderViewport_COUNT];

    Ptr<Renderer::IText>        _pTextStats;

    Ptr<Renderer::IText>        _pTextInfo;
    Ptr<Renderer::IRectangle>   _pBackgroundTextInfo;

    Core::FPSCounter            _fps;

    Gfx::EDeviceState           deviceState;
    IHMFlag                     ihmFlag;
};
//-----------------------------------------------------------------------------
WorldViewportContainer::WorldViewportContainer(const Ptr<GuiDocument>& pGDoc, const ViewportDesc& desc, QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent)
{
    //setStyleSheet("QWidget {background-color: black;}");
    

    _view  = new WorldViewport(pGDoc, desc, this, f);
    _layout = new QGridLayout(this);
    _layout->addWidget(_view);

    
    //_layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //_layout->setContentsMargins(10,40,10,40);
    setLayout(_layout);

    QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::blue);
        pal.setColor(QPalette::WindowText, Qt::blue);
        pal.setColor(QPalette::Button, Qt::blue);
        pal.setColor(QPalette::Light, Qt::blue);
        pal.setColor(QPalette::Midlight, Qt::blue);
        pal.setColor(QPalette::Dark, Qt::blue);
        pal.setColor(QPalette::Mid, Qt::blue);
        pal.setColor(QPalette::Text, Qt::blue);
        pal.setColor(QPalette::BrightText, Qt::blue);
        pal.setColor(QPalette::ButtonText, Qt::blue);
        pal.setColor(QPalette::Base, Qt::blue);
        pal.setColor(QPalette::Window, Qt::blue);
        pal.setColor(QPalette::Shadow, Qt::blue);
        pal.setColor(QPalette::Highlight, Qt::blue);
        pal.setColor(QPalette::HighlightedText, Qt::blue);
        pal.setColor(QPalette::Link, Qt::blue);
        pal.setColor(QPalette::LinkVisited, Qt::blue);
        pal.setColor(QPalette::AlternateBase, Qt::blue);
        pal.setColor(QPalette::NoRole, Qt::blue);
        pal.setColor(QPalette::ToolTipBase, Qt::blue);
        pal.setColor(QPalette::ToolTipText, Qt::blue);

    setPalette(pal);

  //  makeLayoutMargins(desc._aspectRatio);
}
//-----------------------------------------------------------------------------
WorldViewportContainer::~WorldViewportContainer()
{
}
//-----------------------------------------------------------------------------
void WorldViewportContainer::mousePressEvent(QMouseEvent* event)
{
    if (_view!=NULL)
    {
        if (_view->_impl->_pGDoc!=null)
        {
            _view->_impl->_pGDoc->getGuiContext()->setCurrentViewport(const_cast<WorldViewportContainer*>(this), _view->_impl->_pGDoc->getDocument()->getWorld()->getNodeListener());
        }

        //Code temporaire pour permetttre le click droit sur le l'ihm viewType
        QPoint posE = _view->pos();
        QPoint pos (
            event->pos().x() - _view->pos().x(), 
            event->pos().y() - _view->pos().y());

        if (_view->posIsInIhmViewport(pos) == false)
        {
            QWidget::mousePressEvent(event);
        }
    }
 
    
}
//-----------------------------------------------------------------------------
void WorldViewportContainer::setDesc(const ViewportDesc& desc)
{
    ViewportDesc oldDesc = _view->getDesc();

    if (oldDesc._aspectRatio != desc._aspectRatio)
    {
        makeLayoutMargins(desc._aspectRatio);
    }

    _view->setDesc(desc);
}
//-----------------------------------------------------------------------------
const ViewportDesc& WorldViewportContainer::getDesc() const
{
    return _view->getDesc();
}
//-----------------------------------------------------------------------------
void WorldViewportContainer::repaint()
{
    _view->repaint();
}
//-----------------------------------------------------------------------------
void WorldViewportContainer::makeLayoutMargins(ECameraAspectRatio type)
{
    QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::blue);
        pal.setColor(QPalette::WindowText, Qt::blue);
        pal.setColor(QPalette::Button, Qt::blue);
        pal.setColor(QPalette::Light, Qt::blue);
        pal.setColor(QPalette::Midlight, Qt::blue);
        pal.setColor(QPalette::Dark, Qt::blue);
        pal.setColor(QPalette::Mid, Qt::blue);
        pal.setColor(QPalette::Text, Qt::blue);
        pal.setColor(QPalette::BrightText, Qt::blue);
        pal.setColor(QPalette::ButtonText, Qt::blue);
        pal.setColor(QPalette::Base, Qt::blue);
        pal.setColor(QPalette::Window, Qt::blue);
        pal.setColor(QPalette::Shadow, Qt::blue);
        pal.setColor(QPalette::Highlight, Qt::blue);
        pal.setColor(QPalette::HighlightedText, Qt::blue);
        pal.setColor(QPalette::Link, Qt::blue);
        pal.setColor(QPalette::LinkVisited, Qt::blue);
        pal.setColor(QPalette::AlternateBase, Qt::blue);
        pal.setColor(QPalette::NoRole, Qt::blue);
        pal.setColor(QPalette::ToolTipBase, Qt::blue);
        pal.setColor(QPalette::ToolTipText, Qt::blue);
                     
    setPalette(pal);


    Core::Vector2f size = getRenderAreaSize(type);

    const int32 widgetWidth  = width();
    const int32 widgetHeight = height();

    int32 margeL = (widgetWidth - size.x)/2.f;
    int32 margeR = widgetWidth  - size.x - margeL;

    int32 margeT = (widgetHeight - size.y)/2.f;
    int32 margeB = widgetHeight  - size.y - margeT;

    _layout->setContentsMargins(margeL, margeT, margeR, margeB);
}
//-----------------------------------------------------------------------------
Core::Vector2f WorldViewportContainer::getRenderAreaSize(ECameraAspectRatio type)
{
    Core::Vector2f size = Core::Vector2f(width(), height());

    float maxWidth = size.x;
    float maxHeight = size.y;
    float renderTargetWidth = maxWidth;
    float renderTargetHeight = maxHeight;


    if (renderTargetWidth< 40) renderTargetWidth= 40;
    if (renderTargetHeight< 40) renderTargetHeight= 40;

    float fct = 0.f;

    switch(type)
    {
    case CAMERA_RATIO_FREE:
        renderTargetWidth = renderTargetWidth;
        renderTargetHeight= renderTargetHeight;
    break;

    case CAMERA_RATIO_5_4:      fct = 5/4.f;    break;
    case CAMERA_RATIO_4_3:      fct = 4/3.f;    break;
    case CAMERA_RATIO_3_2:      fct = 3/2.f;    break;
    case CAMERA_RATIO_16_10:    fct = 16/10.f;  break;
    case CAMERA_RATIO_16_9:     fct = 16/9.f;   break;
    }


    if (fct!=0)
    {
        if ((maxWidth * 1/fct) <= maxHeight)
        {
            renderTargetWidth  = maxWidth;
            renderTargetHeight = (maxWidth * 1/fct);
        }
        else if ((maxHeight * fct) < maxWidth)
        {
            renderTargetWidth  = (maxHeight * fct);
            renderTargetHeight = maxHeight;
        }
        else
        {
            renderTargetWidth  = 640; 
            renderTargetHeight = 480;
        }
    }

    size.x = renderTargetWidth;
    size.y = renderTargetHeight;
    
/*
    int32 marge = 10;//(maxWidth-size.x)/2;
    int32 marge2 = 10;//(maxHeight-size.y)/2;


    setGeometry(marge,marge2,size.x,size.y);*/

    return size;
}

void WorldViewportContainer::resizeEvent ( QResizeEvent * event  )
{
    
    ViewportDesc desc = _view->getDesc();
    makeLayoutMargins(desc._aspectRatio);


    QWidget::resizeEvent(event);
}






const Core::Vector4f colorNotSelected (0.6f, 0.6f, 0.6f, 1.f);
const Core::Vector4f colorSelected (0.8f, 0.8f, 0.1f, 1.f);
//-----------------------------------------------------------------------------
bool isNodeCameraEdition(EViewportCameraView view)
{
    if (view == VIEWPORT_CAMERA_TOP    || 
        view == VIEWPORT_CAMERA_BOTTOM ||
        view == VIEWPORT_CAMERA_BACK   ||
        view == VIEWPORT_CAMERA_FRONT  ||
        view == VIEWPORT_CAMERA_RIGHT  ||
        view == VIEWPORT_CAMERA_LEFT)
    return true;

    return false;
}
//-----------------------------------------------------------------------------
void setCameraEditionView(Ptr<NodeCameraEdition> pCamera, ViewportDesc desc)
{
    ECameraEditionView view = CAM_EDIT_TOP;

    if      (desc._cameraView == VIEWPORT_CAMERA_TOP)      view = CAM_EDIT_TOP;
    else if (desc._cameraView == VIEWPORT_CAMERA_BOTTOM)   view = CAM_EDIT_BOTTOM;
    else if (desc._cameraView == VIEWPORT_CAMERA_BACK)     view = CAM_EDIT_BACK;
    else if (desc._cameraView == VIEWPORT_CAMERA_FRONT)    view = CAM_EDIT_FRONT;
    else if (desc._cameraView == VIEWPORT_CAMERA_RIGHT)    view = CAM_EDIT_RIGHT;
    else if (desc._cameraView == VIEWPORT_CAMERA_LEFT)     view = CAM_EDIT_LEFT;

    pCamera->setActiveView(view);
}
//-----------------------------------------------------------------------------            
Ptr<IViewportPicking> createViewportPicking(EViewportPickingType type, Ptr<GuiDocument> pGDoc)
{
    Ptr<IViewportPicking> pPick;

    switch(type)
    {
    case VIEWPORT_PICKING_DEFAULT:
        pPick = Ptr<IViewportPicking> (new ViewportPickingOnGuiDocument(pGDoc));
        break;
    case VIEWPORT_PICKING_PTY:
        pPick = Ptr<IViewportPicking> (new ViewportPickingOnPty(pGDoc->getPtyForEdition()));
        break;
    }

    return pPick;
}
//-----------------------------------------------------------------------------            
Ptr<ISelectionManager> createViewportSelectionManager(EViewportPickingType type, Ptr<GuiDocument> pGDoc)
{
    Ptr<ISelectionManager> pSelectMng = null;
    
    Ptr<Property> pPty;
    Ptr<PtyDocEdition> pPtyDocEdit;
    Ptr<PropertySelection> pPtySelection;

    switch(type)
    {
    case VIEWPORT_PICKING_DEFAULT:
        pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (pGDoc->getDocument()->getPropertyDocumentEdition());
        pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());
        pSelectMng = Ptr<DocumentSelectionManager> (new DocumentSelectionManager(pPtySelection));
        break;
    case VIEWPORT_PICKING_PTY:
 /*       pPty = pGDoc->getPtyForEdition();


        if (pPty!=null && pPty->isEditable() == true)
        {
            Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPty->getPtySelection());
            pSelectMng = Ptr<DocumentSelectionManager> (new DocumentSelectionManager(pPtySelection));
        }
        else*/
        if(pGDoc->getDocument()->getType() == DOC_SPLINE)
        {
            Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pGDoc->getDocument()->getDefaultProperty()->getPtySelection());
            pSelectMng = Ptr<DocumentSelectionManager> (new DocumentSelectionManager(pPtySelection));
        }
        else if(pGDoc->getDocument()->getType() == DOC_MODEL)
        {
            pPty = pGDoc->getPtyForEdition();
            Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPty->getPtySelection());
            pSelectMng = Ptr<DocumentSelectionManager> (new DocumentSelectionManager(pPtySelection));
        }

        break;
    }
        


    return pSelectMng;
}
//-----------------------------------------------------------------------------
Ptr<INavigationStyle> createNavigationStyle(EViewportCameraView view, EViewportCameraNavigation nav, Ptr<Universe::NodeCamera> pCam, Ptr<GuiDocument> pGDoc)
{
    Ptr<INavigationStyle> pNav;

    if (isNodeCameraEdition(view) == true)
    {
        pNav = Ptr<INavigationStyle> (new NavigationStyleEditionOrthogonal());
    }
    else
    {
        Ptr<PtyDocEdition> pPtyDocEdit;
        Ptr<PropertySelection> pPtySelection;
        switch(nav)
        {
        case VIEWPORT_CAMERA_QUAKE:
            pNav = Ptr<NavigationStyleQuake> (new NavigationStyleQuake());
            break;
        case VIEWPORT_CAMERA_ORBITAL:
            pNav = Ptr<NavigationStyleOrbital> (new NavigationStyleOrbital());
            pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (pGDoc->getDocument()->getPropertyDocumentEdition());
            pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());

            if (pPtySelection->getSelectedPtiesCount() > 0)
            {
                Core::Vector3f center = getCenterProperties(pPtySelection->getSelectedPties());
                LM_DEBUG_PTR_CAST<NavigationStyleOrbital> (pNav)->setTargetPosition(center);
            }
            else
            {
                Core::Vector3f center = pGDoc->getDocument()->getDefaultCameraTargetPosition();
                LM_DEBUG_PTR_CAST<NavigationStyleOrbital> (pNav)->setTargetPosition(center);
            }
            break;
        }
    }

    return pNav;
}
//-----------------------------------------------------------------------------
Renderer::DebugSettings viewportDescToDebugSetting(const ViewportDesc& desc)
{
    Renderer::DebugSettings debugSettings;

    //render mode
    switch(desc._renderMode)
    {
        case VIEWPORT_RDR_NORMAL:
            debugSettings.renderMode = Renderer::RDR_NORMAL;
            break;
        case VIEWPORT_RDR_HQ_PREVIEW:
            debugSettings.renderMode = Renderer::RDR_HQ_PREVIEW;
            break;
        case VIEWPORT_RDR_DEBUG_SHADOW_MAP:
            debugSettings.renderMode = Renderer::RDR_DEBUG_SHADOW_MAP;
            break;
        case VIEWPORT_RDR_DEBUG_UV:
            debugSettings.renderMode = Renderer::RDR_DEBUG_UV;
            break;
        case VIEWPORT_RDR_DEBUG_NORMAL:
            debugSettings.renderMode = Renderer::RDR_DEBUG_NORMAL;
            break;
        case VIEWPORT_RDR_DEBUG_T:
            debugSettings.renderMode = Renderer::RDR_DEBUG_T;
            break;
        case VIEWPORT_RDR_DEBUG_B:
            debugSettings.renderMode = Renderer::RDR_DEBUG_B;
            break;
        case VIEWPORT_RDR_DEBUG_N:
            debugSettings.renderMode = Renderer::RDR_DEBUG_N;
            break;
        case VIEWPORT_RDR_DEBUG_MIP:
            debugSettings.renderMode = Renderer::RDR_DEBUG_MIP;
            break;
    }

    //polygon mode
    switch(desc._polygonMode)
    {
        case VIEWPORT_POLY_SOLID:
            debugSettings.polyMode = Renderer::POLY_SOLID;
            break;
        case VIEWPORT_POLY_SOLID_WIREFRAME:
            debugSettings.polyMode = Renderer::POLY_SOLID_WIREFRAME;
            break;
        case VIEWPORT_POLY_WIREFRAME:
            debugSettings.polyMode = Renderer::POLY_WIREFRAME;
            break;
    }
/*
    //shadow mode
    switch(desc._shadowMode)
    {
        case VIEWPORT_SHADOW_ON:
            debugSettings.shadowOn = true;
            break;
        case VIEWPORT_SHADOW_OFF:
            debugSettings.shadowOn = false;
            break;
    }
*/
    return debugSettings;
}
//-----------------------------------------------------------------------------
WorldViewport::WorldViewport(const Ptr<GuiDocument>& pGDoc, const ViewportDesc& desc, QWidget* parent, Qt::WindowFlags f) :
    WidgetRenderArea(parent, f),
    _impl(new implementation(pGDoc, desc))
{
    Core::Vector4f colorBgMenuButton (0.36f, 0.36f, 0.36f, 0.6f);

    _impl->_pHUD = _impl->_pGDoc->getDocument()->getDocumentRenderer()->getRenderer()->createHUD();

    _impl->_pBackgroundTextViewType = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_pBackgroundTextViewType->setColor(colorBgMenuButton);
        _impl->_pBackgroundTextViewType->setSize(Core::Vector2f(100, 20));
        _impl->_pBackgroundTextViewType->setPriority(0);

    _impl->_pBackgroundTextAspectType = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_pBackgroundTextAspectType->setColor(colorBgMenuButton);
        _impl->_pBackgroundTextAspectType->setSize(Core::Vector2f(100, 20));
        _impl->_pBackgroundTextAspectType->setPriority(0);

    _impl->_pBackgroundTextRenderMode = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_pBackgroundTextRenderMode->setColor(colorBgMenuButton);
        _impl->_pBackgroundTextRenderMode->setSize(Core::Vector2f(100, 20));
        _impl->_pBackgroundTextRenderMode->setPriority(0);

    _impl->_pBackgroundTextPolygonMode = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_pBackgroundTextPolygonMode->setColor(colorBgMenuButton);
        _impl->_pBackgroundTextPolygonMode->setSize(Core::Vector2f(100, 20));
        _impl->_pBackgroundTextPolygonMode->setPriority(0);

    _impl->_pBackgroundTextInfo = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_pBackgroundTextInfo->setColor(Core::Vector4f(0.36f, 0.36f, 0.36f, 0.6f));
        _impl->_pBackgroundTextInfo->setSize(Core::Vector2f(50, 20));
        _impl->_pBackgroundTextInfo->setPriority(0);

    _impl->_borders[BORDER_RIGHT] = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_borders[BORDER_RIGHT]->setColor(colorNotSelected);
        _impl->_borders[BORDER_RIGHT]->setSize(Core::Vector2f(10, 10));
        _impl->_borders[BORDER_RIGHT]->setPriority(2);

    _impl->_borders[BORDER_LEFT] = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_borders[BORDER_LEFT]->setColor(colorNotSelected);
        _impl->_borders[BORDER_LEFT]->setSize(Core::Vector2f(10, 10));
        _impl->_borders[BORDER_LEFT]->setPriority(2);

    _impl->_borders[BORDER_TOP] = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_borders[BORDER_TOP]->setColor(colorNotSelected);
        _impl->_borders[BORDER_TOP]->setSize(Core::Vector2f(10, 10));
        _impl->_borders[BORDER_TOP]->setPriority(2);

    _impl->_borders[BORDER_BOTTOM] = _impl->_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
        _impl->_borders[BORDER_BOTTOM]->setColor(colorNotSelected);
        _impl->_borders[BORDER_BOTTOM]->setSize(Core::Vector2f(10, 10));
        _impl->_borders[BORDER_BOTTOM]->setPriority(2);

    Core::Vector4f colorTextMenuButton (0.2f, 0.2f, 0.2f, 1.0f);

    _impl->_pTextViewType = _impl->_pHUD->createText(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getDefaultFont(), L"This is a test.");
        _impl->_pTextViewType->setBold(false);
        _impl->_pTextViewType->setColor(colorTextMenuButton);
        _impl->_pTextViewType->setPriority(1);

    _impl->_pTextAspectType = _impl->_pHUD->createText(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getDefaultFont(), L"This is a test.");
        _impl->_pTextAspectType->setBold(false);
        _impl->_pTextAspectType->setColor(colorTextMenuButton);
        _impl->_pTextAspectType->setPriority(1);

    _impl->_pTextRenderMode = _impl->_pHUD->createText(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getDefaultFont(), L"This is a test.");
        _impl->_pTextRenderMode->setBold(false);
        _impl->_pTextRenderMode->setColor(colorTextMenuButton);
        _impl->_pTextRenderMode->setPriority(1);

    _impl->_pTextPolygonMode = _impl->_pHUD->createText(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getDefaultFont(), L"This is a test.");
        _impl->_pTextPolygonMode->setBold(false);
        _impl->_pTextPolygonMode->setColor(colorTextMenuButton);
        _impl->_pTextPolygonMode->setPriority(1);
        

    _impl->_pTextInfo = _impl->_pHUD->createText(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getDefaultFont(), L"This is a test.");
        _impl->_pTextInfo->setBold(false);
        _impl->_pTextInfo->setColor(Core::Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
        _impl->_pTextInfo->setPriority(1);

    _impl->_pTextStats = _impl->_pHUD->createText(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getDefaultFont(), L"This is a test.");
        _impl->_pTextStats->setBold(false);
        _impl->_pTextStats->setColor(Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
        _impl->_pTextStats->setPriority(1);

    Ptr<IViewportPicking>  pPicking = createViewportPicking(_impl->_desc._pickingType, _impl->_pGDoc);
   

    setMouseTracking(true);
    Ptr<ViewportToolInteraction> pVTInteraction(new ViewportToolInteraction(_impl->_pGDoc));

    Ptr<ISelectionManager> pSelectMng = createViewportSelectionManager(_impl->_desc._pickingType, _impl->_pGDoc);


    Ptr<INavigationStyle>   pNav = createNavigationStyle(_impl->_desc._cameraView, _impl->_desc._cameraNavigation, getNodeCamera(), _impl->_pGDoc);
    pNav->updateCamera(getNodeCamera());


    setAcceptDrops(true);
    

    _impl->_pInteraction = Ptr<DefaultInteractionStyle> (new DefaultInteractionStyle(pNav, pVTInteraction, pPicking, pSelectMng, getNodeCamera()));
    setFocusPolicy(Qt::StrongFocus);
    
    initText();

    setMinimumSize(160, 120);
    //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
//-----------------------------------------------------------------------------
WorldViewport::~WorldViewport()
{
    delete _impl;
}
//-----------------------------------------------------------------------------
void WorldViewport::turnOffNavigation()
{
    if (_impl->_pInteraction!=null)
    {
        _impl->_pInteraction->getNavigationStyle()->turnOffNavigation();
    }

    turnOffIHM();
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> WorldViewport::getNodeCamera()
{
    Ptr<Universe::NodeCamera> pCam;

    if (isNodeCameraEdition(_impl->_desc._cameraView) == true)
    {
        pCam = _impl->_pGDoc->getGuiContext()->getCameraManager()->getDefaultCameraEdition();
        setCameraEditionView(LM_DEBUG_PTR_CAST<NodeCameraEdition>(pCam), _impl->_desc);
    }
    else
    {
        if (_impl->_desc._cameraView == VIEWPORT_CAMERA_CONTENT)
        {
            Ptr<PtyNodeCamera> pPtyNodeCam = _impl->_pGDoc->getDocument()->getCameraContent(_impl->_desc._idCameraContent);

            if (pPtyNodeCam!=null)
            {
                pCam = LM_DEBUG_PTR_CAST<Universe::NodeCamera>(pPtyNodeCam->getUniverseNode());
            }
            else
            {
                pCam = _impl->_pGDoc->getGuiContext()->getCameraManager()->getDefaultCameraUser();
            }
        }
        else if (_impl->_desc._cameraView == VIEWPORT_CAMERA_EDITION)
        {
            pCam = _impl->_pGDoc->getDocument()->getCameraEdition();
        }
        else
        {
            pCam = _impl->_pGDoc->getGuiContext()->getCameraManager()->getDefaultCameraUser();
        }
    }

    return pCam;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> WorldViewport::getNodeRenderCamera()
{
    Ptr<Universe::NodeCamera> pCam = getNodeCamera();

    if (isNodeCameraEdition(_impl->_desc._cameraView) == true)
    {
        pCam = LM_DEBUG_PTR_CAST<NodeCameraEdition>(_impl->_pGDoc->getGuiContext()->getCameraManager()->getDefaultCameraEdition())->getActiveViewCamera();
    }

    return pCam;
}
//-----------------------------------------------------------------------------
void WorldViewport::dragEnterEvent(QDragEnterEvent * dragEnterEvent)
{
    if (_impl->_pGDoc!=null)
    {
        if(_impl->_pGDoc->getDocument()->dropInViewportIsPossible() == true)
        {
            const QMimeData * mimeData = dragEnterEvent->mimeData();
            QString format;
            if(_impl->_pGDoc->getDocument()->dropThisFormatIsAuthorized(mimeData, format) == true)
            {
                dragEnterEvent->accept();
            }
            else
                dragEnterEvent->ignore();
        }
    }
    QtToolbox::WidgetRenderArea::dragEnterEvent(dragEnterEvent);
}
//-----------------------------------------------------------------------------
void WorldViewport::dragMoveEvent(QDragMoveEvent * dragMoveEvent)
{
    if(_impl->_pGDoc->getDocument()->dropInViewportIsPossible() == true)
    {   
        const QMimeData * mimeData = dragMoveEvent->mimeData();
        QString format;
        if(_impl->_pGDoc->getDocument()->dropThisFormatIsAuthorized(mimeData, format) == true)
        {
            dragMoveEvent->accept();
        }
        else
            dragMoveEvent->ignore();
    }
    else
        QtToolbox::WidgetRenderArea::dragMoveEvent(dragMoveEvent);
    
}
//-----------------------------------------------------------------------------
void WorldViewport::dragLeaveEvent(QDragLeaveEvent * dragLeaveEvent)
{
    QtToolbox::WidgetRenderArea::dragLeaveEvent(dragLeaveEvent);
}
//-----------------------------------------------------------------------------
void WorldViewport::dropEvent(QDropEvent * dropEvent)
{
    if(_impl->_pGDoc->getDocument()->dropInViewportIsPossible() == true)
    {
        const QMimeData * mimeData = dropEvent->mimeData();
        QString format;
        if(_impl->_pGDoc->getDocument()->dropThisFormatIsAuthorized(mimeData,format))
        {
            dropEvent->accept();
            
            _impl->_pGDoc->getDocument()->dropThisFile(mimeData, format);
/*
            QString text = mimeData->data(format);
            Core::String fileName(text.toStdString().c_str());

            if(format == "asset/terrain")
            {
                _impl->_pGDoc->getDocument()->createPropertyTerrain(fileName);
            }
            else if(format == "asset/model" || format == "asset/particles")
            {
                _impl->_pGDoc->getDocument()->createPropertyNodeInstance(fileName);
            }*/
            setFocus();
        }
        else
            dropEvent->ignore();
    }
    else
        QtToolbox::WidgetRenderArea::dropEvent(dropEvent);
}
//-----------------------------------------------------------------------------
/*bool WorldViewport::isAuthorizedMimeData(const QMimeData * mimeData, QString & format)
{
    bool isMimeDataAuthorized = false;

    int iMimeData = 0;
    while(iMimeData < _impl->_authorizedMimeData.size() && isMimeDataAuthorized == false)
    {
        if(mimeData->hasFormat(_impl->_authorizedMimeData[iMimeData]))
        {
            isMimeDataAuthorized = true;
            format = _impl->_authorizedMimeData[iMimeData];
        }

        ++iMimeData;
    }

    return isMimeDataAuthorized;
}*/
//-----------------------------------------------------------------------------
String WorldViewport::getDistanceCamera()
{
    Ptr<Universe::NodeCamera> pNodeCam = getNodeCamera();

    float distance=0;

 
    switch(_impl->_pInteraction->getNavigationStyle()->getType())
    {
        case NAVIGATION_QUAKE:
            distance = pNodeCam->getWorldPosition().y;
        break;
        case NAVIGATION_ORBITAL:
            distance = (pNodeCam->getWorldPosition() - LM_DEBUG_PTR_CAST<NavigationStyleOrbital>(_impl->_pInteraction->getNavigationStyle())->getTargetPosition()).getLengthSafe();
        break;
        case NAVIGATION_EDITION_ORTHOGONAL:
            Ptr<NodeCameraEdition> pNodeCameraEdit = LM_DEBUG_PTR_CAST<NodeCameraEdition>(pNodeCam);
            distance = pNodeCameraEdit->getWorkdistance();
        break;
    }
    return Core::toString(distance,2);
}
//-----------------------------------------------------------------------------

bool WorldViewport::checkRenderer(Renderer::IRenderer& renderer)
{
    bool ret = false;

    Gfx::EDeviceState deviceState = renderer.getDeviceState();

    if (_impl->deviceState!=deviceState)
    {
        INF << L"Device state changed to " << Gfx::toString(deviceState) << L"\n";

        switch(deviceState)
        {
        case Gfx::DS_DEVICE_LOST:
            renderer.onDeviceLost();
            break;
        case Gfx::DS_DEVICE_NOT_RESET:
            renderer.onDeviceReset();
            break;
        case Gfx::DS_DEVICE_ERROR:
            ERR << L"Device is in error state, exiting\n";
            PostQuitMessage(0);
            break;
        }

        _impl->deviceState = deviceState;
    }
    else if(_impl->deviceState == Gfx::DS_DEVICE_NOT_RESET)
    {
        ERR << L"Device is still not reset, even after doing a reset, exiting\n";
    }

    switch(_impl->deviceState)
    {
        case Gfx::DS_DEVICE_OK:
        {
           break;
        }
        case Gfx::DS_DEVICE_LOST:
        case Gfx::DS_DEVICE_NOT_RESET:
        {
            Core::Thread::sleep(20);
            break;
        }
        case Gfx::DS_DEVICE_ERROR:
            break;
    }

    if (_impl->deviceState == Gfx::DS_DEVICE_OK)  ret = true;

    return ret;
}

void WorldViewport::paintRenderArea()
{
    if (_impl->_pGDoc != null)
    {
        if (_impl->_pView == null || getNodeCamera() == null)
            resizeRenderArea();


        if (_impl->_pView != null && getNodeCamera() != null)
        {
            _impl->_pInteraction->getNavigationStyle()->updateCamera(getNodeCamera());

            DrawContext dc;
                dc.width = width();
                dc.height = height();
                dc.pCamera = getNodeCamera();

            const Ptr<Renderer::IRenderer>& pRenderer    = _impl->_pGDoc->getDocument()->getDocumentRenderer()->getRenderer();

            if (checkRenderer(*pRenderer) == true)
            {
                Renderer::DebugSettings oldDebugSettings = pRenderer->getDebugSettings();
                Renderer::DebugSettings debugSettings    = viewportDescToDebugSetting(_impl->_desc);
                pRenderer->setDebugSettings(debugSettings);


                Ptr<Universe::NodeCamera> pCam = getNodeRenderCamera();
                pCam->setAspectRatio(width()/(float)(height()));
               // pCam->set


                Core::List<Ptr<Universe::World> > worldsForRenderer = _impl->_pGDoc->getWorlds(dc);

                float memoFogOpacity    = 0.f;
                float memoFogRangeMin   = 0.f;
                float memoFogRangeMax   = 0.f;


                if (worldsForRenderer.size()>0)
                {
                    if (isNodeCameraEdition(_impl->_desc._cameraView) == true)
                    {
                        worldsForRenderer[0]->getFogRange(memoFogRangeMin, memoFogRangeMax);
                        memoFogOpacity = worldsForRenderer[0]->getFogSkyOpacity();

                        worldsForRenderer[0]->setFogRange(1000.f, 1000.f);
                        worldsForRenderer[0]->setFogSkyOpacity(0.f);
                    }
                }
                
                _impl->_pGDoc->getWorldOverlay2()->configureOverlayForRendering(pCam);
                makeUpdateCameraMove();
                Universe::World::render(_impl->_pView, pCam, worldsForRenderer);


                if (worldsForRenderer.size()>0)
                {
                    if (isNodeCameraEdition(_impl->_desc._cameraView) == true)
                    {
                        worldsForRenderer[0]->setFogRange(memoFogRangeMin, memoFogRangeMax);
                        worldsForRenderer[0]->setFogSkyOpacity(memoFogOpacity);
                    }
                }

                String textFpsCounter = Core::toString(_impl->_fps.getFPS(), 1) + L" fps  " + Core::toString(_impl->_fps.getFrameDuration(), 1) + L" ms/f";

                _impl->_fps.nextFrame();
                _impl->_pTextInfo->setText(textFpsCounter + L"  " + getDistanceCamera() + L" m");
                _impl->_pTextStats->setText(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getRenderer()->getRenderStats());
                _impl->_pTextStats->setVisible(_impl->_desc._renderStatsMode == VIEWPORT_RENDERSTATS_ON);
                _impl->_pGDoc->getDocument()->getDocumentRenderer()->getRenderer()->renderHUD(_impl->_pView, _impl->_pHUD);


                Ptr<IViewportTool> pVtool = _impl->_pGDoc->getViewportTool();
                if (pVtool!=null)
                {
                    Ptr<Renderer::IHUD> pVtoolHUD = pVtool->getHUD();
                    if (pVtoolHUD!=null)
                    {
                        _impl->_pGDoc->getDocument()->getDocumentRenderer()->getRenderer()->renderHUD(_impl->_pView, pVtoolHUD);
                    }
                }


                _impl->_pView->present();

                pRenderer->setDebugSettings(oldDebugSettings);
            }
        }
    }
}
//-----------------------------------------------------------------------------
/*QSize WorldViewport::sizeHint() const
{
    QSize size;

    int a = this->geometry().width();
    int b = this->geometry().height();
    float renderTargetWidth = ((QWidget*)parent())->width();
    float renderTargetHeight = ((QWidget*)parent())->height();



    switch(_impl->_desc._aspectRatio)
    {
    case CAMERA_RATIO_FREE:
        renderTargetWidth = renderTargetWidth;
        renderTargetHeight= renderTargetHeight;
    break;

    case CAMERA_RATIO_5_4:
        renderTargetWidth =  800;
        renderTargetHeight= 800*4/5.;
        break;
    case CAMERA_RATIO_4_3:
        renderTargetWidth =  800;
        renderTargetHeight= 800*3/4.;
        break;
    case CAMERA_RATIO_3_2:
        renderTargetWidth =  800;
        renderTargetHeight= 800*2/3.;
        break;
    case CAMERA_RATIO_16_10:
        renderTargetWidth =  800;
        renderTargetHeight= 800*10/16.;
        break;
    case CAMERA_RATIO_16_9:
        renderTargetWidth =  800;
        renderTargetHeight= 800*9/16.;
        break;
    }

    size.setWidth (renderTargetWidth);
    size.setHeight (renderTargetHeight);

    return size;
}*/
//-----------------------------------------------------------------------------
Core::Vector2f WorldViewport::getRenderAreaSize() const
{
    Core::Vector2f size;
    float renderTargetWidth =width();
    float renderTargetHeight = height();

/*
    float renderTargetWidth = ((QWidget*)parent())->width();
    float renderTargetHeight = ((QWidget*)parent())->height();

    if (renderTargetWidth< 160) renderTargetWidth= 160;
    if (renderTargetHeight< 160) renderTargetHeight= 160;

    switch(_impl->_desc._aspectRatio)
    {
    case CAMERA_RATIO_FREE:
        renderTargetWidth = renderTargetWidth;
        renderTargetHeight= renderTargetHeight;
    break;

    case CAMERA_RATIO_5_4:
        renderTargetWidth =  800;
        renderTargetHeight= 800*4/5.;
        break;
    case CAMERA_RATIO_4_3:
        renderTargetWidth =  800;
        renderTargetHeight= 800*3/4.;
        break;
    case CAMERA_RATIO_3_2:
        renderTargetWidth =  800;
        renderTargetHeight= 800*2/3.;
        break;
    case CAMERA_RATIO_16_10:
        renderTargetWidth =  800;
        renderTargetHeight= 800*10/16.;
        break;
    case CAMERA_RATIO_16_9:
        renderTargetWidth =  800;
        renderTargetHeight= 800*9/16.;
        break;
    }*/

    size.x = renderTargetWidth;
    size.y = renderTargetHeight;

    return size;
}
//-----------------------------------------------------------------------------
void WorldViewport::resizeRenderArea()
{
    if (_impl->_pGDoc != null && width()>0 && height()>0)
    {

        Core::Vector2f size = getRenderAreaSize();
        float renderTargetWidth = size.x;
        float renderTargetHeight = size.y;

        float ratio = renderTargetWidth/renderTargetHeight;

        Renderer::IRenderer & renderer = *(_impl->_pGDoc->getDocument()->getDocumentRenderer()->getRenderer());

        if (checkRenderer(renderer) == true)
        {
            _impl->_pView = null; // On dÈtruit explicitement la view avant d'en crÈer une autre, pour ne pas prendre 2x la place en mÈmoire
            _impl->_pView = renderer.createView(renderTargetWidth, renderTargetHeight, 2, winId());

            getNodeCamera()->setAspectRatio(ratio);

            int32 widthSeparator = 2;
            int32 startPosBg = _impl->_pBackgroundTextViewType->getSize().x;
            _impl->_pTextViewType->setPosition(Core::Vector2f( renderTargetWidth-startPosBg+5, 2));
            _impl->_pBackgroundTextViewType->setPosition(Core::Vector2f( renderTargetWidth-startPosBg, 0));

            startPosBg += _impl->_pBackgroundTextRenderMode->getSize().x + widthSeparator;
            _impl->_pTextRenderMode->setPosition(Core::Vector2f( renderTargetWidth-startPosBg+5, 2));
            _impl->_pBackgroundTextRenderMode->setPosition(Core::Vector2f( renderTargetWidth-startPosBg, 0));

            startPosBg += _impl->_pBackgroundTextPolygonMode->getSize().x + widthSeparator;
            _impl->_pTextPolygonMode->setPosition(Core::Vector2f( renderTargetWidth-startPosBg+5, 2));
            _impl->_pBackgroundTextPolygonMode->setPosition(Core::Vector2f( renderTargetWidth-startPosBg, 0));

            startPosBg += _impl->_pBackgroundTextAspectType->getSize().x + widthSeparator;
            _impl->_pTextAspectType->setPosition(Core::Vector2f( renderTargetWidth-startPosBg+5, 2));
            _impl->_pBackgroundTextAspectType->setPosition(Core::Vector2f( renderTargetWidth-startPosBg, 0));

            
            
            _impl->_pBackgroundTextInfo->setSize(Core::Vector2f( width(), 20));
            _impl->_pBackgroundTextInfo->setPosition(Core::Vector2f( renderTargetWidth-_impl->_pBackgroundTextInfo->getSize().x, renderTargetHeight-_impl->_pBackgroundTextInfo->getSize().y));
            _impl->_pTextInfo->setPosition(Core::Vector2f(_impl->_pBackgroundTextInfo->getPosition().x+2, _impl->_pBackgroundTextInfo->getPosition().y+2));
            _impl->_pTextStats->setPosition(Core::Vector2f(10.0f, 10.0f));

            _impl->_borders[BORDER_RIGHT]->setPosition(Core::Vector2f(renderTargetWidth-1, 0));
            _impl->_borders[BORDER_LEFT]->setPosition(Core::Vector2f(0, 0));
            _impl->_borders[BORDER_TOP]->setPosition(Core::Vector2f(0, 0));
            _impl->_borders[BORDER_BOTTOM]->setPosition(Core::Vector2f(0, renderTargetHeight-1));

            _impl->_borders[BORDER_RIGHT]->setSize(Core::Vector2f(1, renderTargetHeight));
            _impl->_borders[BORDER_LEFT]->setSize(Core::Vector2f(1, renderTargetHeight));
            _impl->_borders[BORDER_TOP]->setSize(Core::Vector2f(renderTargetWidth,1));
            _impl->_borders[BORDER_BOTTOM]->setSize(Core::Vector2f(renderTargetWidth,1));
        }
    }
}
//-----------------------------------------------------------------------------
void WorldViewport::setGuiDocument(const Ptr<GuiDocument>& pGDoc)
{
    _impl->_pGDoc = pGDoc;
    //TODO pour le HUD le detruire et le recreer dans le renderer du document
    update();
}
//-----------------------------------------------------------------------------
void WorldViewport::keyPressEvent(QKeyEvent* event)
{
    int a = event->key();
    Qt::KeyboardModifiers m = event->modifiers();

    if (/*(event->modifiers()& Qt::AltModifier) && */event->key() == Qt::Key_Space)
    {
        int a = 5;
    }

    if (_impl->_pInteraction!=null)
    {
        DrawContext dc;
            dc.width   = width();
            dc.height  = height();
            dc.pCamera = getNodeCamera();

        _impl->_pInteraction->keyPressEvent(event, dc);
    }

    if (_impl->_pGDoc->isAnimated() == false)
    {
        update();
    }

    QtToolbox::WidgetRenderArea::keyPressEvent(event);
}
//-----------------------------------------------------------------------------
void WorldViewport::keyReleaseEvent(QKeyEvent* event)
{
    if (_impl->_pInteraction!=null)
    {
        DrawContext dc;
            dc.width   = width();
            dc.height  = height();
            dc.pCamera = getNodeCamera();

        _impl->_pInteraction->keyReleaseEvent(event, dc);
        updateCameraContent();
    }

    if (_impl->_pGDoc->isAnimated() == false)
    {
       update();
    }

    QtToolbox::WidgetRenderArea::keyReleaseEvent(event);
}
//-----------------------------------------------------------------------------
bool WorldViewport::posIsInIhmViewport(const QPoint& pos)
{
    return (
    isInRectangle(pos, *(_impl->_pBackgroundTextViewType)) ||
    isInRectangle(pos, *(_impl->_pBackgroundTextRenderMode)) ||
    isInRectangle(pos, *(_impl->_pBackgroundTextPolygonMode))
    );
}
//-----------------------------------------------------------------------------
void WorldViewport::mousePressEvent(QMouseEvent* event)
{
    if (mousePressEventIHMViewport(event) == false)
    {
        if (_impl->_pInteraction!=null)
        {
            DrawContext dc;
                dc.width   = width();
                dc.height  = height();
                dc.pCamera = getNodeCamera();

             _impl->_pInteraction->mousePressEvent(event, dc);
             updateCameraContent();
        }

        if (_impl->_pGDoc->isAnimated() == false)
        {
           update();
        }

        _impl->ihmFlag.reset();  
    }

    QtToolbox::WidgetRenderArea::mousePressEvent(event);
}
//-----------------------------------------------------------------------------
void WorldViewport::mouseMoveEvent(QMouseEvent* event)
{
    mouseMoveEventIHMViewport(event);

    if (_impl->_pInteraction!=null)
    {
        DrawContext dc;
            dc.width   = width();
            dc.height  = height();
            dc.pCamera = getNodeCamera();
        _impl->_pInteraction->mouseMoveEvent(event, dc);
        updateCameraContent();
    }

    if (_impl->_pGDoc->isAnimated() == false)
    {
        repaint();
    }

    QtToolbox::WidgetRenderArea::mouseMoveEvent(event);
}
//-----------------------------------------------------------------------------
void WorldViewport::wheelEvent(QWheelEvent* event)
{
    if (_impl->_pInteraction!=null)
    {
        DrawContext dc;
            dc.width   = width();
            dc.height  = height();
            dc.pCamera = getNodeCamera();

        _impl->_pInteraction->wheelEvent(event, dc);
        updateCameraContent();
    }

    if (_impl->_pGDoc->isAnimated() == false)
    {
        update();
    }
  //  QtToolbox::WidgetRenderArea::wheelEvent(event);
}
//-----------------------------------------------------------------------------
void WorldViewport::mouseReleaseEvent(QMouseEvent* event)
{
    if (mouseReleaseEventIHMViewport(event) == false)
    {
        if (_impl->_pInteraction!=null)
        {
            //------------------------------------
            //Les objets ayant une taille fixe sont modifies a chaque update (la valeur du scale est mis a jour en fonction de la distance de la camera)
            //on force ici un rendu pour que la scene soit calculee pour la camera du viewport. Les valeurs de scale des objet de taille fixe sont alors mis a jour.
            //Sans faire cela on ne pourra avoir un picking correct sur les objets de taille fixe.
            paintRenderArea();
            //------------------------------------

            DrawContext dc;
                dc.width   = width();
                dc.height  = height();
                dc.pCamera = getNodeCamera();

            _impl->_pInteraction->mouseReleaseEvent(event, dc);
            updateCameraContent();
        }

        if (_impl->_pGDoc->isAnimated() == false)
        {
            update();
        }

        _impl->ihmFlag.reset();

        
    }

    QtToolbox::WidgetRenderArea::mouseReleaseEvent(event);
}
//-----------------------------------------------------------------------------
void WorldViewport::mouseDoubleClickEvent(QMouseEvent * event)
{/*
    if (_impl->_pInteraction!=null)
    {
        DrawContext dc;
            dc.width   = width();
            dc.height  = height();
            dc.pCamera = getNodeCamera();

        _impl->_pInteraction->mouseReleaseEvent(event, dc);
        updateCameraContent();
    }

    if (_impl->_pGDoc->isAnimated() == false)
    {
        update();
    }*/

    QtToolbox::WidgetRenderArea::mouseDoubleClickEvent(event);
}
//-----------------------------------------------------------------------------
bool WorldViewport::mouseReleaseEventIHMViewport(QMouseEvent* event)
{
    bool ret = false;
    //ViewType
    if (_impl->ihmFlag.viewTypePress == true)
    {
        ret = true;
        _impl->ihmFlag.viewTypePress = false;
        if(isInRectangle(event->pos(), *(_impl->_pBackgroundTextViewType)) == true)
        {
            menuChooseViewType(event);
        }
    }
    else if (_impl->ihmFlag.renderModePress == true)
    {
        ret = true;
        _impl->ihmFlag.renderModePress = false;
        if(isInRectangle(event->pos(), *(_impl->_pBackgroundTextRenderMode)) == true)
        {
            menuChooseRenderMode(event);
        }
    }
    else if (_impl->ihmFlag.polygonModePress == true)
    {
        ret = true;
        _impl->ihmFlag.polygonModePress = false;
        if(isInRectangle(event->pos(), *(_impl->_pBackgroundTextPolygonMode)) == true)
        {
            menuChoosePolygonMode(event);
        }
    }
    else if (_impl->ihmFlag.AspectTypePress == true)
    {
        ret = true;
        _impl->ihmFlag.AspectTypePress = false;
        if(isInRectangle(event->pos(), *(_impl->_pBackgroundTextAspectType)) == true)
        {
            menuChooseAspectRatio(event);
        }
    }
 

    return ret;
}
//-----------------------------------------------------------------------------
bool WorldViewport::mousePressEventIHMViewport(QMouseEvent* event)
{
    bool ret = false;

    if(_impl->_pBackgroundTextViewType->isVisible() == true &&
       isInRectangle(event->pos(), *(_impl->_pBackgroundTextViewType)) == true)
    {
        ret = true;
        _impl->ihmFlag.viewTypePress = true;
    }
    else if(_impl->_pBackgroundTextRenderMode->isVisible() == true &&
            isInRectangle(event->pos(), *(_impl->_pBackgroundTextRenderMode)) == true)
    {
        ret = true;
        _impl->ihmFlag.renderModePress = true;
    }
    else if(_impl->_pBackgroundTextPolygonMode->isVisible() == true &&
        isInRectangle(event->pos(), *(_impl->_pBackgroundTextPolygonMode)) == true)
    {
        ret = true;
        _impl->ihmFlag.polygonModePress = true;
    }
    else if(_impl->_pBackgroundTextAspectType->isVisible() == true &&
        isInRectangle(event->pos(), *(_impl->_pBackgroundTextAspectType)) == true)
    {
        ret = true;
        _impl->ihmFlag.AspectTypePress = true;
    }

    return ret;
}
//-----------------------------------------------------------------------------
void WorldViewport::mouseMoveEventIHMViewport(QMouseEvent* event)
{
    Core::Vector4f colorBgMenuButton   (0.36f, 0.36f, 0.36f, 0.6f);
    Core::Vector4f colorBgMenuButtonHL (0.56f, 0.56f, 0.36f, 0.6f);

    if(_impl->_pBackgroundTextViewType->isVisible() == true &&
       isInRectangle(event->pos(), *(_impl->_pBackgroundTextViewType)) == true)
    {
          _impl->_pBackgroundTextViewType->setColor(colorBgMenuButtonHL);
    }
    else
    {
        _impl->_pBackgroundTextViewType->setColor(colorBgMenuButton);
    }

    if(_impl->_pBackgroundTextRenderMode->isVisible() == true &&
            isInRectangle(event->pos(), *(_impl->_pBackgroundTextRenderMode)) == true)
    {
          _impl->_pBackgroundTextRenderMode->setColor(colorBgMenuButtonHL);
    }
    else
    {
        _impl->_pBackgroundTextRenderMode->setColor(colorBgMenuButton);
    }

    if(_impl->_pBackgroundTextPolygonMode->isVisible() == true &&
        isInRectangle(event->pos(), *(_impl->_pBackgroundTextPolygonMode)) == true)
    {
          _impl->_pBackgroundTextPolygonMode->setColor(colorBgMenuButtonHL);
    }
    else
    {
        _impl->_pBackgroundTextPolygonMode->setColor(colorBgMenuButton);
    }

    if(_impl->_pBackgroundTextAspectType->isVisible() == true &&
        isInRectangle(event->pos(), *(_impl->_pBackgroundTextAspectType)) == true)
    {
          _impl->_pBackgroundTextAspectType->setColor(colorBgMenuButtonHL);
    }
    else
    {
        _impl->_pBackgroundTextAspectType->setColor(colorBgMenuButton);
    }
}
//-----------------------------------------------------------------------------
void WorldViewport::turnOffIHM()
{
    Core::Vector4f colorBgMenuButton   (0.30f, 0.30f, 0.30f, 0.5f);

    _impl->_pBackgroundTextViewType->setColor(colorBgMenuButton);
    _impl->_pBackgroundTextRenderMode->setColor(colorBgMenuButton);
    _impl->_pBackgroundTextPolygonMode->setColor(colorBgMenuButton);
    _impl->_pBackgroundTextAspectType->setColor(colorBgMenuButton);
}
//-----------------------------------------------------------------------------
const Ptr<INavigationStyle>& WorldViewport::getNavigationStyle()
{
    return _impl->_pInteraction->getNavigationStyle();
}
//-----------------------------------------------------------------------------
void WorldViewport::initText()
{
    switch(_impl->_desc._cameraView)
    {
        case VIEWPORT_CAMERA_TOP:
            _impl->_pTextViewType->setText(VIEW_TYPE_NAME_TOP);
            break;
        case VIEWPORT_CAMERA_BOTTOM:
            _impl->_pTextViewType->setText(VIEW_TYPE_NAME_BOTTOM);
            break;
        case VIEWPORT_CAMERA_BACK:
            _impl->_pTextViewType->setText(VIEW_TYPE_NAME_BACK);
            break;
        case VIEWPORT_CAMERA_FRONT:
            _impl->_pTextViewType->setText(VIEW_TYPE_NAME_FRONT);
            break;
        case VIEWPORT_CAMERA_RIGHT:
            _impl->_pTextViewType->setText(VIEW_TYPE_NAME_RIGHT);
            break;
        case VIEWPORT_CAMERA_LEFT:
            _impl->_pTextViewType->setText(VIEW_TYPE_NAME_LEFT);
        break;
        case VIEWPORT_CAMERA_USER:
            _impl->_pTextViewType->setText(VIEW_TYPE_NAME_USER);
        break;
        case VIEWPORT_CAMERA_CONTENT:
            Ptr<Universe::NodeCamera>   pNCam = getNodeRenderCamera();
            _impl->_pTextViewType->setText(pNCam->getName());
        break;
    }

    switch(_impl->_desc._renderMode)
    {
    case VIEWPORT_RDR_NORMAL:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_RGB);
        break;
    case VIEWPORT_RDR_HQ_PREVIEW:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_HQ_PREVIEW);
        break;
    case VIEWPORT_RDR_DEBUG_SHADOW_MAP:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_SHADOWMAP);
        break;
    case VIEWPORT_RDR_DEBUG_UV:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_UV);
        break;
    case VIEWPORT_RDR_DEBUG_NORMAL:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_NORMAL);
        break;
    case VIEWPORT_RDR_DEBUG_T:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_T);
        break;
    case VIEWPORT_RDR_DEBUG_B:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_B);
        break;
    case VIEWPORT_RDR_DEBUG_N:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_N);
        break;
    case VIEWPORT_RDR_DEBUG_MIP:
        _impl->_pTextRenderMode->setText(RENDER_MODE_NAME_MIP);
        break;
    }

    switch(_impl->_desc._polygonMode)
    {
    case VIEWPORT_POLY_SOLID:
        _impl->_pTextPolygonMode->setText(POLYGON_MODE_NAME_SOLID);
        break;
    case VIEWPORT_POLY_SOLID_WIREFRAME:
        _impl->_pTextPolygonMode->setText(POLYGON_MODE_NAME_SOLID_WIREFRAME);
        break;
    case VIEWPORT_POLY_WIREFRAME:
        _impl->_pTextPolygonMode->setText(POLYGON_MODE_NAME_POLY_WIREFRAME);
        break;
    }

    switch(_impl->_desc._aspectRatio)
    {
    case CAMERA_RATIO_FREE:
        _impl->_pTextAspectType->setText(ASPECT_RATIO_NAME_FREE);
        break;
    case CAMERA_RATIO_5_4:
        _impl->_pTextAspectType->setText(ASPECT_RATIO_NAME_5_4);
        break;
    case CAMERA_RATIO_4_3:
        _impl->_pTextAspectType->setText(ASPECT_RATIO_NAME_4_3);
        break;
    case CAMERA_RATIO_3_2:
        _impl->_pTextAspectType->setText(ASPECT_RATIO_NAME_3_2);
        break;
    case CAMERA_RATIO_16_10:
        _impl->_pTextAspectType->setText(ASPECT_RATIO_NAME_16_10);
        break;
    case CAMERA_RATIO_16_9:
        _impl->_pTextAspectType->setText(ASPECT_RATIO_NAME_16_9);
        break;
    }

    
    
}
//-----------------------------------------------------------------------------
void WorldViewport::setDesc(const ViewportDesc& desc)
{
    ViewportDesc oldDesc = _impl->_desc;
    _impl->_desc = desc;

    if (oldDesc._cameraView != desc._cameraView)
    {
        getNodeCamera()->setAspectRatio(width()/(float)height());
    }   

    if (oldDesc._cameraView != desc._cameraView || oldDesc._cameraNavigation != desc._cameraNavigation)
    {
        Ptr<INavigationStyle> pNav = createNavigationStyle(desc._cameraView, desc._cameraNavigation, getNodeCamera(), _impl->_pGDoc);
        _impl->_pInteraction->setNavigationStyle(pNav);
    }

    if (oldDesc._pickingType != desc._pickingType)
    {
        Ptr<IViewportPicking>  pPicking = createViewportPicking(desc._pickingType, _impl->_pGDoc);
        _impl->_pInteraction->setPicking(pPicking);
    }

    if (_impl->_desc._isSelected == true)
    {
        for (int32 ii=0; ii<EBorderViewport_COUNT; ++ii)
        {
            _impl->_borders[ii]->setColor(colorSelected);
        }
    }
    else
    {
        for (int32 ii=0; ii<EBorderViewport_COUNT; ++ii)
        {
            _impl->_borders[ii]->setColor(colorNotSelected);
        }
    }

    if (oldDesc._aspectRatio != desc._aspectRatio)
    {
        resizeRenderArea();
    }

    initText();

    _impl->_pTextViewType->setVisible(_impl->_desc._showIHM);
    _impl->_pTextRenderMode->setVisible(_impl->_desc._showIHM);
    _impl->_pTextPolygonMode->setVisible(_impl->_desc._showIHM);
    _impl->_pTextAspectType->setVisible(_impl->_desc._showIHM);
    _impl->_pBackgroundTextViewType->setVisible(_impl->_desc._showIHM);
    _impl->_pBackgroundTextRenderMode->setVisible(_impl->_desc._showIHM);
    _impl->_pBackgroundTextPolygonMode->setVisible(_impl->_desc._showIHM);
    _impl->_pBackgroundTextAspectType->setVisible(_impl->_desc._showIHM);
    _impl->_pTextInfo->setVisible(_impl->_desc._showIHM);
    _impl->_pBackgroundTextInfo->setVisible(_impl->_desc._showIHM);
}
//-----------------------------------------------------------------------------
const ViewportDesc& WorldViewport::getDesc() const
{
    return _impl->_desc;
}
//-----------------------------------------------------------------------------
ViewportDesc& WorldViewport::getDesc()
{
    return _impl->_desc;
}
//-----------------------------------------------------------------------------
void WorldViewport::updateCameraContent()
{
    if (_impl->_pGDoc!=null && _impl->_desc._cameraView == VIEWPORT_CAMERA_CONTENT)
    {
        Ptr<PtyNodeCamera> pPtyNodeCam = _impl->_pGDoc->getDocument()->getCameraContent(_impl->_desc._idCameraContent);

        if (pPtyNodeCam!=null)
        {
            pPtyNodeCam->updateProperty();
            pPtyNodeCam->launchSignalProeprtyChanged();
        }
    }
}
//-----------------------------------------------------------------------------
void WorldViewport::makeUpdateCameraMove()
{
    _impl->_pInteraction->updateCameraMove(getNodeRenderCamera());
}
//-----------------------------------------------------------------------------
void WorldViewport::menuChooseViewType(QMouseEvent* event)
{
    QMenu cameraViewMenu ("View", this);
    
    const Core::List<Ptr<PtyNodeCamera> >& cameraContents =  _impl->_pGDoc->getDocument()->getAllCameraContent();
    QList<QAction*> actionsCameraContent;
    
    Ptr<PtyNodeCamera> pPtyNodeCamContent = _impl->_pGDoc->getDocument()->getCameraContent(_impl->_desc._idCameraContent);

    for (int32 ii=0; ii<cameraContents.size(); ++ii)
    {
        QAction* action = new QAction(Core::String8(cameraContents[ii]->getName()).c_str(), &cameraViewMenu);

        QFont font = action->font();
            font.setBold(false);
            if (pPtyNodeCamContent          != null                        && 
                pPtyNodeCamContent->getId() == cameraContents[ii]->getId() && 
                _impl->_desc._cameraView    == VIEWPORT_CAMERA_CONTENT)
            {
                font.setBold(true);
            }

        action->setFont(font);

        actionsCameraContent.push_back(action);
    }
    

    if (_impl->_pGDoc->getDocument()->isEditing())
    {
        if (_impl->_pGDoc->getDocument()->getCameraEdition() != null)
        {
            QAction* actionCameraEdition = new QAction("CamEdit", this);
            connect(actionCameraEdition, SIGNAL(triggered()), _impl->_pGDoc->getGuiContext().get(), SLOT(setCurrentViewportCameraViewEdition()));
            cameraViewMenu.addAction(actionCameraEdition);
            cameraViewMenu.addSeparator();
        }
    }

    if (actionsCameraContent.size()>0)
    {
        cameraViewMenu.addActions(actionsCameraContent);
        cameraViewMenu.addSeparator();
    }

    cameraViewMenu.addActions(_impl->_pGDoc->getGuiContext()->getActionsViewportCameraView());


    QList<QAction*> actionsMenu = cameraViewMenu.actions();
    for (int32 ii=0; ii<actionsMenu.size(); ++ii)
    {
        QFont font = actionsMenu[ii]->font();
            

             if (actionsMenu[ii]->text() == StrToQStr(VIEW_TYPE_NAME_TOP))           {if (_impl->_desc._cameraView ==  VIEWPORT_CAMERA_TOP)     font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(VIEW_TYPE_NAME_BOTTOM))        {if (_impl->_desc._cameraView ==  VIEWPORT_CAMERA_BOTTOM)  font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(VIEW_TYPE_NAME_LEFT))          {if (_impl->_desc._cameraView ==  VIEWPORT_CAMERA_LEFT)    font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(VIEW_TYPE_NAME_RIGHT))         {if (_impl->_desc._cameraView ==  VIEWPORT_CAMERA_RIGHT)   font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(VIEW_TYPE_NAME_FRONT))         {if (_impl->_desc._cameraView ==  VIEWPORT_CAMERA_FRONT)   font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(VIEW_TYPE_NAME_BACK))          {if (_impl->_desc._cameraView ==  VIEWPORT_CAMERA_BACK)    font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(VIEW_TYPE_NAME_USER))          {if (_impl->_desc._cameraView ==  VIEWPORT_CAMERA_USER)    font.setBold(true); else font.setBold(false);}

        actionsMenu[ii]->setFont(font);
    }

    QAction * ret = cameraViewMenu.exec(event->globalPos());

    for (int32 ii=0; ii<actionsCameraContent.size(); ++ii)
    {
        if (ret==actionsCameraContent[ii])
        {
            _impl->_pGDoc->getGuiContext()->setActiveCameraContent(cameraContents[ii]->getId());
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void WorldViewport::menuChooseRenderMode(QMouseEvent* event)
{
    QMenu menu ("Render Mode", this);

    menu.addActions(_impl->_pGDoc->getGuiContext()->getActionsViewportRenderMode());
    
    QList<QAction*> actionsMenu = menu.actions();
    for (int32 ii=0; ii<actionsMenu.size(); ++ii)
    {
        QFont font = actionsMenu[ii]->font();
            

             if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_RGB))                {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_NORMAL)             font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_HQ_PREVIEW))         {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_HQ_PREVIEW)         font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_SHADOWMAP))          {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_DEBUG_SHADOW_MAP)   font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_UV))                 {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_DEBUG_UV)           font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_NORMAL))             {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_DEBUG_NORMAL)       font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_T))                  {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_DEBUG_T)            font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_B))                  {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_DEBUG_B)            font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_N))                  {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_DEBUG_N)            font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(RENDER_MODE_NAME_MIP))                {if (_impl->_desc._renderMode ==  VIEWPORT_RDR_DEBUG_MIP)          font.setBold(true); else font.setBold(false);}

        actionsMenu[ii]->setFont(font);
    }
          
    menu.exec(event->globalPos());
}
//-----------------------------------------------------------------------------
void WorldViewport::menuChoosePolygonMode(QMouseEvent* event)
{
    QMenu menu ("Polygon Mode", this);

    menu.addActions(_impl->_pGDoc->getGuiContext()->getActionsViewportPolygonMode());
         
    QList<QAction*> actionsMenu = menu.actions();
    for (int32 ii=0; ii<actionsMenu.size(); ++ii)
    {
        QFont font = actionsMenu[ii]->font();
            

             if (actionsMenu[ii]->text() == StrToQStr(POLYGON_MODE_NAME_SOLID))                  {if (_impl->_desc._polygonMode ==  VIEWPORT_POLY_SOLID)            font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(POLYGON_MODE_NAME_SOLID_WIREFRAME))        {if (_impl->_desc._polygonMode ==  VIEWPORT_POLY_SOLID_WIREFRAME)  font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(POLYGON_MODE_NAME_POLY_WIREFRAME))         {if (_impl->_desc._polygonMode ==  VIEWPORT_POLY_WIREFRAME)        font.setBold(true); else font.setBold(false);}

        actionsMenu[ii]->setFont(font);
    }

    menu.exec(event->globalPos());
}
void WorldViewport::menuChooseAspectRatio(QMouseEvent* event)
{
    QMenu menu ("Aspect Ratio", this);

    menu.addActions(_impl->_pGDoc->getGuiContext()->getActionsViewportCameraAspectRatio());
         
    QList<QAction*> actionsMenu = menu.actions();
    for (int32 ii=0; ii<actionsMenu.size(); ++ii)
    {
        QFont font = actionsMenu[ii]->font();
            
             if (actionsMenu[ii]->text() == StrToQStr(ASPECT_RATIO_NAME_FREE))       {if (_impl->_desc._aspectRatio ==  CAMERA_RATIO_FREE)       font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(ASPECT_RATIO_NAME_5_4))        {if (_impl->_desc._aspectRatio ==  CAMERA_RATIO_5_4)        font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(ASPECT_RATIO_NAME_4_3))        {if (_impl->_desc._aspectRatio ==  CAMERA_RATIO_4_3)        font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(ASPECT_RATIO_NAME_3_2))        {if (_impl->_desc._aspectRatio ==  CAMERA_RATIO_3_2)        font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(ASPECT_RATIO_NAME_16_10))      {if (_impl->_desc._aspectRatio ==  CAMERA_RATIO_16_10)      font.setBold(true); else font.setBold(false);}
        else if (actionsMenu[ii]->text() == StrToQStr(ASPECT_RATIO_NAME_16_9))       {if (_impl->_desc._aspectRatio ==  CAMERA_RATIO_16_9)       font.setBold(true); else font.setBold(false);}

        actionsMenu[ii]->setFont(font);
    }


    menu.exec(event->globalPos());
}
//-----------------------------------------------------------------------------
void WorldViewport::focusOutEvent (QFocusEvent * event)
{
    turnOffNavigation();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
