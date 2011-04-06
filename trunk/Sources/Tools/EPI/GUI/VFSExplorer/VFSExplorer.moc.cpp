/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include "VFSExplorer.moc.h"

#include <QMenu>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <QMessageBox>

#include <EPI/ControllerMain.moc.h>
#include <EPI/RendererManager.h>
#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/GUI/VFSExplorer/VFSExplorerOperationDialog.moc.h>
#include <EPI/GUI/VFSExplorer/ImportTextureDialog.moc.h>
#include <EPI/GUI/VFSExplorer/ImportTerrainDialog.moc.h>
#include <EPI/GUI/VFSExplorer/ImportModelDialog.moc.h>
#include <EPI/GUI/VFSExplorer/ImportPicBankDialog.moc.h>
#include <EPI/GUI/VFSExplorer/GenerateLightmapsDialog.moc.h>
#include <EPI/GUI/VFSExplorer/GenerateOcclusionmapsDialog.moc.h>

#include <EPI/ImportInfo/ImportTextureInfo.h>
#include <EPI/ImportInfo/ImportModelInfo.h>
#include <EPI/ImportInfo/ImportTerrainInfo.h>
#include <EPI/ImportInfo/ImportParticlesInfo.h>
#include <EPI/ImportInfo/ImportWorldInfo.h>
#include <EPI/ImportInfo/ImportGroupInfo.h>
#include <EPI/ImportInfo/ImportScriptInfo.h>
#include <EPI/ImportInfo/ImportSplineInfo.h>
#include <EPI/ImportInfo/ImportPicBankInfo.h>
#include <EPI/ImportInfo/LightmapsGenerationInfo.h>
#include <EPI/ImportInfo/OcclusionmapsGenerationInfo.h>

#include <Core/Strings/Tools.h>

#include <Core/XML/XMLDocument.h>

namespace EPI
{

LM_ENUM_2(EVFSItemType, EVFS_DIR, EVFS_FILE);

int32 getTreeViewItemType(EVFSItemType type)
{
    return 1000 + type;
}

//-----------------------------------------------------------------------------
VFSExplorer::VFSExplorer(   const Ptr<ControllerMain>& pCtrl, 
                            QWidget * parent)
:   _vfs(*(pCtrl->getRendererManager()->getDefaultRenderer()->getVFS())),
    _pVFS(pCtrl->getRendererManager()->getDefaultRenderer()->getVFS()),
    _pRenderer(pCtrl->getRendererManager()->getDefaultRenderer()->getRenderer()),
    _source(NULL),
    QTreeWidget(parent),
    _pCtrl(pCtrl)
{
    setupUi();
}
//-----------------------------------------------------------------------------
VFSExplorer::~VFSExplorer()
{
}
//-----------------------------------------------------------------------------
void VFSExplorer::openFile(const Core::String & fileName)
{
    if(_vfs.fileExists(fileName))
    {
        if (fileName.endsWith(L"wav") || fileName.endsWith(L"ogg")) return; 


        Core::String openFile(fileName);
        if(!openFile.endsWith(L".xml") && !openFile.endsWith(L".lua"))
            openFile << L".xml";

        QTreeWidgetItem * item = locateFromString(openFile);
        openItemProperties(item, 0);
    }
    else
    {
        Core::String message;
        message << L"Cannot open file, the file : " << fileName << L" doesn't exist!";
        QMessageBox::critical(this, "Nyx", String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::setupUi()
{
    header()->setVisible(false);
    setDragEnabled(true);
    setAcceptDrops(false);

    populateTree();

    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), 
            this, SLOT(openItemProperties(QTreeWidgetItem *, int)));
}
//-----------------------------------------------------------------------------
void VFSExplorer::populateTree()
{
    QTreeWidgetItem * root = new QTreeWidgetItem(getTreeViewItemType(EVFS_DIR));
    root->setText(0, "/");
    addTopLevelItem(root);

    exploreVFS(root);
    if(root->childCount() > 0)
        expandItem(root);
}
//-----------------------------------------------------------------------------
void VFSExplorer::exploreVFS(QTreeWidgetItem * item)
{
    Core::String location = locate(item);

    Core::List<Core::String> directories;
    Core::List<Core::String> files;
    _vfs.listDirs(location, directories);
    _vfs.listFiles(location, files);

    int32 ii = 0;
    //On ajoute les répertoires
    while(ii < directories.size())
    {
        Core::String8 directory(directories[ii]);
        QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_DIR));
        newItem->setIcon(0, QIcon(":/icons/folder.png"));
        newItem->setText(0, directory.c_str());
        
        item->addChild(newItem);
        exploreVFS(newItem);

        ++ii;
    }

    ii = 0;
    //On ajoute les fichier
    while(ii < files.size())
    {
        if(files[ii].endsWith(L".xml") || files[ii].endsWith(L".lua"))
        {
            Core::String8 file(files[ii]);
            QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_FILE));
            if(files[ii].endsWith(L".mdl.xml"))
                newItem->setIcon(0, QIcon(":/icons/model.png"));
            else if(files[ii].endsWith(L".tex.xml"))
                newItem->setIcon(0, QIcon(":/icons/texture.png"));
            else if(files[ii].endsWith(L".tmdl.xml"))
                newItem->setIcon(0, QIcon(":/icons/terrain.png"));
            else if(files[ii].endsWith(L".parts.xml"))
                newItem->setIcon(0, QIcon(":/icons/particles.png"));
            else if(files[ii].endsWith(L".world.xml"))
                newItem->setIcon(0, QIcon(":/icons/world.png"));
            else if(files[ii].endsWith(L".lua"))
                newItem->setIcon(0, QIcon(":/icons/script.png"));
            else if(files[ii].endsWith(L".group.xml"))
                newItem->setIcon(0, QIcon(":/icons/group.png"));
            else if(files[ii].endsWith(L".spline.xml"))
                newItem->setIcon(0, QIcon(":/icons/spline.png"));
            else if(files[ii].endsWith(L".pbk.xml"))
                newItem->setIcon(0, QIcon(":/icons/picbank.png"));
            newItem->setText(0, file.c_str());

            item->addChild(newItem);
        }
        else if (files[ii].endsWith(L".wav") || files[ii].endsWith(L".ogg") )
        {
            Core::String8 file(files[ii]);
            QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_FILE));
            newItem->setIcon(0, QIcon(":/icons/ptySound.png"));
            newItem->setText(0, file.c_str());

            item->addChild(newItem);
        }
        
        ++ii;
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::getExpandedItems(QTreeWidgetItem * item, Core::List<Core::String> & expandedItems)
{
    if(item->isExpanded())
    {
        expandedItems.push_back(locate(item));
        for(int32 iChild = 0; iChild < item->childCount(); ++iChild)
        {
            getExpandedItems(item->child(iChild), expandedItems);
        }
    }
}
//-----------------------------------------------------------------------------
Core::String VFSExplorer::locate(QTreeWidgetItem * item) const
{
    Core::String location;
    location << Core::String8(item->text(0).toStdString().c_str());
    while(item->parent() != NULL)
    {
        item = item->parent();
        Core::String temp;
        temp << Core::String8(item->text(0).toStdString().c_str());
        if(temp != L"/")
            temp << L"/";
        temp << location;
        location = temp;
    }

    return location;
}
//-----------------------------------------------------------------------------
QTreeWidgetItem * VFSExplorer::locateFromString(const Core::String & path)
{
    Core::List<String> assetPath;
    Core::tokenize(path, assetPath, L"/");

    QTreeWidgetItem * item = NULL;
    bool childFound = false;

    if( assetPath.size() > 1 && assetPath[0] == L"")
    {
        item = topLevelItem(0);
        int32 ii = 1;
        while(ii < assetPath.size())
        {
            childFound = false;
            int32 iChild = 0;
            while(!childFound && iChild < item->childCount())
            {
                if(Core::String(item->child(iChild)->text(0).toStdString().c_str()) == assetPath[ii])
                {
                    childFound = true;
                    item = item->child(iChild);
                }
                ++iChild;
            }
            ++ii;
        }
    }

    return item;
}
//-----------------------------------------------------------------------------
void VFSExplorer::contextMenuEvent(QContextMenuEvent *event)
{
    QTreeWidgetItem * current = currentItem();

    if(current == NULL)
    {
        current = topLevelItem(0);
        setCurrentItem(current);
    }

    QMenu menu("VFSExplorerMenu", this);
    QAction * newDirAction = menu.addAction("New Directory...");
    
    QAction * optimizeModelAction = NULL;
    QAction * generateLightmapsAction = NULL;
    QAction * generateOcclusionmapsAction = NULL;
    QAction * importTextureAction = NULL;
    QAction * importModelAction = NULL;
    QAction * importTerrainAction = NULL;
    QAction * importPicBankAction = NULL;

    menu.addSeparator();
    if(current->type() == getTreeViewItemType(EVFS_DIR))
    {
        importTextureAction = menu.addAction("Import texture...");
        importModelAction = menu.addAction("Import model...");
        importTerrainAction = menu.addAction("Import terrain...");
        importPicBankAction = menu.addAction("Import picture bank...");
        menu.addSeparator();
    }
    else
    {
        if(current->text(0).endsWith(".mdl.xml"))
        {
            optimizeModelAction = menu.addAction("Optimize model");
            menu.addSeparator();
        }
        else if(current->text(0).endsWith(".world.xml"))
        {
            generateOcclusionmapsAction = menu.addAction("Generate occlusion");
            generateLightmapsAction = menu.addAction("Generate lightmaps");
            menu.addSeparator();
        }
    }

    QAction * refreshAction = menu.addAction("Refresh");

    QAction * action = menu.exec(event->globalPos());

    if(action != NULL)
    {
        if(action == newDirAction)
        {
            newDirectory();
        }
        else if(action == importTextureAction)
        {
            importTexture();
        }
        else if(action == importModelAction)
        {
            importModel();
        }
        else if(action == importTerrainAction)
        {
            importTerrain();
        }
        else if(action == importPicBankAction)
        {
            importPicBank();
        }
        else if(action == refreshAction)
        {
            refreshTree();
        }
        else if(action == optimizeModelAction)
        {
            optimizeModel();
        }
        else if(action == generateOcclusionmapsAction)
        {
            generateOcclusionmaps();
        }
        else if(action == generateLightmapsAction)
        {
            generateLightmaps();
        }
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::refreshTree()
{
    //On sauvegarde l'item courant
    QTreeWidgetItem * current = currentItem();
    if(current == NULL)
        current = topLevelItem(0);

    Core::String currentPath = locate(current);

    //On récupère les items ouvert
    Core::List<Core::String> expandedItems;
    QTreeWidgetItem * root = topLevelItem(0);
    getExpandedItems(root, expandedItems);

    //On reconstruit l'arbre
    clear();
    populateTree();

    //On réouvre les items
    for(int32 iExpanded = 0; iExpanded < expandedItems.size(); ++iExpanded)
    {
        QTreeWidgetItem * item = locateFromString(expandedItems[iExpanded]);
        if(item != NULL)
        {
            expandItem(item);
        }
    }

    //On restaure l'item courant
    current = locateFromString(currentPath);
    if(current == NULL)
        current = topLevelItem(0);

    setCurrentItem(current);
}
//-----------------------------------------------------------------------------
void VFSExplorer::newDirectory()
{
    QTreeWidgetItem * current = currentItem();
    Core::List<Core::String> otherDirs;
    for(int32 ii=0; ii<current->childCount(); ++ii)
    {
        QTreeWidgetItem * item = current->child(ii);
        if(item->type() == getTreeViewItemType(EVFS_DIR))
        {
            Core::String text(item->text(0).toStdString().c_str());
            otherDirs.push_back(text);
        }
    }

    VFSExplorerOperationDialog vfsExplorerOperationDialog(otherDirs, EVFSE_NEW_DIR, L"", this);
    if(vfsExplorerOperationDialog.exec())
    {
        Core::String newName = vfsExplorerOperationDialog.getNewName();
        QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_DIR));
        newItem->setText(0, String8(newName).c_str());
        newItem->setIcon(0, QIcon(":/icons/folder.png"));
        current->addChild(newItem);
        expandItem(current);
        setCurrentItem(newItem);
        Core::String vfsLocation = locate(newItem);

        _vfs.createDir(vfsLocation);
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importTexture()
{
    ImportTextureDialog importTextureDialog(this);
    if(importTextureDialog.fileOpen() && importTextureDialog.exec())
    {
        QTreeWidgetItem * current = currentItem();

        Core::String textureBaseName = locate(current);

        Core::List<Core::String> imageNames = importTextureDialog.getImageNames();
        for(int32 ii=0; ii<imageNames.size(); ++ii)
        {
            Core::String textureName = textureBaseName;
            if(textureName != L"/")
                textureName << L"/";
            if(imageNames.size() == 1)
                textureName << importTextureDialog.getTextureName() << L".tex"; //pas de getFileBaseName ici, la dialog se charge de supprimer l'extention du fichier
            else
                textureName << Core::System::getFileBaseName(imageNames[ii]) << L".tex";

            Ptr<ImportTextureInfo> pImportTextureInfo(new ImportTextureInfo());
                pImportTextureInfo->imageName   = imageNames[ii];
                pImportTextureInfo->textureName     = textureName;
                pImportTextureInfo->pTextureOptions = importTextureDialog.getTextureOptions();
                pImportTextureInfo->openFile        =   true;
            Ptr<Core::FileInputStream> pFileInputStream(new Core::FileInputStream());
            pFileInputStream->open(imageNames[ii]);

            pImportTextureInfo->pStreamImage = pFileInputStream;
            emit importAsset(pImportTextureInfo);
        }
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importModel()
{
    ImportModelDialog importModelDialog(this);
    
    if(importModelDialog.fileOpen() && importModelDialog.exec())
    {
        QTreeWidgetItem * current = currentItem();

        Core::String modelBaseName = locate(current);

        Core::List<Core::String> modelNames = importModelDialog.getModelNames();
        for(int32 ii=0; ii<modelNames.size(); ++ii)
        {
            Core::String modelName = modelBaseName;
            if(modelName != L"/")
                modelName << L"/";
            
            modelName << Core::System::getFileBaseName(modelNames[ii]) << L".mdl";

            Ptr<ImportModelInfo> pImportModelInfo(new ImportModelInfo());
            pImportModelInfo->sourceName = modelNames[ii];
            pImportModelInfo->modelName = modelName;
            importModelDialog.getModelPackingOptions(pImportModelInfo->modelPackingOptions);
            
            Ptr<Core::FileInputStream> pFileInputStream(new Core::FileInputStream());
            pFileInputStream->open(modelNames[ii]);

            pImportModelInfo->pVFS = _pVFS;
            pImportModelInfo->pStreamModel = pFileInputStream;

            emit importAsset(pImportModelInfo);
        }
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importPicBank()
{
    ImportPicBankDialog importPicBankDialog(this);
    if(importPicBankDialog.dirOpen() && importPicBankDialog.exec())
    {
        QTreeWidgetItem * current = currentItem();
        Core::String picBankName = locate(current);

        if(picBankName != L"/")
            picBankName << L"/";

        picBankName << importPicBankDialog.getPicBankName() << L".pbk";

        Ptr<ImportPicBankInfo> pImportPicBankInfo(new ImportPicBankInfo());
        pImportPicBankInfo->picBankName = picBankName;
        pImportPicBankInfo->dirName = importPicBankDialog.getDirName();
        pImportPicBankInfo->maxSize = importPicBankDialog.getMaxSize();


        emit importAsset(pImportPicBankInfo);
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importTerrain()
{
    ImportTerrainDialog importTerrainDialog(this);
    if(importTerrainDialog.fileOpen() && importTerrainDialog.exec())
    {
        QTreeWidgetItem * current = currentItem();

        Core::String terrainName = locate(current);

        Core::String heightMap = importTerrainDialog.getHeightMap();
        Core::String normalMap = importTerrainDialog.getNormalMap();
        
        if(terrainName != L"/")
            terrainName << L"/";

        terrainName << importTerrainDialog.getTerrainName() << L".tmdl";

        Ptr<ImportTerrainInfo> pImportTerrainInfo(new ImportTerrainInfo());
        pImportTerrainInfo->heightMapName = heightMap;
        pImportTerrainInfo->normalMapName = normalMap;
        pImportTerrainInfo->pTerrainOptions = importTerrainDialog.getTerrainOptions();
        pImportTerrainInfo->terrainModelName = terrainName;

        Ptr<Core::FileInputStream> pStreamHeightMap(new Core::FileInputStream());
        pStreamHeightMap->open(heightMap);
        
        Ptr<Core::FileInputStream> pStreamNormalMap(new Core::FileInputStream());
        pStreamNormalMap->open(normalMap);

        pImportTerrainInfo->pStreamHeightMap = pStreamHeightMap;
        pImportTerrainInfo->pStreamNormalMap = pStreamNormalMap;

        emit importAsset(pImportTerrainInfo);
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::generateLightmaps()
{
    GenerateLightmapsDialog genDialog(this);

    if(genDialog.exec())
    {
        QTreeWidgetItem * current = currentItem();
        String currentFile = locate(current);

        Ptr<LightmapsGenerationInfo> pLightmapsInfo = genDialog.getInfo();

        pLightmapsInfo->sourceWorld = Core::System::getPath(currentFile) + Core::System::getFileBaseName(currentFile);
        pLightmapsInfo->lmPath = Core::System::getPath(pLightmapsInfo->sourceWorld);
        pLightmapsInfo->lmPath << Core::System::getFileBaseName(Core::System::getFileBaseName(pLightmapsInfo->sourceWorld))
                               << pLightmapsInfo->suffix;

        Ptr<ImportWorldInfo> pWorldInfo(new ImportWorldInfo());
        pWorldInfo->lightmaps = *pLightmapsInfo;
        pWorldInfo->worldFileName = pLightmapsInfo->lmPath;
        pWorldInfo->worldFileName << pLightmapsInfo->suffix << L".world";

        Ptr<Core::InputStream> pInputDoc = _pVFS->readFile(pLightmapsInfo->sourceWorld);
        if(pInputDoc == null)
            throw Core::Exception(L"Missing source world for lightmap generation");

        Core::XMLDocument doc;
        doc.loadDocument(*pInputDoc);

        pWorldInfo->lightmaps.pWorld = Ptr<Universe::World>(new Universe::World(_pRenderer, _pVFS, doc));
        pWorldInfo->lightmaps.pWorld->prepareForPacking();

        emit importAsset(pWorldInfo);
    }
}

//-----------------------------------------------------------------------------
void VFSExplorer::generateOcclusionmaps()
{
    GenerateOcclusionmapsDialog genDialog(this);

    if(genDialog.exec())
    {
        QTreeWidgetItem * current = currentItem();
        String currentFile = locate(current);

        Ptr<OcclusionmapsGenerationInfo> pOcclusionmapsInfo = genDialog.getInfo();

        pOcclusionmapsInfo->sourceWorld = Core::System::getPath(currentFile) + Core::System::getFileBaseName(currentFile);
        pOcclusionmapsInfo->omPath = Core::System::getPath(pOcclusionmapsInfo->sourceWorld);
        pOcclusionmapsInfo->omPath << Core::System::getFileBaseName(Core::System::getFileBaseName(pOcclusionmapsInfo->sourceWorld))
                               << pOcclusionmapsInfo->suffix;

        Ptr<ImportWorldInfo> pWorldInfo(new ImportWorldInfo());
        pWorldInfo->occlusionmaps = *pOcclusionmapsInfo;
        pWorldInfo->worldFileName = pOcclusionmapsInfo->omPath;
        pWorldInfo->worldFileName << pOcclusionmapsInfo->suffix << L".world";

        Ptr<Core::InputStream> pInputDoc = _pVFS->readFile(pOcclusionmapsInfo->sourceWorld);
        if(pInputDoc == null)
            throw Core::Exception(L"Missing source world for occlusionmaps generation");

        Core::XMLDocument doc;
        doc.loadDocument(*pInputDoc);

        pWorldInfo->occlusionmaps.pWorld = Ptr<Universe::World>(new Universe::World(_pRenderer, _pVFS, doc));
        pWorldInfo->occlusionmaps.pWorld->prepareForPacking();

        emit importAsset(pWorldInfo);
    }
}

//-----------------------------------------------------------------------------
void VFSExplorer::optimizeModel()
{
    QTreeWidgetItem * current = currentItem();

    Core::String modelXmlName = locate(current);
    Ptr<Core::InputStream> pStreamFile = _vfs.readFile(modelXmlName);
    Ptr<Core::XMLDocument> pXMLModelInfo(new Core::XMLDocument());
    pXMLModelInfo->loadDocument(*pStreamFile);

    Ptr<ImportModelInfo> pImportModelInfo(new ImportModelInfo());
    pImportModelInfo->importXML(pXMLModelInfo);
    
    ImportModelDialog importModelDialog(this, pImportModelInfo);
    if(importModelDialog.exec())
    {
        importModelDialog.getModelPackingOptions(pImportModelInfo->modelPackingOptions);
        pImportModelInfo->import = IMPORT_MODEL_GENERATE;
        Ptr<Core::InputStream> pModelStreamFile = _vfs.readFile(pImportModelInfo->modelName);
        pImportModelInfo->pModel->read(*pModelStreamFile);

        emit importAsset(pImportModelInfo);
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::openSource(const Ptr<ImportInfo> & pImportInfo)
{
    if(pImportInfo->assetType == TEXTURE_ASSET)
    {
        Ptr<ImportTextureInfo> pImportTextureInfo = LM_DEBUG_PTR_CAST<ImportTextureInfo>(pImportInfo);
        Ptr<Core::FileInputStream> pFileInputStream(new Core::FileInputStream());
        pFileInputStream->open(pImportTextureInfo->imageName);
        pImportTextureInfo->pStreamImage = pFileInputStream;

        emit importAsset(pImportTextureInfo);
    }
    else if(pImportInfo->assetType == TERRAIN_ASSET)
    {
        Ptr<ImportTerrainInfo> pImportTerrainInfo = LM_DEBUG_PTR_CAST<ImportTerrainInfo>(pImportInfo);
        
        Ptr<Core::FileInputStream> pHeightMapInputStream(new Core::FileInputStream());
        pHeightMapInputStream->open(pImportTerrainInfo->heightMapName);
        pImportTerrainInfo->pStreamHeightMap = pHeightMapInputStream;
        
        Ptr<Core::FileInputStream> pNormalMapInputStream(new Core::FileInputStream());
        pNormalMapInputStream->open(pImportTerrainInfo->normalMapName);
        pImportTerrainInfo->pStreamNormalMap = pNormalMapInputStream;

        emit importAsset(pImportTerrainInfo);
    }
    else
    {
        Core::String message;
        message << L"Cannot open source, the asset type is unknown : " << pImportInfo->assetType;
        QMessageBox::critical(this, "Nyx", String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importDone(const Ptr<ImportInfo> & pImportInfo)
{
    if(pImportInfo->assetType == TEXTURE_ASSET)
    {
        importTextureDone(LM_DEBUG_PTR_CAST<ImportTextureInfo>(pImportInfo));
    }
    else if(pImportInfo->assetType == MODEL_ASSET)
    {
        importModelDone(LM_DEBUG_PTR_CAST<ImportModelInfo>(pImportInfo));
    }
    else if(pImportInfo->assetType == TERRAIN_ASSET)
    {
        importTerrainDone(LM_DEBUG_PTR_CAST<ImportTerrainInfo>(pImportInfo));
    }
    else if(pImportInfo->assetType == PIC_BANK_ASSET)
    {
        importPicBankDone(LM_DEBUG_PTR_CAST<ImportPicBankInfo>(pImportInfo));
    }
    else if(pImportInfo->assetType == WORLD_ASSET)
    {
        Ptr<ImportWorldInfo> pWorldInfo = LM_DEBUG_PTR_CAST<ImportWorldInfo>(pImportInfo);
        
        if(pWorldInfo->lightmaps.sourceWorld != String::EMPTY)
            lightmapsGenerationDone(pWorldInfo);
        else
            occlusionmapsGenerationDone(pWorldInfo);
    }
    else
    {
        Core::String message;
        message << L"Import failed, the asset type is unknown : " << pImportInfo->assetType;
        QMessageBox::critical(this, "Nyx", String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importTextureDone(const Ptr<ImportTextureInfo> & pTextureInfo)
{
    QTreeWidgetItem * item = locateFromString(Core::System::getPath(pTextureInfo->textureName));
    if(item != NULL)
    {
        Ptr<Core::XMLDocument> pXMLTextureInfo(new Core::XMLDocument());
        pTextureInfo->exportXML(pXMLTextureInfo);

        Core::String textureInfoName;
        textureInfoName << pTextureInfo->textureName << L".xml";
        Ptr<Core::OutputStream> pTextureInfoOutput = _vfs.writeFile(textureInfoName);
        pXMLTextureInfo->saveDocument(*pTextureInfoOutput);
        pTextureInfoOutput->close();

        Core::String newItemName = Core::System::getFileName(textureInfoName);

        bool childAlreadyExists = false;
        for(int32 iItem=0; iItem<item->childCount() && !childAlreadyExists; ++iItem)
        {
            String itemText(item->child(iItem)->text(0).toStdString().c_str());
            if( itemText == newItemName )
                childAlreadyExists = true;
        }

        if(!childAlreadyExists)
        {
            QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_FILE));
            newItem->setText(0, String8(newItemName).c_str());
            newItem->setIcon(0, QIcon(":/icons/texture.png"));
            item->addChild(newItem);
        }

        Ptr<Core::OutputStream> pTextureOutput = _vfs.writeFile(pTextureInfo->textureName);
        pTextureInfo->pTexture->write(*pTextureOutput);
        pTextureOutput->close();

        if (pTextureInfo->openFile == true)
        {
            pTextureInfo->openFile = false;
            QTreeWidgetItem * textureItem = locateFromString(textureInfoName);
            openItemProperties(textureItem, 0);
        }
    }
    else
    {
        Core::String message;
        message << L"An error has occured while trying create the texture : " << pTextureInfo->textureName;
        QMessageBox::critical(this, "Nyx", String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importModelDone(const Ptr<ImportModelInfo> & pModelInfo)
{
    QTreeWidgetItem * item = locateFromString(Core::System::getPath(pModelInfo->modelName));
    if(item != NULL)
    {
        Ptr<Core::XMLDocument> pXMLModelInfo(new Core::XMLDocument());
        pModelInfo->exportXML(pXMLModelInfo);

        Core::String modelInfoName;
        modelInfoName << pModelInfo->modelName << L".xml";
        Ptr<Core::OutputStream> pModelInfoOutput = _vfs.writeFile(modelInfoName);
        pXMLModelInfo->saveDocument(*pModelInfoOutput);
        pModelInfoOutput->close();

        Core::String newItemName = Core::System::getFileName(modelInfoName);

        bool childAlreadyExists = false;
        for(int32 iItem=0; iItem<item->childCount() && !childAlreadyExists; ++iItem)
        {
            String itemText(item->child(iItem)->text(0).toStdString().c_str());
            if( itemText == newItemName )
                childAlreadyExists = true;
        }

        if(!childAlreadyExists)
        {
            QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_FILE));
            newItem->setText(0, String8(newItemName).c_str());
            newItem->setIcon(0, QIcon(":/icons/model.png"));
            item->addChild(newItem);
        }

        Ptr<Core::OutputStream> pModelOutput = _vfs.writeFile(pModelInfo->modelName);
        pModelInfo->pModel->write(*pModelOutput);
        pModelOutput->close();

        QTreeWidgetItem * modelItem = locateFromString(modelInfoName);
        openItemProperties(modelItem, 0);
    }
    else
    {
        Core::String message;
        message << L"An error has occured while trying create the model : " << pModelInfo->modelName;
        QMessageBox::critical(this, "Nyx", String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::importTerrainDone(const Ptr<ImportTerrainInfo> & pTerrainInfo)
{
    QTreeWidgetItem * item = locateFromString(Core::System::getPath(pTerrainInfo->terrainModelName));
    if(item != NULL)
    {
        //On recopie la normal map dans le dossier partagé à coté de la heightMap.
        pTerrainInfo->normalMapName = Core::System::getPath(pTerrainInfo->heightMapName);
        pTerrainInfo->normalMapName << Core::System::getFileBaseName(pTerrainInfo->terrainModelName) << L"-normal.jpg";

        Ptr<Core::XMLDocument> pXMLTerrainInfo(new Core::XMLDocument());
        pTerrainInfo->exportXML(pXMLTerrainInfo);

        Core::String terrainInfoName;
        terrainInfoName << pTerrainInfo->terrainModelName << L".xml";
        Ptr<Core::OutputStream> pTerrainInfoOutput = _vfs.writeFile(terrainInfoName);
        pXMLTerrainInfo->saveDocument(*pTerrainInfoOutput);
        pTerrainInfoOutput->close();

        Core::String newItemName = Core::System::getFileName(terrainInfoName);

        bool childAlreadyExists = false;
        for(int32 iItem=0; iItem<item->childCount() && !childAlreadyExists; ++iItem)
        {
            String itemText(item->child(iItem)->text(0).toStdString().c_str());
            if( itemText == newItemName )
                childAlreadyExists = true;
        }

        if(!childAlreadyExists)
        {
            QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_FILE));
            newItem->setText(0, String8(newItemName).c_str());
            newItem->setIcon(0, QIcon(":/icons/terrain.png"));
            item->addChild(newItem);
        }

        Ptr<Core::OutputStream> pTerrainOutput = _vfs.writeFile(pTerrainInfo->terrainModelName);
        pTerrainInfo->pTerrainModel->write(*pTerrainOutput);
        pTerrainOutput->close();

        pTerrainInfo->pTerrainOptions->pNormalMap->saveFile(pTerrainInfo->normalMapName);

        QTreeWidgetItem * terrainItem = locateFromString(terrainInfoName);
        openItemProperties(terrainItem, 0);
    }
    else
    {
        Core::String message;
        message << L"An error has occured while trying create the model : " << pTerrainInfo->terrainModelName;
        QMessageBox::critical(this, "Nyx", String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::lightmapsGenerationDone(const Ptr<ImportWorldInfo> & pWorldInfos)
{
    Ptr<Core::VirtualFileSystem> pVFS(pWorldInfos->lightmaps.pWorld->getRessourcesPool()->getVFS());

    Ptr<Core::OutputStream> pWorldOutput(pVFS->writeFile(pWorldInfos->worldFileName));
    Ptr<Core::OutputStream> pMetaOutput(pVFS->writeFile(pWorldInfos->worldFileName + L".xml"));

    if(pWorldOutput == null) throw Core::Exception(L"Unable to write open target world file");
    if(pMetaOutput == null) throw Core::Exception(L"Unable to write open target world meta file");

    // Fichier infos .xml
    {
        Ptr<Core::XMLDocument> pDoc(new Core::XMLDocument());
        pWorldInfos->exportXML(pDoc);
        pDoc->saveDocument(*pMetaOutput);
    }
    // Fichiers infos .xml des textures générées pour les visualiser sous Nyx.
    {
        Core::List<String>  pics;
        pVFS->listFiles(pWorldInfos->lightmaps.lmPath, pics);

        ImportTextureInfo texInfo;
        texInfo.pTextureOptions = Ptr<Workflow::TextureOptions>(new Workflow::TextureOptions());
        texInfo.pTextureOptions->format = Assets::TEX_FORMAT_DXTC1;

        for(int32 ii=0; ii < pics.size(); ii++)
        {
            if(pics[ii].endsWith(L".tex"))
            {
                Ptr<Core::XMLDocument> pDoc(new Core::XMLDocument());
                texInfo.textureName = pWorldInfos->lightmaps.lmPath + L"/" + pics[ii];
                texInfo.exportXML(pDoc);
                pDoc->saveDocument(*pVFS->writeFile(texInfo.textureName + L".xml"));
            }
        }
    }
    // Fichier world
    {
        Core::XMLDocument doc;
        pWorldInfos->lightmaps.pWorld->exportWorldXML(doc);
        doc.saveDocument(*pWorldOutput);
    }

    refreshTree();
}
//-----------------------------------------------------------------------------
void VFSExplorer::occlusionmapsGenerationDone(const Ptr<ImportWorldInfo> & pWorldInfos)
{
    Ptr<Core::VirtualFileSystem> pVFS(pWorldInfos->occlusionmaps.pWorld->getRessourcesPool()->getVFS());

    Ptr<Core::OutputStream> pWorldOutput(pVFS->writeFile(pWorldInfos->worldFileName));
    Ptr<Core::OutputStream> pMetaOutput(pVFS->writeFile(pWorldInfos->worldFileName + L".xml"));

    if(pWorldOutput == null) throw Core::Exception(L"Unable to write open target world file");
    if(pMetaOutput == null) throw Core::Exception(L"Unable to write open target world meta file");

    // Fichier infos .xml
    {
        Ptr<Core::XMLDocument> pDoc(new Core::XMLDocument());
        pWorldInfos->exportXML(pDoc);
        pDoc->saveDocument(*pMetaOutput);
    }
    // Fichiers infos .xml des textures générées pour les visualiser sous Nyx.
    {
        Core::List<String>  pics;
        pVFS->listFiles(pWorldInfos->occlusionmaps.omPath, pics);

        ImportTextureInfo texInfo;
        texInfo.pTextureOptions = Ptr<Workflow::TextureOptions>(new Workflow::TextureOptions());
        texInfo.pTextureOptions->format = pWorldInfos->occlusionmaps.compressedTextures ? Assets::TEX_FORMAT_DXTC1 : Assets::TEX_FORMAT_ARGB8;

        for(int32 ii=0; ii < pics.size(); ii++)
        {
            if(pics[ii].startsWith(L"om-") && pics[ii].endsWith(L".tex"))
            {
                Ptr<Core::XMLDocument> pDoc(new Core::XMLDocument());
                texInfo.textureName = pWorldInfos->occlusionmaps.omPath + L"/" + pics[ii];
                texInfo.exportXML(pDoc);
                pDoc->saveDocument(*pVFS->writeFile(texInfo.textureName + L".xml"));
            }
        }
    }
    // Fichier world
    {
        Core::XMLDocument doc;
        pWorldInfos->occlusionmaps.pWorld->exportWorldXML(doc);
        doc.saveDocument(*pWorldOutput);
    }

    refreshTree();
}
//-----------------------------------------------------------------------------
void VFSExplorer::importPicBankDone(const Ptr<ImportPicBankInfo> & pPicBankInfo)
{
    QTreeWidgetItem * item = locateFromString(Core::System::getPath(pPicBankInfo->picBankName));
    if(item != NULL)
    {
        String picBankInfosName(pPicBankInfo->picBankName + L".xml");

        // Fichier infos .xml
        {
            Ptr<Core::XMLDocument> pXMLPicBankInfo(new Core::XMLDocument());
            pPicBankInfo->exportXML(pXMLPicBankInfo);

            Ptr<Core::OutputStream> pPicBankInfoOutput = _vfs.writeFile(picBankInfosName);
            pXMLPicBankInfo->saveDocument(*pPicBankInfoOutput);
            pPicBankInfoOutput->close();
        }
        // MAJ tree
        {
            String newItemName = Core::System::getFileName(picBankInfosName);

            bool childAlreadyExists = false;
            for(int32 iItem=0; iItem<item->childCount() && !childAlreadyExists; ++iItem)
            {
                String itemText(item->child(iItem)->text(0).toStdString().c_str());
                if(itemText == newItemName)
                    childAlreadyExists = true;
            }

            if(!childAlreadyExists)
            {
                QTreeWidgetItem * newItem = new QTreeWidgetItem(getTreeViewItemType(EVFS_FILE));
                newItem->setText(0, String8(newItemName).c_str());
                newItem->setIcon(0, QIcon(":/icons/terrain.png"));
                item->addChild(newItem);
            }
        }

        // Save pic bank
        {
            Ptr<Core::OutputStream> pPicBankOutput = _vfs.writeFile(pPicBankInfo->picBankName);
            pPicBankInfo->pPicBank->write(*pPicBankOutput);
            pPicBankOutput->close();

            if (pPicBankInfo->gDocId != -1)
            {
                _pCtrl->deleteGuiDocument(pPicBankInfo->gDocId);
            }
            
            QTreeWidgetItem * pbkItem = locateFromString(picBankInfosName);
            openItemProperties(pbkItem, 0);
        }
    }
    else
    {
        Core::String message;
        message << L"An error has occured while trying create the picture bank : " << pPicBankInfo->picBankName;
        QMessageBox::critical(this, "Nyx", String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
void VFSExplorer::openItemProperties(QTreeWidgetItem * item, int column)
{
    if(item->type() != getTreeViewItemType(EVFS_FILE))
    {
        item->setExpanded(true);
        return;
    }

    Core::String itemName = locate(item);
    try
    {
        Ptr<ImportInfo> pInfo;

        if (itemName.endsWith(L".xml"))
            pInfo = getImportInfo(itemName);
        else if(itemName.endsWith(L".lua"))
        {
            Ptr<ImportScriptInfo> pScriptInfo (new ImportScriptInfo());
            pScriptInfo->fileName = itemName;
            pInfo = pScriptInfo;
        }

        if (itemName.endsWith(L".wav") || itemName.endsWith(L".ogg")) return;
        emit openItemProperties(pInfo);
    }
    catch(Core::Exception & e)
    {
        Core::String message;
        message << L"An error occured while opening a document in the VFS :\n" <<e.getMessage();
        QMessageBox::critical(this, "Nyx", Core::String8(message).c_str());
    }
}
//-----------------------------------------------------------------------------
Ptr<ImportInfo> VFSExplorer::getImportInfo(const Core::String & itemName) const
{
    Ptr<Core::InputStream> pStreamFile = _vfs.readFile(itemName);
    Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());
    pXmlDocument->loadDocument(*pStreamFile);

    Core::String assetType;
    pXmlDocument->getAttribute(L"asset", L"type", assetType);

    Ptr<ImportInfo> pInfo;

    if(assetType == EPI::toString(TEXTURE_ASSET))
    {
        Ptr<ImportTextureInfo> pTextureInfo(new ImportTextureInfo());
        pTextureInfo->importXML(pXmlDocument);
        pInfo = pTextureInfo;
    }
    else if(assetType == EPI::toString(MODEL_ASSET))
    {
        Ptr<ImportModelInfo> pModelInfo(new ImportModelInfo());
        pModelInfo->importXML(pXmlDocument);

        Ptr<Core::InputStream> pStreamModel = _vfs.readFile(pModelInfo->modelName);
        if(pStreamModel != null)
            pModelInfo->pModel->read(*pStreamModel);

        pInfo = pModelInfo;
    }
    else if(assetType == EPI::toString(TERRAIN_ASSET))
    {
        Ptr<ImportTerrainInfo> pTerrainInfo(new ImportTerrainInfo());
        pTerrainInfo->importXML(pXmlDocument);

        Ptr<Core::InputStream> pStreamTerrain = _vfs.readFile(pTerrainInfo->terrainModelName);
        if(pStreamTerrain != null)
            pTerrainInfo->pTerrainModel->read(*pStreamTerrain);

        pInfo = pTerrainInfo;
    }
    else if(assetType == EPI::toString(PARTICLES_ASSET))
    {
        Ptr<ImportParticlesInfo> pParticlesInfo(new ImportParticlesInfo());
        pParticlesInfo->importXML(pXmlDocument);

        pInfo = pParticlesInfo;
    }
    else if(assetType == EPI::toString(WORLD_ASSET))
    {
        Ptr<ImportWorldInfo> pWorldInfo(new ImportWorldInfo());
        pWorldInfo->importXML(pXmlDocument);

        pInfo = pWorldInfo;
    }
    else if(assetType == EPI::toString(GROUP_ASSET))
    {
        Ptr<ImportGroupInfo> pGroupInfo(new ImportGroupInfo());
        pGroupInfo->importXML(pXmlDocument);

        pInfo = pGroupInfo;
    }
    else if(assetType == EPI::toString(SPLINE_ASSET))
    {
        Ptr<ImportSplineInfo> pSplineInfo(new ImportSplineInfo());
        pSplineInfo->importXML(pXmlDocument);

        pInfo = pSplineInfo;
    }
    else if(assetType == EPI::toString(PIC_BANK_ASSET))
    {
        Ptr<ImportPicBankInfo> pPicBankInfo(new ImportPicBankInfo());
        pPicBankInfo->importXML(pXmlDocument);

        pInfo = pPicBankInfo;
    }
    else
    {
        Core::String message;
        message << L"Unknown asset type : " << assetType << L".";
        throw Core::Exception(message);
    }

    return pInfo;
}
//-----------------------------------------------------------------------------
QMimeData * VFSExplorer::mimeData(const QList<QTreeWidgetItem *> items) const
{
    QMimeData * mimeData = NULL;

    if(items.size() == 1)
    {
        QTreeWidgetItem * item = currentItem();
        Core::String itemName = locate(items[0]);
        
        mimeData = new QMimeData();

        if(item->type() == getTreeViewItemType(EVFS_DIR))
        {
            mimeData->setData("directory", Core::String8(itemName).c_str());
        }
        else
        {
            if(itemName.endsWith(L".ogg") || itemName.endsWith(L".wav"))
            {
                mimeData->setData("asset/sound", String8(itemName.c_str()).c_str());
            }
            else
            {
                Ptr<ImportInfo> pInfo = getImportInfo(itemName);

                if(pInfo->assetType == TEXTURE_ASSET)
                {
                    Ptr<ImportTextureInfo> pTextureInfo = LM_DEBUG_PTR_CAST<ImportTextureInfo>(pInfo);
                    mimeData->setData("asset/texture", Core::String8(pTextureInfo->textureName).c_str());
                }
                else if(pInfo->assetType == MODEL_ASSET)
                {
                    Ptr<ImportModelInfo> pModelInfo = LM_DEBUG_PTR_CAST<ImportModelInfo>(pInfo);
                    mimeData->setData("asset/model", Core::String8(pModelInfo->modelName).c_str());
                }
                else if(pInfo->assetType == TERRAIN_ASSET)
                {
                    Ptr<ImportTerrainInfo> pTerrainInfo = LM_DEBUG_PTR_CAST<ImportTerrainInfo>(pInfo);
                    mimeData->setData("asset/terrain", Core::String8(pTerrainInfo->terrainModelName).c_str());
                }
                else if(pInfo->assetType == WORLD_ASSET)
                {
                    Ptr<ImportWorldInfo> pWorldInfo = LM_DEBUG_PTR_CAST<ImportWorldInfo>(pInfo);
                    mimeData->setData("asset/world", Core::String8(pWorldInfo->worldFileName).c_str());
                }
                else if(pInfo->assetType == GROUP_ASSET)
                {
                    Ptr<ImportGroupInfo> pGroupInfo = LM_DEBUG_PTR_CAST<ImportGroupInfo>(pInfo);
                    mimeData->setData("asset/group", Core::String8(pGroupInfo->fileName).c_str());
                }
                else if(pInfo->assetType == SPLINE_ASSET)
                {
                    Ptr<ImportSplineInfo> pSplineInfo = LM_DEBUG_PTR_CAST<ImportSplineInfo>(pInfo);
                    mimeData->setData("asset/spline", Core::String8(pSplineInfo->fileName).c_str());
                }
                else if(pInfo->assetType == PARTICLES_ASSET)
                {
                    Ptr<ImportParticlesInfo> pParticlesInfo = LM_DEBUG_PTR_CAST<ImportParticlesInfo>(pInfo);
                    mimeData->setData("asset/particles", Core::String8(pParticlesInfo->particlesFileName).c_str());
                }
                else if(pInfo->assetType == PIC_BANK_ASSET)
                {
                    Ptr<ImportPicBankInfo> pPicBankInfo = LM_DEBUG_PTR_CAST<ImportPicBankInfo>(pInfo);
                    mimeData->setData("asset/picbank", Core::String8(pPicBankInfo->picBankName).c_str());
                }
            }
        }
    }

    return mimeData;
}
//-----------------------------------------------------------------------------
void VFSExplorer::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        QList<QTreeWidgetItem*> selecItems = this->selectedItems();

        if (selecItems.size()==1)
        {
            openItemProperties(selecItems[0], 0);
        }
    }
}
//-----------------------------------------------------------------------------
} //namespace EPI
