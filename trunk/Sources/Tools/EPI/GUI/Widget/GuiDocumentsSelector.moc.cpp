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
#include <EPI/GUI/Widget/GuiDocumentsSelector.moc.h>

#include <QAction>
#include <QMenu>

#include <EPI/ControllerMain.moc.h>
#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/Document/DocumentBase.moc.h>
namespace EPI
{
//-----------------------------------------------------------------------------
GuiDocumentsSelector::GuiDocumentsSelector(const Ptr<ControllerMain>& pCtrl, const String& title, QWidget * parent):
    QToolButton(parent),
    _pGDocMng(pCtrl->getDocumentManager()),
    _isPressed(false)
{
    setText(String8(title).c_str());

   // connect(this, SIGNAL(released()), this, SLOT(launchMenu()));
    //connection au signal aucun guiDocument dans la liste
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalNoGuiDocument()),
            this, SLOT(makeDisable()));

    //connection au signal la list de guiDocument vien d'etre cree
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalFirstGuiDocument()),
            this, SLOT(makeEnable()));

    //pour selectionner un document
    connect(this, SIGNAL(signalActiveGuiDocument(int32)),
            const_cast<ControllerMain*>(pCtrl.get()), SLOT(activeGuiDocument(int32)));

    //connection au choix dans le menu
    connect(this, SIGNAL(triggered(QAction*)), 
            this, SLOT(actionSelected(QAction*)));

/*
    //connection au signal de creation de document
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalNewGuiDocument(const Ptr<GuiDocument>&)),
            this, SLOT(addGuiDocument(const Ptr<GuiDocument>&)));

    //connection au signal de destruction de document
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalDeleteGuiDocument(const Ptr<GuiDocument>&)),
            this, SLOT(removeGuiDocument(const Ptr<GuiDocument>&)));

    

    

    //signal un guiDocument est actif
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>&)),
            this, SLOT(selectActiveGuiDocument(const Ptr<EPI::GuiDocument>&)));*/
    
}
//-----------------------------------------------------------------------------
GuiDocumentsSelector::~GuiDocumentsSelector()
{

}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::launchMenu()
{
    const Core::List<Ptr<GuiDocument> >& docs = _pGDocMng->getAllGuiDocument();
    const int32 idCurrentDoc = _pGDocMng->getCurrentGuiDocumentId();
    QMenu*  menu = new QMenu (this);

    _docIdToAction.clear();
    for (int32 ii=0; ii<docs.size(); ++ii)
    {
        QIcon icon = documentTypeToIcon(docs[ii]->getDocument()->getType());
        Core::String8 title8 (docs[ii]->getDocument()->getTitle());
        QAction* action = new QAction(icon, title8.c_str(), this);
        menu->addAction(action);
        _docIdToAction[docs[ii]->getID()] = action;

        QFont font = action->font();
        font.setBold(docs[ii]->getID() == idCurrentDoc);
        action->setFont(font);
    }

    setMenu(menu);
    showMenu();
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::makeDisable()
{
    setDisabled(true);
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::makeEnable()
{
    setEnabled(true);
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::mousePressEvent (QMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
    {
        _isPressed = true;
    }
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::mouseReleaseEvent (QMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
    {
        _isPressed = false;
        launchMenu();
    }
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::actionSelected(QAction* pAction)
{
    Core::Map<int32, QAction*>::iterator iAction = _docIdToAction.begin();
    Core::Map<int32, QAction*>::iterator iActionEnd  = _docIdToAction.end();

    while ((iAction->second != pAction) && (iAction!=iActionEnd))
    {
        ++iAction;
    }

    if (iAction!=iActionEnd)
    {
        emit signalActiveGuiDocument(iAction->first);
    }
}
//-----------------------------------------------------------------------------
/*//-----------------------------------------------------------------------------
void GuiDocumentsSelector::addGuiDocument(const Ptr<GuiDocument>& pGDoc)
{
    if (pGDoc != null)
    {
        Core::String8 title8 (pGDoc->getDocument()->getTitle());
        QAction* action = addAction(title8.c_str());
        
        _docIdToAction[pGDoc->getID()] = action;
    }
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::removeGuiDocument(const Ptr<GuiDocument>& pGDoc)
{
    if (pGDoc != null)
    {
        removeAction(_docIdToAction[pGDoc->getID()]);
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GuiDocumentsSelector::selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc)
{
    if (pGDoc != null)
    {
        Core::Map<int32, QAction*>::iterator iAction = _docIdToAction.begin();
        Core::Map<int32, QAction*>::iterator iActionEnd  = _docIdToAction.end();

        for (; iAction!=iActionEnd; ++iAction)
        {
            QFont font = iAction->second->font();
            font.setBold(iAction->first == pGDoc->getID());
            iAction->second->setFont(font);
        }
    }
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::aboutToShow()
{
    int a = 5;
}
//-----------------------------------------------------------------------------
QAction * GuiDocumentsSelector::exec(QList<QAction *> actions, const QPoint & pos, QAction * at, QWidget * parent)
{
    return QMenu::exec(actions, pos, at, parent);
}
//-----------------------------------------------------------------------------
QAction * GuiDocumentsSelector::exec(QList<QAction *> actions, const QPoint & pos, QAction * at)
{
    return QMenu::exec(actions, pos, at);
}
//-----------------------------------------------------------------------------
QAction * 	GuiDocumentsSelector::exec ()
{
        return QMenu::exec();
}
//-----------------------------------------------------------------------------
QAction * 	GuiDocumentsSelector::exec ( const QPoint & p, QAction * action)
{
        return QMenu::exec(p, action);
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelector::popup ( const QPoint & p, QAction * atAction)
{
        int a = 5;
}
//-----------------------------------------------------------------------------*/
} // namespace EPI
