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
#ifndef WORLD_VIEWPORT_H_
#define WORLD_VIEWPORT_H_



#include <QtToolbox/WidgetRenderArea.moc.h>


namespace Universe
{
    class NodeCamera;
}

namespace Core
{
   struct  Vector2f;
}

class QMimeData;
class QGridLayout;


namespace EPI
{
//-----------------------------------------------------------------------------
class IInteractionStyle;
class INavigationStyle;
class WorldViewportContainer;
class GuiDocument;
struct ViewportDesc;
enum ECameraAspectRatio;
//-----------------------------------------------------------------------------
/**
*
*/
LM_ENUM_4(EBorderViewport,
          BORDER_RIGHT,
          BORDER_LEFT,
          BORDER_TOP,
          BORDER_BOTTOM)
class LM_API_EPI WorldViewport : public QtToolbox::WidgetRenderArea
{
    Q_OBJECT

    friend class WorldViewportContainer;

public:
    WorldViewport(const Ptr<GuiDocument>& pGDoc, const ViewportDesc& desc, QWidget* parent=0, Qt::WindowFlags f=0);
    virtual ~WorldViewport();

    virtual void paintRenderArea();
    virtual void resizeRenderArea();
    const Ptr<INavigationStyle>& getNavigationStyle();

    void setDesc(const ViewportDesc& desc);
    ViewportDesc& getDesc();
    const ViewportDesc& getDesc() const;
    void turnOffNavigation();
    void turnOffIHM();

    Core::Vector2f getRenderAreaSize() const;
    //virtual QSize 	sizeHint () const;

public Q_SLOTS:
/*    virtual void startAnimate();
    virtual void stopAnimate();
    virtual void animate();*/

    void setGuiDocument(const Ptr<GuiDocument>& pGDoc);

public:
    Ptr<Universe::NodeCamera> getNodeCamera();
    Ptr<Universe::NodeCamera> getNodeRenderCamera();
//Gestion du drag and drop
public:
    void dragEnterEvent(QDragEnterEvent * dragEnterEvent);
    void dragMoveEvent(QDragMoveEvent * dragMoveEvent);
    void dragLeaveEvent(QDragLeaveEvent * dragLeaveEvent);
    void dropEvent(QDropEvent * dropEvent);

private:
   // bool isAuthorizedMimeData(const QMimeData * mimeData, QString & format);
    bool mouseReleaseEventIHMViewport(QMouseEvent* event);
    bool mousePressEventIHMViewport(QMouseEvent* event);
    void mouseMoveEventIHMViewport(QMouseEvent* event);

private:
    String getDistanceCamera();
    void updateCameraContent();
    void makeUpdateCameraMove();
    void menuChooseViewType(QMouseEvent* event);
    void menuChooseRenderMode(QMouseEvent* event);
    void menuChoosePolygonMode(QMouseEvent* event);
    void menuChooseAspectRatio(QMouseEvent* event);
    bool posIsInIhmViewport(const QPoint& pos);

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void focusOutEvent ( QFocusEvent * event );
private:
    void initText();    //TODO temporaire
    bool checkRenderer(Renderer::IRenderer& renderer);

private:
    class implementation;
	implementation* const _impl;
    /*
private:
    Ptr<GuiDocument>            _pGDoc;

    Ptr<Renderer::IRenderView>  _pView;
    Ptr<IInteractionStyle>      _pInteraction;
    //Ptr<Universe::NodeCamera>   _pCamera;
    ViewportDesc                _desc;
    Ptr<Renderer::IHUD>         _pHUD;
    Ptr<Renderer::IText>        _pTextViewType;
    Ptr<Renderer::IRectangle>   _pBackgroundTextViewType;
    Ptr<Renderer::IRectangle>   _borders    [EBorderViewport_COUNT];

    Ptr<Renderer::IText>        _pTextInfo;
    Ptr<Renderer::IRectangle>   _pBackgroundTextInfo;

    Core::FPSCounter            _fps;

    QStringList                 _authorizedMimeData;*/
};
//-----------------------------------------------------------------------------
class WorldViewportContainer : public QWidget, public Core::Object
{
    Q_OBJECT

public:
    WorldViewportContainer(const Ptr<GuiDocument>& pGDoc, const ViewportDesc& desc, QWidget* parent=0, Qt::WindowFlags f=0);
    virtual ~WorldViewportContainer();

    void setDesc(const ViewportDesc& desc);
    const ViewportDesc& getDesc() const;

    WorldViewport* getView() {return _view;}
    Core::Vector2f getRenderAreaSize(ECameraAspectRatio type);
    void resizeEvent ( QResizeEvent * event  ) ;

protected:
    virtual void mousePressEvent(QMouseEvent* event);

public Q_SLOTS:
    void repaint();

public:
    void makeLayoutMargins(ECameraAspectRatio type);

private:
    WorldViewport*   _view;
    QGridLayout*    _layout;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif // WORLD_VIEWPORT_H_
