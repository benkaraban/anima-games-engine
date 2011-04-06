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
#include "PtySky.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <QtToolbox/ComboBox.moc.h>

#include <Universe/World.h>

#include <QGridLayout>
#include <QCheckBox>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtySky::PtySky(const Ptr<Universe::World>& pWorld, const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _skyTexture(pWorld->getSkyTexture()),
    _color(pWorld->getSkyColor()),
    _glow(pWorld->getSkyGlow()),
    _horizon(pWorld->getSkyHorizon()),
    _angle(pWorld->getSkyAngle()),
    _roof(pWorld->getSkyRoof()),
    _sphericity(pWorld->getSkySphericity()),
    _atInfinity(pWorld->getSkyAtInfinity()),
    _isProcedural(pWorld->isSkyProcedural()),
    _model(pWorld->getSkyModel()),
    _skyColor(pWorld->getSkyBackColor()),
    _sunColor(pWorld->getSkySunColor()),
    _intensity(pWorld->getSkySunIntensity()),
    _pWorld(pWorld)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtySky::~PtySky()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtySky::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetSky> pPW (new PtyWidgetSky(pDataProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtySky::updateProperty()
{
    _skyTexture     = _pWorld->getSkyTexture();
    _color          = _pWorld->getSkyColor();
    _glow           = _pWorld->getSkyGlow();
    _horizon        = _pWorld->getSkyHorizon();
    _angle          = _pWorld->getSkyAngle();
    _roof           = _pWorld->getSkyRoof();
    _sphericity     = _pWorld->getSkySphericity();
    _atInfinity     = _pWorld->getSkyAtInfinity();
    _isProcedural   = _pWorld->isSkyProcedural();
    _model          = _pWorld->getSkyModel();
    _skyColor       = _pWorld->getSkyBackColor();
    _sunColor       = _pWorld->getSkySunColor();
    _intensity      = _pWorld->getSkySunIntensity();
}
//-----------------------------------------------------------------------------
void PtySky::updateData()
{
    _pWorld->setSkyTexture(_skyTexture);
    _pWorld->setSkyColor(_color);
    _pWorld->setSkyGlow(_glow);
    _pWorld->setSkyHorizon(_horizon);
    _pWorld->setSkyAngle(_angle);
    _pWorld->setSkyRoof(_roof);
    _pWorld->setSkySphericity(_sphericity);
    _pWorld->setSkyAtInfinity(_atInfinity);
    _pWorld->setSkyProcedural(_isProcedural);
    _pWorld->setSkyModel(_model);
    _pWorld->setSkyBackColor(_skyColor);
    _pWorld->setSkySunColor(_sunColor);
    _pWorld->setSkySunIntensity(_intensity);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtySky::clone() const
{
    return Ptr<Property>(new PtySky( *this ));
}
//-----------------------------------------------------------------------------
void PtySky::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtySky> pPtySky = LM_DEBUG_PTR_CAST<PtySky>(pSrc);

    _skyTexture     = pPtySky->_skyTexture;
    _color          = pPtySky->_color;
    _glow           = pPtySky->_glow;
    _horizon        = pPtySky->_horizon;
    _angle          = pPtySky->_angle;
    _roof           = pPtySky->_roof;
    _sphericity     = pPtySky->_sphericity;
    _atInfinity     = pPtySky->_atInfinity;
    _isProcedural   = pPtySky->_isProcedural;
    _pWorld         = pPtySky->_pWorld;
    _model          = pPtySky->_model;
    _skyColor       = pPtySky->_skyColor;
    _sunColor       = pPtySky->_sunColor;
    _intensity      = pPtySky->_intensity;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetSky::PtyWidgetSky(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetSky::~PtyWidgetSky()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetSky::setupUi()
{
    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Sky", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    QGridLayout* layoutGroupBox = new QGridLayout(NULL);
        layoutGroupBox->setContentsMargins(0, 0, 0, 0);    
        _groupBox->setLayout(layoutGroupBox);

    _skyTexture = new CustomLine(_groupBox, "Texture");
        _skyTexture->pushAuthorizedDropMimeData("asset/texture");
        _skyTexture->setReadOnly(true);
    _color      = new QtToolbox::QuickColorPicker(_groupBox, "Color", Qt::white, false);
    _glow       = new QtToolbox::QuickColorPicker(_groupBox, "Glow", Qt::white, false);
    _horizon    = new QtToolbox::SingleSlidingDouble(_groupBox, "Horizon", -1.0, 0.99);
    _angle      = new QtToolbox::SingleSlidingDouble(_groupBox, "Angle", 0, Core::rad2deg(d_PI_MUL_2));
    _roof       = new QtToolbox::SingleSlidingDouble(_groupBox, "Roof", 0.0, 5000.0);
    _sphericity = new QtToolbox::SingleSlidingDouble(_groupBox, "Sphericity", 0.0, 1.0);
    _atInfinity = new QCheckBox("At infinity", _groupBox);
    _atInfinity->setChecked(true);
    _isProcedural = new QCheckBox("Is procedural", _groupBox);
    _isProcedural->setChecked(true);
    _model      = new QtToolbox::ComboBox(_groupBox, "Model");
    _skyColor   = new QtToolbox::QuickColorPicker(_groupBox, "Sky color", Qt::white, false);
    _sunColor   = new QtToolbox::QuickColorPicker(_groupBox, "Sun color", Qt::white, false);
    _intensity  = new QtToolbox::SingleSlidingDouble(_groupBox, "Intensity", 1.0, 4.0);

    _model->addItem("Foggy");
    _model->addItem("Cloudy");
    _model->addItem("Clear sky");

    layoutGroupBox->addWidget(_skyTexture,  0,  0,  Qt::AlignCenter);
    layoutGroupBox->addWidget(_color,       1,  0,  Qt::AlignCenter);
    layoutGroupBox->addWidget(_glow,        2,  0,  Qt::AlignCenter);
    layoutGroupBox->addWidget(_horizon,     3,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_angle,       4,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_roof,        5,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_sphericity,  6,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_atInfinity,  7,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_isProcedural,8,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_model,       9,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_skyColor,   10,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_sunColor,   11,  0,  Qt::AlignLeft);
    layoutGroupBox->addWidget(_intensity,  12,  0,  Qt::AlignLeft);
    
    _layout->addWidget(_groupBox);

    getWidgetsForUndoRedo().push_back(_skyTexture);
    getWidgetsForUndoRedo().push_back(_color);
    getWidgetsForUndoRedo().push_back(_glow);
    getWidgetsForUndoRedo().push_back(_horizon);
    getWidgetsForUndoRedo().push_back(_angle);
    getWidgetsForUndoRedo().push_back(_roof);
    getWidgetsForUndoRedo().push_back(_sphericity);
    getWidgetsForUndoRedo().push_back(_atInfinity);
    getWidgetsForUndoRedo().push_back(_isProcedural);
    getWidgetsForUndoRedo().push_back(_model);
    getWidgetsForUndoRedo().push_back(_skyColor);
    getWidgetsForUndoRedo().push_back(_sunColor);
    getWidgetsForUndoRedo().push_back(_intensity);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetSky::readProperty()
{
    Ptr<PtySky> pPtySky = LM_DEBUG_PTR_CAST<PtySky>(getDataProxy()->getProperty());

    _skyTexture->setText(Core::String8(pPtySky->_skyTexture).c_str());
    _color->setColorLinear(pPtySky.get()->_color.r, pPtySky.get()->_color.g, pPtySky.get()->_color.b, pPtySky.get()->_color.a);
    _glow->setColorLinear(pPtySky.get()->_glow.r, pPtySky.get()->_glow.g, pPtySky.get()->_glow.b, pPtySky.get()->_glow.a);
    _horizon->setSingleValue(pPtySky->_horizon);
    _angle->setSingleValue(Core::rad2deg(pPtySky->_angle));
    _roof->setSingleValue(pPtySky->_roof);
    _sphericity->setSingleValue(pPtySky->_sphericity);
    _atInfinity->setChecked(pPtySky->_atInfinity);
    _isProcedural->setChecked(pPtySky->_isProcedural);
    _skyColor->setColorLinear(pPtySky->_skyColor);
    _sunColor->setColorLinear(pPtySky->_sunColor);
    _model->selectIndex(int(pPtySky->_model));
    _intensity->setSingleValue(pPtySky->_intensity);
}
//-----------------------------------------------------------------------------
void PtyWidgetSky::writeProperty(QWidget* pWidget)
{
    Ptr<PtySky> pPtySky = LM_DEBUG_PTR_CAST<PtySky>(getDataProxy()->getProperty());

    pPtySky->_skyTexture = Core::String(_skyTexture->text().toStdString().c_str());
    _color->getColorLinear(pPtySky.get()->_color.r, pPtySky.get()->_color.g, pPtySky.get()->_color.b, pPtySky.get()->_color.a);
    _glow->getColorLinear(pPtySky.get()->_glow.r, pPtySky.get()->_glow.g, pPtySky.get()->_glow.b, pPtySky.get()->_glow.a);
    _horizon->getSingleValue(pPtySky->_horizon);
    double angle = 0.0;
    _angle->getSingleValue(angle);
    pPtySky->_angle = Core::deg2rad(angle);
    _roof->getSingleValue(pPtySky->_roof);
    _sphericity->getSingleValue(pPtySky->_sphericity);
    pPtySky->_atInfinity = _atInfinity->isChecked();
    pPtySky->_isProcedural = _isProcedural->isChecked();
    _skyColor->getColorLinear(pPtySky->_skyColor);
    _sunColor->getColorLinear(pPtySky->_sunColor);
    pPtySky->_model = Renderer::ELightingModel(_model->selectedIndex());
    _intensity->getSingleValue(pPtySky->_intensity);
}
//-----------------------------------------------------------------------------
} // namespace EPI