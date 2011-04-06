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
#include "PtyNodePointLight.h"


#include <QGridLayout>
#include <QCheckBox>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <EPI/GUI/Widget/CustomLine.moc.h>
#include <Renderer/IFreeForm.h>
#include <EPI/Document/ContentInfo/PointLightRepresentation.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodePointLight::PtyNodePointLight( const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodePointLight>& pNodePL, 
                            const Core::String& title)
:   PtyNode(pWorld, pNodePL, title),
    _pWorldForRepresentation(pWorldForRepresentation)
{
    _pRepresentation = Ptr<IContentRepresentation>(new PointLightRepresentation(_pWorldForRepresentation, getUniverseNode()));
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodePointLight::PtyNodePointLight(const PtyNodePointLight& other):
    PtyNode(other)
{
    _pWorldForRepresentation = other._pWorldForRepresentation;

    _diffuse                    = other._diffuse;
    _ambiant                    = other._ambiant;
    _specular                   = other._specular;
    _radius                     = other._radius;
    _shadowCasting              = other._shadowCasting;
    _isBurst                    = other._isBurst;
    _turnOnLatency              = other._turnOnLatency;
    _turnOffLatency             = other._turnOffLatency;
    _isStatic                   = other._isStatic;

    _pRepresentation = null;
    _pWorldForRepresentation = null;
}
//-----------------------------------------------------------------------------
PtyNodePointLight::~PtyNodePointLight()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodePointLight::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodePointLight> pPW (new PtyWidgetNodePointLight(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodePointLight::updateProperty()
{
    Ptr<Universe::NodePointLight>  pNodePointLight = LM_DEBUG_PTR_CAST<Universe::NodePointLight>(getUniverseNode());

    _diffuse                    = pNodePointLight->getDiffuseColor();
    _ambiant                    = pNodePointLight->getAmbientColor();
    _specular                   = pNodePointLight->getSpecularColor();
    _radius                     = pNodePointLight->getRadius();
    _shadowCasting              = pNodePointLight->getShadowCasting();
    _isBurst                    = pNodePointLight->getBurst();
    _turnOnLatency              = pNodePointLight->getTurnOnLatency();
    _turnOffLatency             = pNodePointLight->getTurnOffLatency();
    _isStatic                   = pNodePointLight->isStatic();

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
void PtyNodePointLight::updateData()
{
    PtyNode::updateData();

    Ptr<Universe::NodePointLight>  pNodePointLight = LM_DEBUG_PTR_CAST<Universe::NodePointLight>(getUniverseNode());

    pNodePointLight->setDiffuseColor(_diffuse);
    pNodePointLight->setAmbientColor(_ambiant);
    pNodePointLight->setSpecularColor(_specular);
    pNodePointLight->setRadius(_radius);
    pNodePointLight->setShadowCasting(_shadowCasting);
    pNodePointLight->setBurst(_isBurst);
    pNodePointLight->setTurnOnLatency(_turnOnLatency);
    pNodePointLight->setTurnOffLatency(_turnOffLatency);
    pNodePointLight->setStatic(_isStatic);

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodePointLight::clone() const
{
    Ptr<PtyNodePointLight> ret (new PtyNodePointLight( *this ));

    return ret;
}
//-----------------------------------------------------------------------------
void PtyNodePointLight::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodePointLight> pPty = LM_DEBUG_PTR_CAST<PtyNodePointLight>(pSrc);

    _diffuse                    = pPty->_diffuse;
    _ambiant                    = pPty->_ambiant;
    _specular                   = pPty->_specular;
    _radius                     = pPty->_radius;
    _shadowCasting              = pPty->_shadowCasting;
    _isBurst                    = pPty->_isBurst;
    _turnOnLatency              = pPty->_turnOnLatency;
    _turnOffLatency             = pPty->_turnOffLatency;
    _isStatic                   = pPty->_isStatic;

    updateData();
}
//-----------------------------------------------------------------------------
bool PtyNodePointLight::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (_pRepresentation->isItMe(pNode) == true)
        return true;
    else
        return PtyNode::isItMe(pNode);
}
//-----------------------------------------------------------------------------
bool PtyNodePointLight::isIntersecting(const Core::Rayf& ray, float& distance)
{
    bool flag = PtyNode::isIntersecting(ray, distance);

    if (flag == false)
    {
        flag = _pRepresentation->isIntersecting(ray, distance);
    }

    return flag;
}
//-----------------------------------------------------------------------------
void PtyNodePointLight::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PtyNode::internalResurrect(pWorld, pWorldInfoContent, pty);
    getUniverseNode()->restart(false);
    _pWorldForRepresentation = pWorldInfoContent;

    LM_ASSERT(_pRepresentation==null);

    _pRepresentation = Ptr<IContentRepresentation>(new PointLightRepresentation(_pWorldForRepresentation, getUniverseNode()));

    updateProperty();
}
//-----------------------------------------------------------------------------
void PtyNodePointLight::setVisible(bool flag)
{
    PtyNode::setVisible(flag);
    _pRepresentation->setVisible(flag);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodePointLight::PtyWidgetNodePointLight(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodePointLight::~PtyWidgetNodePointLight()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetNodePointLight::readProperty()
{
    Ptr<PtyNodePointLight> pP = LM_DEBUG_PTR_CAST<PtyNodePointLight>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();

    _radius->setSingleValue(pP->_radius);
    _turnOnLatency->setSingleValue(pP->_turnOnLatency);
    _turnOffLatency->setSingleValue(pP->_turnOffLatency);
    _diffuse->setColorLinear(pP->_diffuse);
    _ambiant->setColorLinear(pP->_ambiant);
    _specular->setColorLinear(pP->_specular);
    _shadowCasting->setChecked(pP->_shadowCasting);
    _isBurst->setChecked(pP->_isBurst);
    _isStatic->setChecked(pP->_isStatic);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodePointLight::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodePointLight> pP = LM_DEBUG_PTR_CAST<PtyNodePointLight>(getDataProxy()->getProperty());

    _radius->getSingleValue(pP->_radius);
    _turnOnLatency->getSingleValue(pP->_turnOnLatency);
    _turnOffLatency->getSingleValue(pP->_turnOffLatency);
    _diffuse->getColorLinear(pP->_diffuse);
    _ambiant->getColorLinear(pP->_ambiant);
    _specular->getColorLinear(pP->_specular);

    pP->_shadowCasting = _shadowCasting->isChecked();
    pP->_isBurst = _isBurst->isChecked();
    pP->_isStatic = _isStatic->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodePointLight::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
        Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
        _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);

    _groupBox      = new QtToolbox::CollapsibleWidget(this, "Point Light", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupBoxColor = new QtToolbox::CollapsibleWidget(this, "Light");

    _radius         = new QtToolbox::SingleSlidingHDR(this, "radius", false);
    _turnOnLatency  = new QtToolbox::SingleSlidingHDR(this, "turn on latency", false);
    _turnOffLatency = new QtToolbox::SingleSlidingHDR(this, "turn off latency", false);
    _diffuse        = new QtToolbox::QuickColorPicker(this, "diffuse", Qt::white);
    _ambiant        = new QtToolbox::QuickColorPicker(this, "ambiant", Qt::white);
    _specular       = new QtToolbox::QuickColorPicker(this, "specular", Qt::white);
    _shadowCasting  = new QCheckBox ("shadow casting", this);
    _isBurst        = new QCheckBox ("burst mode", this);
    _isStatic      = new QCheckBox ("is static", this);

    _groupBoxColor->getLayout()->addWidget(_diffuse);
    _groupBoxColor->getLayout()->addWidget(_ambiant);
    _groupBoxColor->getLayout()->addWidget(_specular);
    _groupBoxColor->getLayout()->addWidget(_radius);
    _groupBoxColor->getLayout()->addWidget(_turnOnLatency);
    _groupBoxColor->getLayout()->addWidget(_turnOffLatency);
    _groupBoxColor->getLayout()->addWidget(_shadowCasting);
    _groupBoxColor->getLayout()->addWidget(_isBurst);
    _groupBoxColor->getLayout()->addWidget(_isStatic);

    _groupBox->getLayout()->addWidget(_pPWNode.get());
    _groupBox->getLayout()->addWidget(_groupBoxColor);

    _layout->addWidget(_groupBox);
        setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_radius);
    getWidgetsForUndoRedo().push_back(_turnOnLatency);
    getWidgetsForUndoRedo().push_back(_turnOffLatency);
    getWidgetsForUndoRedo().push_back(_diffuse);
    getWidgetsForUndoRedo().push_back(_ambiant);
    getWidgetsForUndoRedo().push_back(_specular);
    getWidgetsForUndoRedo().push_back(_shadowCasting);
    getWidgetsForUndoRedo().push_back(_isBurst);
    getWidgetsForUndoRedo().push_back(_isStatic);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI