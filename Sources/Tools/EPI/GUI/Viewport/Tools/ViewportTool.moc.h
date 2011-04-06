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
#ifndef VIEWPORT_TOOL_H_
#define VIEWPORT_TOOL_H_

#include <Core/Standard.h>
#include <EPI/IWidgetManageUndoRedo.moc.h>

class QSignalMapper;

namespace Universe
{
    class World;
};

namespace Renderer
{
    class IHUD;
};

namespace EPI
{
struct DrawContext;
class Property;
class StateRecorder;
//-----------------------------------------------------------------------------
class GuiDocument;
class IViewportToolWidget;
//-----------------------------------------------------------------------------
/*LM_ENUM_3 (EViewportToolState,
           VT_INACTIVE, //no HIGHLIGHTED and no SELECTED
           VT_HIGHLIGHTED,
           VT_SELECTED)
*/

LM_ENUM_7 (EViewportToolType,
           MULTI_TRANSFORM_TOOL,
           MOVE_TOOL,
           ROTATE_TOOL,
           SCALE_TOOL,
           SUN_TOOL,
           COPY_LOCATION_TOOL,
           COPY_CAMERA_LOCATION_TOOL)
           
class LM_API_EPI IViewportTool : public QObject, public Core::Object
{
Q_OBJECT

public:
    virtual ~IViewportTool();

    virtual Ptr<IViewportToolWidget> createViewportToolWidget(QWidget * parent) = 0;
    virtual void guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc) = 0;

    virtual void mouseMoveEvent         (QMouseEvent* event,const DrawContext& dc) = 0;
    virtual void mousePressEvent        (QMouseEvent* event,const DrawContext& dc) = 0;
    virtual void mouseReleaseEvent      (QMouseEvent* event,const DrawContext& dc) = 0;
    virtual void mouseMiddleSingleClick (QMouseEvent* event, const DrawContext& dc) {};
    virtual void updateCameraMove  (Ptr<Universe::NodeCamera> pNCam) {};

    virtual const Core::List<Ptr<Property> >& getProperties() const =0;
    virtual Core::List<Ptr<Property> >& getProperties() =0;

    virtual bool isHighlighted() const = 0;
    virtual bool isSelected() const = 0;

    virtual EViewportToolType   getType() const = 0;
    void beginPropertiesChange    ();
    void finishPropertiesChange   ();
    void cancelPropertiesChange   ();
    void commitPropertiesChange   ();

    virtual Ptr<Renderer::IHUD> getHUD() const;

public Q_SLOTS:
    virtual void selectionChanged(const Core::List<Ptr<Property> >& pties);
    virtual void updateViewportTool() =0;

Q_SIGNALS:
    void viewportToolChanged();

protected:
    IViewportTool(const Ptr<StateRecorder>& pStateRecorder);

private:
    class implementation;
	implementation* const _impl;
};
//-----------------------------------------------------------------------------
class LM_API_EPI IViewportToolWidget : public IWidgetManageUndoRedo
{
    Q_OBJECT

public:
    virtual ~IViewportToolWidget();
    virtual void setupUi();
    virtual void setConnectToCommitPropertyChange(bool makeConnection);
    QList<QWidget*>& getWidgetsForUndoRedo();
    const QList<QWidget*>& getWidgetsForUndoRedo() const;

    virtual IViewportTool& getViewportTool() = 0;

    virtual void readViewportTool();
    virtual void writeViewportTool(QWidget* pWidget = NULL);
protected:
    IViewportToolWidget(QWidget * parent);
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);

protected Q_SLOTS:
    virtual void beginPropertyChange(QWidget* pWidget = NULL);
    virtual void finishPropertyChange(QWidget* pWidget = NULL);
    virtual void cancelPropertyChange();

    virtual void commitPropertyChange(QWidget* pWidget = NULL);

public Q_SLOTS:
    //->viewportTool to ViewportToolWidget
    void updateViewportToolWidget();

private:
    QList<QWidget*> _widgetsForUndoRedo;    //contient les pointeurs vers les widgets utilisants le system undo/redo

    QSignalMapper* _mapperBeginPropertyChange;
    QSignalMapper* _mapperFinishPropertyChange;
    QSignalMapper* _mapperCommitPropertyChange;

    bool _isViewportToolChanging;
};
//-----------------------------------------------------------------------------
} //namespace EPI

#endif //VIEWPORT_TOOL_H_
