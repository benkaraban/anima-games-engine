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
#include <EPI/GUI/Widget/PropertiesPanel.moc.h>

#include <QVBoxLayout>

#include <EPI/ApplicationBase.moc.h>
#include <EPI/Document/PropertyWidgetFactory.h>
#include <EPI/Document/Property.moc.h>
#include <EPI/Document/IDocumentWidget.h>

#include <EPI/ControllerMain.moc.h>
#include <EPI/GUI/GuiDocument.moc.h>


#include <QTextEdit>
namespace EPI
{
//-----------------------------------------------------------------------------
PropertiesPanel::PropertiesPanel(const Ptr<ControllerMain>& pCtrl, QWidget * parent):
    QWidget(parent),
    _pPW(null),
    _layout(NULL)
{
    //connection au signal aucun guiDocument dans la liste
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalNoGuiDocument()),
            this, SLOT(clear()));

    //signal un guiDocument est actif
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>&)),
            this, SLOT(selectActiveGuiDocument(const Ptr<EPI::GuiDocument>&)));

    setupUi();
}
//-----------------------------------------------------------------------------
PropertiesPanel::~PropertiesPanel()
{
    delete _layout;
}
//-----------------------------------------------------------------------------
void PropertiesPanel::setupUi()
{
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    setLayout(_layout);


    
}
//-----------------------------------------------------------------------------
void PropertiesPanel::cleanPanel()
{
    _pDW = null;
    _pPW = null;

    update();
}
//-----------------------------------------------------------------------------
void PropertiesPanel::selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc)
{
    if (_pGDoc!=null)
    {
        //disconnect vers la selection du document
        disconnect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                   this, SLOT(changeSelection(const Core::List<Ptr<Property> >& )));

        disconnect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(editingChange(bool)),
            this, SLOT(documentEditingChange(bool)));
    }
    _pGDoc = null;
    cleanPanel();

    _pGDoc = pGDoc;

    if (_pGDoc!=null)
    _pDW = _pGDoc->getDocument()->createDocumentWidget(this);

    if (_pDW!=null)    _layout->addWidget(_pDW.get(), 0, Qt::AlignTop);

    if (_pGDoc!=null)
        changeSelection(_pGDoc->getDocument()->getSelectedProperties());

    if (_pGDoc!=null)
    {
        //connect vers la selection du document
        connect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                this, SLOT(changeSelection(const Core::List<Ptr<Property> >& )));

        connect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(editingChange(bool)),
            this, SLOT(documentEditingChange(bool)));
    }
}
//-----------------------------------------------------------------------------
void PropertiesPanel::changeSelection(const Core::List<Ptr<Property> >& pties)
{
    if (pties.size()==0 || pties.size()>1)
    {
        _pPW = null;
        update();
    }
    else
    {
        if (_pGDoc != null && pties[0] != null)
        {
            bool create = false;

            if (_pPW == null)   create = true;
            else if (_pPW->getDataProxy()->getProperties().size() == 1 && _pPW->getDataProxy()->getProperty()->getId() == pties[0]->getId()) create = false;
            else create = true;

            if (create == true)
            {
               _pPW = createPropertyWidget(_pGDoc, pties[0], this);
               _layout->addWidget(_pPW.get(), 10);
            }
        }
    }
    
}
//-----------------------------------------------------------------------------
void PropertiesPanel::clear()
{
    _pGDoc = null;
    cleanPanel();
}
//-----------------------------------------------------------------------------
void PropertiesPanel::documentEditingChange(bool isEditing)
{
    setEnabled(isEditing);
}
//-----------------------------------------------------------------------------
} // namespace EPI
