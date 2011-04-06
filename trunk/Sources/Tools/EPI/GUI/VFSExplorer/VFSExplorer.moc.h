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
#ifndef VFS_EXPLORER_H_
#define VFS_EXPLORER_H_

#include <QTreeWidget>
#include <Core/VFS/VirtualFileSystem.h>
#include <Workflow/TexCompression.h>

class QTreeWidgetItem;

namespace EPI
{

class ControllerMain;

struct ImportInfo;
struct ImportTextureInfo;
struct ImportTerrainInfo;
struct ImportModelInfo;
struct ImportPicBankInfo;
struct ImportWorldInfo;

class LM_API_EPI VFSExplorer : public QTreeWidget
{
    Q_OBJECT
public:
    VFSExplorer(const Ptr<ControllerMain>& pCtrl,
                QWidget * parent = 0);
    
    virtual ~VFSExplorer();

    void openFile(const Core::String & fileName);

    virtual void keyReleaseEvent(QKeyEvent* event);

public Q_SLOTS:
    void refreshTree();
    void openSource(const Ptr<ImportInfo> & pImportInfo);
    void importDone(const Ptr<ImportInfo> & pImportInfo);

Q_SIGNALS:
    void importAsset(const Ptr<ImportInfo> & pImportInfo);
    void openItemProperties(const Ptr<ImportInfo> & pImportInfo);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

    virtual QMimeData * mimeData(const QList<QTreeWidgetItem *> items) const;

private:
    void setupUi();
    void populateTree();
    void exploreVFS(QTreeWidgetItem * item);
    void getExpandedItems(QTreeWidgetItem * item, Core::List<Core::String> & expandedItems);

    Core::String locate(QTreeWidgetItem * item) const;
    QTreeWidgetItem * locateFromString(const Core::String & path);

    void importTextureDone(const Ptr<ImportTextureInfo> & pTextureInfo);
    void importModelDone(const Ptr<ImportModelInfo> & pModelInfo);
    void importTerrainDone(const Ptr<ImportTerrainInfo> & pTerrainInfo);
    void importPicBankDone(const Ptr<ImportPicBankInfo> & pPicBankInfo);
    void lightmapsGenerationDone(const Ptr<ImportWorldInfo> & pLMInfo);
    void occlusionmapsGenerationDone(const Ptr<ImportWorldInfo> & pLMInfo);

    Ptr<ImportInfo> getImportInfo(const Core::String & itemName) const;

    //Int�ration avec le VFSExplorer
    void newDirectory();
    
    void importTexture();
    void importModel();
    void importTerrain();
    void importPicBank();
    void optimizeModel();
    void generateLightmaps();
    void generateOcclusionmaps();

private Q_SLOTS:
    void openItemProperties(QTreeWidgetItem * item, int column);

private:
    Core::VirtualFileSystem &   _vfs;
    Ptr<Core::VirtualFileSystem> _pVFS;
    Ptr<Renderer::IRenderer>    _pRenderer;
    QTreeWidgetItem *           _source;
    Core::String                _draggedFrom;
    const  Ptr<ControllerMain>& _pCtrl;
};

}//namespace EPI

#endif/*VFS_EXPLORER_H_*/
