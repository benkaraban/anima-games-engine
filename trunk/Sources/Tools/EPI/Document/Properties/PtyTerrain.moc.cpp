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
#include "PtyTerrain.moc.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>


#include <QtToolbox/LineEdit.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/CollapsibleWidget.moc.h>

#include <EPI/Document/Properties/PtyTerrainLayer.moc.h>
#include <EPI/Document/StateRecorder.h>
#include <EPI/GUI/Widget/CustomLine.moc.h>
#include <EPI/GUI/Widget/TerrainLayersTree.moc.h>

namespace EPI
{
String EResolutionToString(EResolution type)
{
    String ret;

    switch(type)
    {
    case RESOLUTION_64:
        ret = L"64";
        break;
    case RESOLUTION_128:
        ret = L"128";
        break;
    case RESOLUTION_256:
        ret = L"256";
        break;
    case RESOLUTION_512:
        ret = L"512";
        break;
    case RESOLUTION_1024:
        ret = L"1024";
        break;
    case RESOLUTION_2048:
        ret = L"2048";
        break;
    case RESOLUTION_4096:
        ret = L"4096";
        break;
    }

    return ret;
}

int32 StringToEResolutionValue(const String& str)
{
    int32 ret = 0;

    if(str == EResolutionToString(RESOLUTION_64))
        ret = 64;
    else if(str == EResolutionToString(RESOLUTION_128))
        ret = 128;
    else if(str == EResolutionToString(RESOLUTION_256))
        ret = 256;
    else if(str == EResolutionToString(RESOLUTION_512))
        ret = 512;
    else if(str == EResolutionToString(RESOLUTION_1024))
        ret = 1024;
    else if(str == EResolutionToString(RESOLUTION_2048))
        ret = 2048;
    else if(str == EResolutionToString(RESOLUTION_4096))
        ret = 4096;

    return ret;
}

EResolution StringToEResolution(const String& str)
{
    EResolution ret = RESOLUTION_64;

    if(str == EResolutionToString(RESOLUTION_64))
        ret = RESOLUTION_64;
    else if(str == EResolutionToString(RESOLUTION_128))
        ret = RESOLUTION_128;
    else if(str == EResolutionToString(RESOLUTION_256))
        ret = RESOLUTION_256;
    else if(str == EResolutionToString(RESOLUTION_512))
        ret = RESOLUTION_512;
    else if(str == EResolutionToString(RESOLUTION_1024))
        ret = RESOLUTION_1024;
    else if(str == EResolutionToString(RESOLUTION_2048))
        ret = RESOLUTION_2048;
    else if(str == EResolutionToString(RESOLUTION_4096))
        ret = RESOLUTION_4096;

    return ret;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyTerrain::PtyTerrain( const Ptr<Universe::World>& pWorld,
                        const Ptr<Universe::NodeTerrain> & pNodeT,
                        const Ptr<DocumentRenderer> & pDocumentRenderer,
                        const Core::String& title)
:   PtyNode(pWorld, pNodeT, title),
    _pRessourcePool(pWorld->getRessourcesPool()),
    _pDocumentRenderer(pDocumentRenderer),
    _currentLayer(-1),
    _resolutionPreview(RESOLUTION_128),
    _resolutionSave(RESOLUTION_256),
    _modePreview(true),
    _pImportTerrainLayersInfo(null)
{
    setVisibleWidgetTransform(true);
    setVisibleWidgetTag(true);
    setVisibleWidgetShow(true);

   

    updateProperty();

    if(_layers.size() == 0)
    {
        Ptr<PtyTerrainLayer> pPtyTerrainLayer (new PtyTerrainLayer(L"ground^earth"));

        _layers.push_back(pPtyTerrainLayer);
        updateData();
    }

    _currentLayer = 0;
}

PtyTerrain::PtyTerrain(const PtyTerrain& other) :
    PtyNode(other)
{
    _pRessourcePool = getUniverseWorld()->getRessourcesPool();

    _pDocumentRenderer = other._pDocumentRenderer;

    _currentLayer = other._currentLayer;

    _resolutionPreview        = other._resolutionPreview;
    _resolutionSave           = other._resolutionSave;
    _modePreview              = other._modePreview;
    _pImportTerrainLayersInfo = other._pImportTerrainLayersInfo;
    
    _terrainName = other._terrainName;

    _globalNormalTexture = other._globalNormalTexture;
    _globalNormalWeight  = other._globalNormalWeight;


    _previewDiffuseTextures  = other._previewDiffuseTextures;
    _previewNormalTextures   = other._previewNormalTextures;
    _saveDiffuseTextures     = other._saveDiffuseTextures;
    _saveNormalTextures      = other._saveNormalTextures;

    _layers.clear();
    for (int32 ii=0; ii<other._layers.size(); ++ii)
    {
        _layers.push_back(LM_DEBUG_PTR_CAST<PtyTerrainLayer>(other._layers[ii]->clone()));
    }
}

void PtyTerrain::copyId(const Property& pSrc)
{
    PtyNode::copyId(pSrc);
    const PtyTerrain& pOther = reinterpret_cast<const PtyTerrain&>(pSrc);

    for (int32 ii=0; ii<pOther._layers.size(); ++ii)
    {
        _layers[ii]->copyId(*pOther._layers[ii]);
    }
}

void PtyTerrain::layerUpdated()
{
    updateData();
}

PtyTerrain::~PtyTerrain()
{
}

Ptr<PropertyWidget> PtyTerrain::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetTerrain> pPW (new PtyWidgetTerrain(pDataProxy, parent));
    return pPW;
}

void PtyTerrain::updateData()
{
    PtyNode::updateData();
    Ptr<Universe::NodeTerrain> pNodeTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(getUniverseNode());

    Core::List<Universe::NodeTerrain::Layer> & layersNode = pNodeTerrain->getLayers();
    layersNode.clear();

    for (int32 ii=0; ii<_layers.size(); ++ii)
    {
        Universe::NodeTerrain::Layer l;

        _layers[ii]->getLayer(l);
        layersNode.insert(layersNode.begin(), l);
    }

    if(_modePreview)
        if(_previewDiffuseTextures.size() != 0 && _previewNormalTextures.size())
            pNodeTerrain->setBlockTextures(_previewDiffuseTextures, _previewNormalTextures);
    else
        if(_saveDiffuseTextures.size() != 0 && _saveNormalTextures.size())
            pNodeTerrain->setBlockTextures(_saveDiffuseTextures, _saveNormalTextures);
}

void PtyTerrain::updateProperty()
{
    Ptr<Universe::NodeTerrain> pNodeTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(getUniverseNode());

    _globalNormalTexture = pNodeTerrain->getGlobalNormalTexture();
    _globalNormalWeight  = pNodeTerrain->getGlobalNormalWeight();
    pNodeTerrain->getBlockTextures(_saveDiffuseTextures, _saveNormalTextures);
    pNodeTerrain->getBlockTextures(_previewDiffuseTextures, _previewNormalTextures);
    _terrainName = pNodeTerrain->getTerrain();
    
    Core::List<Universe::NodeTerrain::Layer> & layers = pNodeTerrain->getLayers();
    int32 index = 0;
    _layers.clear();
    for(int32 ii=layers.size()-1; ii>=0; --ii)
    {
        Ptr<PtyTerrainLayer> pPtyTerrainLayer (new PtyTerrainLayer(layers[ii].name));
        _layers.push_back(pPtyTerrainLayer);
        pPtyTerrainLayer->setLayer(layers[ii]);
    }
}

bool PtyTerrain::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    bool flag = PtyNode::findPtyWithId(pResult, id);

    if (flag == false)
    {
        for (int32 ii=0; ii<_layers.size(); ++ii)
        {
            if (_layers[ii]->getId() == id)
            {
                pResult = _layers[ii];
                return true;
            }
        }
    }

    return false;
}

void PtyTerrain::save(bool preview, int32 resolution)
{
    Ptr<Universe::NodeTerrain> pNodeTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(getUniverseNode());

    Core::List<Universe::NodeTerrain::Layer> & layers = pNodeTerrain->getLayers();

    pNodeTerrain->setGlobalNormalTexture(_globalNormalTexture);
    pNodeTerrain->setGlobalNormalWeight(_globalNormalWeight);

    layers.resize(_layers.size());
    for(int ii = _layers.size()-1; ii >= 0; --ii)
    {
        layers[_layers.size()-1-ii].name            = _layers[ii]->getName();
        layers[_layers.size()-1-ii].diffuseTexName  = _layers[ii]->getDiffuseTexName();
        layers[_layers.size()-1-ii].normalTexName   = _layers[ii]->getNormalTexName();
        layers[_layers.size()-1-ii].maskTexName     = _layers[ii]->getMaskTexName();
        layers[_layers.size()-1-ii].width           = _layers[ii]->getWidth();
        layers[_layers.size()-1-ii].height          = _layers[ii]->getHeight();
        layers[_layers.size()-1-ii].angle           = _layers[ii]->getAngle();
        layers[_layers.size()-1-ii].color           = _layers[ii]->getColor();
    }

    String texturePath;
    if(preview)
        texturePath << L"/Temp/";
    else
        texturePath << Core::System::getPath(_terrainName);
    texturePath << Core::System::getFileBaseName(_terrainName);

    Ptr<ImportTerrainLayersInfo> pImportTerrainLayersInfo(new ImportTerrainLayersInfo());
    pImportTerrainLayersInfo->options.pPool = _pRessourcePool;
    pImportTerrainLayersInfo->options.pRenderer = _pDocumentRenderer->getRenderer();
    pImportTerrainLayersInfo->options.pVFS = _pDocumentRenderer->getVFS();
    pImportTerrainLayersInfo->options.path = texturePath;
    pImportTerrainLayersInfo->options.prefix = Core::System::getFileBaseName(_terrainName);
    pImportTerrainLayersInfo->options.compressTextures = !preview;
    pImportTerrainLayersInfo->options.pTerrain = pNodeTerrain;
    pImportTerrainLayersInfo->options.resolution.push_back(resolution);

    _pImportTerrainLayersInfo = pImportTerrainLayersInfo;
    emit generate(_pImportTerrainLayersInfo);
}

Ptr<Property> PtyTerrain::clone() const
{
    return Ptr<Property>(new PtyTerrain( *this ));
}

void PtyTerrain::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);
    Ptr<PtyTerrain> pPty = LM_DEBUG_PTR_CAST<PtyTerrain>(pSrc);

    _pRessourcePool = pPty->_pRessourcePool;
    _pDocumentRenderer = pPty->_pDocumentRenderer;
    _globalNormalTexture = pPty->_globalNormalTexture;
    _globalNormalWeight = pPty->_globalNormalWeight;
    _terrainName = pPty->_terrainName;
    _currentLayer = pPty->_currentLayer;
    _pImportTerrainLayersInfo = pPty->_pImportTerrainLayersInfo;
    _resolutionPreview = pPty->_resolutionPreview;
    _resolutionSave = pPty->_resolutionSave;
    _modePreview = pPty->_modePreview;

    _layers.clear();
    for (int32 ii=0; ii<pPty->_layers.size(); ++ii)
    {
        _layers.push_back(LM_DEBUG_PTR_CAST<PtyTerrainLayer>(pPty->_layers[ii]->clone()));
    }


    _previewDiffuseTextures = pPty->_previewDiffuseTextures;
    _previewNormalTextures = pPty->_previewNormalTextures;
    _saveDiffuseTextures = pPty->_saveDiffuseTextures;
    _saveNormalTextures = pPty->_saveNormalTextures;
    
    updateData();
}

Ptr<PtyTerrainLayer> PtyTerrain::addTerrainLayer(int32 index, const Core::String & name)
{
    Ptr<PtyTerrainLayer> pPtyTerrainLayer (new PtyTerrainLayer(name));

    _layers.insert(_layers.begin() + index, pPtyTerrainLayer);
    _currentLayer = index;

    updateData();
    return pPtyTerrainLayer;
}

void PtyTerrain::deleteLayer(int32 index)
{
    _layers.erase(_layers.begin() + index);
    _currentLayer = _currentLayer - 1;
}

Ptr<PtyTerrainLayer> PtyTerrain::getCurrentLayer() const
{
    if(_currentLayer != -1 && _layers.size() > _currentLayer)
        return _layers[_currentLayer];
    else
        return null;
}

void PtyTerrain::setCurrentLayer(int32 currentLayer)
{
    _currentLayer = currentLayer;
}

void PtyTerrain::setTerrain(const Core::String & terrainName)
{
    _terrainName = terrainName;
    Ptr<Universe::NodeTerrain> pNodeTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(getUniverseNode());

    pNodeTerrain->setTerrain(terrainName);
    if(_modePreview)
        pNodeTerrain->getBlockTextures(_previewDiffuseTextures, _previewNormalTextures);
    else
        pNodeTerrain->getBlockTextures(_saveDiffuseTextures, _saveNormalTextures);
}
//-----------------------------------------------------------------------------
Ptr<PtyTerrainLayer> PtyTerrain::getLayer(int32 index) const
{
    if(index >= 0 && index < _layers.size())
        return _layers[index];
    else
        return null;
}

void PtyTerrain::renameLayer(int32 index, Core::String name)
{
    LM_ASSERT(index < _layers.size());

    getLayer(index)->setName(name);
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetTerrain::PtyWidgetTerrain( const Ptr<PropertyWidgetDataProxy>& data,
                                    QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}

PtyWidgetTerrain::~PtyWidgetTerrain()
{}

void PtyWidgetTerrain::setupUi()
{
    _layout = new QVBoxLayout(NULL);
    _layout->setSpacing(1);
    _layout->setContentsMargins(0, 0, 0, 0);

    _terrainName = new QtToolbox::LineEdit(this, "Terrain name");
        _terrainName->pushAuthorizedDropMimeData("asset/terrain");
        _terrainName->setReadOnly(true);

    _groupBox = new QtToolbox::CollapsibleWidget(this, "Terrain", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupLayout = new QGridLayout(NULL);
        _groupLayout->setContentsMargins(0, 0, 0, 0);
        _groupLayout->setSpacing(1);

    _groupBoxGlobalNormal = new QtToolbox::CollapsibleWidget(this, "Global Normal");

    _groupBoxTexture = new QtToolbox::CollapsibleWidget(this, "Texture");

    QGridLayout* textureLayout = new QGridLayout(NULL);
        textureLayout->setContentsMargins(0, 0, 0, 0);
        textureLayout->setSpacing(1);
        _groupBoxTexture->setLayout(textureLayout);

    _globalNormalTexture = new CustomLine(this, "Normal texture");
        _globalNormalTexture->pushAuthorizedDropMimeData("asset/texture");
    _globalNormalTexture->setReadOnly(true);
        _globalNormalWeight = new QtToolbox::SingleSlidingDouble(this, "Normal weight", 0.0, 1.0);

    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
        Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
        _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);
        _pPWNode->collapse(true);

    _terrainLayersTree = new TerrainLayersTree(this);

    _previewRes = new QtToolbox::ComboBox(this, "Preview");
        for (int32 ii=0; ii<EResolution_COUNT; ++ii)
        {
            _previewRes->addItem(String8(EResolutionToString((EResolution)ii)).c_str());
        }
        _previewRes->selectItem(String8(EResolutionToString(RESOLUTION_128)).c_str());

    _saveRes = new QtToolbox::ComboBox(this, "Save");
        for (int32 ii=0; ii<EResolution_COUNT; ++ii)
        {
            _saveRes->addItem(String8(EResolutionToString((EResolution)ii)).c_str());
        }
        _saveRes->selectItem(String8(EResolutionToString(RESOLUTION_256)).c_str());

    _previewRadio   = new QRadioButton("Preview", this);
    _saveRadio      = new QRadioButton("Save", this);
    _previewRadio->setChecked(true);

    _generate = new QPushButton(QIcon(":/icons/gears.png"), "", this);



    _groupBoxGlobalNormal->getLayout()->addWidget(_globalNormalTexture);
    _groupBoxGlobalNormal->getLayout()->addWidget(_globalNormalWeight);

    textureLayout->addWidget(_terrainLayersTree, 0, 0, 6, 1);
    textureLayout->addWidget(_previewRes, 0, 1);
    textureLayout->addWidget(_saveRes, 1, 1);
    textureLayout->addWidget(_previewRadio, 2, 1);
    textureLayout->addWidget(_saveRadio, 3, 1);
    textureLayout->addWidget(_generate, 4, 1);


    _groupLayout->addWidget(_pPWNode.get());
    _groupLayout->addWidget(_groupBoxGlobalNormal);
    _groupLayout->addWidget(_groupBoxTexture);

    _groupBox->setLayout(_groupLayout);

    _layout->addWidget(_terrainName);
    _layout->addWidget(_groupBox);

    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());
    Ptr<PtyTerrainLayer> pCurrentLayer = pPtyTerrain->getCurrentLayer();
    Ptr<PropertyWidgetDataProxy> pDataProxyWidgetLayer (new PropertyWidgetDataProxy(pCurrentLayer, L"Terrain Layer", getDataProxy()->getStateRecorder()));
    _ptyWidgetLayer = pCurrentLayer->createPropertyWidget(pDataProxyWidgetLayer, this);
    _layout->addWidget(_ptyWidgetLayer.get());


    setLayout(_layout);

    

    getWidgetsForUndoRedo().push_back(_terrainName);
    getWidgetsForUndoRedo().push_back(_globalNormalTexture);
    getWidgetsForUndoRedo().push_back(_globalNormalWeight);
    getWidgetsForUndoRedo().push_back(_previewRadio);
    getWidgetsForUndoRedo().push_back(_saveRadio);
    getWidgetsForUndoRedo().push_back(_previewRes);
    getWidgetsForUndoRedo().push_back(_saveRes);

    

    connect(_generate, SIGNAL(clicked()), this, SLOT(generate()));

    connect(_ptyWidgetLayer.get(), SIGNAL(propertyChangeFinished()), 
                pPtyTerrain.get(), SLOT(layerUpdated()));

    makeConnectionLayerTree(true);

    PropertyWidget::setupUi();
}

void PtyWidgetTerrain::makeConnectionLayerTree(bool flag)
{
    if (flag==true)
    {
        connect(_terrainLayersTree, SIGNAL(layersOrderChanged()),              this, SLOT(layersOrderChanged()));
        connect(_terrainLayersTree, SIGNAL(layerDeleted(int32)),               this, SLOT(deleteLayer(int32)));
        connect(_terrainLayersTree, SIGNAL(currentLayerChanged(int32)),        this, SLOT(currentLayerChanged(int32)));
        connect(_terrainLayersTree, SIGNAL(layerCreated(int32, Core::String)), this, SLOT(createLayer(int32, Core::String)));
        connect(_terrainLayersTree, SIGNAL(layerRenamed(int32, Core::String)), this, SLOT(renameLayer(int32, Core::String)));
    }
    else
    {
        disconnect(_terrainLayersTree, SIGNAL(layersOrderChanged()),              this, SLOT(layersOrderChanged()));
        disconnect(_terrainLayersTree, SIGNAL(layerDeleted(int32)),               this, SLOT(deleteLayer(int32)));
        disconnect(_terrainLayersTree, SIGNAL(currentLayerChanged(int32)),        this, SLOT(currentLayerChanged(int32)));
        disconnect(_terrainLayersTree, SIGNAL(layerCreated(int32, Core::String)), this, SLOT(createLayer(int32, Core::String)));
        disconnect(_terrainLayersTree, SIGNAL(layerRenamed(int32, Core::String)), this, SLOT(renameLayer(int32, Core::String)));
    }
}

void PtyWidgetTerrain::readProperty()
{
    makeConnectionLayerTree(false);

    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());
    Ptr<Universe::NodeTerrain> pNodeTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(pPtyTerrain->getUniverseNode());

    _pPWNode->updatePropertyWidget();

    _terrainName->setText(Core::String8(pPtyTerrain->_terrainName).c_str());

    _globalNormalTexture->setText(pPtyTerrain->_globalNormalTexture);
    _globalNormalWeight->setSingleValue(pPtyTerrain->_globalNormalWeight);

    _saveRes->selectItem(String8(EResolutionToString(pPtyTerrain->_resolutionSave)).c_str());
    _previewRes->selectItem(String8(EResolutionToString(pPtyTerrain->_resolutionPreview)).c_str());

    if(pPtyTerrain->_modePreview)
    {
        _previewRadio->setChecked(true);
        if(pPtyTerrain->_previewDiffuseTextures.size() != 0 && pPtyTerrain->_previewNormalTextures.size())
            pNodeTerrain->setBlockTextures(pPtyTerrain->_previewDiffuseTextures, pPtyTerrain->_previewNormalTextures);
    }
    else
    {
        _saveRadio->setChecked(true);
        if(pPtyTerrain->_saveDiffuseTextures.size() != 0 && pPtyTerrain->_saveNormalTextures.size())
            pNodeTerrain->setBlockTextures(pPtyTerrain->_saveDiffuseTextures, pPtyTerrain->_saveNormalTextures);
    }
    
    _terrainLayersTree->clear();
    for(int32 ii = 0; ii < pPtyTerrain->getLayerCount(); ++ii)
    {
        Ptr<PtyTerrainLayer> pTerrainLayer = pPtyTerrain->getLayer(ii);
        if(pTerrainLayer != null)
        {
            _terrainLayersTree->addLayer(pTerrainLayer->getName(), pTerrainLayer->getId());
        }
    }

    Ptr<PtyTerrainLayer> pCurrentLayer = pPtyTerrain->getCurrentLayer(); 

    if (pCurrentLayer!=null)
    {
        _terrainLayersTree->setCurrentLayer(pCurrentLayer->getName());

        LM_DEBUG_PTR_CAST<PtyWidgetTerrainLayer>(_ptyWidgetLayer)->setProperty(pCurrentLayer);
    }

    _groupBox->setEnabled(pPtyTerrain->_terrainName != Core::String::EMPTY);
    _ptyWidgetLayer->setEnabled(pPtyTerrain->_terrainName != Core::String::EMPTY);

    makeConnectionLayerTree(true);
}

void PtyWidgetTerrain::writeProperty(QWidget* pWidget)
{
    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());
    Ptr<Universe::NodeTerrain> pNodeTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(pPtyTerrain->getUniverseNode());

    if(pPtyTerrain->_modePreview)
        pNodeTerrain->getBlockTextures(pPtyTerrain->_previewDiffuseTextures, pPtyTerrain->_previewNormalTextures);
    else
        pNodeTerrain->getBlockTextures(pPtyTerrain->_saveDiffuseTextures, pPtyTerrain->_saveNormalTextures);

    if (pWidget == NULL || pWidget == _terrainLayersTree)
    {/*
        //On commence remettre les PtyLayers dans l'ordre.
        Core::List<int32> layersId;
        _terrainLayersTree->getLayersId(layersId);
        
        pPtyTerrain->_layers.clear();

        for(int32 iID=0; iID < layersId.size(); ++iID)
        {
            for(int32 iChild=0; iChild < pPtyTerrain->getNbChildren(); ++iChild)
            {
                if(pPtyTerrain->getChild(iChild)->getId() == layersId[iID])
                {
                    Ptr<PtyTerrainLayer> pPtyTerrainLayer = LM_DEBUG_PTR_CAST<PtyTerrainLayer>(pPtyTerrain->getChild(iChild));
                    pPtyTerrain->_layers.push_back(pPtyTerrainLayer);
                }
            }
        }
      
        Core::String layerName;
        int32 index = _terrainLayersTree->getCurrentLayer(layerName);
        
        pPtyTerrain->setCurrentLayer(index);

        Ptr<PtyTerrainLayer> pTerrainLayer = pPtyTerrain->getCurrentLayer();
        if(pTerrainLayer != null)
        {
            pTerrainLayer->setName(layerName);
            LM_DEBUG_PTR_CAST<PtyWidgetTerrainLayer>(_ptyWidgetLayer)->setProperty(pTerrainLayer);
        }*/

        Core::String layerName;
        int32 index = _terrainLayersTree->getCurrentLayer(layerName);
        
        pPtyTerrain->setCurrentLayer(index);
        Ptr<PtyTerrainLayer> pTerrainLayer = pPtyTerrain->getCurrentLayer();
        if(pTerrainLayer != null)
            LM_DEBUG_PTR_CAST<PtyWidgetTerrainLayer>(_ptyWidgetLayer)->setProperty(pTerrainLayer);
    }

    if (pWidget == NULL || pWidget == _globalNormalTexture)
    {
        _globalNormalTexture->getText(pPtyTerrain->_globalNormalTexture);
    }

    if (pWidget == NULL || pWidget == _globalNormalWeight)
    {
        double weight = 0.0;
        _globalNormalWeight->getSingleValue(weight);
        pPtyTerrain->_globalNormalWeight = weight;
    }

    if (pWidget == NULL || pWidget == _saveRes)
    {
        pPtyTerrain->_resolutionSave = StringToEResolution(_saveRes->selectedText());
    }

    if (pWidget == NULL || pWidget == _previewRes)
    {
        pPtyTerrain->_resolutionPreview = StringToEResolution(_previewRes->selectedText());
    }

    if( (pWidget == NULL || pWidget == _previewRadio) && pPtyTerrain->_modePreview == false)
    {
        pNodeTerrain->getBlockTextures(pPtyTerrain->_saveDiffuseTextures, pPtyTerrain->_saveNormalTextures);
        pPtyTerrain->_modePreview = true;
        if(pPtyTerrain->_previewDiffuseTextures.size() != 0 && pPtyTerrain->_previewNormalTextures.size())
            pNodeTerrain->setBlockTextures(pPtyTerrain->_previewDiffuseTextures, pPtyTerrain->_previewNormalTextures);
    }

    if( (pWidget == NULL || pWidget == _saveRadio) && pPtyTerrain->_modePreview == true)
    {
        pNodeTerrain->getBlockTextures(pPtyTerrain->_previewDiffuseTextures, pPtyTerrain->_previewNormalTextures);
        pPtyTerrain->_modePreview = false;
        if(pPtyTerrain->_saveDiffuseTextures.size() != 0 && pPtyTerrain->_saveNormalTextures.size())
            pNodeTerrain->setBlockTextures(pPtyTerrain->_saveDiffuseTextures, pPtyTerrain->_saveNormalTextures);
    }

    if(pWidget == NULL || pWidget == _terrainName)
    {
        pPtyTerrain->setTerrain(Core::String(_terrainName->text().toStdString().c_str()));
        _groupBox->setEnabled(pPtyTerrain->_terrainName != Core::String::EMPTY);
        _ptyWidgetLayer->setEnabled(pPtyTerrain->_terrainName != Core::String::EMPTY);
    }
}

void PtyWidgetTerrain::layersOrderChanged()
{
    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());
    
    beginChangeProperty (getDataProxy()->getStateRecorder(), pPtyTerrain);
        Core::List<int32> layersId;
        _terrainLayersTree->getLayersId(layersId);

        Core::List<Ptr<PtyTerrainLayer> > newLayerOrder;
        for(int32 iID=0; iID < layersId.size(); ++iID)
        {
            for(int32 ilayer=0; ilayer < pPtyTerrain->_layers.size(); ++ilayer)
            {
                if(pPtyTerrain->_layers[ilayer]->getId() == layersId[iID])
                {
                    newLayerOrder.push_back(pPtyTerrain->_layers[ilayer]);
                }
            }
        }
        pPtyTerrain->_layers = newLayerOrder;

        Core::String layerName;
        int32 index = _terrainLayersTree->getCurrentLayer(layerName);
        
        pPtyTerrain->setCurrentLayer(index);
        Ptr<PtyTerrainLayer> pTerrainLayer = pPtyTerrain->getCurrentLayer();
        if(pTerrainLayer != null)
            LM_DEBUG_PTR_CAST<PtyWidgetTerrainLayer>(_ptyWidgetLayer)->setProperty(pTerrainLayer);
    finishChangeProperty(getDataProxy()->getStateRecorder(), pPtyTerrain);

    emit propertyModified();
}

void PtyWidgetTerrain::createLayer(int32 index, Core::String name)
{
    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());

    beginChangeProperty (getDataProxy()->getStateRecorder(), pPtyTerrain);
        Ptr<PtyTerrainLayer> pTerrainLayer =  pPtyTerrain->addTerrainLayer(index, name);
        _terrainLayersTree->setLayerId(index, pTerrainLayer->getId());
    finishChangeProperty(getDataProxy()->getStateRecorder(), pPtyTerrain);
    
    emit propertyModified();

    LM_DEBUG_PTR_CAST<PtyWidgetTerrainLayer>(_ptyWidgetLayer)->setProperty(pTerrainLayer);
}

void PtyWidgetTerrain::deleteLayer(int32 index)
{
    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());

    beginChangeProperty (getDataProxy()->getStateRecorder(), pPtyTerrain);
        pPtyTerrain->deleteLayer(index);
    finishChangeProperty(getDataProxy()->getStateRecorder(), pPtyTerrain);

    emit propertyModified();
}

void PtyWidgetTerrain::renameLayer(int32 index, Core::String name)
{
    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());

    beginChangeProperty (getDataProxy()->getStateRecorder(), pPtyTerrain);
        pPtyTerrain->renameLayer(index, name);
    finishChangeProperty(getDataProxy()->getStateRecorder(), pPtyTerrain);

    emit propertyModified();
}

void PtyWidgetTerrain::currentLayerChanged(int32 index)
{
    commitPropertyChange(_terrainLayersTree);
}

void PtyWidgetTerrain::generate()
{
    Ptr<PtyTerrain> pPtyTerrain = LM_DEBUG_PTR_CAST<PtyTerrain>(getDataProxy()->getProperty());
    bool preview = _previewRadio->isChecked();
    int32 resolution = preview ? StringToEResolutionValue(_previewRes->selectedText()) : StringToEResolutionValue(_saveRes->selectedText());
    pPtyTerrain->save(preview, resolution);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI
