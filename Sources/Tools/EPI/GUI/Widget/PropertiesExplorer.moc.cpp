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
#include "PropertiesExplorer.moc.h"

#include <QHeaderView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QPainter>

#include <EPI/ControllerMain.moc.h>
#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/Tools.h>
#include <EPI/Document/StateRecorder.h>
#include <EPI/Document/Properties/PtyScript.moc.h>

#include <EPI/Constants.h>

namespace EPI
{
//-----------------------------------------------------------------------------
PropertiesExplorer::PropertiesExplorer(const Ptr<ControllerMain>& pCtrl, QWidget * parent):
    QTreeWidget(parent)
{
    header()->setVisible(false);

    //connection au signal aucun guiDocument dans la liste
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalNoGuiDocument()),
            this, SLOT(cleanPanel()));

    //signal un guiDocument est actif
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>&)),
            this, SLOT(selectActiveGuiDocument(const Ptr<EPI::GuiDocument>&)));

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    _pActionDeleteCurrentPtiesSelected  = pCtrl->getActionDeleteCurrentPties();
    _pActionCopyCurrentPtiesSelected    = pCtrl->getActionCopyCurrentPties();
    _pActionPastePtiesCopy              = pCtrl->getActionPastePtiesCopy();
    _pActionRenamePty                   = pCtrl->getActionRenameCurrentPty();

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);


/*
    this->addAction(_pActionDeleteCurrentPtiesSelected);
    this->addAction(_pActionCopyCurrentPtiesSelected);
    this->addAction(_pActionPastePtiesCopy);*/
}
//-----------------------------------------------------------------------------
PropertiesExplorer::~PropertiesExplorer()
{
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::cleanPanel()
{
    clear();
    _pGDoc = null;
    _pyIdToItem.clear();
    update();
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc)
{
    if (_pGDoc!=null)
    {
        makeGuiDocumentConnection(false);
    }

    cleanPanel();
    _pGDoc = pGDoc;

    if (_pGDoc!=null)
    {
        makeGuiDocumentConnection(true);
        configPropertiesExplorerEditing(_pGDoc->getDocument()->isEditing());
    }
    
    updateGuiDocument();
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::makeGuiDocumentConnection(bool makeConnection)
{
    if (makeConnection==true)
    {
        //connect vers la selection du document
        connect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                this, SLOT(changeSelection(const Core::List<Ptr<Property> >& )));

  /*      //pour selectionner une property
        connect(this, SIGNAL(signalActiveProperty(int32)),
                const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SLOT(activeProperty(int32)));*/

        //les pties content du document ont ete maj
        connect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(documentContentUpdated()),
                this, SLOT(updateGuiDocument()));

        connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(newItemsSelected()));

        connect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(editingChange(bool)),
            this, SLOT(documentEditingChange(bool)));
        
    }
    else
    {
        //disconnect vers la selection du document
        disconnect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                   this, SLOT(changeSelection(const Core::List<Ptr<Property> >& )));
        
  /*      //pour selectionner une property
        disconnect(this, SIGNAL(signalActiveProperty(int32)),
            const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SLOT(activeProperty(int32)));*/

        //les pties content du document ont ete maj
        disconnect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(documentContentUpdated()),
                this, SLOT(updateGuiDocument()));

        disconnect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(newItemsSelected()));

        disconnect(const_cast<DocumentBase*>(_pGDoc->getDocument().get()), SIGNAL(editingChange(bool)),
            this, SLOT(documentEditingChange(bool)));
    }
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::travelProperties(const Ptr<Property>& pPty, QTreeWidgetItem * pItem)
{
    if (pPty->getNbChildren()>0)
    {
        Ptr<PropertyNode> pPyNode = LM_DEBUG_PTR_CAST<PropertyNode> (pPty);

        Core::List<Ptr<Property> >::const_iterator itPy = pPyNode->getChildren().begin();
        Core::List<Ptr<Property> >::const_iterator itPyEnd = pPyNode->getChildren().end();

        while(itPy!=itPyEnd)
        {
            if ((*itPy)->isSelectable() == true)
            {
                QTreeWidgetItem* pItemChild = new QTreeWidgetItem((QTreeWidget*)0, QStringList(String8((*itPy)->getName()).c_str()));
                QString iconName (StrToQStr((*itPy)->getIconFileName()));
                QIcon iconItem (iconName);

                
                if ((*itPy)->hasFlag(FLAG_PTY_NODE))
                {
                    Ptr<PtyNode>   pPtyNode   = LM_DEBUG_PTR_CAST<PtyNode>  ((*itPy));
                    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(pPtyNode->getPtyScript());
                            
                    if (pPtyScript->getScript()!="")
                    {
                        QPixmap pixmap = iconItem.pixmap (64, 64);

                        QPainter painter(&pixmap);
                            painter.setPen(Qt::red);
                            painter.setBrush(QBrush(Qt::red));
                            painter.setFont(QFont("Arial", 18));

                        int32 left =  0.2*pixmap.width();
                        int32 top  =  0.2*pixmap.height();
                        QRect  rectangle (left, top, pixmap.width()- left, pixmap.height()-top);
                        painter.drawText (rectangle, Qt::AlignRight | Qt::AlignBottom, "S" );

                        iconItem.addPixmap(pixmap);
                    }
                }
                pItemChild->setIcon(0, iconItem);

    

                Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    
                if ((*itPy)->isLeaf() == false)
                {
                    flags |= Qt::ItemIsDropEnabled;
                }

                if ((*itPy)->isDeletable() == true)
                {
                    flags |= Qt::ItemIsDragEnabled;
                }

                pItemChild->setFlags(flags);

                pItem->addChild(pItemChild);
                _pyIdToItem[(*itPy)->getId()] = pItemChild;
                travelProperties((*itPy), pItemChild);
            }

            ++itPy;
        } 
    }
}

 Qt::DropActions PropertiesExplorer::supportedDropActions() const
 {
     return Qt::MoveAction;
 }

bool PropertiesExplorer::dropOn(QDropEvent  * dropEvent, int32& dropRow, int32& dropCol, QModelIndex& dropIndex)
{
    dropRow               = -1;
    dropCol               = -1;
    dropIndex             = indexAt (dropEvent->pos());

    QModelIndex             root                  = rootIndex();
    
    DropIndicatorPosition 	dropPosition          = dropIndicatorPosition();

    if (!dropIndex.isValid())
        dropIndex = root;

    if (model()->supportedDropActions() & dropEvent->proposedAction())
    {
        if ((dropIndex != root) &&
            (model()->flags(dropIndex) & Qt::ItemIsDropEnabled || model()->flags(dropIndex.parent()) & Qt::ItemIsDropEnabled))
        {
             switch (dropPosition)
             {
             case QAbstractItemView::AboveItem:
                 dropRow = dropIndex.row();
                 dropCol = dropIndex.column();
                 dropIndex = dropIndex.parent();
                 break;
             case QAbstractItemView::BelowItem:
                 dropRow = dropIndex.row() + 1;
                 dropCol = dropIndex.column();
                 dropIndex = dropIndex.parent();
                 break;
             case QAbstractItemView::OnItem:
             case QAbstractItemView::OnViewport:
                 break;
            }
        }

        return true;
    }
    return false;
}

void PropertiesExplorer::dropEvent (QDropEvent * dropEvent)
{
    int32       dropRow = -1;
    int32       dropCol = -1;
    QModelIndex dropIndex;

    if (dropEvent->source() == this && (dropEvent->dropAction() == Qt::MoveAction || dragDropMode() == QAbstractItemView::InternalMove))
    {
        if (dropOn(dropEvent,  dropRow,  dropCol,  dropIndex))
        {
            QList<QModelIndex> idxs = selectedIndexes();
            QList<QPersistentModelIndex> indexes;
            for (int i = 0; i < idxs.count(); i++)
                indexes.append(idxs.at(i));

            if (indexes.contains(dropIndex))
                return;

            // lorsque l'on supprime les items la position du drop peut changer
            QPersistentModelIndex dropModelIndex = model()->index(dropRow, dropCol, dropIndex);

            // On supprime les items
            QList<QTreeWidgetItem *> taken;
            for (int32 ii = indexes.count() - 1; ii >= 0; --ii)
            {
                QTreeWidgetItem *parent = itemFromIndex(indexes.at(ii));
                if (!parent || !parent->parent())
                {
                    taken.append(takeTopLevelItem(indexes.at(ii).row()));
                }
                else
                {
                    taken.append(parent->parent()->takeChild(indexes.at(ii).row()));
                }
            }

            _pGDoc->getDocument()->getStateRecorder()->startRecording();
            // on insert les items dans leur nouvelle position
            for (int32 ii = 0; ii < indexes.count(); ++ii)
            {
                int32 indexChild;
                //on recherche la pty associe � l'item
                Ptr<Property> pPtyChild;
                Ptr<Property> pPtyParent;
                QTreeWidgetItem* currentDropItem = taken[0];
                Core::Map<int32, QTreeWidgetItem*>::iterator iItem     = _pyIdToItem.begin();
                Core::Map<int32, QTreeWidgetItem*>::iterator iItemEnd  = _pyIdToItem.end();

                while ((iItem->second != currentDropItem) && (iItem!=iItemEnd))
                {
                    ++iItem;
                }

                if (iItem!=iItemEnd)
                {
                    pPtyChild = _pGDoc->getDocument()->getProperty(iItem->first);
                }

                //on recherche la pty Parent
                if (dropIndex.isValid())
                {
                    QTreeWidgetItem *parent = itemFromIndex(dropIndex);
                    Core::Map<int32, QTreeWidgetItem*>::iterator iItem     = _pyIdToItem.begin();
                    Core::Map<int32, QTreeWidgetItem*>::iterator iItemEnd  = _pyIdToItem.end();

                    while ((iItem->second != parent) && (iItem!=iItemEnd))
                    {
                        ++iItem;
                    }

                    if (iItem!=iItemEnd)
                    {
                        pPtyParent = _pGDoc->getDocument()->getProperty(iItem->first);
                    }
                }

                //on retire la pty
                Property& pOldPtyParent = *pPtyChild->getParent();
                removeChildRecording(_pGDoc->getDocument()->getStateRecorder(), pPtyChild);
                

                indexChild = -1;
                if (dropRow == -1)
                {
                    
                    if (dropIndex.isValid())
                    {
                        QTreeWidgetItem *parent = itemFromIndex(dropIndex);
                        parent->insertChild(parent->childCount(), taken.takeFirst());
                        parent->setExpanded(true);
                        //pPtyParent->addChild(pPtyChild);
                    }
                    else
                    {
                        insertTopLevelItem(topLevelItemCount(), taken.takeFirst());
                        pPtyParent = _pGDoc->getDocument()->getPropertyDocumentContentRoot();//->addChild(pPtyChild);
                    }
                }
                else
                {
                    int r = dropModelIndex.row() >= 0 ? dropModelIndex.row() : dropRow;
                    indexChild = r;
                    if (dropIndex.isValid())
                    {
                        QTreeWidgetItem *parent = itemFromIndex(dropIndex);
                        parent->insertChild(qMin(r, parent->childCount()), taken.takeFirst());
                        //pPtyParent->addChild(pPtyChild);
                    }
                    else
                    {
                        insertTopLevelItem(qMin(r, topLevelItemCount()), taken.takeFirst());
                        pPtyParent = _pGDoc->getDocument()->getPropertyDocumentContentRoot();//->addChild(pPtyChild);
                    }
                }
    
                if (pOldPtyParent.getId() == pPtyParent->getId())
                {
                    indexChild--;
                    if (indexChild<0) indexChild = 0;
                }

                addChildRecording(_pGDoc->getDocument()->getStateRecorder(), pPtyChild, pPtyParent, indexChild);
            
            }
            _pGDoc->getDocument()->getStateRecorder()->stopRecording();
            
            dropEvent->accept();

            //pour que la QAbstractItemView ne delete pas l'item puisqu'on s'en est charge
            dropEvent->setDropAction(Qt::CopyAction);
        }
    }

    QTreeWidget::dropEvent(dropEvent);
}

void PropertiesExplorer::updateGuiDocument()
{
    if (_pGDoc!=null)
    {
        
        makeGuiDocumentConnection(false);
        clear();
        _pyIdToItem.clear();
        Ptr<PropertyNode> pPyNode = LM_DEBUG_PTR_CAST<PropertyNode> (_pGDoc->getDocument()->getPropertyDocumentContentStart());
        if (pPyNode!=null)
        {
            QTreeWidgetItem* pItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("root"));
            travelProperties(pPyNode, pItem);

            QList<QTreeWidgetItem *> items;
           // items.append(pItem);
            for(int32 ii=0; ii<pItem->childCount(); ++ii)
            {
                QTreeWidgetItem* pItemChild = pItem->child(ii);
                
                items.append(pItemChild);
            }
            for(int32 ii=0; ii<items.size(); ++ii)
            {
                //QTreeWidgetItem* pItemChild = pItem->child(ii);
                pItem->removeChild(items[ii]);
            }
        

        
    /*
            Core::List<Ptr<Property> >::const_iterator itPy = pPyNode->getChildren().begin();
            Core::List<Ptr<Property> >::const_iterator itPyEnd = pPyNode->getChildren().end();

            while(itPy!=itPyEnd)
            {
                if ((*itPy)->isSelectable())
                {
                    items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(String8((*itPy)->getName()).c_str())));
                    _pyIdToItem[(*itPy)->getId()] = items.back();
                }
                ++itPy;
            }*/

            insertTopLevelItems(0, items);

            changeSelection(_pGDoc->getDocument()->getSelectedProperties());

            
        /*    if (items.size()>0)
                setCurrentItem(items[0]);*/
          //  setCurrentItem (_pyIdToItem[_pGDoc->getDocument()->getCurrentProperties()->getId()]);
    /*
            connect(this, SIGNAL(itemSelectionChanged()),
                this, SLOT(newItemsSelected()));*/
        }
        makeGuiDocumentConnection(true);
    }
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::newItemsSelected()
{
    if (_pGDoc!=null)
    {
        QList<QTreeWidgetItem  *> itemsSelected = selectedItems();

        Core::List<int32>    ids;

        for (int32 iSelect=0; iSelect<itemsSelected.size(); ++iSelect)
        {
            Core::Map<int32, QTreeWidgetItem*>::iterator iItem     = _pyIdToItem.begin();
            Core::Map<int32, QTreeWidgetItem*>::iterator iItemEnd  = _pyIdToItem.end();

            while ((iItem->second != itemsSelected[iSelect]) && (iItem!=iItemEnd))
            {
                ++iItem;
            }

            if (iItem!=iItemEnd)
            {
                ids.push_back(iItem->first);
            }
        }

        _pGDoc->getDocument()->selectProperties(ids);
    }
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::changeSelection(const Core::List<Ptr<Property> >& pties)
{
 /*   if (pties.size() == 1)
    {
        makeGuiDocumentConnection(false);
        setCurrentItem (_pyIdToItem[pties[0]->getId()]);
        makeGuiDocumentConnection(true);
    }
    else
    {*/
        makeGuiDocumentConnection(false);

        QList<QTreeWidgetItem *> selection = selectedItems();
        for (int32 ii=0; ii<selection.size(); ++ii)
        {
            selection[ii]->setSelected(false);
        }

        for(int32 ii=0; ii<pties.size(); ++ii)
        {
            QTreeWidgetItem* pTWI = _pyIdToItem[pties[ii]->getId()];
            if (pTWI!= NULL)
            {
                pTWI->setSelected(true);
                pTWI->setExpanded(true);
                if (pTWI->parent()!=NULL) pTWI->parent()->setExpanded(true);
            }
        }
        makeGuiDocumentConnection(true);
  //  }
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::contextMenuEvent(QContextMenuEvent *event)
{
    //QList<QTreeWidgetItem  *> itemsSelected = selectedItems();

    QMenu menu("PropertiesExplorerMenu", this);


    QAction* pActionDuplicate        = (QAction*)-1;
    QAction* pActionExportGroup      = (QAction*)-1;
    QAction* pActionConvertInstance  = (QAction*)-1;
    QAction* pActionCreateGizmo      = (QAction*)-1;

    if (_pGDoc!=null)
    {
        const Core::List<Ptr<Property> >& ptiesSelected = _pGDoc->getDocument()->getSelectedProperties();

        //Menu Delete
        if (ptiesSelected.size() > 0 && ptiesAreDeletable(ptiesSelected) == true)
        {
            menu.addAction(_pActionDeleteCurrentPtiesSelected);
        }

        //Menu Copy
        if (ptiesSelected.size() > 0 && ptiesAreDeletable(ptiesSelected) == true)
        {
            menu.addAction(_pActionCopyCurrentPtiesSelected);
        }

        //Menu Paste
        //if (ptiesSelected.size() > 0 && ptiesAreDeletable(ptiesSelected) == true)
        {
            menu.addAction(_pActionPastePtiesCopy);
        }

        //Menu Duplicate
        if (ptiesSelected.size() > 0)
        {
            pActionDuplicate = menu.addAction("Duplicate");
        }

        //Menu Rename
        if (ptiesSelected.size() == 1)
        {
            menu.addAction(_pActionRenamePty);
        }

        //Menu Create
        if (ptiesSelected.size() <= 1)
        {
            QList <QAction*>& actionsCreate = _pGDoc->getDocument()->getActionsCreateProperty();

            if (actionsCreate.size()>0)
            {
               QMenu* menuCreate = menu.addMenu("Create");
               menuCreate->addActions(actionsCreate);

               //Menu create Gizmo
             /*   if (_pGDoc->getDocument()->getType() == DOC_WORLD && ptiesSelected.size() <= 1)
                {
                    pActionCreateGizmo = menuCreate->addAction("CreateGizmo");
                }*/
            }  
        }

        //Menu convert Instance
        if (ptiesSelected.size() > 0 && ptiesAreInstance(ptiesSelected) == true)
        {
            pActionConvertInstance = menu.addAction("Convert Instance");
        }

        //Menu Export Group
        if (ptiesSelected.size() == 1 && ptiesAreDeletable(ptiesSelected) == true)
        {
            pActionExportGroup = menu.addAction("Export As Group");
        }

        


        QAction * action = menu.exec(event->globalPos());

        if (action == pActionDuplicate)
        {
            _pGDoc->getDocument()->duplicateSelectedProperties();
        }
        else if (action == pActionExportGroup)
        {   
            _pGDoc->getDocument()->exportCurrentPtyAsGroup();
        }
        else if (action == pActionConvertInstance)
        {
            _pGDoc->getDocument()->convertInstance(ptiesSelected);
        }
        else if (action == pActionCreateGizmo)
        {/*
            if (ptiesSelected.size() == 0)
                _pGDoc->getDocument()->createGizmo();
            else 
                _pGDoc->getDocument()->createGizmo(ptiesSelected[0]);*/
        }

    }
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::documentEditingChange(bool isEditing)
{
    configPropertiesExplorerEditing(isEditing);
}
//-----------------------------------------------------------------------------
void PropertiesExplorer::configPropertiesExplorerEditing(bool isEditing)
{
    this->setEnabled(isEditing);

    if (isEditing == true)
    {
        setStyleSheet(
        "QFrame                    { background-color: rgb(250,250,250); }"
        );

    }
    else
    {
        setStyleSheet(
        "QFrame                    { background-color: rgb(230,130,130); }"
        );
    }

    
}
//-----------------------------------------------------------------------------
} //namespace EPI
