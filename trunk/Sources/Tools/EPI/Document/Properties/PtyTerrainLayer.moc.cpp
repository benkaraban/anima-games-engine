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
#include "PtyTerrainLayer.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/LineEdit.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <EPI/GUI/Widget/CustomLine.moc.h>

#include <QGridLayout>
#include <QCheckBox>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyTerrainLayer::PtyTerrainLayer(const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _name(title),
    _diffuseTexName(L""),
    _normalTexName(L""),
    _maskTexName(L""),
    _width(100),
    _height(100),
    _angle(0.0f),
    _color(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f)),
    _contrast(0.f),
    _useDisplacementMap(false),
    _normalStrength(1.f)
{

}


PtyTerrainLayer::PtyTerrainLayer(const Universe::NodeTerrain::Layer & layer)
{
    setLayer(layer);
}

PtyTerrainLayer::~PtyTerrainLayer()
{}

void PtyTerrainLayer::setLayer(const Universe::NodeTerrain::Layer & layer)
{
    _name = layer.name;

    _diffuseTexName     = layer.diffuseTexName;
    _normalTexName      = layer.normalTexName;
    _maskTexName        = layer.maskTexName;
    _width              = layer.width;
    _height             = layer.height;
    _angle              = layer.angle;
    _color              = layer.color;
    _contrast           = layer.contrast;
    _useDisplacementMap = layer.useDisplacementMap;
    _normalStrength     = layer.normalStrength;
}

void PtyTerrainLayer::getLayer(Universe::NodeTerrain::Layer & layer)
{
    layer.name               = _name;

    layer.diffuseTexName     = _diffuseTexName;
    layer.normalTexName      = _normalTexName;
    layer.maskTexName        = _maskTexName;
    layer.width              = _width;
    layer.height             = _height;
    layer.angle              = _angle;
    layer.color              = _color;
    layer.contrast           = _contrast;
    layer.useDisplacementMap = _useDisplacementMap;
    layer.normalStrength     = _normalStrength;
}

Ptr<PropertyWidget> PtyTerrainLayer::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetTerrainLayer> pPW (new PtyWidgetTerrainLayer(pDataProxy, parent));

    return pPW;
}

void PtyTerrainLayer::updateData()
{
}

void PtyTerrainLayer::updateProperty()
{
}

Ptr<Property> PtyTerrainLayer::clone() const
{
    return Ptr<Property>(new PtyTerrainLayer( *this ));
}

void PtyTerrainLayer::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyTerrainLayer> pPty = LM_DEBUG_PTR_CAST<PtyTerrainLayer>(pSrc);

    _name               = pPty->_name;
                        
    _diffuseTexName     = pPty->_diffuseTexName;
    _normalTexName      = pPty->_normalTexName;
    _maskTexName        = pPty->_maskTexName;
    _width              = pPty->_width;
    _height             = pPty->_height;
    _angle              = pPty->_angle;
    _color              = pPty->_color;
    _contrast           = pPty->_contrast;
    _useDisplacementMap = pPty->_useDisplacementMap;
    _normalStrength     = pPty->_normalStrength;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetTerrainLayer::PtyWidgetTerrainLayer(   const Ptr<PropertyWidgetDataProxy>& data,
                                                QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
PtyWidgetTerrainLayer::~PtyWidgetTerrainLayer()
{}

void PtyWidgetTerrainLayer::readProperty()
{
    Ptr<PtyTerrainLayer> pPtyTerrainLayer = LM_DEBUG_PTR_CAST<PtyTerrainLayer>(getDataProxy()->getProperty());

    _groupBox->setTitle(L"Layer :      " + pPtyTerrainLayer->_name);

    _diffuse->setText(pPtyTerrainLayer->_diffuseTexName);
    _normal->setText(pPtyTerrainLayer->_normalTexName);
    _mask->setText(pPtyTerrainLayer->_maskTexName);

    Core::String width = Core::toString(pPtyTerrainLayer->_width);
    _width->setText(Core::String8(width).c_str());

    Core::String height = Core::toString(pPtyTerrainLayer->_height);
    _height->setText(Core::String8(height).c_str());

    Core::String angle = Core::toString(int32(Core::rad2deg(pPtyTerrainLayer->_angle)));
    _angle->setText(Core::String8(angle).c_str());
    
    _color->setColorLinear( pPtyTerrainLayer->_color);

    _contrast->setSingleValue(pPtyTerrainLayer->_contrast);
    _normalStrength->setSingleValue(pPtyTerrainLayer->_normalStrength);
    _useDisplacementMap->setChecked(pPtyTerrainLayer->_useDisplacementMap);
}

void PtyWidgetTerrainLayer::writeProperty(QWidget* pWidget)
{
    Ptr<PtyTerrainLayer> pPtyTerrainLayer = LM_DEBUG_PTR_CAST<PtyTerrainLayer>(getDataProxy()->getProperty());

    if (pWidget == NULL || pWidget == _diffuse)
        _diffuse->getText(pPtyTerrainLayer->_diffuseTexName);

    if (pWidget == NULL || pWidget == _normal)
        _normal->getText(pPtyTerrainLayer->_normalTexName);

    if (pWidget == NULL || pWidget == _mask)
        _mask->getText(pPtyTerrainLayer->_maskTexName);

    if (pWidget == NULL || pWidget == _width)
    {
        Core::String width(_width->text().toStdString().c_str());
        pPtyTerrainLayer->_width = Core::toInteger(width);
    }

    if (pWidget == NULL || pWidget == _height)
    {
        Core::String height(_height->text().toStdString().c_str());
        pPtyTerrainLayer->_height = Core::toInteger(height);
    }

    if (pWidget == NULL || pWidget == _angle)
    {
        Core::String angle(_angle->text().toStdString().c_str());
        pPtyTerrainLayer->_angle = Core::deg2rad(Core::toInteger(angle));
    }

    if (pWidget == NULL || pWidget == _color)
    {
        _color->getColorLinear(pPtyTerrainLayer->_color);
    }

    if (pWidget == NULL || pWidget == _contrast)
    {
        _contrast->getSingleValue(pPtyTerrainLayer->_contrast);
    }

    if (pWidget == NULL || pWidget == _normalStrength)
    {
        _normalStrength->getSingleValue(pPtyTerrainLayer->_normalStrength);
    }

    if (pWidget == NULL || pWidget == _useDisplacementMap)
    {
        pPtyTerrainLayer->_useDisplacementMap = _useDisplacementMap->isChecked();
    }
}

void PtyWidgetTerrainLayer::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Layer");


    _diffuse    = new CustomLine(this, "Diffuse");
    _diffuse->pushAuthorizedDropMimeData("asset/texture");
    _diffuse->setReadOnly(true);
    _normal     = new CustomLine(this, "Normal");
        _normal->pushAuthorizedDropMimeData("asset/texture");
        _normal->setReadOnly(true);
    _mask       = new CustomLine(this, "Mask");
        _mask->pushAuthorizedDropMimeData("asset/texture");
        _mask->setReadOnly(true);
    _width      = new QtToolbox::LineEdit(this, "Width", false, "100", 70);
        _width->setValidator(0, 10000);
    _height     = new QtToolbox::LineEdit(this, "Height", false, "100", 70);
    _height->setValidator(0, 10000);
    _angle            = new QtToolbox::LineEdit(this, "Angle", false, "0", 70);
        _angle->setValidator(0, Core::rad2deg(d_PI_MUL_2));
    _contrast         = new QtToolbox::SingleSlidingDouble(this, "Contrast", -1, 1);
    _normalStrength   = new QtToolbox::SingleSlidingDouble(this, "normal strength", 0, 3);
    _useDisplacementMap = new QCheckBox("displacement map", this);
    _color            = new QtToolbox::QuickColorPicker(this, "Color");

    _groupBox->getLayout()->addWidget(_diffuse);
    _groupBox->getLayout()->addWidget(_useDisplacementMap);
    _groupBox->getLayout()->addWidget(_normal);
    _groupBox->getLayout()->addWidget(_normalStrength);
    _groupBox->getLayout()->addWidget(_mask);
    _groupBox->getLayout()->addWidget(_width);
    _groupBox->getLayout()->addWidget(_height);
    _groupBox->getLayout()->addWidget(_angle);
    _groupBox->getLayout()->addWidget(_contrast);
    _groupBox->getLayout()->addWidget(_color);

    _layout->addWidget(_groupBox);

    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_diffuse);
    getWidgetsForUndoRedo().push_back(_normal);
    getWidgetsForUndoRedo().push_back(_mask);
    getWidgetsForUndoRedo().push_back(_width);
    getWidgetsForUndoRedo().push_back(_height);
    getWidgetsForUndoRedo().push_back(_angle);
    getWidgetsForUndoRedo().push_back(_contrast);
    getWidgetsForUndoRedo().push_back(_color);
    getWidgetsForUndoRedo().push_back(_useDisplacementMap);
    getWidgetsForUndoRedo().push_back(_normalStrength);
    

    PropertyWidget::setupUi();
}

void PtyWidgetTerrainLayer::setProperty(const Ptr<PtyTerrainLayer> & pPtyTerrainLayer)
{
    getDataProxy()->setProperty(pPtyTerrainLayer);


    /*
    setConnectToCommitPropertyChange(false);
    readProperty();
    setConnectToCommitPropertyChange(true);*/


    updatePropertyWidget();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI