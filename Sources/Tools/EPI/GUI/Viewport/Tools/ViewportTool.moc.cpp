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
#include <EPI/GUI/Viewport/Tools/ViewportTool.moc.h>

#include <QSignalMapper>
#include <QApplication>

#include <EPI/Document/Property.moc.h>

#include <EPI/Document/Properties/EpiQtToolbox.h>
#include <EPI/Document/StateRecorder.h>

#include <Universe/World.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class IViewportTool::implementation
{
public:
    implementation(const Ptr<StateRecorder>& pStateRecorder) : _pStateRecorder(pStateRecorder) {}
    ~implementation() {};

    const Ptr<StateRecorder>&   getStateRecorder() const    {return _pStateRecorder;}
    Ptr<StateRecorder>&         getStateRecorder()          {return _pStateRecorder;}

private:
    Ptr<StateRecorder> _pStateRecorder;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IViewportToolWidget::IViewportToolWidget(QWidget * parent) :
    IWidgetManageUndoRedo(parent),
    _mapperBeginPropertyChange(NULL),
    _mapperFinishPropertyChange(NULL),
    _mapperCommitPropertyChange(NULL),
    _isViewportToolChanging(false)
{
}
//-----------------------------------------------------------------------------
IViewportToolWidget::~IViewportToolWidget()
{
    delete _mapperBeginPropertyChange;
    delete _mapperFinishPropertyChange;
    delete _mapperCommitPropertyChange;
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::setupUi()
{
    if (_mapperBeginPropertyChange==NULL)    _mapperBeginPropertyChange = new QSignalMapper(this);
    if (_mapperFinishPropertyChange==NULL)   _mapperFinishPropertyChange = new QSignalMapper(this);
    if (_mapperCommitPropertyChange==NULL)   _mapperCommitPropertyChange = new QSignalMapper(this);

    setConnectUndoRedoWidgetToMapper(_mapperBeginPropertyChange, _mapperFinishPropertyChange, _mapperCommitPropertyChange, getWidgetsForUndoRedo());

    setConnectToCommitPropertyChange(true);

    connect(&getViewportTool(), SIGNAL(viewportToolChanged()),
            this, SLOT(updateViewportToolWidget()));
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::setConnectToCommitPropertyChange(bool makeConnection)
{
    setQtToolboxConnection(this, _mapperBeginPropertyChange, _mapperFinishPropertyChange, _mapperCommitPropertyChange, makeConnection);
}
//-----------------------------------------------------------------------------
const QList<QWidget*>& IViewportToolWidget::getWidgetsForUndoRedo() const
{
    return _widgetsForUndoRedo;
}
//-----------------------------------------------------------------------------
QList<QWidget*>& IViewportToolWidget::getWidgetsForUndoRedo()
{
    return _widgetsForUndoRedo;
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::beginPropertyChange(QWidget* pWidget)
{
    _isViewportToolChanging = true;
    getViewportTool().beginPropertiesChange();
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::finishPropertyChange(QWidget* pWidget)
{
    if (_isViewportToolChanging==true)
    {
        getViewportTool().finishPropertiesChange();
        _isViewportToolChanging = false;
    }
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::cancelPropertyChange()
{
    getViewportTool().cancelPropertiesChange();
    _isViewportToolChanging = false;
    updateViewportToolWidget();
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::commitPropertyChange(QWidget* pWidget)
{
    if (_isViewportToolChanging==false)
    {
        getViewportTool().beginPropertiesChange();
    }

    writeViewportTool(pWidget);

    if (_isViewportToolChanging == false)
    {
        getViewportTool().finishPropertiesChange();
    }
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::readViewportTool()
{
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::writeViewportTool(QWidget* pWidget)
{
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::updateViewportToolWidget()
{
    setConnectToCommitPropertyChange(false);
    readViewportTool();
    setConnectToCommitPropertyChange(true);
}
//-----------------------------------------------------------------------------
void IViewportToolWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)    //l'evenement est declenche par le bouton droit
    {
        if (mouseEvent->buttons() & Qt::LeftButton || mouseEvent->buttons() & Qt::MidButton)     //lors de l'evenement le bouton "valide" est enfonce
        {
            if (_isViewportToolChanging == true)
            {
                cancelPropertyChange();

                QMouseEvent e (QEvent::MouseButtonRelease, mouseEvent->pos(), Qt::LeftButton, 0, mouseEvent->modifiers());

                QList<QWidget *> listChild = findChildren<QWidget *>();

                for (int32 ii=0; ii<listChild.size(); ii++)
                {
                    QApplication::sendEvent(listChild[ii], &e);
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IViewportTool::IViewportTool(const Ptr<StateRecorder>& pStateRecorder) :
    _impl(new implementation(pStateRecorder))
{

}
//-----------------------------------------------------------------------------
IViewportTool::~IViewportTool()
{
    delete _impl;
}
//-----------------------------------------------------------------------------
void IViewportTool::beginPropertiesChange()
{
    beginChangeProperties(_impl->getStateRecorder(), getProperties());
}
//-----------------------------------------------------------------------------
void IViewportTool::finishPropertiesChange()
{
    finishChangeProperties(_impl->getStateRecorder(), getProperties());
}
//-----------------------------------------------------------------------------
void IViewportTool::cancelPropertiesChange()
{
    cancelChangeProperties(_impl->getStateRecorder(), getProperties());
}
//-----------------------------------------------------------------------------
void IViewportTool::commitPropertiesChange()
{

}
//-----------------------------------------------------------------------------
void IViewportTool::selectionChanged(const Core::List<Ptr<Property> >& pties)
{}
//-----------------------------------------------------------------------------
Ptr<Renderer::IHUD> IViewportTool::getHUD() const
{
    return null;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}; //namespace EPI
