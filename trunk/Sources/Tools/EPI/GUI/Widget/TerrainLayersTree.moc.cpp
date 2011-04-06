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
#include "TerrainLayersTree.moc.h"

#include <QMenu>
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMessageBox>

#include <Core/Standard.h>
#include <Core/Strings/Tools.h>
#include <Core/List.h>
#include <Core/String.h>

namespace EPI
{

TerrainLayersTree::TerrainLayersTree(QWidget * parent)
:   QTreeWidget(parent),
    _nbTags(0),
    _nbLayers(0),
    _root(NULL),
    _source(NULL)
{
    setupUi();
}
//-----------------------------------------------------------------------------
TerrainLayersTree::~TerrainLayersTree()
{
}
//-----------------------------------------------------------------------------
int32 TerrainLayersTree::getLayersCount()
{
    int32 layersCount = 0;
    layersCount = getLayersCount(_root);
    return layersCount;
}
//-----------------------------------------------------------------------------
int32 TerrainLayersTree::getCurrentLayer(Core::String & layerName)
{
    int32 index = -1;
    bool notFound = true;
    layerName = L"";

    QTreeWidgetItem * current = currentItem();
    if(isLayer(current))
    {
        layerName   << Core::String(current->parent()->text(0).toStdString().c_str()) 
                    << L"^" 
                    << Core::String(current->text(0).toStdString().c_str());

        for(int32 iTag = 0; notFound && iTag < _root->childCount(); ++iTag)
        {
            for(int32 iLayer = 0; notFound && iLayer < _root->child(iTag)->childCount(); ++iLayer)
            {
                ++index;
                if(_root->child(iTag)->child(iLayer) == current)
                    notFound = false;
            }
        }
    }

    if(notFound)
        index = -1;

    return index;
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::getLayersName(Core::List<Core::String> & layersName)
{
    layersName.clear();
    
    for(int32 iTag = 0; iTag < _root->childCount(); ++iTag)
    {
        for(int32 iLayer = 0; iLayer < _root->child(iTag)->childCount(); ++iLayer)
        {
            Core::String layerName;
            layerName   << Core::String(_root->child(iTag)->text(0).toStdString().c_str()) 
                        << L"^" 
                        << Core::String(_root->child(iTag)->child(iLayer)->text(0).toStdString().c_str());
            layersName.push_back(layerName);
        }
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::getLayersId(Core::List<int32> & layersId)
{
    layersId.clear();

    for(int32 iTag = 0; iTag < _root->childCount(); ++iTag)
    {
        for(int32 iLayer = 0; iLayer < _root->child(iTag)->childCount(); ++iLayer)
        {
            layersId.push_back(_itemsId[_root->child(iTag)->child(iLayer)]);
        }
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::addLayer(const Core::String & name, int32 id)
{
    Core::List<Core::String> tokens;
    Core::tokenize(name, tokens, L"^");

    LM_ASSERT(tokens.size() == 2);

    Core::String tag    = tokens[0];
    Core::String layer  = tokens[1];

    QTreeWidgetItem * tagFound = NULL;
    for(int32 iTag=0; tagFound == NULL && iTag<_root->childCount(); ++iTag)
    {
        Core::String currentTag(_root->child(iTag)->text(0).toStdString().c_str());
        if(currentTag == tag)
            tagFound = _root->child(iTag);
    }

    //Le tag n'existe pas on le cr�e
    if(tagFound == NULL)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        item->setText(0, Core::String8(tag).c_str());

        tagFound = item;

        _root->addChild(item);
    }

    QTreeWidgetItem * layerFound = NULL;
    for(int32 iLayer=0; layerFound == NULL && iLayer<tagFound->childCount(); ++iLayer)
    {
        Core::String currentLayer(tagFound->child(iLayer)->text(0).toStdString().c_str());
        if(currentLayer == layer)
            layerFound = tagFound->child(iLayer);
    }

    if(layerFound == NULL)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        item->setText(0, Core::String8(layer).c_str());

        tagFound->addChild(item);
        _itemsId[item] = id;
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::setLayerId(int32 index, int32 id)
{
    int32 currentIndex = -1;
    bool itemFound = false;
    for(int32 iTag = 0; !itemFound && iTag < _root->childCount(); ++iTag)
    {
        for(int32 iLayer = 0; !itemFound && iLayer < _root->child(iTag)->childCount(); ++iLayer)
        {
            currentIndex++;

            if(currentIndex == index)
            {
                _itemsId[_root->child(iTag)->child(iLayer)] = id;
                itemFound = true;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::setCurrentLayer(const Core::String & name)
{
    Core::List<Core::String> tokens;
    Core::tokenize(name, tokens, L"^");

    LM_ASSERT(tokens.size() == 2);
    
    Core::String tag    = tokens[0];
    Core::String layer  = tokens[1];

    QTreeWidgetItem * tagFound = NULL;
    for(int32 iTag=0; tagFound == NULL && iTag<_root->childCount(); ++iTag)
    {
        Core::String currentTag(_root->child(iTag)->text(0).toStdString().c_str());
        if(currentTag == tag)
            tagFound = _root->child(iTag);
    }

    if(tagFound != NULL)
    {
        QTreeWidgetItem * layerFound = NULL;
        for(int32 iLayer=0; layerFound == NULL && iLayer<tagFound->childCount(); ++iLayer)
        {
            Core::String currentLayer(tagFound->child(iLayer)->text(0).toStdString().c_str());
            if(currentLayer == layer)
                layerFound = tagFound->child(iLayer);
        }

        if(layerFound != NULL)
        {
            disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
            setCurrentItem(layerFound);
            connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
        }
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::contextMenuEvent(QContextMenuEvent *event)
{
    QTreeWidgetItem * current = currentItem();

    QMenu menu("TerrainLayersTreeMenu", this);
    QAction * newTagAction = menu.addAction("New Tag");
    
    QAction * newLayerAction = NULL;
    QAction * deleteAction = NULL;
    QAction * renameAction = NULL;
    
    if(current != NULL)
    {
        if(current->parent() == _root)
            newLayerAction = menu.addAction("New Layer");

        deleteAction = menu.addAction("Delete");
        renameAction = menu.addAction("Rename");
    }

    QAction * action = menu.exec(event->globalPos());

    if(action != NULL)
    {
        if(action == newTagAction)
        {
            newTag();
        }
        else if(action == newLayerAction)
        {
            newLayer();
        }
        else if(action == deleteAction)
        {
            deleteItem();
        }
        else if(action == renameAction)
        {
            renameItem();
        }
    }
}
//-----------------------------------------------------------------------------
bool TerrainLayersTree::dropMimeData(QTreeWidgetItem  * parent, int index, const QMimeData * data, Qt::DropAction  action)
{
    bool returnCode = false;
    if(_source != NULL && parent != NULL)
    {
        disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
        disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemRenamed(QTreeWidgetItem *, int)));
        if(isTag(_source) && parent == _root)
        {
            returnCode = QTreeWidget::dropMimeData(parent, index, data, action);
            //Les enfants perdent le flag editable lors du drop.
            parent->child(index)->setFlags(parent->child(index)->flags()|Qt::ItemIsEditable);
            duplicateTag(_source, parent->child(index));
            expandItem(parent->child(index));
            _source->parent()->removeChild(_source);
        }
        else if(isLayer(_source) && isTag(parent))
        {
            returnCode = QTreeWidget::dropMimeData(parent, index, data, action);
            _itemsId[parent->child(index)] = _itemsId[_source];
            _itemsId.erase(_source);
            //Les enfants perdent le flag editable lors du drop.
            parent->child(index)->setFlags(parent->child(index)->flags()|Qt::ItemIsEditable);
            expandItem(parent->child(index));
            _source->parent()->removeChild(_source);
        }

        setCurrentItem(parent->child(index));
        connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
        connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemRenamed(QTreeWidgetItem *, int)));
        emit layersOrderChanged();
    }

    return returnCode;
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::mousePressEvent(QMouseEvent * mouseEvent)
{
    QTreeWidget::mousePressEvent(mouseEvent);

    _source = QTreeWidget::currentItem();
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::selectionChanged()
{
    QTreeWidgetItem * current = currentItem();
    if(isLayer(current))
    {
        Core::String layerName;
        int32 layerIndex = getCurrentLayer(layerName);

        emit currentLayerChanged(layerIndex);
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::itemRenamed(QTreeWidgetItem * item, int column)
{
    Core::String layerName;
    if(isLayer(item))
    {
        int32 layerIndex = getCurrentLayer(layerName);
        emit layerRenamed(layerIndex, layerName);
    }
    else if(isTag(item))
    {
        for(int iTag=0; iTag<item->childCount(); ++iTag)
        {
            int32 layerIndex = getLayer(item->child(iTag), layerName);
            emit layerRenamed(layerIndex, layerName);
        }
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::newTag()
{
    Core::String newTag;
    newTag <<L"new_tag_" << _nbTags;
    _nbTags++;
    QTreeWidgetItem * item = new QTreeWidgetItem();
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    item->setText(0, Core::String8(newTag).c_str());

    _root->addChild(item);
    expandItem(_root);
    setCurrentItem(item);
}
//------------------------------------------------------
void TerrainLayersTree::duplicateTag(QTreeWidgetItem * source, QTreeWidgetItem * destination)
{
    if(isTag(source) && isTag(destination))
    {
        for(int iLayer = 0; iLayer<source->childCount(); ++iLayer)
        {
            QTreeWidgetItem * newItem = new QTreeWidgetItem(source->child(iLayer)->type());
            newItem->setText(0, source->child(iLayer)->text(0));
            destination->addChild(newItem);
            _itemsId[newItem] = _itemsId[source->child(iLayer)];
            _itemsId.erase(source->child(iLayer));
        }
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::newLayer()
{
    Core::String newLayer;
    newLayer <<L"new_layer_" << _nbLayers;
    _nbLayers++;

    QTreeWidgetItem * current = currentItem();
    if(isTag(current))
    {
        QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        item->setText(0, Core::String8(newLayer).c_str());

        current->addChild(item);
        expandItem(current);
        
        disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
        setCurrentItem(item);
        connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

        Core::String layerName;
        int32 layerIndex = getCurrentLayer(layerName);

        emit layerCreated(layerIndex, layerName);
    }
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::deleteItem()
{
    QTreeWidgetItem * current = currentItem();
    if(isLayer(current))
    {
        if(getLayersCount() > 1)
        {
            _itemsId.erase(current);

            Core::String layerName;
            int32 layerIndex = getCurrentLayer(layerName);
            current->parent()->removeChild(current);
            emit layerDeleted(layerIndex);
            
            Core::List<Core::String> layersName;
            getLayersName(layersName);

            setCurrentLayer(layersName[0]);
        }
        else
        {
            QMessageBox::critical(this, "Nyx", "You cannot delete the last layer.");
        }
    }
    else if(isTag(current))
    {
        for(int iLayer=current->childCount()-1; iLayer>=0; --iLayer)
        {
            Core::String layerName;
            emit layerDeleted(getLayer(current->child(iLayer), layerName));
        }
        current->parent()->removeChild(current);
    }
}
//-----------------------------------------------------------------------------
void  TerrainLayersTree::renameItem()
{
    QTreeWidgetItem * current = currentItem();
    itemRenamed(current, 0);
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::setupUi()
{
    header()->setVisible(false);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragEnabled(true);

    _root = new QTreeWidgetItem();
    _root->setText(0, "/");
    addTopLevelItem(_root);

    if(_root->childCount() > 0)
        expandItem(_root);

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemRenamed(QTreeWidgetItem *, int)));
}
//-----------------------------------------------------------------------------
bool TerrainLayersTree::isTag(QTreeWidgetItem * item)
{
    return(item != NULL && item->parent() != NULL && item->parent() == _root);
}
//-----------------------------------------------------------------------------
bool TerrainLayersTree::isLayer(QTreeWidgetItem * item)
{
    return(item != NULL && item->parent() != NULL && item->parent() != _root);
}
//-----------------------------------------------------------------------------
int32  TerrainLayersTree::getLayersCount(QTreeWidgetItem * item)
{
    int32 nbLayers = 0;
    for(int32 ii = 0; ii < item->childCount(); ++ii)
    {
        if(isTag(item->child(ii)))
            nbLayers += getLayersCount(item->child(ii));
        else if(isLayer(item->child(ii)))
            ++nbLayers;
    }
    return nbLayers;
}
//-----------------------------------------------------------------------------
int32 TerrainLayersTree::getLayer(QTreeWidgetItem * item, Core::String & layerName)
{
    int32 index = -1;
    bool notFound = true;
    layerName = L"";
    if(isLayer(item))
    {
        for(int32 iTag = 0; notFound && iTag < _root->childCount(); ++iTag)
        {
            for(int32 iLayer = 0; notFound && iLayer < _root->child(iTag)->childCount(); ++iLayer)
            {
                ++index;
                if(_root->child(iTag)->child(iLayer) == item)
                {
                    layerName   << Core::String(item->parent()->text(0).toStdString().c_str()) 
                                << L"^" 
                                << Core::String(item->text(0).toStdString().c_str());
                    notFound = false;
                }
            }
        }
    }

    if(notFound)
        index = -1;

    return index;
}
//-----------------------------------------------------------------------------
void TerrainLayersTree::clear()
{
    for (int32 ii=0; _root->childCount(); ++ii)
    {
        _root->removeChild(_root->child(ii));
    }
}
//-----------------------------------------------------------------------------
}//namespace EPI