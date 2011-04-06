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
#include "PtyWaterPatch.h"

#include <QGridLayout>
#include <QCheckBox>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QtToolbox/SingleSlidingValue.moc.h>
#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWaterLayer::PtyWaterLayer(const Ptr<Universe::NodeWaterPatch>& pNodeWP, int32 idLayer, const Core::String& title) :
    PropertyNode(title, false, true, CONTENT),
    _pNodeWP(pNodeWP),
    _idLayer(idLayer)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyWaterLayer::~PtyWaterLayer()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyWaterLayer::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetWaterLayer> pPW (new PtyWidgetWaterLayer(pDataProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyWaterLayer::clone() const
{
    return Ptr<Property>(new PtyWaterLayer( *this ));
}
//-----------------------------------------------------------------------------
void PtyWaterLayer::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyWaterLayer> pPty = LM_DEBUG_PTR_CAST<PtyWaterLayer>(pSrc);

    _idLayer = pPty->_idLayer;

    _layer = pPty->_layer;
    _scaleU = pPty->_scaleU;
    _scaleV = pPty->_scaleV;
    _speedU = pPty->_speedU;
    _speedV = pPty->_speedV;
    _amplitude = pPty->_amplitude;

    updateData();
}
//-----------------------------------------------------------------------------
void PtyWaterLayer::updateData()
{
    _pNodeWP->setLayerTexture(_idLayer, _layer);
    _pNodeWP->setLayerScaleU(_idLayer, _scaleU);
    _pNodeWP->setLayerScaleV(_idLayer, _scaleV);
    _pNodeWP->setLayerSpeedU(_idLayer, _speedU);
    _pNodeWP->setLayerSpeedV(_idLayer, _speedV);
    _pNodeWP->setLayerAmplitude(_idLayer, _amplitude);
}
//-----------------------------------------------------------------------------
void PtyWaterLayer::updateProperty()
{
    _layer     = _pNodeWP->getLayerTexture(_idLayer);
    _scaleU    = _pNodeWP->getLayerScaleU(_idLayer);
    _scaleV    = _pNodeWP->getLayerScaleV(_idLayer);
    _speedU    = _pNodeWP->getLayerSpeedU(_idLayer);
    _speedV    = _pNodeWP->getLayerSpeedV(_idLayer);
    _amplitude = _pNodeWP->getLayerAmplitude(_idLayer);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetWaterLayer::PtyWidgetWaterLayer(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent ) :
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetWaterLayer::~PtyWidgetWaterLayer()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetWaterLayer::setupUi()
{
     Ptr<Property> pP = getDataProxy()->getProperty();

    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(1);
    
    _pLayer = new CustomLine(this, Core::String8(pP->getName()).c_str());
        _pLayer->pushAuthorizedDropMimeData("asset/texture");
        _pLayer->setReadOnly(true);

    _pScaleU    = new QtToolbox::SingleSlidingHDR(this, "Scale U",   false);
    _pScaleV    = new QtToolbox::SingleSlidingHDR(this, "Scale V",   false);
    _pSpeedU    = new QtToolbox::SingleSlidingHDR(this, "Speed U",   true);
    _pSpeedV    = new QtToolbox::SingleSlidingHDR(this, "Speed V",   true);
    _pAmplitude = new QtToolbox::SingleSlidingDouble(this, "Amplitude", 0.0, 10.0);


     //_groupBox   = new QtToolbox::SmartGroupBox(this, "WaterPatch", true);
        //QGridLayout* layoutGB = new QGridLayout(_groupBox);
        _layout->addWidget(_pLayer,    0,0,1,3, Qt::AlignTop | Qt::AlignLeft);
        _layout->addWidget(_pScaleU,   1,2,1,1, Qt::AlignTop | Qt::AlignLeft);
        _layout->addWidget(_pScaleV,   2,2,1,1, Qt::AlignTop | Qt::AlignLeft);
        _layout->addWidget(_pSpeedU,   3,2,1,1, Qt::AlignTop | Qt::AlignLeft);
        _layout->addWidget(_pSpeedV,   4,2,1,1, Qt::AlignTop | Qt::AlignLeft);
        _layout->addWidget(_pAmplitude,5,2,1,1, Qt::AlignTop | Qt::AlignLeft);
//        _groupBox->setLayout(layoutGB);

    //_layout->addWidget(_groupBox);
    setFixedWidth( Core::max(_pLayer->width(), _pScaleU->width(), _pScaleV->width(), _pSpeedU->width(), _pSpeedV->width(), _pAmplitude->width()) *1.1);
    setFixedHeight( _pLayer->height() + _pScaleU->height() + _pScaleV->height() + _pSpeedU->height() + _pSpeedV->height() + _pAmplitude->height() + _layout->spacing()*5 );
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_pLayer);
    getWidgetsForUndoRedo().push_back(_pScaleU);
    getWidgetsForUndoRedo().push_back(_pScaleV);
    getWidgetsForUndoRedo().push_back(_pSpeedU);
    getWidgetsForUndoRedo().push_back(_pSpeedV);
    getWidgetsForUndoRedo().push_back(_pAmplitude);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetWaterLayer::readProperty()
{
    Ptr<PtyWaterLayer> pPtyWaterLayer = LM_DEBUG_PTR_CAST<PtyWaterLayer>(getDataProxy()->getProperty());

    _pLayer->setText(pPtyWaterLayer->_layer);
    _pScaleU->setSingleValue(pPtyWaterLayer->_scaleU);
    _pScaleV->setSingleValue(pPtyWaterLayer->_scaleV);
    _pSpeedU->setSingleValue(pPtyWaterLayer->_speedU);
    _pSpeedV->setSingleValue(pPtyWaterLayer->_speedV);
    _pAmplitude->setSingleValue(pPtyWaterLayer->_amplitude);
}
//-----------------------------------------------------------------------------
void PtyWidgetWaterLayer::writeProperty(QWidget* pWidget)
{
    Ptr<PtyWaterLayer> pPtyWaterLayer = LM_DEBUG_PTR_CAST<PtyWaterLayer>(getDataProxy()->getProperty());

    if (pWidget == NULL || pWidget == _pLayer)
        _pLayer->getText(pPtyWaterLayer->_layer);

    if (pWidget == NULL || pWidget == _pScaleU)
        _pScaleU->getSingleValue(pPtyWaterLayer->_scaleU);

    if (pWidget == NULL || pWidget == _pScaleV)
        _pScaleV->getSingleValue(pPtyWaterLayer->_scaleV);

    if (pWidget == NULL || pWidget == _pSpeedU)
        _pSpeedU->getSingleValue(pPtyWaterLayer->_speedU);

    if (pWidget == NULL || pWidget == _pSpeedV)
        _pSpeedV->getSingleValue(pPtyWaterLayer->_speedV);

    if (pWidget == NULL || pWidget == _pAmplitude)
        _pAmplitude->getSingleValue(pPtyWaterLayer->_amplitude);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWaterPatch::PtyWaterPatch(const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeWaterPatch>& pNodeW, const Core::String& title):
    PtyNode(pWorld, pNodeW, title)
{
    for (int32 ii=0; ii<Renderer::WATER_LAYER_COUNT; ++ii)
    {
        Ptr<Property>   pChild (new PtyWaterLayer(pNodeW, ii, L"Layer " + Core::toString(ii)));
        _indexChildLayer[ii] = getNbChildren();
        addChild(pChild);
    }

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyWaterPatch::~PtyWaterPatch()
{
   // _pNodeW->kill();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyWaterPatch::getPtyLayer(int32 id)
{
    return getChild(_indexChildLayer[id]);
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyWaterPatch::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetWaterPatch> pPW (new PtyWidgetWaterPatch(pDataProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyWaterPatch::updateData()
{
    PtyNode::updateData();

    Ptr<Universe::NodeWaterPatch>   pNodeWP = LM_DEBUG_PTR_CAST<Universe::NodeWaterPatch>  (getUniverseNode());

    pNodeWP->setSize(_width, _height);
    pNodeWP->setDiffuse(_diffuse);
    pNodeWP->setSpecular(_specular);
    pNodeWP->setShininess(_shininess);
    pNodeWP->setRefractive(_isRefractive);
    pNodeWP->setReflective(_isReflective);
    pNodeWP->setReflectionScale(_reflectionScale);
    pNodeWP->setRefractionScale(_refractionScale);
    pNodeWP->setFresnelMin(_fresnelMin);
    pNodeWP->setFresnelPower(_FresnelPower);
    pNodeWP->setDiffuseTexture(_diffuseTex);
}
//-----------------------------------------------------------------------------
void PtyWaterPatch::updateProperty()
{
    Ptr<Universe::NodeWaterPatch>   pNodeWP = LM_DEBUG_PTR_CAST<Universe::NodeWaterPatch>  (getUniverseNode());

    _width              =   pNodeWP->getWidth();
    _height             =   pNodeWP->getHeight();
    _diffuse            =   pNodeWP->getDiffuse();
    _specular           =   pNodeWP->getSpecular();
    _shininess          =   pNodeWP->getShininess();
    _isRefractive       =   pNodeWP->isRefractive();
    _isReflective       =   pNodeWP->isReflective();
    _reflectionScale    =   pNodeWP->getReflectionScale();
    _refractionScale    =   pNodeWP->getRefractionScale();
    _fresnelMin         =   pNodeWP->getFresnelMin();
    _FresnelPower       =   pNodeWP->getFresnelPower();
    _diffuseTex         =   pNodeWP->getDiffuseTexture();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyWaterPatch::clone() const
{
    return Ptr<Property>(new PtyWaterPatch( *this ));
}
//-----------------------------------------------------------------------------
void PtyWaterPatch::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);
    Ptr<PtyWaterPatch> pPtyWaterPatch = LM_DEBUG_PTR_CAST<PtyWaterPatch>(pSrc);

    _width = pPtyWaterPatch->_width;
    _height = pPtyWaterPatch->_height;
    _diffuse = pPtyWaterPatch->_diffuse;
    _specular = pPtyWaterPatch->_specular;
    _shininess = pPtyWaterPatch->_shininess;
    _isRefractive = pPtyWaterPatch->_isRefractive;
    _isReflective = pPtyWaterPatch->_isReflective;
    _reflectionScale = pPtyWaterPatch->_reflectionScale;
    _refractionScale = pPtyWaterPatch->_refractionScale;
    _fresnelMin = pPtyWaterPatch->_fresnelMin;
    _FresnelPower = pPtyWaterPatch->_FresnelPower;
    _diffuseTex = pPtyWaterPatch->_diffuseTex;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetWaterPatch::PtyWidgetWaterPatch(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetWaterPatch::~PtyWidgetWaterPatch()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetWaterPatch::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = Ptr<PtyWidgetNode> (new PtyWidgetNode(pDataPNodeW, this));
    _pPWNode->collapse(true);

    _width              = new QtToolbox::SingleSlidingHDR(this, "Width",   false);
    _height             = new QtToolbox::SingleSlidingHDR(this, "Height",  false);
    _pDiffuseTex        = new CustomLine(this, "Diffuse texture");
    _diffuse            = new QtToolbox::QuickColorPicker(this, "Diffuse", Qt::white);
    _specular           = new QtToolbox::QuickColorPicker(this, "Specular", Qt::white);
    _shininess          = new QtToolbox::SingleSlidingDouble(this, "Shininess",   0.0, 100.0);
    _isRefractive       = new QCheckBox("Refractive", this);
    _isReflective       = new QCheckBox("Reflective", this);
    _refractionScale    = new QtToolbox::SingleSlidingHDR(this, "Refraction Scale",  false);
    _reflectionScale    = new QtToolbox::SingleSlidingHDR(this, "Reflection scale",  false);
    _fresnelMin         = new QtToolbox::SingleSlidingDouble(this, "FresnelMin", 0.0, 1.0);
    _FresnelPower       = new QtToolbox::SingleSlidingHDR(this, "FresnelPower", false);

    _pDiffuseTex->pushAuthorizedDropMimeData("asset/texture");
    _pDiffuseTex->setReadOnly(true);

    for (int32 ii=0; ii<Renderer::WATER_LAYER_COUNT; ++ii)
    {
        Ptr<Property> pP = LM_DEBUG_PTR_CAST<PtyWaterPatch>(getDataProxy()->getProperty())->getPtyLayer(ii);
        Ptr<PropertyWidgetDataProxy> pDataPW (new PropertyWidgetDataProxy(pP, pP->getName(), getDataProxy()->getStateRecorder()));
        _tabLayerWidget[ii] = LM_DEBUG_PTR_CAST<PtyWidgetWaterLayer>(pP->createPropertyWidget(pDataPW, this));
    }

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "WaterPatch", false);
    _groupBox->setStyle(QtToolbox::CW_TITLE_1);
    _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupBoxMaterial   = new QtToolbox::CollapsibleWidget(this, "Material");
    _groupBoxLayer      = new QtToolbox::CollapsibleWidget(this, "Layer");
    _groupBoxPlane      = new QtToolbox::CollapsibleWidget(this, "Plane");
        
    _groupBoxPlane->getLayout()->addWidget(_width);
    _groupBoxPlane->getLayout()->addWidget(_height);

    _groupBoxMaterial->getLayout()->addWidget(_diffuse);
    _groupBoxMaterial->getLayout()->addWidget(_specular);
    _groupBoxMaterial->getLayout()->addWidget(_shininess);
    _groupBoxMaterial->getLayout()->addWidget(_isRefractive);
    _groupBoxMaterial->getLayout()->addWidget(_isReflective);
    _groupBoxMaterial->getLayout()->addWidget(_refractionScale);
    _groupBoxMaterial->getLayout()->addWidget(_reflectionScale);
    _groupBoxMaterial->getLayout()->addWidget(_fresnelMin);
    _groupBoxMaterial->getLayout()->addWidget(_FresnelPower);
    _groupBoxMaterial->getLayout()->addWidget(_pDiffuseTex);

    for (int32 ii=0; ii<Renderer::WATER_LAYER_COUNT; ++ii)
    {
        _groupBoxLayer->getLayout()->addWidget(_tabLayerWidget[ii].get());
    }

    _groupBox->getLayout()->addWidget(_pPWNode.get());
    _groupBox->getLayout()->addWidget(_groupBoxPlane);
    _groupBox->getLayout()->addWidget(_groupBoxMaterial);
    _groupBox->getLayout()->addWidget(_groupBoxLayer);

    _layout->addWidget(_groupBox);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_width);
    getWidgetsForUndoRedo().push_back(_height);
    getWidgetsForUndoRedo().push_back(_diffuse);
    getWidgetsForUndoRedo().push_back(_specular);
    getWidgetsForUndoRedo().push_back(_shininess);
    getWidgetsForUndoRedo().push_back(_isRefractive);
    getWidgetsForUndoRedo().push_back(_isReflective);
    getWidgetsForUndoRedo().push_back(_reflectionScale);
    getWidgetsForUndoRedo().push_back(_refractionScale);
    getWidgetsForUndoRedo().push_back(_fresnelMin);
    getWidgetsForUndoRedo().push_back(_FresnelPower);
    getWidgetsForUndoRedo().push_back(_pDiffuseTex);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetWaterPatch::readProperty()
{
    Ptr<PtyWaterPatch> pPtyWaterPatch = LM_DEBUG_PTR_CAST<PtyWaterPatch>(getDataProxy()->getProperty());

    _pPWNode->updatePropertyWidget();

    _width->setSingleValue(pPtyWaterPatch->_width);
    _height->setSingleValue(pPtyWaterPatch->_height);
    _diffuse->setColorLinear(pPtyWaterPatch->_diffuse);
    _specular->setColorLinear(pPtyWaterPatch->_specular);
    _shininess->setSingleValue(pPtyWaterPatch->_shininess);
    _isRefractive->setChecked(pPtyWaterPatch->_isRefractive);
    _isReflective->setChecked(pPtyWaterPatch->_isReflective);
    _reflectionScale->setSingleValue(pPtyWaterPatch->_reflectionScale);
    _refractionScale->setSingleValue(pPtyWaterPatch->_refractionScale);
    _fresnelMin->setSingleValue(pPtyWaterPatch->_fresnelMin);
    _FresnelPower->setSingleValue(pPtyWaterPatch->_FresnelPower);
    _pDiffuseTex->setText(pPtyWaterPatch->_diffuseTex);


    for (int32 ii=0; ii<Renderer::WATER_LAYER_COUNT; ++ii)
    {
        _tabLayerWidget[ii]->updatePropertyWidget();
    }
    
}
//-----------------------------------------------------------------------------
void PtyWidgetWaterPatch::writeProperty(QWidget* pWidget)
{
    Ptr<PtyWaterPatch> pPtyWaterPatch = LM_DEBUG_PTR_CAST<PtyWaterPatch>(getDataProxy()->getProperty());

    if (pWidget == NULL || pWidget == _width)
        _width->getSingleValue(pPtyWaterPatch->_width);
    
    if (pWidget == NULL || pWidget == _height)
        _height->getSingleValue(pPtyWaterPatch->_height);

    if (pWidget == NULL || pWidget == _diffuse)
        _diffuse->getColorLinear(pPtyWaterPatch->_diffuse);

    if (pWidget == NULL || pWidget == _specular)
        _specular->getColorLinear(pPtyWaterPatch->_specular);

    if (pWidget == NULL || pWidget == _shininess)
        _shininess->getSingleValue(pPtyWaterPatch->_shininess);

    if (pWidget == NULL || pWidget == _isRefractive)
        (_isRefractive->checkState () == Qt::Checked) ? pPtyWaterPatch->_isRefractive = true : pPtyWaterPatch->_isRefractive = false;

    if (pWidget == NULL || pWidget == _isReflective)
        (_isRefractive->checkState () == Qt::Checked) ? pPtyWaterPatch->_isReflective = true : pPtyWaterPatch->_isReflective = false;

    if (pWidget == NULL || pWidget == _reflectionScale)
        _reflectionScale->getSingleValue(pPtyWaterPatch->_reflectionScale);

    if (pWidget == NULL || pWidget == _refractionScale)
        _refractionScale->getSingleValue(pPtyWaterPatch->_refractionScale);

    if (pWidget == NULL || pWidget == _fresnelMin)
        _fresnelMin->getSingleValue(pPtyWaterPatch->_fresnelMin);

    if (pWidget == NULL || pWidget == _FresnelPower)
        _FresnelPower->getSingleValue(pPtyWaterPatch->_FresnelPower);

    if (pWidget == NULL || pWidget == _pDiffuseTex)
        _pDiffuseTex->getText(pPtyWaterPatch->_diffuseTex);
}
//-----------------------------------------------------------------------------
} // namespace EPI