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
#ifndef EPI_PROPERTY_TERRAIN_H_
#define EPI_PROPERTY_TERRAIN_H_

#include <EPI/Document/Properties/PtyNode.moc.h>

#include <EPI/Document/Properties/PtyTerrainLayer.moc.h>

#include <EPI/DocumentRenderer.h>
#include <EPI/ImportInfo/ImportTerrainLayersInfo.h>

#include <Universe/NodeTerrain.h>

namespace QtToolbox
{
class LineEdit;
class ComboBox;
class SingleSlidingDouble;
class CollapsibleWidget;
}//namespace QtToolbox

class QVBoxLayout;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QRadioButton;

namespace EPI
{
class CustomLine;

class TerrainLayersTree;
class PtyWidgetTerrain;
class PtyWidgetTerrainLayer;

LM_ENUM_7(EResolution, RESOLUTION_64, RESOLUTION_128, RESOLUTION_256, RESOLUTION_512, RESOLUTION_1024, RESOLUTION_2048, RESOLUTION_4096)

//-----------------------------------------------------------------------------
class LM_API_EPI PtyTerrain : public PtyNode
{
    Q_OBJECT

    friend class PtyWidgetTerrain;

public:
    PtyTerrain( const Ptr<Universe::World>&         pWorld, 
                const Ptr<Universe::NodeTerrain> &  pNodeTerrain,
                const Ptr<DocumentRenderer> &       pDocumentRenderer,
                const Core::String& title = L"Terrain");
    PtyTerrain(const PtyTerrain& other);
    virtual ~PtyTerrain();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual void updateData();
    void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void copyId(const Property& pSrc);

    void save(bool preview, int32 resolution);

    virtual String getIconFileName() {return L":/icons/ptyTerrain.png";}
    void renameLayer(int32 index, Core::String name);

    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);
    virtual EPropertyClassName getCalssName() const {return PTYNODE_TERRAIN;}
private:
    Ptr<PtyTerrainLayer> addTerrainLayer(int32 index, const Core::String & name);
    void deleteLayer(int32 index);
    Ptr<PtyTerrainLayer> getCurrentLayer() const;
    void setCurrentLayer(int32 currentLayer);
    void setTerrain(const Core::String & terrainName);

    inline int32 getLayerCount() const { return _layers.size(); }
    Ptr<PtyTerrainLayer> getLayer(int32 index) const;


private Q_SLOTS:
    void layerUpdated();

private:
    Ptr<Universe::RessourcePool>        _pRessourcePool;
    Ptr<DocumentRenderer>               _pDocumentRenderer;
    Core::String                        _terrainName;
    Core::String                        _globalNormalTexture;
    float                               _globalNormalWeight;
    int32                               _currentLayer;
    Ptr<ImportTerrainLayersInfo>        _pImportTerrainLayersInfo;
    EResolution                         _resolutionPreview;
    EResolution                         _resolutionSave;
    bool                                _modePreview;
    Core::List<Ptr<PtyTerrainLayer> >   _layers;
    Core::List<Core::List<String> >     _previewDiffuseTextures;
    Core::List<Core::List<String> >     _previewNormalTextures;
    Core::List<Core::List<String> >     _saveDiffuseTextures;
    Core::List<Core::List<String> >     _saveNormalTextures;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetTerrain : public PropertyWidget
{
    Q_OBJECT
    friend class PtyTerrain;
public:
    PtyWidgetTerrain(   const Ptr<PropertyWidgetDataProxy>& data,
                        QWidget * parent = 0);
    virtual ~PtyWidgetTerrain();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void layersOrderChanged();
    void createLayer(int32 index, Core::String name);
    void deleteLayer(int32 index);
    void renameLayer(int32 index, Core::String name);
    void currentLayerChanged(int32 index);    

    void generate();

private:
    void makeConnectionLayerTree(bool flag);

private:
    QVBoxLayout *                       _layout;
    QtToolbox::LineEdit *               _terrainName;
    QtToolbox::CollapsibleWidget *      _groupBox;
    QtToolbox::CollapsibleWidget *      _groupBoxGlobalNormal;
    QtToolbox::CollapsibleWidget *      _groupBoxTexture;
    QGridLayout *                       _groupLayout;
    CustomLine *                        _globalNormalTexture;
    QtToolbox::SingleSlidingDouble*     _globalNormalWeight;
    TerrainLayersTree *                 _terrainLayersTree;
    QtToolbox::ComboBox *               _previewRes;
    QtToolbox::ComboBox *               _saveRes;
    QRadioButton *                      _previewRadio;
    QRadioButton *                      _saveRadio;
    QPushButton *                       _generate;

    Ptr<PropertyWidget>                 _ptyWidgetLayer;
    Ptr<PropertyWidget>                 _pPWNode;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //EPI_PROPERTY_TERRAIN_H_