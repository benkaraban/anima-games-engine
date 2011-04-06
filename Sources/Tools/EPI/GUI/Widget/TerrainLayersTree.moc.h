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
#ifndef EPI_TERRAIN_LAYERS_TREE_H_
#define EPI_TERRAIN_LAYERS_TREE_H_

#include <QTreeWidget>
#include <Core/Map.h>

namespace Core
{
    class String;
    template <class TElement> class List;
}

namespace EPI
{

class TerrainLayersTree : public QTreeWidget
{
    Q_OBJECT
public:
    TerrainLayersTree(QWidget * parent = 0);
    virtual ~TerrainLayersTree();

    int32   getLayersCount();
    int32   getCurrentLayer(Core::String & layerName);
    void    getLayersName(Core::List<Core::String> & layersName);
    void    getLayersId(Core::List<int32> & layersId);

    void    addLayer(const Core::String & name, int32 id);
    void    setLayerId(int32 index, int32 id);

    void    setCurrentLayer(const Core::String & name);
    virtual void clear();

Q_SIGNALS:
    void layersOrderChanged();
    void layerCreated(int32, Core::String);
    void layerDeleted(int32);
    void layerRenamed(int32, Core::String);
    void currentLayerChanged(int32);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual bool dropMimeData(QTreeWidgetItem  * parent, int index, const QMimeData * data, Qt::DropAction  action);
    virtual void mousePressEvent(QMouseEvent * mouseEvent);

private Q_SLOTS:
    void selectionChanged();
    void itemRenamed(QTreeWidgetItem * item, int column);

private:
    void newTag();
    void newLayer();
    void deleteItem();
    void renameItem();

    void setupUi();

    bool isTag(QTreeWidgetItem * item);
    bool isLayer(QTreeWidgetItem * item);

    int32 getLayersCount(QTreeWidgetItem * item);
    int32 getLayer(QTreeWidgetItem * item, Core::String & layerName);

    void duplicateTag(QTreeWidgetItem * source, QTreeWidgetItem * destination);

private:
    int                 _nbTags;
    int                 _nbLayers;
    QTreeWidgetItem *   _root;
    QTreeWidgetItem *   _source;

    Core::Map<QTreeWidgetItem *, int32> _itemsId;
};

}//namespace EPI

#endif/*EPI_TERRAIN_LAYERS_TREE_H_*/