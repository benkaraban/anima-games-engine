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
#include "PtyTag.moc.h"

#include <QMouseEvent>
#include <QGridLayout>
#include <QListWidget>
#include <QInputDialog>
#include <QMenu>
#include <QtToolbox/CollapsibleWidget.moc.h>

#include <Universe/Node.h>

namespace EPI
{

//-----------------------------------------------------------------------------
class ListTagEventFilter : public QObject
{
public:
    ListTagEventFilter(PtyWidgetTag* pPtyW, QObject* pParent = 0) : QObject(pParent), _pPtyW(pPtyW) {}
    virtual bool eventFilter(QObject* pWatched, QEvent* pEvent)
    {
        if (pEvent->type() == QEvent::ContextMenu)
        {
            QMouseEvent *mouseEvent = (QMouseEvent*)(pEvent);
            
            QMenu menu;

            QAction*    pActionRename = menu.addAction("Rename");
            QAction*    pActionDelete = menu.addAction("Delete");

            QAction*     retMenu = menu.exec(mouseEvent->globalPos());

            if (retMenu == pActionRename)
            {
                _pPtyW->renameCurrentTag();
            }
            else if (retMenu == pActionDelete)
            {
                _pPtyW->deleteSelectedTags();
            }
            
            return true;
        }

        return false;
    }

private:
    PtyWidgetTag*  _pPtyW;
};
//-----------------------------------------------------------------------------
PtyTag::PtyTag(const Ptr<Universe::Node> pNode, const Core::String& title):
    PropertyNode(title, false, false, CONTENT),
    _pNode(pNode)
{  

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyTag::~PtyTag()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyTag::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetTag> pPW (new PtyWidgetTag(pDtaProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyTag::updateData()
{
    getUniverseNode()->setTags(_tags);
}
//-----------------------------------------------------------------------------
void PtyTag::updateProperty()
{
    _tags = getUniverseNode()->getTags();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyTag::clone() const
{
    Ptr<PtyTag> pRet (new PtyTag( *this ));
    pRet->setUniverseNode(null);
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyTag::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<Universe::Node> tmpPNode = getUniverseNode();

    Ptr<PtyTag> pPty = LM_DEBUG_PTR_CAST<PtyTag>(pSrc);
    _tags = pPty->_tags;

    setUniverseNode(tmpPNode);

    updateData();
}
//-----------------------------------------------------------------------------
void PtyTag::setUniverseNode(const Ptr<Universe::Node>& pNode)
{
    _pNode = pNode;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::Node>& PtyTag::getUniverseNode() const
{
    return _pNode;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetTag::PtyWidgetTag(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent) :
    PropertyWidget(data, parent),
    _pLayout(NULL),
    _pGoupBox(NULL),
    _listTag(NULL),
    _filter(NULL)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetTag::~PtyWidgetTag()
{
    delete _pAddTag;
    delete _buttonBar;
    delete _filter;
    delete _listTag;
    delete _pLayout;
    delete _pGoupBox;
}
//-----------------------------------------------------------------------------
void PtyWidgetTag::readProperty()
{
    Ptr<PtyTag> pP = LM_DEBUG_PTR_CAST<PtyTag>(getDataProxy()->getProperty());

    _listTag->clear();

    Core::Set<String>::const_iterator
        it (pP->_tags.begin()),
        tagsEnd(pP->_tags.end());

    for(;it!=tagsEnd;++it)
    {
        _listTag->addItem(Core::String8((*it)).c_str());
    }

    _listTag->sortItems(Qt::AscendingOrder);
}
//-----------------------------------------------------------------------------
void PtyWidgetTag::writeProperty(QWidget* pWidget)
{
    Ptr<PtyTag> pP = LM_DEBUG_PTR_CAST<PtyTag>(getDataProxy()->getProperty());

    pP->_tags.clear();

    for (int32 ii=0; ii<_listTag->count(); ++ii)
    {
        String tag (Core::String8(_listTag->item(ii)->text().toStdString().c_str()));
        pP->_tags.insert(tag);
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTag::setupUi()
{
    _pLayout = new QGridLayout(this);
    _pLayout->setContentsMargins(0, 0, 0, 0);

    _pGoupBox   = new QtToolbox::CollapsibleWidget(this, "Tags");
    QGridLayout* layoutGroupBox = new QGridLayout(NULL);
        layoutGroupBox->setContentsMargins(0, 0, 0, 0);
        layoutGroupBox->setVerticalSpacing(1);
        layoutGroupBox->setHorizontalSpacing(5);

    _buttonBar = new QWidget(this);
    _buttonBar->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* buttonLayout = new QHBoxLayout(_buttonBar);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    _buttonBar->setLayout(buttonLayout);
    _buttonBar->setFixedHeight(30);
    _buttonBar->setFixedWidth(20);

    _pAddTag = new QPushButton(QIcon(":/icons/add.png"), "", this);
    buttonLayout->addWidget(_pAddTag);

    _listTag = new QListWidget(this);
    _filter = new ListTagEventFilter(this, this);
    _listTag->installEventFilter(_filter);
    _listTag->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _listTag->setSortingEnabled(true);

    _listTag->setFixedHeight(100);
    _listTag->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    layoutGroupBox->addWidget(_buttonBar);
    layoutGroupBox->addWidget(_listTag);
    layoutGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    _pGoupBox->setLayout(layoutGroupBox);
    _pLayout->addWidget(_pGoupBox);
    _pLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_pLayout);

    

    connect(_pAddTag, SIGNAL(clicked()), this, SLOT(addTag()));

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetTag::collapse(bool flag)
{
    _pGoupBox->collapse(flag);
}
//-----------------------------------------------------------------------------
void PtyWidgetTag::renameCurrentTag()
{
    QListWidgetItem *curitem = _listTag->currentItem();

    int r = _listTag->row(curitem);

    QString text = curitem->text();
    QString item = QInputDialog::getText(this, "Item", 
             "Enter new tag", QLineEdit::Normal, text);
    item = item.simplified();

    if (!item.isEmpty())
    {
        _listTag->takeItem(r);
        delete curitem;
        _listTag->insertItem(r, item);
        _listTag->setCurrentRow(r);
        _listTag->sortItems(Qt::AscendingOrder);
        commitPropertyChange(_listTag);
    }

}
//-----------------------------------------------------------------------------
void PtyWidgetTag::deleteSelectedTags()
{
    QList<QListWidgetItem *> selectedItems = _listTag->selectedItems();

    if (selectedItems.size() > 0)
    {
        for (int32 ii=0; ii<selectedItems.size(); ++ii)
        {
            QListWidgetItem *item = selectedItems[ii];
            if (item) 
            {
                int r = _listTag->row(item);
                _listTag->takeItem(r);
                delete item;
            }
        }
        _listTag->sortItems(Qt::AscendingOrder);
        commitPropertyChange(_listTag);
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTag::addTag()
{
    QString item = QInputDialog::getText(this, "Item", "Enter new tag");
    item = item.simplified();

    if (!item.isEmpty())
    {
        _listTag->addItem(item);
        int r = _listTag->count() - 1;
        _listTag->setCurrentRow(r);
        _listTag->sortItems(Qt::AscendingOrder);
        commitPropertyChange(_listTag);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI


