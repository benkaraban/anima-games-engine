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
#ifndef PROPERTYDESCRIPTION_H_
#define PROPERTYDESCRIPTION_H_

#include <QWidget>
#include <QLineEdit>
#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

namespace EPI
{

class PyWidgetDocumentInformation;

/**
*  Cette propriete stocke toute les information d'un document
* pour le moment seul le titre du document est stocke
*/
class LM_API_EPI PyDocumentInformation : public PropertyNode
{
    Q_OBJECT

public:
    PyDocumentInformation(const Core::String title = L"");
    virtual ~PyDocumentInformation();

    const String& getTitle() const {return _title;}
    void setTitle(const String& title) {_title=title;}

protected Q_SLOTS:
    void updateProperty(PyWidgetDocumentInformation* pWDD);

Q_SIGNALS:
    void propertyUpdated(PyDocumentInformation*);

protected:
    Core::String _title;
};

/**
*  Le widget associe a PyDocumentInformation
* contient l'ensemble des elements IHM permettant d'editer un PyDocumentInformation
*/
class LM_API_EPI PyWidgetDocumentInformation : public PropertyWidget
{
    Q_OBJECT

public:
    friend PyDocumentInformation;

public:
    virtual void setupUi();
    PyWidgetDocumentInformation(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PyWidgetDocumentInformation();
    virtual void readProperty();
    virtual void writeProperty();

private slots:
    virtual void PropertyWidgetChanged(const Ptr<Property> p);

signals:
    void propertyWidgetUpdated(PyWidgetDocumentInformation* pWD);

private:
    QLineEdit*  _pWTitle;
};

} // namespace EPI

#endif PROPERTYNODE_H_