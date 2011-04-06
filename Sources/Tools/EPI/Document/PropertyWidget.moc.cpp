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
#include <EPI/Document/PropertyWidget.moc.h>

#include <QApplication>
#include <QMouseEvent>
#include <QWidget>
#include <QSignalMapper>

#include <Core/List.h>

#include <EPI/Document/Property.moc.h>
#include <EPI/Document/StateRecorder.h>
#include <EPI/Document/Properties/EpiQtToolbox.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PropertyWidget::implementation
{
public:
    implementation(const Ptr<PropertyWidgetDataProxy>& data);
    ~implementation();

    void setupUi();


    const QList<QWidget*>&  getWidgetsForUndoRedo() const;
    QList<QWidget*>&        getWidgetsForUndoRedo();

    virtual void launchGeneration();

    void PropertyWidgetUpdated();
    void propertyModified();

    const Ptr<PropertyWidgetDataProxy>& getDataProxy() const;
    Ptr<PropertyWidgetDataProxy>&       getDataProxy();

public:
    Ptr<PropertyWidgetDataProxy> _data;

    QList<QWidget*>         _widgetsForUndoRedo;    //contient les pointeurs vers les widgets utilisants le system undo/redo
    QSignalMapper*          _mapperBeginPropertyChange;
    QSignalMapper*          _mapperFinishPropertyChange;
    QSignalMapper*          _mapperCommitPropertyChange;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertyWidget::implementation::implementation(const Ptr<PropertyWidgetDataProxy>& data):
    _data(data),
    _mapperBeginPropertyChange(NULL),
    _mapperFinishPropertyChange(NULL),
    _mapperCommitPropertyChange(NULL)
{
    
}
//-----------------------------------------------------------------------------
PropertyWidget::implementation::~implementation()
{
    delete _mapperBeginPropertyChange;
    delete _mapperFinishPropertyChange;
    delete _mapperCommitPropertyChange;
    _data = null;
}
//-----------------------------------------------------------------------------
void PropertyWidget::implementation::setupUi()
{
    if (_mapperBeginPropertyChange==NULL)    _mapperBeginPropertyChange = new QSignalMapper(NULL);
    if (_mapperFinishPropertyChange==NULL)   _mapperFinishPropertyChange = new QSignalMapper(NULL);
    if (_mapperCommitPropertyChange==NULL)   _mapperCommitPropertyChange = new QSignalMapper(NULL);
}
//-----------------------------------------------------------------------------
void PropertyWidget::implementation::launchGeneration()
{
    //Par d�faut on ne fait rien.
}
//-----------------------------------------------------------------------------
const QList<QWidget*>& PropertyWidget::implementation::getWidgetsForUndoRedo() const
{
    return _widgetsForUndoRedo;
}
//-----------------------------------------------------------------------------
QList<QWidget*>& PropertyWidget::implementation::getWidgetsForUndoRedo()
{
    return _widgetsForUndoRedo;
}
//-----------------------------------------------------------------------------
const Ptr<PropertyWidgetDataProxy>& PropertyWidget::implementation::getDataProxy() const
{
    return _data;
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidgetDataProxy>& PropertyWidget::implementation::getDataProxy()
{
    return _data;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertyWidget::PropertyWidget(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent) :
    IWidgetManageUndoRedo(parent),
    _isPropertiesChanging(false),
    _impl(new implementation(data))
{
    makeConnectionToProperties(true);
}
//-----------------------------------------------------------------------------
PropertyWidget::~PropertyWidget()
{
    makeConnectionToProperties(false);
    delete _impl;
}
//-----------------------------------------------------------------------------
void PropertyWidget::setupUi()
{
    _impl->setupUi();

    setConnectUndoRedoWidgetToMapper(_impl->_mapperBeginPropertyChange, _impl->_mapperFinishPropertyChange, _impl->_mapperCommitPropertyChange, getWidgetsForUndoRedo());
    setConnectToCommitPropertyChange(true);
}
//-----------------------------------------------------------------------------
void PropertyWidget::setConnectToCommitPropertyChange(bool makeConnection)
{
    setQtToolboxConnection(this, _impl->_mapperBeginPropertyChange, _impl->_mapperFinishPropertyChange, _impl->_mapperCommitPropertyChange, makeConnection);
}
//-----------------------------------------------------------------------------
const QList<QWidget*>&  PropertyWidget::getWidgetsForUndoRedo() const
{
    return _impl->getWidgetsForUndoRedo();
}
//-----------------------------------------------------------------------------
QList<QWidget*>& PropertyWidget::getWidgetsForUndoRedo()
{
    return _impl->getWidgetsForUndoRedo();
}
//-----------------------------------------------------------------------------
void PropertyWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)    //l'evenement est declenche par le bouton droit
    {
        if (mouseEvent->buttons() & Qt::LeftButton || mouseEvent->buttons() & Qt::MidButton)     //lors de l'evenement le bouton "valide" est enfonce
        {
            if (_isPropertiesChanging == true)
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
void PropertyWidget::makeConnectionToProperties(bool makeConnection)
{
    const Core::List<Ptr<Property> >& properties = getDataProxy()->getProperties();

    if (makeConnection==true)
    {
        for (int32 iPty=0; iPty<properties.size(); ++iPty)
        {
            connect(properties[iPty].get(), SIGNAL(propertyChanged()),
                        this, SLOT(updatePropertyWidget()));
        }
    }
    else
    {
        for (int32 iPty=0; iPty<properties.size(); ++iPty)
        {
            disconnect(properties[iPty].get(), SIGNAL(propertyChanged()),
                        this, SLOT(updatePropertyWidget()));
        }
    }
}
//-----------------------------------------------------------------------------
void PropertyWidget::beginPropertyChange(QWidget* pWidget)
{
    beginChangeProperties(getDataProxy()->getStateRecorder(), getDataProxy()->getProperties());
    _isPropertiesChanging = true;
}
//-----------------------------------------------------------------------------
void PropertyWidget::finishPropertyChange(QWidget* pWidget)
{
    if (_isPropertiesChanging == true) 
    {
        finishChangeProperties(getDataProxy()->getStateRecorder(), getDataProxy()->getProperties());
        _isPropertiesChanging = false;
        emit propertyChangeFinished();
    }
}
//-----------------------------------------------------------------------------
void PropertyWidget::cancelPropertyChange()
{
    cancelChangeProperties(getDataProxy()->getStateRecorder(), getDataProxy()->getProperties());

    _isPropertiesChanging = false;
    updatePropertyWidget();
    emit propertyChangeFinished();
}
//-----------------------------------------------------------------------------
void PropertyWidget::commitPropertyChange(QWidget* pWidget)
{
    const Core::List<Ptr<Property> >& properties = getDataProxy()->getProperties();

    if (_isPropertiesChanging == false)
    {
        beginChangeProperties(getDataProxy()->getStateRecorder(), properties);
    }

    writeProperty(pWidget);

    
///////////////////////////
        
        makeConnectionToProperties(false);
        Core::List<Ptr<IState> > undoStates;
        for (int32 iPty=0; iPty<properties.size(); ++iPty)
        {
            properties[iPty]->launchSignalProeprtyChanged();
        }

        makeConnectionToProperties(true);
/////////////////////


    emit propertyModified();

    if (_isPropertiesChanging == false)
    {
        finishChangeProperties(getDataProxy()->getStateRecorder(), properties);
    }


    for (int32 iPty=0; iPty<properties.size(); ++iPty)
    {
        properties[iPty]->updateData();
    }

    if (_isPropertiesChanging == false)
    {
        emit propertyChangeFinished();
    }
}
//-----------------------------------------------------------------------------
void PropertyWidget::launchGeneration()
{
    _impl->launchGeneration();
}
//-----------------------------------------------------------------------------
void PropertyWidget::updatePropertyWidget()
{
    setConnectToCommitPropertyChange(false);
    readProperty();
    setConnectToCommitPropertyChange(true);
}
//-----------------------------------------------------------------------------
const Ptr<PropertyWidgetDataProxy>& PropertyWidget::getDataProxy() const
{
    return _impl->getDataProxy();
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidgetDataProxy>& PropertyWidget::getDataProxy()
{
    return _impl->getDataProxy();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PropertyWidgetDataProxy::implementation 
{
public:
    implementation (const Ptr<Property>& p, const Core::String& label=L"NoName", const Ptr<StateRecorder>& stateRecorder = null);
    implementation (const Core::List<Ptr<Property> >& pties, const Core::String& label=L"NoName", const Ptr<StateRecorder>& stateRecorder = null);
    virtual ~implementation();

    const Core::List<Ptr<Property> >&   getProperties       () const;
    Core::List<Ptr<Property> >&         getProperties       ();
    int32                               getNbProperties     () const;
    const Ptr<Property>&                getProperty         (int32 index=0) const;
    Ptr<Property>&                      getProperty         (int32 index=0);
    void                                setProperty         (const Ptr<Property> & pPty);
    void                                setProperties       (const Core::List<Ptr<Property> > & pties) { _properties=pties;}

    Ptr<StateRecorder>&                 getStateRecorder    ();

private:
    Core::List<Ptr<Property> >  _properties;
    Core::String                _changeLabel;
    Ptr<StateRecorder>          _pStateRecorder;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertyWidgetDataProxy::implementation::implementation(const Ptr<Property>& p, const Core::String& label, const Ptr<StateRecorder>& stateRecorder) :
    _changeLabel(label),
    _pStateRecorder(stateRecorder)
{
    _properties.push_back(p);
}
//-----------------------------------------------------------------------------
PropertyWidgetDataProxy::implementation::implementation(const Core::List<Ptr<Property> >& pties, const Core::String& label, const Ptr<StateRecorder>& stateRecorder) :
    _changeLabel(label),
    _pStateRecorder(stateRecorder),
    _properties(pties)
{
}
//-----------------------------------------------------------------------------
PropertyWidgetDataProxy::implementation::~implementation()
{
}
//-----------------------------------------------------------------------------
Ptr<StateRecorder>& PropertyWidgetDataProxy::implementation::getStateRecorder()
{
    return _pStateRecorder;
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& PropertyWidgetDataProxy::implementation::getProperties() const
{
    return _properties;
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Property> >& PropertyWidgetDataProxy::implementation::getProperties()
{
    return _properties;
}
//-----------------------------------------------------------------------------
int32 PropertyWidgetDataProxy::implementation::getNbProperties() const
{
    return _properties.size();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PropertyWidgetDataProxy::implementation::getProperty(int32 index) const
{
    LM_ASSERT(_properties.size()-1 <= index);
    return _properties[index];
}
//-----------------------------------------------------------------------------
Ptr<Property>& PropertyWidgetDataProxy::implementation::getProperty(int32 index)
{
    LM_ASSERT(_properties.size()-1 <= index);
    return _properties[index];
}
//-----------------------------------------------------------------------------
void PropertyWidgetDataProxy::implementation::setProperty(const Ptr<Property> & pPty)
{
    _properties.clear();
    _properties.push_back(pPty);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertyWidgetDataProxy::PropertyWidgetDataProxy(const Ptr<Property>& p, const Core::String& label, const Ptr<StateRecorder>& stateRecorder) :
    _impl(new implementation(p, label, stateRecorder))
{
}
//-----------------------------------------------------------------------------
PropertyWidgetDataProxy::PropertyWidgetDataProxy(const Core::List<Ptr<Property> >& pties, const Core::String& label, const Ptr<StateRecorder>& stateRecorder) :
    _impl(new implementation(pties, label, stateRecorder))
{
}
//-----------------------------------------------------------------------------
PropertyWidgetDataProxy::~PropertyWidgetDataProxy()
{
    delete _impl;
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& PropertyWidgetDataProxy::getProperties() const
{
    return _impl->getProperties();
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Property> >& PropertyWidgetDataProxy::getProperties()
{
    return _impl->getProperties();
}
//-----------------------------------------------------------------------------
int32 PropertyWidgetDataProxy::getNbProperties() const
{
    return _impl->getNbProperties();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PropertyWidgetDataProxy::getProperty(int32 index) const
{
    return _impl->getProperty(index);
}
//-----------------------------------------------------------------------------
Ptr<Property>& PropertyWidgetDataProxy::getProperty(int32 index)
{
    return _impl->getProperty(index);
}
//-----------------------------------------------------------------------------
void PropertyWidgetDataProxy::setProperty(const Ptr<Property> & pPty)
{
    _impl->setProperty(pPty);
}
//-----------------------------------------------------------------------------
void PropertyWidgetDataProxy::setProperties(const Core::List<Ptr<Property> > & pties)
{
    _impl->setProperties(pties);
}
//-----------------------------------------------------------------------------
Ptr<StateRecorder>& PropertyWidgetDataProxy::getStateRecorder()
{
    return _impl->getStateRecorder();
}
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
} // namespace EPI
