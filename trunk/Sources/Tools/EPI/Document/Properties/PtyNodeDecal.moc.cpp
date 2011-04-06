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
#include "PtyNodeDecal.moc.h"

#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{

Core::String EDecalModeToString(Renderer::EDecalMode mode)
{
    String ret;

    switch(mode)
    {
    case Renderer::DECAL_ADD:
        ret = L"ADD";
        break;
    case Renderer::DECAL_LERP:
        ret = L"LERP";
        break;
    case Renderer::DECAL_REFRAC:
        ret = L"REFRAC";
        break;
    }

    return ret;
}

Renderer::EDecalMode StringToEDecalMode(const String& str)
{
    Renderer::EDecalMode ret = Renderer::DECAL_ADD;

    if(str == EDecalModeToString(Renderer::DECAL_LERP))
        ret = Renderer::DECAL_LERP;
    else if(str == EDecalModeToString(Renderer::DECAL_ADD))
        ret = Renderer::DECAL_ADD;
    else if(str == EDecalModeToString(Renderer::DECAL_REFRAC))
        ret = Renderer::DECAL_REFRAC;

    return ret;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeDecal::PtyNodeDecal( const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeDecal>& pNodeDecal, 
                            const Core::String& title)
:   PtyNode(pWorld, pNodeDecal, title),
    _textureName(L""),
    _decalMode(Renderer::DECAL_ADD),
    _width(0.0f),
    _height(0.0f),
    _color(1.0f, 1.0f, 1.0f, 0.0f),
    _uSpriteLayout(1),
    _vSpriteLayout(1),
    _glow(1.0f, 1.0f, 1.0f, 0.0f),
    _zBias(0.0f),
    _fogFactor(0.0f),
    _duration(1.0f),
    _keyFrom(0.0f),
    _keyTo(1.0f),
    _loop(true)
{
    setVisibleWidgetTransform(true);
    setVisibleWidgetTag(true);

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeDecal::~PtyNodeDecal()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeDecal::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodeDecal> pPW (new PtyWidgetNodeDecal(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodeDecal::updateProperty()
{
    Ptr<Universe::NodeDecal>  pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(getUniverseNode());

    _textureName    = pNodeDecal->getTexture();
    _decalMode      = pNodeDecal->getMode();
    _uSpriteLayout  = pNodeDecal->getUSpriteLayout();
    _vSpriteLayout  = pNodeDecal->getVSpriteLayout();
    
    _width          = pNodeDecal->getWidth();
    _height         = pNodeDecal->getHeight();

    _color          = pNodeDecal->getColor();
    _glow           = pNodeDecal->getGlow();

    _zBias          = pNodeDecal->getZBias();
    _fogFactor      = pNodeDecal->getFogFactor();

    _duration       = pNodeDecal->getDuration();
    _keyFrom        = pNodeDecal->getKeyFrom();
    _keyTo          = pNodeDecal->getKeyTo();
    _loop           = pNodeDecal->getLoop();
}
//-----------------------------------------------------------------------------
void PtyNodeDecal::updateData()
{
    PtyNode::updateData();

    Ptr<Universe::NodeDecal>  pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(getUniverseNode());

    pNodeDecal->setTexture(_textureName);
    pNodeDecal->setMode(_decalMode);
    pNodeDecal->setSpriteLayout(_uSpriteLayout, _vSpriteLayout);
    pNodeDecal->setSize(_width, _height);

    pNodeDecal->setColor(_color);
    pNodeDecal->setGlow(_glow);

    pNodeDecal->setZBias(_zBias);
    pNodeDecal->setFogFactor(_fogFactor);

    pNodeDecal->animateSprite(_duration, _keyFrom, _keyTo, _loop);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeDecal::clone() const
{
    return Ptr<Property>(new PtyNodeDecal( *this ));
}
//-----------------------------------------------------------------------------
void PtyNodeDecal::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeDecal> pPty = LM_DEBUG_PTR_CAST<PtyNodeDecal>(pSrc);

    _textureName    = pPty->_textureName;
    _decalMode      = pPty->_decalMode;
    _uSpriteLayout  = pPty->_uSpriteLayout;
    _vSpriteLayout  = pPty->_vSpriteLayout;
                      
    _width          = pPty->_width;
    _height         = pPty->_height;
                      
    _color          = pPty->_color;
    _glow           = pPty->_glow;

    _zBias          = pPty->_zBias;
    _fogFactor      = pPty->_fogFactor;
                      
    _duration       = pPty->_duration;
    _keyFrom        = pPty->_keyFrom;
    _keyTo          = pPty->_keyTo;
    _loop           = pPty->_loop;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeDecal::PtyWidgetNodeDecal(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeDecal::~PtyWidgetNodeDecal()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeDecal::readProperty()
{
    Ptr<PtyNodeDecal> pP = LM_DEBUG_PTR_CAST<PtyNodeDecal>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();

    _textureName->setText(pP->_textureName);
    _decalMode->selectItem(Core::String8(EDecalModeToString(pP->_decalMode)).c_str());
    
    _uSpriteLayout->setText(Core::String8(Core::toString(pP->_uSpriteLayout)).c_str());
    _vSpriteLayout->setText(Core::String8(Core::toString(pP->_vSpriteLayout)).c_str());

    _width->setSingleValue(pP->_width);
    _height->setSingleValue(pP->_height);

    _color->setColorLinear(pP->_color);
    _glow->setColorLinear(pP->_glow);

    _zBias->setSingleValue(pP->_zBias);
    _fogFactor->setSingleValue(pP->_fogFactor);
    
    _keys->setRangedValue(pP->_keyFrom, pP->_keyTo);
    _duration->setSingleValue(pP->_duration);
    _loop->setChecked(pP->_loop);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeDecal::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeDecal> pP = LM_DEBUG_PTR_CAST<PtyNodeDecal>(getDataProxy()->getProperty());

    _textureName->getText(pP->_textureName);
    pP->_decalMode = StringToEDecalMode(_decalMode->selectedText());
    
    Core::String uSpriteLayout(_uSpriteLayout->text().toStdString().c_str());
    pP->_uSpriteLayout = Core::toInteger(uSpriteLayout);
    Core::String vSpriteLayout(_vSpriteLayout->text().toStdString().c_str());
    pP->_vSpriteLayout = Core::toInteger(vSpriteLayout);

    _color->getColorLinear(pP->_color);
    _glow->getColorLinear(pP->_glow);

    _zBias->getSingleValue(pP->_zBias);
    _fogFactor->getSingleValue(pP->_fogFactor);

    _width->getSingleValue(pP->_width);
    _height->getSingleValue(pP->_height);
    _keys->getRangedValue(pP->_keyFrom, pP->_keyTo);
    _duration->getSingleValue(pP->_duration);
    pP->_loop = _loop->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeDecal::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);


    _groupBox       = new QtToolbox::CollapsibleWidget(this, "Decal", false);
    _groupBox->setStyle(QtToolbox::CW_TITLE_1);
    _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupBox->getLayout()->addWidget(_pPWNode.get());

    _groupTexture   = new QtToolbox::CollapsibleWidget(this, "Texture");
    _textureName    = new CustomLine(this, "Texture name");
    _textureName->pushAuthorizedDropMimeData("asset/texture");
    _textureName->setReadOnly(true);
    
    _decalMode  = new QtToolbox::ComboBox(this, "Mode");
    _decalMode->addItem(Core::String8(EDecalModeToString(Renderer::DECAL_ADD)).c_str());
    _decalMode->addItem(Core::String8(EDecalModeToString(Renderer::DECAL_LERP)).c_str());
    _decalMode->addItem(Core::String8(EDecalModeToString(Renderer::DECAL_REFRAC)).c_str());

    _uSpriteLayout = new QtToolbox::LineEdit(this, "U sprite layout", false, "1", 70);
    _uSpriteLayout->setValidator(1, 100);
    _vSpriteLayout = new QtToolbox::LineEdit(this, "V sprite layout", false, "1", 70);
    _vSpriteLayout->setValidator(1, 100);

    _groupTexture->getLayout()->addWidget(_textureName);
    _groupTexture->getLayout()->addWidget(_decalMode);
    _groupTexture->getLayout()->addWidget(_uSpriteLayout);
    _groupTexture->getLayout()->addWidget(_vSpriteLayout);

    _groupColors    = new QtToolbox::CollapsibleWidget(this, "Colors");
    _color          = new QtToolbox::QuickColorPicker(this, "Color", Qt::white);
    _glow           = new QtToolbox::QuickColorPicker(this, "Glow", Qt::white);
    _groupColors->getLayout()->addWidget(_color);
    _groupColors->getLayout()->addWidget(_glow);
    _groupColors->collapse(true);

    _groupAnimation = new QtToolbox::CollapsibleWidget(this, "Animation");
    _width          = new QtToolbox::SingleSlidingHDR(this, "Width", false);
    _height         = new QtToolbox::SingleSlidingHDR(this, "Height", false);

    _zBias          = new QtToolbox::SingleSlidingHDR(this, "Z Bias", true);
    _fogFactor      = new QtToolbox::SingleSlidingDouble(this, "Fog factor", 0.0, 1.0);

    _keys           = new QtToolbox::RangedSlidingDouble(this, "Keys", 0.0, 1.0);
    _duration       = new QtToolbox::SingleSlidingDouble(this, "Duration", 0.0, 10.0);
    _loop           = new QCheckBox("Loop", this);
    
    _groupAnimation->getLayout()->addWidget(_width);
    _groupAnimation->getLayout()->addWidget(_height);
    _groupAnimation->getLayout()->addWidget(_keys);
    _groupAnimation->getLayout()->addWidget(_zBias);
    _groupAnimation->getLayout()->addWidget(_fogFactor);
    _groupAnimation->getLayout()->addWidget(_duration);
    _groupAnimation->getLayout()->addWidget(_loop);
    _groupAnimation->getLayout()->setAlignment(Qt::AlignCenter);

    _groupBox->getLayout()->addWidget(_groupTexture);
    _groupBox->getLayout()->addWidget(_groupColors);
    _groupBox->getLayout()->addWidget(_groupAnimation);        

    _layout->addWidget(_groupBox);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_textureName);
    getWidgetsForUndoRedo().push_back(_decalMode);
    getWidgetsForUndoRedo().push_back(_uSpriteLayout);
    getWidgetsForUndoRedo().push_back(_vSpriteLayout);
    getWidgetsForUndoRedo().push_back(_color);
    getWidgetsForUndoRedo().push_back(_glow);
    getWidgetsForUndoRedo().push_back(_width);
    getWidgetsForUndoRedo().push_back(_height);
    getWidgetsForUndoRedo().push_back(_zBias);
    getWidgetsForUndoRedo().push_back(_fogFactor);
    getWidgetsForUndoRedo().push_back(_keys);
    getWidgetsForUndoRedo().push_back(_duration);
    getWidgetsForUndoRedo().push_back(_loop);

    PropertyWidget::setupUi();

    connect(_groupColors, SIGNAL(newSize(int, int)), this, SLOT(applyNewSize()));
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeDecal::applyNewSize()
{
    _groupBox->applyNewSize();
    setFixedWidth(_groupBox->width());
    setFixedHeight(_groupBox->height());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI