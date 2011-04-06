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
#include <EPI/Document/Properties/PtyDocInformation.moc.h>

namespace EPI
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PyDocumentInformation::PyDocumentInformation(const Core::String title) :
    PropertyNode(L"Description", true, true, DOCUMENT_DESCRIPTION),
    _title(title)
{

}
//-----------------------------------------------------------------------------
PyDocumentInformation::~PyDocumentInformation()
{
}
//-----------------------------------------------------------------------------
/*Ptr<PropertyWidget> PyDocumentInformation::createPropertyWidget(QWidget* parent)
{
    Ptr<PropertyWidget> pPW = Ptr<PropertyWidget>((PropertyWidget*)new PyWidgetDocumentInformation(parent));
    pPW->setupUi();

    //PropertyWidget -> Property
    connect(pPW.get(), SIGNAL(propertyWidgetUpdated(PyWidgetDocumentInformation*)), 
            this, SLOT(updateProperty(PyWidgetDocumentInformation*)));

    //Property -> PropertyWidget
    connect(this, SIGNAL(propertyUpdated(PyDocumentInformation*)), 
            pPW.get(), SLOT(updatePropertyWidget(PyDocumentInformation*)));


    emit propertyUpdated(this);

    //connection au signal d'update de la proprete
 //   connect(this, SIGNAL(createDocumentSignal(Ptr<DocumentBase>)),
 //           pPW.get(), SLOT(PropertyWidgetChanged(Property*)));

    //connection au signal d'update de la widget
  //  connect(pPW.get(), SIGNAL())
    return pPW;
}*/
//-----------------------------------------------------------------------------
void PyDocumentInformation::updateProperty(PyWidgetDocumentInformation* pWDD)
{
    _title = Core::String(pWDD->_pWTitle->text().toStdString().c_str());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PyWidgetDocumentInformation::PyWidgetDocumentInformation(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent) :
    PropertyWidget(data, parent),
    _pWTitle(NULL)
{
}
//-----------------------------------------------------------------------------
PyWidgetDocumentInformation::~PyWidgetDocumentInformation()
{
    delete _pWTitle;
}
//-----------------------------------------------------------------------------
void PyWidgetDocumentInformation::setupUi()
{
    delete _pWTitle;
    _pWTitle = new QLineEdit(this);

    //connect _pWTitle
    connect(_pWTitle, SIGNAL(clicked()), this, SLOT(beginPropertyChange()));
    connect(_pWTitle, SIGNAL(editingFinished ()), this, SLOT(commitPropertyChange()));
}
//-----------------------------------------------------------------------------
void PyWidgetDocumentInformation::PropertyWidgetChanged(const Ptr<Property> p)
{
  /*  Ptr<PyDocumentInformation> pDD = LM_DEBUG_PTR_CAST<PyDocumentInformation>(p);
    _pWTitle->setText(Core::String8(pDD->_title).c_str());*/
}
//-----------------------------------------------------------------------------
void PyWidgetDocumentInformation::readProperty()
{
 /*   Ptr<PyDocumentInformation> p = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getDataProxy()->getProperty());

    //title
    _pWTitle->setText(Core::String8(p->_title).c_str());*/
}
//-----------------------------------------------------------------------------
void PyWidgetDocumentInformation::writeProperty()
{
  /*  Ptr<PyDocumentInformation> p = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getDataProxy()->getProperty());

    //title
    p->_title = Core::String(_pWTitle->text().toStdString().c_str());*/
}
//-----------------------------------------------------------------------------

} // namespace EPI