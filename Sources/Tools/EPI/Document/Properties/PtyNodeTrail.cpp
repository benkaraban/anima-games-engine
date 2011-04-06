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
#include "PtyNodeTrail.h"


#include <QGridLayout>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <EPI/GUI/Widget/CustomLine.moc.h>
#include <Renderer/IFreeForm.h>
#include <EPI/Document/ContentInfo/TrailRepresentation.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeTrail::PtyNodeTrail( const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeTrail>& pNodeTrail, 
                            const Core::String& title)
:   PtyNode(pWorld, pNodeTrail, title),
    _pWorldForRepresentation(pWorldForRepresentation)
{
    _pRepresentation = Ptr<IContentRepresentation>(new TrailRepresentation(_pWorldForRepresentation, getUniverseNode()));
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeTrail::PtyNodeTrail(const PtyNodeTrail& other):
    PtyNode(other)
{
    _pWorldForRepresentation = null;
    _pRepresentation = null;

    _mode            = other._mode;
    _trailMode       = other._trailMode;
    _textureName     = other._textureName;
    _length          = other._length;
    _headWidth       = other._headWidth;
    _tailWidth       = other._tailWidth;
    _headU           = other._headU;
    _tailU           = other._tailU;
    _headColor       = other._headColor;
    _tailColor       = other._tailColor;
    _headGlow        = other._headGlow;
    _tailGlow        = other._tailGlow;
    _divisions       = other._divisions;

}
//-----------------------------------------------------------------------------
PtyNodeTrail::~PtyNodeTrail()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeTrail::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodeTrail> pPW (new PtyWidgetNodeTrail(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodeTrail::updateProperty()
{
    Ptr<Universe::NodeTrail>  pNodeTrail = LM_DEBUG_PTR_CAST<Universe::NodeTrail>(getUniverseNode());

    _mode            = pNodeTrail->getMode();
    _trailMode       = pNodeTrail->getTrailMode();
    _textureName     = pNodeTrail->getTexture();
    _length          = pNodeTrail->getLength();
    _headWidth       = pNodeTrail->getHeadWidth();
    _tailWidth       = pNodeTrail->getTailWidth();
    _headU           = pNodeTrail->getHeadU();
    _tailU           = pNodeTrail->getTailU();
    _headColor       = pNodeTrail->getHeadColor();
    _tailColor       = pNodeTrail->getTailColor();
    _headGlow        = pNodeTrail->getHeadGlow();
    _tailGlow        = pNodeTrail->getTailGlow();
    _divisions       = pNodeTrail->getDivisions();

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
void PtyNodeTrail::updateData()
{
    PtyNode::updateData();

    Ptr<Universe::NodeTrail>  pNodeTrail = LM_DEBUG_PTR_CAST<Universe::NodeTrail>(getUniverseNode());

    pNodeTrail->setMode(_mode);
    pNodeTrail->setTrailMode(_trailMode);
    pNodeTrail->setTexture(_textureName);
    pNodeTrail->setLength(_length);
    pNodeTrail->setHeadWidth(_headWidth);
    pNodeTrail->setTailWidth(_tailWidth);
    pNodeTrail->setHeadU(_headU);
    pNodeTrail->setTailU(_tailU);
    pNodeTrail->setHeadColor(_headColor);
    pNodeTrail->setTailColor(_tailColor);
    pNodeTrail->setHeadGlow(_headGlow);
    pNodeTrail->setTailGlow(_tailGlow);
    pNodeTrail->setDivisions(_divisions);

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeTrail::clone() const
{
    Ptr<PtyNodeTrail> ret (new PtyNodeTrail( *this ));

    return ret;
}
//-----------------------------------------------------------------------------
void PtyNodeTrail::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeTrail> pPty = LM_DEBUG_PTR_CAST<PtyNodeTrail>(pSrc);

    _mode            = pPty->_mode;
    _trailMode       = pPty->_trailMode;
    _textureName     = pPty->_textureName;
    _length          = pPty->_length;
    _headWidth       = pPty->_headWidth;
    _tailWidth       = pPty->_tailWidth;
    _headU           = pPty->_headU;
    _tailU           = pPty->_tailU;
    _headColor       = pPty->_headColor;
    _tailColor       = pPty->_tailColor;
    _headGlow        = pPty->_headGlow;
    _tailGlow        = pPty->_tailGlow;
    _divisions       = pPty->_divisions;

    updateData();
}
//-----------------------------------------------------------------------------
bool PtyNodeTrail::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (_pRepresentation->isItMe(pNode) == true)
        return true;
    else
        return PtyNode::isItMe(pNode);
}
//-----------------------------------------------------------------------------
bool PtyNodeTrail::isIntersecting  (const Core::Rayf& ray, float& distance)
{
    bool flag = PtyNode::isIntersecting(ray, distance);

    if (flag == false)
    {
        flag = _pRepresentation->isIntersecting(ray, distance);
    }

    return flag;
}
//-----------------------------------------------------------------------------
void PtyNodeTrail::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PtyNode::internalResurrect(pWorld, pWorldInfoContent, pty);
    getUniverseNode()->restart(false);
    _pWorldForRepresentation = pWorldInfoContent;

    LM_ASSERT(_pRepresentation==null);

    _pRepresentation = Ptr<IContentRepresentation>(new TrailRepresentation(_pWorldForRepresentation, getUniverseNode()));

    updateProperty();
}
//-----------------------------------------------------------------------------
void PtyNodeTrail::setVisible(bool flag)
{
    PtyNode::setVisible(flag);
    _pRepresentation->setVisible(flag);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeTrail::PtyWidgetNodeTrail(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeTrail::~PtyWidgetNodeTrail()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeTrail::readProperty()
{
    Ptr<PtyNodeTrail> pP = LM_DEBUG_PTR_CAST<PtyNodeTrail>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();


    _mode->selectIndex(pP->_mode);
    _trailMode->selectIndex(pP->_trailMode);
    _textureName->setText(pP->_textureName);
    _length->setSingleValue(pP->_length);
    _headWidth->setSingleValue(pP->_headWidth);
    _tailWidth->setSingleValue(pP->_tailWidth);
    _headU->setSingleValue(pP->_headU);
    _tailU->setSingleValue(pP->_tailU);
    _headColor->setColorLinear(pP->_headColor);
    _tailColor->setColorLinear(pP->_tailColor);
    _headGlow->setColorLinear(pP->_headGlow);
    _tailGlow->setColorLinear(pP->_tailGlow);
    _divisions->setSingleValue(pP->_divisions);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeTrail::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeTrail> pP = LM_DEBUG_PTR_CAST<PtyNodeTrail>(getDataProxy()->getProperty());

    pP->_mode = (Renderer::EFreeFormMode)_mode->selectedIndex();
    pP->_trailMode = (Renderer::ETrailMode)_trailMode->selectedIndex();
    _textureName->getText(pP->_textureName);
    _length->getSingleValue(pP->_length);
    _headWidth->getSingleValue(pP->_headWidth);
    _tailWidth->getSingleValue(pP->_tailWidth);
    _headU->getSingleValue(pP->_headU);
    _tailU->getSingleValue(pP->_tailU);
    _headColor->getColorLinear(pP->_headColor);
    _tailColor->getColorLinear(pP->_tailColor);
    _headGlow->getColorLinear(pP->_headGlow);
    _tailGlow->getColorLinear(pP->_tailGlow);
    _divisions->getSingleValue(pP->_divisions);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeTrail::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);


    _groupBox       = new QtToolbox::CollapsibleWidget(this, "Trail", false);
    _groupBox->setStyle(QtToolbox::CW_TITLE_1);
    _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupBoxGeometry = new QtToolbox::CollapsibleWidget(this, "Geometry");
    _groupBoxMaterial = new QtToolbox::CollapsibleWidget(this, "Material");

    _mode = new QtToolbox::ComboBox(this, "mode");
        ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE (_mode, Renderer, Renderer::EFreeFormMode, 10);

    _trailMode = new QtToolbox::ComboBox(this, "trail mode");
        ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE (_trailMode, Renderer, Renderer::ETrailMode, 6);

    _textureName    = new CustomLine(this, "Texture name");
        _textureName->pushAuthorizedDropMimeData("asset/texture");
        _textureName->setReadOnly(true);

    _length    = new QtToolbox::SingleSlidingHDR(this, "Length", false);
    _headWidth = new QtToolbox::SingleSlidingHDR(this, "Head width", false);
    _tailWidth = new QtToolbox::SingleSlidingHDR(this, "Tail width", false);
    _divisions = new QtToolbox::SingleSlidingInt(this, "Division", 1, 100);
 
    _headU = new QtToolbox::SingleSlidingHDR(this, "Head U", false);
    _tailU = new QtToolbox::SingleSlidingHDR(this, "Tail U", false);
    _headColor = new QtToolbox::QuickColorPicker(this, "Head color", Qt::white);
    _tailColor = new QtToolbox::QuickColorPicker(this, "Tail color", Qt::white);
    _headGlow = new QtToolbox::QuickColorPicker(this, "Head glow", Qt::white);
    _tailGlow = new QtToolbox::QuickColorPicker(this, "Tail glow", Qt::white);
    


    _groupBoxGeometry->getLayout()->addWidget(_length);
    _groupBoxGeometry->getLayout()->addWidget(_headWidth);
    _groupBoxGeometry->getLayout()->addWidget(_tailWidth);
    _groupBoxGeometry->getLayout()->addWidget(_divisions);

    _groupBoxMaterial->getLayout()->addWidget(_mode);
    _groupBoxMaterial->getLayout()->addWidget(_trailMode);
    _groupBoxMaterial->getLayout()->addWidget(_textureName);
    _groupBoxMaterial->getLayout()->addWidget(_headU);
    _groupBoxMaterial->getLayout()->addWidget(_tailU);
    _groupBoxMaterial->getLayout()->addWidget(_headColor);
    _groupBoxMaterial->getLayout()->addWidget(_tailColor);
    _groupBoxMaterial->getLayout()->addWidget(_headGlow);
    _groupBoxMaterial->getLayout()->addWidget(_tailGlow);

    _groupBox->getLayout()->addWidget(_pPWNode.get());
    _groupBox->getLayout()->addWidget(_groupBoxGeometry);
    _groupBox->getLayout()->addWidget(_groupBoxMaterial);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_mode);
    getWidgetsForUndoRedo().push_back(_trailMode);
    getWidgetsForUndoRedo().push_back(_textureName);
    getWidgetsForUndoRedo().push_back(_length);
    getWidgetsForUndoRedo().push_back(_headWidth);
    getWidgetsForUndoRedo().push_back(_tailWidth);
    getWidgetsForUndoRedo().push_back(_headU);
    getWidgetsForUndoRedo().push_back(_tailU);
    getWidgetsForUndoRedo().push_back(_headColor);
    getWidgetsForUndoRedo().push_back(_tailColor);
    getWidgetsForUndoRedo().push_back(_headGlow);
    getWidgetsForUndoRedo().push_back(_tailGlow);
    getWidgetsForUndoRedo().push_back(_divisions);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI