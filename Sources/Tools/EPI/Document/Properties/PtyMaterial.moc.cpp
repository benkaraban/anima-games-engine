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
#include "PtyMaterial.moc.h"

#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyMaterial::PtyMaterial(const Ptr<Universe::RessourcePool>& pRessourcePool, const Core::List<Ptr<Universe::NodeMesh> >& pNodesM, const Core::String& title, bool isSelectable):
    _pRessourcePool(pRessourcePool),
    _pNodesM(pNodesM),
    PropertyNode(title, isSelectable, true, CONTENT),
    _dataType(MATERIAL_DATA_NODE_MESH)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyMaterial::PtyMaterial(const Ptr<Universe::RessourcePool>& pRessourcePool, const Core::List<Ptr<Universe::SkeletonSkin> >& pSkins, const Core::String& title, bool isSelectable):
    _pRessourcePool(pRessourcePool),
    _pSkins(pSkins),
    PropertyNode(title, isSelectable, true, CONTENT),
    _dataType(MATERIAL_DATA_SKELETON_SKIN)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
void PtyMaterial::copy(Assets::Material& mat)
{
    mat.type = _matType;

    if(_translucidTexture == true)
        mat.flags |= Assets::MAT_FLAG_TRANSLUCID_TEXTURE;
    else if( (mat.flags & Assets::MAT_FLAG_TRANSLUCID_TEXTURE) != 0)
        mat.flags ^= Assets::MAT_FLAG_TRANSLUCID_TEXTURE;

    if(_clampTextureBorder == true)
        mat.flags |= Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER;
    else if( (mat.flags & Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER) != 0)
        mat.flags ^= Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER;

    if (_dynamicShadowCaster == true)
        mat.flags |= Assets::MAT_FLAG_DYNAMIC_SHADOW_CASTER;
    else if( (mat.flags & Assets::MAT_FLAG_DYNAMIC_SHADOW_CASTER) != 0)
        mat.flags ^= Assets::MAT_FLAG_DYNAMIC_SHADOW_CASTER;

    if (_dynamicShadowReceiver == true)
        mat.flags |= Assets::MAT_FLAG_DYNAMIC_SHADOW_RECEIVER;
    else if( (mat.flags & Assets::MAT_FLAG_DYNAMIC_SHADOW_RECEIVER) != 0)
        mat.flags ^= Assets::MAT_FLAG_DYNAMIC_SHADOW_RECEIVER;

    mat.diffuseTexName  = _diffuseTex;
    mat.specularTexName = _specularTex;
    mat.normalTexName   = _normalTex;

    mat.emissive        = _emissive;
    mat.diffuse         = _diffuse;
    mat.specular        = _specular;
    mat.shininess       = _shininess;
    mat.glowColor       = _glow;
    mat.reflexions      = _reflexions;
    mat.refracColor     = _refracColor;
    mat.refracScale     = _refracScale;
    mat.refracIndex     = _refracIndex;
    mat.glowExtent      = Core::Vector3f(_glowExtent);

    //custom
    mat.texture0Name    = _texture0Name;
    mat.texture1Name    = _texture1Name;
    mat.texture2Name    = _texture2Name;
    mat.customMode      = _customMode;
    mat.customColor     = _customColor;
    mat.colorFactor     = _colorFactor;
    mat.glowFactor      = _glowFactor;
    mat.opColor01       = _opColor01;
    mat.opColor12       = _opColor12;
    mat.duv0            = _duv0;
    mat.duv1            = _duv1;
    mat.duv2            = _duv2;
}
//-----------------------------------------------------------------------------
PtyMaterial::~PtyMaterial()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyMaterial::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetMaterial> pPW (new PtyWidgetMaterial(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyMaterial::updateData()
{
    Renderer::Material mat        = getDataMaterial();
        mat.matType               = _matType;
        mat.emissive              = _emissive;
        mat.diffuse               = _diffuse;
        mat.specular              = _specular;
        mat.glowColor             = _glow;
        mat.reflexions            = _reflexions;
        mat.refracColor           = _refracColor;
        mat.refracScale           = _refracScale;
        mat.refracIndex           = _refracIndex;
        mat.shininess             = _shininess;
        mat.glowExtent            = Core::Vector3f(_glowExtent);
        mat.transDiffuseTex       = _translucidTexture;
        mat.clampTextureBorder    = _clampTextureBorder;
        mat.dynamicShadowCaster   = _dynamicShadowCaster;
        mat.dynamicShadowReceiver = _dynamicShadowReceiver;
            //custom
        mat.customMode            = _customMode;
        mat.customColor           = _customColor;
        mat.colorFactor           = _colorFactor;
        mat.glowFactor            = _glowFactor;
        mat.opColor01             = _opColor01;
        mat.opColor12             = _opColor12;
        mat.duv0                  = _duv0;
        mat.duv1                  = _duv1;
        mat.duv2                  = _duv2;
    
    setDataMaterial(mat);

    String texture;
    getDataDiffuseTexture(texture);
    if (texture != _diffuseTex)
    {
        _pRessourcePool->reloadTexture(_diffuseTex);
        setDataDiffuseTexture(_diffuseTex);
    }

    getDataSpecularTexture(texture);
    if (texture != _specularTex)
    {
        _pRessourcePool->reloadTexture(_specularTex);
        setDataSpecularTexture(_specularTex);
    }

    getDataNormalTexture(texture);
    if (texture != _normalTex)
    {
        _pRessourcePool->reloadTexture(_normalTex);
        setDataNormalTexture(_normalTex);
    }  

    getDataCustomTexture0(texture);
    if (texture != _texture0Name)
    {
        _pRessourcePool->reloadTexture(_texture0Name);
        setDataCustomTexture0(_texture0Name);
    }  

    getDataCustomTexture1(texture);
    if (texture != _texture1Name)
    {
        _pRessourcePool->reloadTexture(_texture1Name);
        setDataCustomTexture1(_texture1Name);
    }  

    getDataCustomTexture2(texture);
    if (texture != _texture2Name)
    {
        _pRessourcePool->reloadTexture(_texture2Name);
        setDataCustomTexture2(_texture2Name);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::updateProperty()
{
    if (_pNodesM.size()>0 || _pSkins.size()>0)
    {
        Renderer::Material mat      = getDataMaterial();
            _matType                = mat.matType;
            _diffuse                = mat.diffuse;
            _translucidTexture      = true;
            _clampTextureBorder     = true;
            _emissive               = mat.emissive;
            _diffuse                = mat.diffuse;
            _specular               = mat.specular;
            _glow                   = mat.glowColor;
            _reflexions             = mat.reflexions;
            _refracColor            = mat.refracColor;
            _refracScale            = mat.refracScale;
            _refracIndex            = mat.refracIndex;
            _shininess              = mat.shininess;
            _glowExtent             = mat.glowExtent.x;
            _dynamicShadowCaster    = mat.dynamicShadowCaster;
            _dynamicShadowReceiver  = mat.dynamicShadowReceiver;
            _translucidTexture      = mat.transDiffuseTex;
            _clampTextureBorder     = mat.clampTextureBorder;
            //custom
            _customMode             = mat.customMode;
            _customColor            = mat.customColor;
            _colorFactor            = mat.colorFactor;
            _glowFactor             = mat.glowFactor;
            _opColor01              = mat.opColor01;
            _opColor12              = mat.opColor12;
            _duv0                   = mat.duv0;
            _duv1                   = mat.duv1;
            _duv2                   = mat.duv2;

        getDataDiffuseTexture(_diffuseTex);
        getDataSpecularTexture(_specularTex);
        getDataNormalTexture(_normalTex);
        getDataCustomTexture0(_texture0Name);
        getDataCustomTexture1(_texture1Name);
        getDataCustomTexture2(_texture2Name);
    }
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyMaterial::clone() const
{
    return Ptr<Property>(new PtyMaterial( *this ));
}
//-----------------------------------------------------------------------------
void PtyMaterial::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyMaterial> pPty = LM_DEBUG_PTR_CAST<PtyMaterial>(pSrc);

    _matType                = pPty->_matType;

    _translucidTexture      = pPty->_translucidTexture;
    _clampTextureBorder     = pPty->_clampTextureBorder;
    _diffuseTex             = pPty->_diffuseTex;
    _specularTex            = pPty->_specularTex;
    _normalTex              = pPty->_normalTex;
    _emissive               = pPty->_emissive;
    _diffuse                = pPty->_diffuse;
    _specular               = pPty->_specular;
    _shininess              = pPty->_shininess;
    _glow                   = pPty->_glow;
    _dynamicShadowCaster    = pPty->_dynamicShadowCaster;
    _dynamicShadowReceiver  = pPty->_dynamicShadowReceiver;
    _reflexions             = pPty->_reflexions;
    _refracColor            = pPty->_refracColor;
    _refracScale            = pPty->_refracScale;
    _refracIndex            = pPty->_refracIndex;
    _glowExtent             = pPty->_glowExtent;
    //custom
    _customMode             = pPty->_customMode;
    _customColor            = pPty->_customColor;
    _colorFactor            = pPty->_colorFactor;
    _glowFactor             = pPty->_glowFactor;
    _opColor01              = pPty->_opColor01;
    _opColor12              = pPty->_opColor12;
    _duv0                   = pPty->_duv0;
    _duv1                   = pPty->_duv1;
    _duv2                   = pPty->_duv2;
    _texture0Name           = pPty->_texture0Name;
    _texture1Name           = pPty->_texture1Name;
    _texture2Name           = pPty->_texture2Name;

    updateData();
}
//-----------------------------------------------------------------------------
void PtyMaterial::setNodeMesh(const Ptr<Universe::NodeMesh>& pNodeM, bool keepPtyMaterial)
{//TODO gerer nodemesh et skeletonskin

    LM_ASSERT(_dataType == MATERIAL_DATA_NODE_MESH);

    _pSkins.clear();
    _pNodesM.clear();

    _pNodesM.push_back(pNodeM);
    if (keepPtyMaterial==true)
    {
        updateData();
    }
    else
    {
        updateProperty();
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataMaterial (const Renderer::Material & material)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->getMeshInstance()->setMaterial(material);
        }
    }
    else if (_dataType==MATERIAL_DATA_SKELETON_SKIN)
    {
        for (int32 ii=0; ii<_pSkins.size(); ++ii)
        {
            _pSkins[ii]->getSkin()->setMaterial(material);
        }
    }
}
//-----------------------------------------------------------------------------
const Renderer::Material& PtyMaterial::getDataMaterial()
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        return _pNodesM[0]->getMeshInstance()->getMaterial();
    }
    else 
    {
        return _pSkins[0]->getSkin()->getMaterial();
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataDiffuseTexture (const String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->setDiffuseTexture(texName);
        }
    }
    else if (_dataType==MATERIAL_DATA_SKELETON_SKIN)
    {
        for (int32 ii=0; ii<_pSkins.size(); ++ii)
        {
            _pSkins[ii]->setDiffuseTexture(texName);
        }
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::getDataDiffuseTexture (String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->getDiffuseTexture(texName);
    }
    else 
    {
        _pSkins[0]->getDiffuseTexture(texName);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataSpecularTexture (const String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->setSpecularTexture(texName);
        }
    }
    else if (_dataType==MATERIAL_DATA_SKELETON_SKIN)
    {
        for (int32 ii=0; ii<_pSkins.size(); ++ii)
        {
            _pSkins[ii]->setSpecularTexture(texName);
        }
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::getDataSpecularTexture (String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->getSpecularTexture(texName);
    }
    else 
    {
        _pSkins[0]->getSpecularTexture(texName);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataNormalTexture (const String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->setNormalTexture(texName);
        }
    }
    else if (_dataType==MATERIAL_DATA_SKELETON_SKIN)
    {
        for (int32 ii=0; ii<_pSkins.size(); ++ii)
        {
            _pSkins[ii]->setNormalTexture(texName);
        }
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::getDataNormalTexture (String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->getNormalTexture(texName);
    }
    else 
    {
        _pSkins[0]->getNormalTexture(texName);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataLightTexture (const String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->setLightTexture(texName);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::getDataLightTexture (String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->getLightTexture(texName);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataCustomTexture0 (const String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->setCustomTexture0(texName);
        }
    }
    // TODO skins
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataCustomTexture1 (const String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->setCustomTexture1(texName);
        }
    }
    // TODO skins
}
//-----------------------------------------------------------------------------
void PtyMaterial::setDataCustomTexture2 (const String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->setCustomTexture2(texName);
        }
    }
    // TODO skins
}
//-----------------------------------------------------------------------------
void PtyMaterial::getDataCustomTexture0 (String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->getCustomTexture0(texName);
    }
    else 
    {
        //_pSkins[0]->getCustomTexture0(texName);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::getDataCustomTexture1 (String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->getCustomTexture1(texName);
    }
    else 
    {
        //_pSkins[0]->getCustomTexture0(texName);
    }
}
//-----------------------------------------------------------------------------
void PtyMaterial::getDataCustomTexture2 (String & texName)
{
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        _pNodesM[0]->getCustomTexture2(texName);
    }
    else 
    {
        //_pSkins[0]->getCustomTexture0(texName);
    }
}
//-----------------------------------------------------------------------------
bool PtyMaterial::isIntersecting(const Core::Rayf& ray, float& distance)
{
    bool ret = false;

    distance = FLT_MAX;
    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            float distanceT = FLT_MAX;
            bool retT = false;
            retT = _pNodesM[ii]->isIntersecting(ray, distanceT);

            if (retT==true)
            {
                ret = true;
                if (distanceT<distance)
                {
                    distance = distanceT;
                }
            }
        }
    }
    else if (_dataType==MATERIAL_DATA_SKELETON_SKIN)
    {
        for (int32 ii=0; ii<_pSkins.size(); ++ii)
        {
            float distanceT = FLT_MAX;
            bool retT = false;
            retT = _pSkins[ii]->isIntersecting(ray, distanceT);

            if (retT==true)
            {
                ret = true;
                if (distanceT<distance)
                {
                    distance = distanceT;
                }
            }
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
void PtyMaterial::setBoundingBoxVisible(bool visible)
{

    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            _pNodesM[ii]->setBoundingBoxVisible(visible);
        }
    }
    else if (_dataType==MATERIAL_DATA_SKELETON_SKIN)
    {
        for (int32 ii=0; ii<_pSkins.size(); ++ii)
        {
            _pSkins[ii]->setBoundingBoxVisible(visible);
        }
    }
}
//-----------------------------------------------------------------------------
Core::AABoxf PtyMaterial::getBoundingBox() const
{
    Core::AABoxf box;

    if (_dataType==MATERIAL_DATA_NODE_MESH)
    {
        for (int32 ii=0; ii<_pNodesM.size(); ++ii)
        {
            Core::AABoxf boxNode = _pNodesM[ii]->getBoundingBox();
            box.setUnion(boxNode);
        }
    }
    else if (_dataType==MATERIAL_DATA_SKELETON_SKIN)
    {
        for (int32 ii=0; ii<_pSkins.size(); ++ii)
        {
            Core::AABoxf boxNode = _pSkins[ii]->getBoundingBox();
            box.setUnion(boxNode);
        }
    }

    return box;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetMaterial::PtyWidgetMaterial(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetMaterial::~PtyWidgetMaterial()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetMaterial::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Material");

    _matType = new QtToolbox::ComboBox(this, "type");
        ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE (_matType, Assets, Assets::EMaterialType, 4)

    _translucidTexture = new QCheckBox("Translucid texture", this);
    _translucidTexture->setFixedHeight(16);
    _clampTextureBorder = new QCheckBox("Clamp texture border", this);
    _clampTextureBorder->setFixedHeight(16);

    _dynamicShadowCaster    = new QCheckBox("Shadow caster", this);
    _dynamicShadowCaster->setFixedHeight(16);
    _dynamicShadowReceiver  = new QCheckBox("Shadow Receiver", this);
    _dynamicShadowReceiver->setFixedHeight(16);

    _diffuseTex = new CustomLine(this, "Diffuse");
    _diffuseTex->pushAuthorizedDropMimeData("asset/texture");
    _diffuseTex->setReadOnly(true);
    _specularTex= new CustomLine(this, "Specular");
    _specularTex->pushAuthorizedDropMimeData("asset/texture");
    _specularTex->setReadOnly(true);
    _normalTex  = new CustomLine(this, "Normal");
    _normalTex->pushAuthorizedDropMimeData("asset/texture");
    _normalTex->setReadOnly(true);

    _diffuse    = new QtToolbox::QuickColorPicker(this, "Diffuse", Qt::white);
    _specular   = new QtToolbox::QuickColorPicker(this, "Specular", Qt::white, false);
    _shininess  = new QtToolbox::SingleSlidingDouble(this, "Shininess", 0.0, 100.0);
    _glowExtent = new QtToolbox::SingleSlidingDouble(this, "Glow Extent", 0.0, 2.0);
    _shininess->setSingleValue(22.0);

    _refracScale  = new QtToolbox::SingleSlidingDouble(this, "Refraction scale", 0.0, 1.0);
    _refracIndex  = new QtToolbox::SingleSlidingDouble(this, "Refraction index", 0.0, 1.0);

    _glow        = new QtToolbox::QuickColorPicker(this, "Glow", Qt::white);
    _reflexions  = new QtToolbox::QuickColorPicker(this, "Reflexions", Qt::white);
    _refracColor = new QtToolbox::QuickColorPicker(this, "Refraction color", Qt::white);
    _emissive    = new QtToolbox::QuickColorPicker(this, "Emissive", Qt::white, false);

    _customMode     = new QtToolbox::ComboBox (this, "custom Mode");
        ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE (_customMode, Assets, Assets::ECustomMode, 3)
    _customColor    = new QtToolbox::QuickColorPicker(this, "custom", Qt::white);
    _colorFactor    = new QtToolbox::SingleSlidingDouble(this, "color factor", 0.0, 1.0);
    _glowFactor     = new QtToolbox::SingleSlidingDouble(this, "glow factor", 0.0, 1.0);
    _opColor01      = new QtToolbox::ComboBox(this, "color 01");
        ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE (_opColor01, Assets, Assets::EOperationType, 3)
    _opColor12      = new QtToolbox::ComboBox(this, "color 12");
        ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE (_opColor12, Assets, Assets::EOperationType, 3)
    _duv0_X         = new QtToolbox::SingleSlidingHDR(this, "duv0 x");
    _duv0_Y         = new QtToolbox::SingleSlidingHDR(this, "duv0 y");
    _duv1_X         = new QtToolbox::SingleSlidingHDR(this, "duv1 x");
    _duv1_Y         = new QtToolbox::SingleSlidingHDR(this, "duv1 y");
    _duv2_X         = new QtToolbox::SingleSlidingHDR(this, "duv2 x");
    _duv2_Y         = new QtToolbox::SingleSlidingHDR(this, "duv2 y");

    _customTex0 = new CustomLine(this, "Texture 0");
    _customTex0->pushAuthorizedDropMimeData("asset/texture");
    _customTex0->setReadOnly(true);
    _customTex1 = new CustomLine(this, "Texture 1");
    _customTex1->pushAuthorizedDropMimeData("asset/texture");
    _customTex1->setReadOnly(true);
    _customTex2 = new CustomLine(this, "Texture 2");
    _customTex2->pushAuthorizedDropMimeData("asset/texture");
    _customTex2->setReadOnly(true);


    _groupBox->getLayout()->addWidget(_matType);
    _groupBox->getLayout()->addWidget(_translucidTexture);
    _groupBox->getLayout()->addWidget(_clampTextureBorder);

    _groupBox->getLayout()->addWidget(_diffuseTex);
    _groupBox->getLayout()->addWidget(_specularTex);
    _groupBox->getLayout()->addWidget(_normalTex);
    
    _groupBox->getLayout()->addWidget(_diffuse);
    _groupBox->getLayout()->addWidget(_specular);
    _groupBox->getLayout()->addWidget(_shininess);
    _groupBox->getLayout()->addWidget(_glow);
    _groupBox->getLayout()->addWidget(_glowExtent);
    _groupBox->getLayout()->addWidget(_reflexions);
    _groupBox->getLayout()->addWidget(_emissive);
    _groupBox->getLayout()->addWidget(_refracColor);
    _groupBox->getLayout()->addWidget(_refracScale);
    _groupBox->getLayout()->addWidget(_refracIndex);
    _groupBox->getLayout()->addWidget(_dynamicShadowCaster);
    _groupBox->getLayout()->addWidget(_dynamicShadowReceiver);


    _groupBox->getLayout()->addWidget(_customMode);
    _groupBox->getLayout()->addWidget(_customColor);
    _groupBox->getLayout()->addWidget(_colorFactor);
    _groupBox->getLayout()->addWidget(_glowFactor);
    _groupBox->getLayout()->addWidget(_customTex0);
    _groupBox->getLayout()->addWidget(_opColor01);
    _groupBox->getLayout()->addWidget(_customTex1);
    _groupBox->getLayout()->addWidget(_opColor12);
    _groupBox->getLayout()->addWidget(_customTex2);
    _groupBox->getLayout()->addWidget(_duv0_X);
    _groupBox->getLayout()->addWidget(_duv0_Y);
    _groupBox->getLayout()->addWidget(_duv1_X);
    _groupBox->getLayout()->addWidget(_duv1_Y);
    _groupBox->getLayout()->addWidget(_duv2_X);
    _groupBox->getLayout()->addWidget(_duv2_Y);

    _layout->addWidget(_groupBox, 0, 0, 1, 1);



    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_matType);
    getWidgetsForUndoRedo().push_back(_translucidTexture);
    getWidgetsForUndoRedo().push_back(_clampTextureBorder);
    getWidgetsForUndoRedo().push_back(_diffuseTex);
    getWidgetsForUndoRedo().push_back(_specularTex);
    getWidgetsForUndoRedo().push_back(_normalTex);
    getWidgetsForUndoRedo().push_back(_emissive);
    getWidgetsForUndoRedo().push_back(_diffuse);
    getWidgetsForUndoRedo().push_back(_specular);
    getWidgetsForUndoRedo().push_back(_shininess);
    getWidgetsForUndoRedo().push_back(_glow);
    getWidgetsForUndoRedo().push_back(_reflexions);
    getWidgetsForUndoRedo().push_back(_glowExtent);
    getWidgetsForUndoRedo().push_back(_refracColor);
    getWidgetsForUndoRedo().push_back(_refracScale);
    getWidgetsForUndoRedo().push_back(_refracIndex);
    getWidgetsForUndoRedo().push_back(_dynamicShadowCaster);
    getWidgetsForUndoRedo().push_back(_dynamicShadowReceiver);
    //custom
    getWidgetsForUndoRedo().push_back(_customMode);
    getWidgetsForUndoRedo().push_back(_customColor);
    getWidgetsForUndoRedo().push_back(_colorFactor);
    getWidgetsForUndoRedo().push_back(_glowFactor);
    getWidgetsForUndoRedo().push_back(_customTex0);
    getWidgetsForUndoRedo().push_back(_opColor01);
    getWidgetsForUndoRedo().push_back(_customTex1);
    getWidgetsForUndoRedo().push_back(_opColor12);
    getWidgetsForUndoRedo().push_back(_customTex2);
    getWidgetsForUndoRedo().push_back(_duv0_X);
    getWidgetsForUndoRedo().push_back(_duv0_Y);
    getWidgetsForUndoRedo().push_back(_duv1_X);
    getWidgetsForUndoRedo().push_back(_duv1_Y);
    getWidgetsForUndoRedo().push_back(_duv2_X);
    getWidgetsForUndoRedo().push_back(_duv2_Y);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetMaterial::readProperty()
{
    //Ptr<PtyMaterial> pPtyMaterial = LM_DEBUG_PTR_CAST<PtyMaterial>(getDataProxy()->getProperty());
    Core::List<Ptr<Property> >& pties = getDataProxy()->getProperties();
    Ptr<PtyMaterial> pPtyMaterial = LM_DEBUG_PTR_CAST<PtyMaterial>(pties[0]);

    _matType->selectIndex((int32)(pPtyMaterial->_matType));

    (pPtyMaterial->_translucidTexture == true) ? _translucidTexture->setCheckState(Qt::Checked) : _translucidTexture->setCheckState(Qt::Unchecked);
    (pPtyMaterial->_clampTextureBorder == true) ? _clampTextureBorder->setCheckState(Qt::Checked) : _clampTextureBorder->setCheckState(Qt::Unchecked);

    _diffuseTex->setText(pPtyMaterial->_diffuseTex);
    _specularTex->setText(pPtyMaterial->_specularTex);
    _normalTex->setText(pPtyMaterial->_normalTex);;
    
    _emissive->setColorLinear(pPtyMaterial->_emissive);
    _diffuse->setColorLinear(pPtyMaterial->_diffuse);
    _specular->setColorLinear(pPtyMaterial->_specular);
    _glow->setColorLinear(pPtyMaterial->_glow);
    _reflexions->setColorLinear(pPtyMaterial->_reflexions);
    _refracColor->setColorLinear(pPtyMaterial->_refracColor);

    _shininess->setSingleValue(pPtyMaterial->_shininess);
    _glowExtent->setSingleValue(pPtyMaterial->_glowExtent);
    _refracScale->setSingleValue(pPtyMaterial->_refracScale);
    _refracIndex->setSingleValue(pPtyMaterial->_refracIndex);

    (pPtyMaterial->_dynamicShadowCaster == true) ?   _dynamicShadowCaster->setCheckState(Qt::Checked)   : _dynamicShadowCaster->setCheckState(Qt::Unchecked);
    (pPtyMaterial->_dynamicShadowReceiver == true) ? _dynamicShadowReceiver->setCheckState(Qt::Checked) : _dynamicShadowReceiver->setCheckState(Qt::Unchecked);

    //custom
    _customTex0->setText(pPtyMaterial->_texture0Name);
    _customTex1->setText(pPtyMaterial->_texture1Name);
    _customTex2->setText(pPtyMaterial->_texture2Name);
    _customMode->selectIndex((int32)(pPtyMaterial->_customMode));
    _customColor->setColorLinear(pPtyMaterial->_customColor);
    _colorFactor->setSingleValue(pPtyMaterial->_colorFactor);
    _glowFactor->setSingleValue(pPtyMaterial->_glowFactor);
    _opColor01->selectIndex((int32)(pPtyMaterial->_opColor01));
    _opColor12->selectIndex((int32)(pPtyMaterial->_opColor12));
    _duv0_X->setSingleValue(pPtyMaterial->_duv0.x);
    _duv1_X->setSingleValue(pPtyMaterial->_duv1.x);
    _duv2_X->setSingleValue(pPtyMaterial->_duv2.x);
    _duv0_Y->setSingleValue(pPtyMaterial->_duv0.y);
    _duv1_Y->setSingleValue(pPtyMaterial->_duv1.y);
    _duv2_Y->setSingleValue(pPtyMaterial->_duv2.y);

    showMaterialParameter();
}
//-----------------------------------------------------------------------------
void PtyWidgetMaterial::writeProperty(QWidget* pWidget)
{
 /*   Ptr<PtyMaterial> pPtyMaterial = LM_DEBUG_PTR_CAST<PtyMaterial>(getDataProxy()->getProperty());*/

    Core::List<Ptr<Property> >& pties = getDataProxy()->getProperties();

    for (int32 iPty=0; iPty<pties.size(); ++iPty)
    {
        Ptr<PtyMaterial> pPtyMaterial = LM_DEBUG_PTR_CAST<PtyMaterial>(pties[iPty]);
        
        if (pWidget == NULL || pWidget == _matType)
        {
            pPtyMaterial->_matType  = (Assets::EMaterialType) _matType->selectedIndex();
            showMaterialParameter();
        }
            

        if (pWidget == NULL || pWidget == _translucidTexture)
            (_translucidTexture->checkState () == Qt::Checked) ? pPtyMaterial->_translucidTexture = true : pPtyMaterial->_translucidTexture = false;

        if (pWidget == NULL || pWidget == _clampTextureBorder)
            (_clampTextureBorder->checkState () == Qt::Checked) ? pPtyMaterial->_clampTextureBorder = true : pPtyMaterial->_clampTextureBorder = false;

        if (pWidget == NULL || pWidget == _diffuseTex)
            _diffuseTex->getText(pPtyMaterial->_diffuseTex);

        if (pWidget == NULL || pWidget == _specularTex)
            _specularTex->getText(pPtyMaterial->_specularTex);

        if (pWidget == NULL || pWidget == _normalTex)
            _normalTex->getText(pPtyMaterial->_normalTex);
        
        if (pWidget == NULL || pWidget == _emissive)
            _emissive->getColorLinear(pPtyMaterial->_emissive);

        if (pWidget == NULL || pWidget == _diffuse)
            _diffuse->getColorLinear(pPtyMaterial->_diffuse);

        if (pWidget == NULL || pWidget == _specular)
            _specular->getColorLinear(pPtyMaterial->_specular);

        if (pWidget == NULL || pWidget == _glow)
            _glow->getColorLinear(pPtyMaterial->_glow);

        if (pWidget == NULL || pWidget == _reflexions)
            _reflexions->getColorLinear(pPtyMaterial->_reflexions);

        if (pWidget == NULL || pWidget == _refracColor)
            _refracColor->getColorLinear(pPtyMaterial->_refracColor);

        if (pWidget == NULL || pWidget == _shininess)
            _shininess->getSingleValue(pPtyMaterial->_shininess);

        if (pWidget == NULL || pWidget == _glowExtent)
            _glowExtent->getSingleValue(pPtyMaterial->_glowExtent);

        if (pWidget == NULL || pWidget == _refracScale)
            _refracScale->getSingleValue(pPtyMaterial->_refracScale);

        if (pWidget == NULL || pWidget == _refracIndex)
            _refracIndex->getSingleValue(pPtyMaterial->_refracIndex);

        if (pWidget == NULL || pWidget == _dynamicShadowCaster)
        {
            if (_dynamicShadowCaster->checkState () == Qt::Checked)
            {
                pPtyMaterial->_dynamicShadowCaster   = true;
                pPtyMaterial->_dynamicShadowReceiver = false;
                setConnectToCommitPropertyChange(false);
                _dynamicShadowReceiver->setChecked(pPtyMaterial->_dynamicShadowReceiver);
                setConnectToCommitPropertyChange(true);
            }
            else
            {
                pPtyMaterial->_dynamicShadowCaster   = false;
            }
        }

        if (pWidget == NULL || pWidget == _dynamicShadowReceiver)
        {
            if (_dynamicShadowReceiver->checkState () == Qt::Checked)
            {
                pPtyMaterial->_dynamicShadowCaster   = false;
                pPtyMaterial->_dynamicShadowReceiver = true;
                setConnectToCommitPropertyChange(false);
                _dynamicShadowCaster->setChecked(pPtyMaterial->_dynamicShadowCaster);
                setConnectToCommitPropertyChange(true);
            }
            else
            {
                pPtyMaterial->_dynamicShadowReceiver   = false;
            }
        }

        //custom
        if (pWidget == NULL || pWidget == _customMode)
        {
            pPtyMaterial->_customMode = (Assets::ECustomMode) _customMode->selectedIndex();
        }

        if (pWidget == NULL || pWidget == _customColor)
        {
            _customColor->getColorLinear(pPtyMaterial->_customColor);
        }

        if (pWidget == NULL || pWidget == _colorFactor)
        {
            _colorFactor->getSingleValue(pPtyMaterial->_colorFactor);
        }

        if (pWidget == NULL || pWidget == _glowFactor)
        {
            _glowFactor->getSingleValue(pPtyMaterial->_glowFactor);
        }

        if (pWidget == NULL || pWidget == _opColor01)
        {
            pPtyMaterial->_opColor01 = (Assets::EOperationType) _opColor01->selectedIndex();
        }

        if (pWidget == NULL || pWidget == _opColor12)
        {
            pPtyMaterial->_opColor12 = (Assets::EOperationType) _opColor12->selectedIndex();
        }

        if (pWidget == NULL || pWidget == _duv0_X)
        {
            _duv0_X->getSingleValue(pPtyMaterial->_duv0.x);
        }

        if (pWidget == NULL || pWidget == _duv1_X)
        {
            _duv1_X->getSingleValue(pPtyMaterial->_duv1.x);
        }

        if (pWidget == NULL || pWidget == _duv2_X)
        {
            _duv2_X->getSingleValue(pPtyMaterial->_duv2.x);
        }

        if (pWidget == NULL || pWidget == _duv0_Y)
        {
            _duv0_Y->getSingleValue(pPtyMaterial->_duv0.y);
        }

        if (pWidget == NULL || pWidget == _duv1_Y)
        {
            _duv1_Y->getSingleValue(pPtyMaterial->_duv1.y);
        }

        if (pWidget == NULL || pWidget == _duv2_Y)
        {
            _duv2_Y->getSingleValue(pPtyMaterial->_duv2.y);
        }

        if (pWidget == NULL || pWidget == _customTex0)
            _customTex0->getText(pPtyMaterial->_texture0Name);
        if (pWidget == NULL || pWidget == _customTex1)
            _customTex1->getText(pPtyMaterial->_texture1Name);
        if (pWidget == NULL || pWidget == _customTex2)
            _customTex2->getText(pPtyMaterial->_texture2Name);
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetMaterial::showMaterialParameter()
{
    Core::List<Ptr<Property> >& pties = getDataProxy()->getProperties();

    if (pties.size() == 0) return;

    Ptr<PtyMaterial> pPtyMaterial = LM_DEBUG_PTR_CAST<PtyMaterial>(pties[0]);
        Assets::EMaterialType defaultType = pPtyMaterial->_matType;

    bool hideAll = false;
    for (int32 iPty=1; iPty<pties.size(); ++iPty)
    {
        pPtyMaterial = LM_DEBUG_PTR_CAST<PtyMaterial>(pties[iPty]);

        if (defaultType != pPtyMaterial->_matType)
        {
            hideAll = true;
            break;
        }
    }

    if (hideAll == true)
    {
        setEnabled(false);
    }
    else
    {
        setEnabled(true);

        Assets::EMaterialType type = (Assets::EMaterialType) _matType->selectedIndex();

        if (type==Assets::MAT_STANDARD)
        {
            showMaterialStandard(true);
            showMaterialCustom(false);
        }
        else
        {
            showMaterialStandard(false);
            showMaterialCustom(true);
        }
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetMaterial::showMaterialStandard(bool flag)
{
    //_refracScale;

    _translucidTexture->setVisible(flag);
    _clampTextureBorder->setVisible(flag);
    _dynamicShadowCaster->setVisible(flag);
    _dynamicShadowReceiver->setVisible(flag);
    _diffuseTex->setVisible(flag);
    _specularTex->setVisible(flag);
    _normalTex->setVisible(flag);
    _emissive->setVisible(flag);
    _diffuse->setVisible(flag);
    _specular->setVisible(flag);
    _shininess->setVisible(flag);
    _glow->setVisible(flag);
    _reflexions->setVisible(flag);
    _glowExtent->setVisible(flag);
    _refracColor->setVisible(flag);
    _refracIndex->setVisible(flag);
}
//-----------------------------------------------------------------------------
void PtyWidgetMaterial::showMaterialCustom(bool flag)
{
    _customMode->setVisible(flag);
    _customColor->setVisible(flag);
    _colorFactor->setVisible(flag);
    _glowFactor->setVisible(flag);
    _opColor01->setVisible(flag);
    _opColor12->setVisible(flag);
    _duv0_X->setVisible(flag);
    _duv1_X->setVisible(flag);
    _duv2_X->setVisible(flag);
    _duv0_Y->setVisible(flag);
    _duv1_Y->setVisible(flag);
    _duv2_Y->setVisible(flag);
    _customTex0->setVisible(flag);
    _customTex1->setVisible(flag);
    _customTex2->setVisible(flag);
}
//-----------------------------------------------------------------------------

} // namespace EPI