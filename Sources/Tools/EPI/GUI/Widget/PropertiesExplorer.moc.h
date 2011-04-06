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
#ifndef PROPERTIES_EXPLORER_H_
#define PROPERTIES_EXPLORER_H_

#include <QTreeWidget>

#include <Core/Standard.h>
#include <Core/Map.h>


class QDragEnterEvent;
class QDragEnterEvent;
class QContextMenuEvent;

namespace Core
{
template <class TElement> class List;
}

namespace EPI
{
class ControllerMain;
class GuiDocument;
class Property;

//-----------------------------------------------------------------------------
 /**
 * 
 */
class LM_API_EPI PropertiesExplorer : public QTreeWidget
{
    Q_OBJECT

public:
    PropertiesExplorer(const Ptr<ControllerMain>& pCtrl, QWidget * parent = 0);
    virtual ~PropertiesExplorer();

protected:
     virtual Qt::DropActions supportedDropActions() const;
     //virtual QMimeData* mimeData ( const QList<QTreeWidgetItem  *> items ) const ;
     //virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
/*
     protected:
    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dragMoveEvent(QDragMoveEvent *e);
*/
     void dropEvent (QDropEvent * dropEvent);



private:
    bool dropOn(QDropEvent  * dropEvent, int32& dropRow, int32& dropCol, QModelIndex& dropIndex);
    void configPropertiesExplorerEditing(bool isEditing);

protected Q_SLOTS:
    void cleanPanel();
    void selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc);
    void newItemsSelected();
    void changeSelection(const Core::List<Ptr<Property> >& pties);
    void updateGuiDocument();
    void documentEditingChange(bool isEditing);

    //void activeProperty(const Ptr<Property>& pPty);
/*
Q_SIGNALS:
    void signalActiveProperty(int32 id);
*/


private:

    void makeGuiDocumentConnection(bool makeConnection);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    Ptr<GuiDocument> _pGDoc;
    Core::Map<int32, QTreeWidgetItem*> _pyIdToItem;

private:
    void travelProperties(const Ptr<Property>& pPty, QTreeWidgetItem * pItem);
    QAction * _pActionDeleteCurrentPtiesSelected;
    QAction * _pActionCopyCurrentPtiesSelected;
    QAction * _pActionPastePtiesCopy;
    QAction * _pActionRenamePty;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif // PROPERTIES_EXPLORER_H_
