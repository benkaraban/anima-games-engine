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
#ifndef PROPERTY_WIDGET_H_
#define PROPERTY_WIDGET_H_

#include <Core/Standard.h>
#include <EPI/IWidgetManageUndoRedo.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
class PropertyWidgetDataProxy;
class StateRecorder;
class Property;
//-----------------------------------------------------------------------------
/**
*  Definit un Widget pour afficher le contenue d'une propriete
*  
*/
class LM_API_EPI PropertyWidget : public IWidgetManageUndoRedo
{
    Q_OBJECT

public:
    PropertyWidget(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PropertyWidget();

    virtual void setupUi();

    virtual void readProperty       ()=0;
    virtual void writeProperty      (QWidget* pWidget = NULL)=0;

    virtual void setConnectToCommitPropertyChange(bool makeConnection);

    const QList<QWidget*>&  getWidgetsForUndoRedo() const;
    QList<QWidget*>&        getWidgetsForUndoRedo();

    const Ptr<PropertyWidgetDataProxy>& getDataProxy() const;
    Ptr<PropertyWidgetDataProxy>&       getDataProxy();
    virtual void collapse(bool flag) {;}
protected:
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);



protected:
    void makeConnectionToProperties(bool makeConnection);

protected Q_SLOTS:
    //->PropertyWidget to property
    virtual void beginPropertyChange(QWidget* pWidget = NULL);
    virtual void finishPropertyChange(QWidget* pWidget = NULL);
    virtual void cancelPropertyChange();
    virtual void commitPropertyChange(QWidget* pWidget = NULL);

    virtual void launchGeneration();

public Q_SLOTS:
    //->property to PropertyWidget
    void updatePropertyWidget();

Q_SIGNALS:
    void PropertyWidgetUpdated();
    void propertyModified();
    void propertyChangeFinished();
    void emitDocumentContentUpdated();
private:
    class implementation;
	implementation* const _impl;

    bool _isPropertiesChanging;
};
//-----------------------------------------------------------------------------
/**
*  Un PropertyWidget s'interface avec une propriete et un StateRecorder pour gerer les changement d'etat.
* PropertyWidgetDataProxy est la classe permettant à la PropertyWidget d'acceder à ces données
*/
class LM_API_EPI PropertyWidgetDataProxy : public Core::Object
{
public:
    PropertyWidgetDataProxy (const Ptr<Property>& p, const Core::String& label=L"NoName", const Ptr<StateRecorder>& stateRecorder = null);
    PropertyWidgetDataProxy (const Core::List<Ptr<Property> >& pties, const Core::String& label=L"NoName", const Ptr<StateRecorder>& stateRecorder = null);
    virtual ~PropertyWidgetDataProxy();

    const Core::List<Ptr<Property> >&   getProperties       () const;
    Core::List<Ptr<Property> >&         getProperties       ();
    int32                               getNbProperties     () const;
    const Ptr<Property>&                getProperty         (int32 index=0) const;
    Ptr<Property>&                      getProperty         (int32 index=0);
    void                                setProperty         (const Ptr<Property> & pPty);
    void                                setProperties       (const Core::List<Ptr<Property> > & pties);

    Ptr<StateRecorder>&                 getStateRecorder    ();

private:
    class implementation;
	implementation* const _impl;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif PROPERTY_WIDGET_H_