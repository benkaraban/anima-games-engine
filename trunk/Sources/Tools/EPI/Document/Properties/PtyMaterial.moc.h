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
#ifndef EPI_PROPERTY_MATERIAL_H_
#define EPI_PROPERTY_MATERIAL_H_

#include <QWidget>
#include <QGridLayout>
#include <QCheckBox>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/NodeMesh.h>
#include <Universe/NodeSkeleton.h>

namespace QtToolbox
{
    class CollapsibleWidget;
    class QuickColorPicker;
    class SingleSlidingDouble;
    class ComboBox;
    class SingleSlidingHDR;
}//namespace QtToolbox

namespace EPI
{
class CustomLine;
class PtyWidgetMaterial;

//-----------------------------------------------------------------------------
LM_ENUM_2   (EPtyMaterialDataType,
             MATERIAL_DATA_NODE_MESH,
             MATERIAL_DATA_SKELETON_SKIN)
class LM_API_EPI PtyMaterial : public PropertyNode
{
    Q_OBJECT

    friend class PtyWidgetMaterial;

public:
    //constraucteur pour modifier le material d'un nodeMesh
    PtyMaterial(const Ptr<Universe::RessourcePool>& pRessourcePool, const Core::List<Ptr<Universe::NodeMesh> >& pNodesM, const Core::String& title = L"Material", bool isSelectable = true);

    //constructeur pour modifier le material d'un SkeletonSkin
    PtyMaterial(const Ptr<Universe::RessourcePool>& pRessourcePool, const Core::List<Ptr<Universe::SkeletonSkin> >& pSkin, const Core::String& title = L"Material", bool isSelectable = true);

    virtual ~PtyMaterial();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    void setNodeMesh(const Ptr<Universe::NodeMesh>& pNodeM, bool keepPtyMaterial = false);

    void copy(Assets::Material& mat);

    virtual bool isIntersecting(const Core::Rayf& ray, float& distance);
    void setBoundingBoxVisible(bool visible);
    Core::AABoxf    getBoundingBox() const;
    virtual EPropertyClassName getCalssName() const {return PTY_MATERIAL;}

    virtual String getIconFileName() {return L":/icons/ptyMaterial.png";}

private:
    void setDataMaterial        (const Renderer::Material & material);
    const Renderer::Material& getDataMaterial();

    void setDataDiffuseTexture  (const String & texName);
    void setDataSpecularTexture (const String & texName);
    void setDataNormalTexture   (const String & texName);
    void setDataLightTexture    (const String & texName);
    void setDataCustomTexture0  (const String & texName);
    void setDataCustomTexture1  (const String & texName);
    void setDataCustomTexture2  (const String & texName);
    

public:
    void getDataDiffuseTexture      (String & texName);
    void getDataSpecularTexture     (String & texName);
    void getDataNormalTexture       (String & texName);
    void getDataLightTexture        (String & texName);
    void getDataCustomTexture0      (String & texName);
    void getDataCustomTexture1      (String & texName);
    void getDataCustomTexture2      (String & texName);

private:
    Ptr<Universe::RessourcePool>                _pRessourcePool;
    Core::List<Ptr<Universe::NodeMesh> >        _pNodesM;
    Core::List<Ptr<Universe::SkeletonSkin> >    _pSkins;
    Core::List<Ptr<Universe::Node> >            _pNodes;
    EPtyMaterialDataType                        _dataType;

    Assets::EMaterialType                       _matType;

    bool                                        _translucidTexture;
    bool                                        _clampTextureBorder;

    String                                      _diffuseTex;
    String                                      _specularTex;
    String                                      _normalTex;

    Core::Vector4f                              _emissive;
    Core::Vector4f                              _diffuse;
    Core::Vector4f                              _specular;
    double                                      _shininess;
    Core::Vector4f                              _glow;
    Core::Vector4f                              _reflexions;
    double                                      _glowExtent;
    Core::Vector4f                              _refracColor;
    double                                      _refracScale;
    double                                      _refracIndex;

    bool                                        _dynamicShadowCaster;
    bool                                        _dynamicShadowReceiver;

    //Custom
    String                                      _texture0Name;
    String                                      _texture1Name;
    String                                      _texture2Name;
    Assets::ECustomMode                         _customMode;
    Core::Vector4f                              _customColor;
    float                                       _colorFactor;
    float                                       _glowFactor;
    Assets::EOperationType                      _opColor01;
    Assets::EOperationType                      _opColor12;
    Core::Vector2f                              _duv0;
    Core::Vector2f                              _duv1;
    Core::Vector2f                              _duv2;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetMaterial : public PropertyWidget
{
    Q_OBJECT

    friend class PtyMaterial;
    friend class PtyWidgetModel;

public:
    PtyWidgetMaterial(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetMaterial();
    void readProperty();
    void writeProperty(QWidget* pWidget);
    void setupUi();

    void showMaterialParameter();

private:
    void showMaterialStandard(bool flag);
    void showMaterialCustom(bool flag);

private:
    QGridLayout * _layout;
    QtToolbox::CollapsibleWidget *          _groupBox;
    
    QtToolbox::ComboBox *                   _matType;
    QCheckBox *                             _translucidTexture;
    QCheckBox *                             _clampTextureBorder;
    
    QCheckBox *                             _dynamicShadowCaster;
    QCheckBox *                             _dynamicShadowReceiver;

    CustomLine *                            _diffuseTex;
    CustomLine *                            _specularTex;
    CustomLine *                            _normalTex;

    QtToolbox::QuickColorPicker *           _emissive;
    QtToolbox::QuickColorPicker *           _diffuse;
    QtToolbox::QuickColorPicker *           _specular;
    QtToolbox::SingleSlidingDouble*         _shininess;
    QtToolbox::QuickColorPicker *           _glow;
    QtToolbox::QuickColorPicker *           _reflexions;
    QtToolbox::SingleSlidingDouble*         _glowExtent;
    QtToolbox::QuickColorPicker *           _refracColor;
    QtToolbox::SingleSlidingDouble*         _refracScale;
    QtToolbox::SingleSlidingDouble*         _refracIndex;

    CustomLine *                            _customTex0;
    CustomLine *                            _customTex1;
    CustomLine *                            _customTex2;
    QtToolbox::ComboBox *                   _customMode;
    QtToolbox::QuickColorPicker *           _customColor;
    QtToolbox::SingleSlidingDouble*         _colorFactor;
    QtToolbox::SingleSlidingDouble*         _glowFactor;
    QtToolbox::ComboBox *                   _opColor01;
    QtToolbox::ComboBox *                   _opColor12;
    QtToolbox::SingleSlidingHDR*            _duv0_X;
    QtToolbox::SingleSlidingHDR*            _duv1_X;
    QtToolbox::SingleSlidingHDR*            _duv2_X;
    QtToolbox::SingleSlidingHDR*            _duv0_Y;
    QtToolbox::SingleSlidingHDR*            _duv1_Y;
    QtToolbox::SingleSlidingHDR*            _duv2_Y;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif /*EPI_PROPERTY_MATERIAL_H_*/
