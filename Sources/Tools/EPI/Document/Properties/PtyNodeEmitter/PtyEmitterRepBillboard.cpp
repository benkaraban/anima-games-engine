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
#include "PtyEmitterRepBillboard.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>

#include <QCheckBox>
//#include <QPushButton>
//#include <QHBoxLayout>
//
//#include <QtToolbox/CollapsibleWidget.moc.h>
//#include <QtToolbox/RangedSlidingTimeValue.h>
//#include <QtToolbox/RangedSlidingValue.moc.h>
//#include <QtToolbox/RangedSlidingHDR.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>
//#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
Core::String EBillboardModeToString(Renderer::EBillboardMode mode)
{
    String ret;

    switch(mode)
    {
    case Renderer::BILLBOARD_LERP:
        ret = L"LERP";
        break;
    case Renderer::BILLBOARD_ADD:
        ret = L"ADD";
        break;
    case Renderer::BILLBOARD_ADD_LERP:
        ret = L"ADD_LERP";
        break;
    case Renderer::BILLBOARD_REFRAC:
        ret = L"REFRAC";
        break;
    }

    return ret;
}

String EBillboardAlignToString(Renderer::EBillboardAlign align)
{
    String ret;

    switch(align)
    {
    case Renderer::BILLBOARD_ALIGN_FACE_CAM:
        ret = L"Face camera";
        break;
    case Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_VEC:
        ret = L"Horizontal align";
        break;
    case Renderer::BILLBOARD_ALIGN_VERTICAL_ON_VEC:
        ret = L"Vertical align";
        break;
    case Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_DIR:
        ret = L"H. direction align";
        break;
    case Renderer::BILLBOARD_ALIGN_VERTICAL_ON_DIR:
        ret = L"V. direction align";
        break;
    }

    return ret;
}

Renderer::EBillboardMode StringToEBillboardMode(const String& str)
{
    Renderer::EBillboardMode ret = Renderer::BILLBOARD_ADD;

    if(str == EBillboardModeToString(Renderer::BILLBOARD_LERP))
        ret = Renderer::BILLBOARD_LERP;
    else if(str == EBillboardModeToString(Renderer::BILLBOARD_ADD))
        ret = Renderer::BILLBOARD_ADD;
    else if(str == EBillboardModeToString(Renderer::BILLBOARD_ADD_LERP))
        ret = Renderer::BILLBOARD_ADD_LERP;
    else if(str == EBillboardModeToString(Renderer::BILLBOARD_REFRAC))
        ret = Renderer::BILLBOARD_REFRAC;

    return ret;
}
//-----------------------------------------------------------------------------
PtyEmitterRepBillboard::PtyEmitterRepBillboard(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::PartRepresentationBillboard>& pRep,
    const Core::String& title):
    PtyEmitterRep(pNodeE, pRep, title),
    _textureName(L""),
    _billboardMode(Renderer::BILLBOARD_ADD),
    _billboardAlign(Renderer::BILLBOARD_ALIGN_FACE_CAM),
    _alignVector(0.0f, 1.0f, 0.0f),
    _uSpriteLayout(1),
    _vSpriteLayout(1),
    _spriteSmooth(false),
    _color1(1.0f, 1.0f, 1.0f, 0.0f),
    _color2(1.0f, 1.0f, 1.0f, 0.0f),
    _minWidth(0.0f), 
    _maxWidth(0.0f),
    _minHeight(0.0f),
    _maxHeight(0.0f),
    _minAngle(0.0f),
    _maxAngle(0.0f),
    _minT(0.0f),
    _maxT(0.0f),
    _colorFactor(0.0f),
    _glowFactor(0.0f),
    _squareParts(false),
    _normSize(false),
    _zBias(0.0f),
    _fogFactor(0.0f)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterRepBillboard::PtyEmitterRepBillboard(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Core::String& title):
    PtyEmitterRep(pNodeE, null, title)
{
    _pRep = Ptr<Universe::PartRepresentationBillboard>(new Universe::PartRepresentationBillboard(*pNodeE->getWorld()));
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterRepBillboard::PtyEmitterRepBillboard(const PtyEmitterRepBillboard & other)
    : PtyEmitterRep(other.getUniverseNodeEmitter(), null, L"Representation billboard")
{
    _pRep = null;

    _textureName    = other._textureName;
    _billboardMode  = other._billboardMode;
    _billboardAlign = other._billboardAlign;
    _alignVector    = other._alignVector;
    _uSpriteLayout  = other._uSpriteLayout;
    _vSpriteLayout  = other._vSpriteLayout;
    _spriteSmooth   = other._spriteSmooth;
    _color1         = other._color1;
    _color2         = other._color2;
    _minWidth       = other._minWidth;
    _maxWidth       = other._maxWidth;
    _minHeight      = other._minHeight;
    _maxHeight      = other._maxHeight;
    _minAngle       = other._minAngle;
    _maxAngle       = other._maxAngle;
    _minT           = other._minT;
    _maxT           = other._maxT;
    _colorFactor    = other._colorFactor;
    _glowFactor     = other._glowFactor;
    _squareParts    = other._squareParts;
    _normSize       = other._normSize;
    _zBias          = other._zBias;
    _fogFactor      = other._fogFactor;
}
//-----------------------------------------------------------------------------
PtyEmitterRepBillboard::~PtyEmitterRepBillboard()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyEmitterRepBillboard::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetEmitterRepBillboard> pPW (new PtyWidgetEmitterRepBillboard(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyEmitterRepBillboard::updateData()
{
    Ptr<Universe::PartRepresentationBillboard> pRepBill = LM_DEBUG_PTR_CAST<Universe::PartRepresentationBillboard>(getRepresentation());

    pRepBill->setTexture(_textureName);
    pRepBill->setMode(_billboardMode);
    pRepBill->setAlign(_billboardAlign);
    pRepBill->setAlignVector(_alignVector);
    pRepBill->setSpriteLayout(_uSpriteLayout, _vSpriteLayout);
    pRepBill->setSpriteSmooth(_spriteSmooth);

    _pNodeE->setStartColor(_color1, _color2);
    _pNodeE->setStartWidth(_minWidth, _maxWidth);
    _pNodeE->setStartHeight(_minHeight, _maxHeight);
    _pNodeE->setStartAngle(_minAngle, _maxAngle);
    _pNodeE->setStartTimeKey(_minT, _maxT);
    _pNodeE->setSquareParticles(_squareParts);
    _pNodeE->setNormalizedSize(_normSize);
    _pNodeE->setColorFactor(_colorFactor);
    _pNodeE->setGlowFactor(_glowFactor);
    _pNodeE->setZBias(_zBias);
    _pNodeE->setFogFactor(_fogFactor);
    _pNodeE->setStartColor(_color1, _color2);
}
//-----------------------------------------------------------------------------
void PtyEmitterRepBillboard::updateProperty()
{
    Ptr<Universe::PartRepresentationBillboard> pRepBill = LM_DEBUG_PTR_CAST<Universe::PartRepresentationBillboard>(getRepresentation());

    _textureName = pRepBill->getTexture();
    _billboardMode = pRepBill->getMode();
    _billboardAlign = pRepBill->getAlign();
    _alignVector = pRepBill->getAlignVector();
    _uSpriteLayout = pRepBill->getUSpriteLayout();
    _vSpriteLayout = pRepBill->getVSpriteLayout();
    _spriteSmooth = pRepBill->isSpriteSmooth();

    _pNodeE->getStartColor    (_color1,    _color2);
    _pNodeE->getStartWidth    (_minWidth,  _maxWidth);
    _pNodeE->getStartHeight   (_minHeight, _maxHeight);
    _pNodeE->getStartAngle    (_minAngle,  _maxAngle);
    _pNodeE->getStartTimeKey  (_minT,      _maxT);
    _squareParts = _pNodeE->getSquareParticles();
    _normSize    = _pNodeE->getNormalizedSize();
    _colorFactor = _pNodeE->getColorFactor();
    _glowFactor  = _pNodeE->getGlowFactor();
    _zBias       = _pNodeE->getZBias();
    _fogFactor   = _pNodeE->getFogFactor();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyEmitterRepBillboard::clone() const
{
    return Ptr<Property>(new PtyEmitterRepBillboard( *this ));
}
//-----------------------------------------------------------------------------
void PtyEmitterRepBillboard::internalCopy(const Ptr<Property>& pSrc)
{
    PtyEmitterRep::internalCopy(pSrc);
    Ptr<PtyEmitterRepBillboard> pPty = LM_DEBUG_PTR_CAST<PtyEmitterRepBillboard>(pSrc);

    _textureName    = pPty->_textureName;
    _billboardMode  = pPty->_billboardMode;
    _billboardAlign = pPty->_billboardAlign;
    _alignVector    = pPty->_alignVector;
    _uSpriteLayout  = pPty->_uSpriteLayout;
    _vSpriteLayout  = pPty->_vSpriteLayout;
    _spriteSmooth   = pPty->_spriteSmooth;
    _color1         = pPty->_color1;
    _color2         = pPty->_color2;
    _minWidth       = pPty->_minWidth;
    _maxWidth       = pPty->_maxWidth;
    _minHeight      = pPty->_minHeight;
    _maxHeight      = pPty->_maxHeight;
    _minAngle       = pPty->_minAngle;
    _maxAngle       = pPty->_maxAngle;
    _minT           = pPty->_minT;
    _maxT           = pPty->_maxT;
    _colorFactor    = pPty->_colorFactor;
    _glowFactor     = pPty->_glowFactor;
    _squareParts    = pPty->_squareParts;
    _normSize       = pPty->_normSize;
    _zBias          = pPty->_zBias;
    _fogFactor      = pPty->_fogFactor;
    
    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterRepBillboard::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getRepresentation() == null);

    Ptr<Universe::PartRepresentationBillboard> pRep = Ptr<Universe::PartRepresentationBillboard>(new Universe::PartRepresentationBillboard(*pWorld));
    
    setRepresentation(pRep);
    getUniverseNodeEmitter()->setRepresentation(getRepresentation());

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetEmitterRepBillboard::PtyWidgetEmitterRepBillboard(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetEmitterRepBillboard::~PtyWidgetEmitterRepBillboard()
{}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterRepBillboard::readProperty()
{
    Ptr<PtyEmitterRepBillboard> pP = LM_DEBUG_PTR_CAST<PtyEmitterRepBillboard>(getDataProxy()->getProperty());

    _textureName->setText(pP->_textureName);
    _billboardMode->selectItem(Core::String8(EBillboardModeToString(pP->_billboardMode)).c_str());
    _billboardAlign->selectIndex(int32(pP->_billboardAlign));
    _alignVectorX->setSingleValue(pP->_alignVector.x);
    _alignVectorY->setSingleValue(pP->_alignVector.y);
    _alignVectorZ->setSingleValue(pP->_alignVector.z);
    _uSpriteLayout->setText(Core::String8(Core::toString(pP->_uSpriteLayout)).c_str());
    _vSpriteLayout->setText(Core::String8(Core::toString(pP->_vSpriteLayout)).c_str());
    _spriteSmooth->setChecked(pP->_spriteSmooth);
    _color1->setColorLinear(pP->_color1);
    _color2->setColorLinear(pP->_color2);
    _width->setRangedValue(pP->_minWidth, pP->_maxWidth);
    _height->setRangedValue(pP->_minHeight, pP->_maxHeight);
    _angle->setRangedValue(Core::rad2deg(pP->_minAngle), Core::rad2deg(pP->_maxAngle));
    _time->setRangedValue(pP->_minT, pP->_maxT);
    _colorFactor->setSingleValue(pP->_colorFactor);
    _glowFactor->setSingleValue(pP->_glowFactor);
    _squareParts->setChecked(pP->_squareParts);
    _normSize->setChecked(pP->_normSize);
    _zBias->setSingleValue(pP->_zBias);
    _fogFactor->setSingleValue(pP->_fogFactor);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterRepBillboard::writeProperty(QWidget* pWidget)
{
    Ptr<PtyEmitterRepBillboard> pP = LM_DEBUG_PTR_CAST<PtyEmitterRepBillboard>(getDataProxy()->getProperty());

    _textureName->getText(pP->_textureName);
    pP->_billboardMode = StringToEBillboardMode(_billboardMode->selectedText());
    pP->_billboardAlign = Renderer::EBillboardAlign(_billboardAlign->selectedIndex());
    _alignVectorX->getSingleValue(pP->_alignVector.x);
    _alignVectorY->getSingleValue(pP->_alignVector.y);
    _alignVectorZ->getSingleValue(pP->_alignVector.z);
    
    Core::String uSpriteLayout(_uSpriteLayout->text().toStdString().c_str());
    pP->_uSpriteLayout = Core::toInteger(uSpriteLayout);
    Core::String vSpriteLayout(_vSpriteLayout->text().toStdString().c_str());
    pP->_vSpriteLayout = Core::toInteger(vSpriteLayout);
    pP->_spriteSmooth = _spriteSmooth->isChecked();

    _color1->getColorLinear(pP->_color1);
    _color2->getColorLinear(pP->_color2);
    _width->getRangedValue(pP->_minWidth, pP->_maxWidth);
    _height->getRangedValue(pP->_minHeight, pP->_maxHeight);
    
    float minAngle, maxAngle;
    _angle->getRangedValue(minAngle, maxAngle);
    pP->_minAngle = Core::deg2rad(minAngle);
    pP->_maxAngle = Core::deg2rad(maxAngle);

    _time->getRangedValue(pP->_minT, pP->_maxT);
    _zBias->getSingleValue(pP->_zBias);
    _fogFactor->getSingleValue(pP->_fogFactor);
    _colorFactor->getSingleValue(pP->_colorFactor);
    _glowFactor->getSingleValue(pP->_glowFactor);

    pP->_squareParts = _squareParts->isChecked();
    pP->_normSize    = _normSize->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterRepBillboard::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _textureName    = new CustomLine(this, "Texture name");
    _textureName->pushAuthorizedDropMimeData("asset/texture");
    _textureName->setReadOnly(true);
    
    _billboardMode  = new QtToolbox::ComboBox(this, "Mode");
    _billboardMode->addItem(Core::String8(EBillboardModeToString(Renderer::BILLBOARD_ADD)).c_str());
    _billboardMode->addItem(Core::String8(EBillboardModeToString(Renderer::BILLBOARD_LERP)).c_str());
    _billboardMode->addItem(Core::String8(EBillboardModeToString(Renderer::BILLBOARD_ADD_LERP)).c_str());
    _billboardMode->addItem(Core::String8(EBillboardModeToString(Renderer::BILLBOARD_REFRAC)).c_str());

    _billboardAlign  = new QtToolbox::ComboBox(this, "Align");
    _billboardAlign->addItem(Core::String8(EBillboardAlignToString(Renderer::BILLBOARD_ALIGN_FACE_CAM)).c_str());
    _billboardAlign->addItem(Core::String8(EBillboardAlignToString(Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_VEC)).c_str());
    _billboardAlign->addItem(Core::String8(EBillboardAlignToString(Renderer::BILLBOARD_ALIGN_VERTICAL_ON_VEC)).c_str());
    _billboardAlign->addItem(Core::String8(EBillboardAlignToString(Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_DIR)).c_str());
    _billboardAlign->addItem(Core::String8(EBillboardAlignToString(Renderer::BILLBOARD_ALIGN_VERTICAL_ON_DIR)).c_str());

    _alignVectorX   = new QtToolbox::SingleSlidingDouble(this, "Align vector X", -1.0, 1.0);
    _alignVectorY   = new QtToolbox::SingleSlidingDouble(this, "Align vector Y", -1.0, 1.0);
    _alignVectorZ   = new QtToolbox::SingleSlidingDouble(this, "Align vector Z", -1.0, 1.0);

    _uSpriteLayout = new QtToolbox::LineEdit(this, "U sprite layout", false, "1", 70);
    _uSpriteLayout->setValidator(1, 100);
    _vSpriteLayout = new QtToolbox::LineEdit(this, "V sprite layout", false, "1", 70);
    _vSpriteLayout->setValidator(1, 100);
    _spriteSmooth   = new QCheckBox("Sprite smooth trans", this);

    _groupColors    = new QtToolbox::SmartGroupBox(this, "Colors", true);
    _color1         = new QtToolbox::QuickColorPicker(this, "Color1", Qt::white);
    _color2         = new QtToolbox::QuickColorPicker(this, "Color2", Qt::white);
    _groupColors->addWidget(_color1);
    _groupColors->addWidget(_color2);
    _groupColors->collapse(true);
    _width          = new QtToolbox::RangedSlidingHDR(this, "Width", false);
    _height         = new QtToolbox::RangedSlidingHDR(this, "Height", false);
    _angle          = new QtToolbox::RangedSlidingDouble(this, "Start angle", 0.0, Core::rad2deg(2 * d_PI));
    _time           = new QtToolbox::RangedSlidingDouble(this, "Time", 0.0, 1.0);
    _squareParts    = new QCheckBox("Square particles", this);
    _normSize       = new QCheckBox("Constant part size", this);
    _colorFactor    = new QtToolbox::SingleSlidingDouble(this, "Color factor", 0.0, 1.0);
    _glowFactor     = new QtToolbox::SingleSlidingDouble(this, "Glow factor", 0.0, 1.0);
    _zBias          = new QtToolbox::SingleSlidingHDR(this, "Z Bias", true);
    _fogFactor      = new QtToolbox::SingleSlidingDouble(this, "Fog factor", 0.0, 1.0);
  
    _layout->addWidget(_textureName);
    _layout->addWidget(_billboardMode);
    _layout->addWidget(_billboardAlign);
    _layout->addWidget(_alignVectorX);
    _layout->addWidget(_alignVectorY);
    _layout->addWidget(_alignVectorZ);
    _layout->addWidget(_uSpriteLayout);
    _layout->addWidget(_vSpriteLayout);
    _layout->addWidget(_spriteSmooth);
    _layout->addWidget(_groupColors);
    _layout->addWidget(_width);
    _layout->addWidget(_height);
    _layout->addWidget(_angle);
    _layout->addWidget(_time);
    _layout->addWidget(_squareParts);
    _layout->addWidget(_normSize);
    _layout->addWidget(_zBias);
    _layout->addWidget(_fogFactor);
    _layout->addWidget(_colorFactor);
    _layout->addWidget(_glowFactor);

    setLayout(_layout);
    readProperty();

    getWidgetsForUndoRedo().push_back(_textureName);
    getWidgetsForUndoRedo().push_back(_billboardMode);
    getWidgetsForUndoRedo().push_back(_billboardAlign);
    getWidgetsForUndoRedo().push_back(_alignVectorX);
    getWidgetsForUndoRedo().push_back(_alignVectorY);
    getWidgetsForUndoRedo().push_back(_alignVectorZ);
    getWidgetsForUndoRedo().push_back(_uSpriteLayout);
    getWidgetsForUndoRedo().push_back(_vSpriteLayout);
    getWidgetsForUndoRedo().push_back(_spriteSmooth);
    getWidgetsForUndoRedo().push_back(_color1);
    getWidgetsForUndoRedo().push_back(_color2);
    getWidgetsForUndoRedo().push_back(_width);
    getWidgetsForUndoRedo().push_back(_height);
    getWidgetsForUndoRedo().push_back(_angle);
    getWidgetsForUndoRedo().push_back(_time);
    getWidgetsForUndoRedo().push_back(_squareParts);
    getWidgetsForUndoRedo().push_back(_normSize);
    getWidgetsForUndoRedo().push_back(_zBias);
    getWidgetsForUndoRedo().push_back(_fogFactor);
    getWidgetsForUndoRedo().push_back(_colorFactor);
    getWidgetsForUndoRedo().push_back(_glowFactor);

    PropertyWidget::setupUi();

    connect(_groupColors, SIGNAL(newSize(int, int)), this, SLOT(applyNewSize()));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI