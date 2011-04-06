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
#include "GuiDocumentsSelectorDialog.moc.h"

#include <QListWidget>
#include <QGridLayout>

#include <EPI/Document/DocumentBase.moc.h>
namespace EPI
{
//-----------------------------------------------------------------------------
GuiDocumentsSelectorDialog::GuiDocumentsSelectorDialog(const Ptr<GuiDocumentManager>& pDocMng, bool isNext, QWidget* parent):
    QDialog(parent),
    _pGDocMng(pDocMng),
    selectNext(isNext),
    _idGDocSelected(-1)
{
    setupUi();

    if (selectNext == true) selectNextDocument();
    else                    selectPreviousDocument();
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelectorDialog::setupUi()
{
    _pDocList = new QListWidget (this);
        _pDocList->setMovement(QListView::Static);
        _pDocList->setMaximumWidth(400);

    QGridLayout*    mainLayout = new QGridLayout(this);

    const Core::List<Ptr<GuiDocument> >& docs = _pGDocMng->getAllGuiDocument();
    int32   idCurrentDoc = _pGDocMng->getCurrentGuiDocumentId();

    int32 selectedRow = -1;

    for (int32 ii=0; ii<docs.size(); ++ii)
    {
        QIcon icon = documentTypeToIcon(docs[ii]->getDocument()->getType());
        String8 title8 (docs[ii]->getDocument()->getTitle());

        QListWidgetItem* docItem = new QListWidgetItem(icon, title8.c_str(), _pDocList);
            docItem->setText(title8.c_str());
            QFont font = docItem->font();
            font.setBold(docs[ii]->getID() == idCurrentDoc);
            docItem->setFont(font);
            docItem->setSelected(docs[ii]->getID() == idCurrentDoc);

            if (docs[ii]->getID() == idCurrentDoc)
            {
                selectedRow = ii;
            }
    }

    mainLayout->addWidget(_pDocList);
    _pDocList->setCurrentRow(selectedRow);

    setFixedSize(400, 300);
    setLayout(mainLayout);
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelectorDialog::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        finish();
    } 
    else if (event->key() == Qt::Key_Shift)
    {
        selectNext = true;
    }
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelectorDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Tab)
    {
        if (selectNext == true) selectNextDocument();
        else                    selectPreviousDocument();
    }
    else if (event->key() == Qt::Key_Shift)
    {
        selectNext = false;
    }
    else
    {
        if (event->key() == 16777218)   //TODO
        {
            selectPreviousDocument();
        }  
    }

}
//-----------------------------------------------------------------------------
void GuiDocumentsSelectorDialog::finish()
{
    int32 row = _pDocList->currentRow();

    const Core::List<Ptr<GuiDocument> >& docs = _pGDocMng->getAllGuiDocument();
    _idGDocSelected = docs[row]->getID();

    close();
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelectorDialog::selectNextDocument()
{
    int32 row = _pDocList->currentRow();

    int32 newRow = (row + 1) % _pDocList->count();
    _pDocList->setCurrentRow(newRow);
}
//-----------------------------------------------------------------------------
void GuiDocumentsSelectorDialog::selectPreviousDocument()
{
    int32 row = _pDocList->currentRow();

    int32 newRow = (row - 1);
    if (newRow<0) newRow =_pDocList->count()-1;

    _pDocList->setCurrentRow(newRow);
}
//-----------------------------------------------------------------------------
} // namepscae EPI