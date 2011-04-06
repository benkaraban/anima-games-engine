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
#include "PtyNodeStorm.moc.h"

#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>
#include <QtToolbox/RangedSlidingTimeValue.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
Core::String EFreeFormModeToString(Renderer::EFreeFormMode mode)
{
    String ret;

    switch(mode)
    {
    case Renderer::FREE_FORM_LERP:
        ret = L"LERP";
        break;
    case Renderer::FREE_FORM_ADD:
        ret = L"ADD";
        break;
    case Renderer::FREE_FORM_REFRAC:
        ret = L"REFRAC";
        break;
    }

    return ret;
}

Renderer::EFreeFormMode StringToEFreeFormMode(const String& str)
{
    Renderer::EFreeFormMode ret = Renderer::FREE_FORM_ADD;

    if(str == EFreeFormModeToString(Renderer::FREE_FORM_LERP))
        ret = Renderer::FREE_FORM_LERP;
    else if(str == EFreeFormModeToString(Renderer::FREE_FORM_ADD))
        ret = Renderer::FREE_FORM_ADD;
    else if(str == EFreeFormModeToString(Renderer::FREE_FORM_REFRAC))
        ret = Renderer::FREE_FORM_REFRAC;

    return ret;
}


//-----------------------------------------------------------------------------
PtyNodeStorm::PtyNodeStorm( const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeStorm>& pNodeStorm, 
                            const Core::String& title)
:   PtyNode(pWorld, pNodeStorm, title)
{
    setVisibleWidgetTransform(true);
    setVisibleWidgetTag(true);

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeStorm::~PtyNodeStorm()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeStorm::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    return Ptr<PtyWidgetNodeStorm>(new PtyWidgetNodeStorm(pDataProxy, parent));
}
//-----------------------------------------------------------------------------
void PtyNodeStorm::updateProperty()
{
    PtyNode::updateProperty();

    Ptr<Universe::NodeStorm>  pStorm = LM_DEBUG_PTR_CAST<Universe::NodeStorm>(getUniverseNode());

    pStorm->getPeriod(_minPeriod, _maxPeriod);
    pStorm->getGenerationTime(_startTime, _endTime);
    pStorm->getColor(_color1, _color2);
    pStorm->getGlow(_glow1, _glow2);

    _textureName    = pStorm->getTexture();
    _maxCount       = pStorm->getMaxCount();
    _burstStart     = pStorm->getBurstStart();
    _entropy        = pStorm->getEntropy();
    _fertility      = pStorm->getFertility();
    _resolution     = pStorm->getResolution();
    _fadeIn         = pStorm->getFadeInTime();
    _duration       = pStorm->getDuration();
    _fadeOut        = pStorm->getFadeOutTime();
    _headWidth      = pStorm->getHeadWidth();
    _tailWidth      = pStorm->getTailWidth();
    _headCenter     = pStorm->getHeadSphereCenter();
    _headSize       = pStorm->getHeadSphereSize();
    _tailCenter     = pStorm->getTailSphereCenter();
    _tailSize       = pStorm->getTailSphereSize();
    _mode           = pStorm->getMode();
    _travelTime     = pStorm->getTravelTime();
}
//-----------------------------------------------------------------------------
void PtyNodeStorm::updateData()
{
    PtyNode::updateData();

    Ptr<Universe::NodeStorm>  pStorm = LM_DEBUG_PTR_CAST<Universe::NodeStorm>(getUniverseNode());

    pStorm->setPeriod(_minPeriod, _maxPeriod);
    pStorm->setGenerationTime(_startTime, _endTime);
    pStorm->setBurstStart(_burstStart);
    pStorm->setTexture(_textureName);
    pStorm->setColor(_color1, _color2);
    pStorm->setGlow(_glow1, _glow2);
    pStorm->setMaxCount(_maxCount);
    pStorm->setEntropy(_entropy);
    pStorm->setFertility(_fertility);
    pStorm->setResolution(_resolution);
    pStorm->setFadeInTime(_fadeIn);
    pStorm->setDuration(_duration);
    pStorm->setFadeOutTime(_fadeOut);
    pStorm->setHeadWidth(_headWidth);
    pStorm->setTailWidth(_tailWidth);
    pStorm->setHeadSphereCenter(_headCenter);
    pStorm->setHeadSphereSize(_headSize);
    pStorm->setTailSphereCenter(_tailCenter);
    pStorm->setTailSphereSize(_tailSize);
    pStorm->setMode(_mode);
    pStorm->setTravelTime(_travelTime);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeStorm::clone() const
{
    return Ptr<Property>(new PtyNodeStorm(*this));
}
//-----------------------------------------------------------------------------
void PtyNodeStorm::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeStorm> pPty = LM_DEBUG_PTR_CAST<PtyNodeStorm>(pSrc);

    _minPeriod      = pPty->_minPeriod;
    _maxPeriod      = pPty->_maxPeriod;
    _startTime      = pPty->_startTime;
    _endTime        = pPty->_endTime;
    _burstStart     = pPty->_burstStart;
    _textureName    = pPty->_textureName;
    _color1         = pPty->_color1;
    _color2         = pPty->_color2;
    _glow1          = pPty->_glow1;
    _glow2          = pPty->_glow2;
    _maxCount       = pPty->_maxCount;
    _entropy        = pPty->_entropy;
    _fertility      = pPty->_fertility;
    _resolution     = pPty->_resolution;
    _fadeIn         = pPty->_fadeIn;
    _duration       = pPty->_duration;
    _fadeOut        = pPty->_fadeOut;
    _headWidth      = pPty->_headWidth;
    _tailWidth      = pPty->_tailWidth;
    _headCenter     = pPty->_headCenter;
    _headSize       = pPty->_headSize;
    _tailCenter     = pPty->_tailCenter;
    _tailSize       = pPty->_tailSize;
    _mode           = pPty->_mode;
    _travelTime     = pPty->_travelTime;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeStorm::PtyWidgetNodeStorm(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeStorm::~PtyWidgetNodeStorm()
{}
//-----------------------------------------------------------------------------
void PtyWidgetNodeStorm::restartStorm()
{
    Ptr<PtyNodeStorm> pPtyNodeEmitter = LM_DEBUG_PTR_CAST<PtyNodeStorm>(getDataProxy()->getProperty());
    Ptr<Universe::NodeStorm>  pStorm = LM_DEBUG_PTR_CAST<Universe::NodeStorm>(pPtyNodeEmitter->getUniverseNode()); 
    pStorm->restart();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeStorm::readProperty()
{
    Ptr<PtyNodeStorm> pP = LM_DEBUG_PTR_CAST<PtyNodeStorm>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();

    _period->setRangedValue(pP->_minPeriod, pP->_maxPeriod);
    _maxCount->setSingleValue(pP->_maxCount);
    _time->setRangedValue(pP->_startTime, pP->_endTime);
    _burstStart->setChecked(pP->_burstStart);
    _entropy->setSingleValue(pP->_entropy);
    _fertility->setSingleValue(pP->_fertility);
    _resolution->setSingleValue(pP->_resolution);
    _textureName->setText(pP->_textureName);
    _color1->setColorLinear(pP->_color1);
    _color2->setColorLinear(pP->_color2);
    _glow1->setColorLinear(pP->_glow1);
    _glow2->setColorLinear(pP->_glow2);
    _fadeIn->setSingleValue(pP->_fadeIn);
    _duration->setSingleValue(pP->_duration);
    _fadeOut->setSingleValue(pP->_fadeOut);
    _headWidth->setSingleValue(pP->_headWidth);
    _tailWidth->setSingleValue(pP->_tailWidth);
    _headCenterX->setSingleValue(pP->_headCenter.x);
    _headCenterY->setSingleValue(pP->_headCenter.y);
    _headCenterZ->setSingleValue(pP->_headCenter.z);
    _headSizeX->setSingleValue(pP->_headSize.x);
    _headSizeY->setSingleValue(pP->_headSize.y);
    _headSizeZ->setSingleValue(pP->_headSize.z);
    _tailCenterX->setSingleValue(pP->_tailCenter.x);
    _tailCenterY->setSingleValue(pP->_tailCenter.y);
    _tailCenterZ->setSingleValue(pP->_tailCenter.z);
    _tailSizeX->setSingleValue(pP->_tailSize.x);
    _tailSizeY->setSingleValue(pP->_tailSize.y);
    _tailSizeZ->setSingleValue(pP->_tailSize.z);
    _mode->selectItem(Core::String8(EFreeFormModeToString(pP->_mode)).c_str());
    _travelTime->setSingleValue(pP->_travelTime);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeStorm::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeStorm> pP = LM_DEBUG_PTR_CAST<PtyNodeStorm>(getDataProxy()->getProperty());

    pP->_burstStart = _burstStart->isChecked();
     _textureName->getText(pP->_textureName);
    _period->getRangedValue(pP->_minPeriod, pP->_maxPeriod);
    _maxCount->getSingleValue(pP->_maxCount);
    _time->getRangedValue(pP->_startTime, pP->_endTime);
    _entropy->getSingleValue(pP->_entropy);
    _fertility->getSingleValue(pP->_fertility);
    _resolution->getSingleValue(pP->_resolution);
    _color1->getColorLinear(pP->_color1);
    _color2->getColorLinear(pP->_color2);
    _glow1->getColorLinear(pP->_glow1);
    _glow2->getColorLinear(pP->_glow2);
    _fadeIn->getSingleValue(pP->_fadeIn);
    _duration->getSingleValue(pP->_duration);
    _fadeOut->getSingleValue(pP->_fadeOut);
    _headWidth->getSingleValue(pP->_headWidth);
    _tailWidth->getSingleValue(pP->_tailWidth);
    _headCenterX->getSingleValue(pP->_headCenter.x);
    _headCenterY->getSingleValue(pP->_headCenter.y);
    _headCenterZ->getSingleValue(pP->_headCenter.z);
    _headSizeX->getSingleValue(pP->_headSize.x);
    _headSizeY->getSingleValue(pP->_headSize.y);
    _headSizeZ->getSingleValue(pP->_headSize.z);
    _tailCenterX->getSingleValue(pP->_tailCenter.x);
    _tailCenterY->getSingleValue(pP->_tailCenter.y);
    _tailCenterZ->getSingleValue(pP->_tailCenter.z);
    _tailSizeX->getSingleValue(pP->_tailSize.x);
    _tailSizeY->getSingleValue(pP->_tailSize.y);
    _tailSizeZ->getSingleValue(pP->_tailSize.z);
    pP->_mode = StringToEFreeFormMode(_mode->selectedText());
    _travelTime->getSingleValue(pP->_travelTime);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeStorm::setupUi()
{
    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setSpacing(0);

        Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
        Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);
    _pPWNode->collapse(true);

    _restartStorm   = new QPushButton(QIcon(":/icons/refresh.png"), "Restart storm", this);
        _restartStorm->setFixedSize(110, 20);

    _groupBox       = new QtToolbox::CollapsibleWidget(this, "Node Storm", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupBoxGeneration = new QtToolbox::CollapsibleWidget(this, "Generation");
    _groupBoxAnimation  = new QtToolbox::CollapsibleWidget(this, "Animation");

    QGridLayout* layoutGroupBox = new QGridLayout(NULL);
        layoutGroupBox->setContentsMargins(0, 0, 0, 0);
        layoutGroupBox->setSpacing(0);
        _groupBox->setLayout(layoutGroupBox);

    _textureName    = new CustomLine(this, "Texture name");
    _textureName->pushAuthorizedDropMimeData("asset/texture");
    _textureName->setReadOnly(true);

    _mode  = new QtToolbox::ComboBox(this, "Mode");
    _mode->addItem(Core::String8(EFreeFormModeToString(Renderer::FREE_FORM_ADD)).c_str());
    _mode->addItem(Core::String8(EFreeFormModeToString(Renderer::FREE_FORM_LERP)).c_str());
    _mode->addItem(Core::String8(EFreeFormModeToString(Renderer::FREE_FORM_REFRAC)).c_str());

    _period         = new QtToolbox::RangedSlidingHDR(this, "Period", false);
    _maxCount       = new QtToolbox::SingleSlidingInt(this, "Max count", 1, 1000);
    _time           = new QtToolbox::RangedSlidingTimeValue(this, "Generation time");
    _burstStart     = new QCheckBox("Burst start", this);
    _entropy        = new QtToolbox::SingleSlidingHDR(this, "Entropy", false);
    _fertility      = new QtToolbox::SingleSlidingInt(this, "Fertility", 0, 20);
    _resolution     = new QtToolbox::SingleSlidingInt(this, "Resolution", 2, 100);
    _color1         = new QtToolbox::QuickColorPicker(this, "Color 1", Qt::white);
    _color2         = new QtToolbox::QuickColorPicker(this, "Color 2", Qt::white);
    _glow1          = new QtToolbox::QuickColorPicker(this, "Glow 1", Qt::white);
    _glow2          = new QtToolbox::QuickColorPicker(this, "Glow 2", Qt::white);
    _fadeIn         = new QtToolbox::SingleSlidingHDR(this, "Fade in", false);
    _duration       = new QtToolbox::SingleSlidingHDR(this, "Duration", false);
    _fadeOut        = new QtToolbox::SingleSlidingHDR(this, "Fade out", false);
    _headWidth      = new QtToolbox::SingleSlidingHDR(this, "Head width", false);
    _tailWidth      = new QtToolbox::SingleSlidingHDR(this, "Tail width", false);
    _headCenterX    = new QtToolbox::SingleSlidingHDR(this, "Head center X");
    _headCenterY    = new QtToolbox::SingleSlidingHDR(this, "Head center Y");
    _headCenterZ    = new QtToolbox::SingleSlidingHDR(this, "Head center Z");
    _headSizeX      = new QtToolbox::SingleSlidingHDR(this, "Head size X", false);
    _headSizeY      = new QtToolbox::SingleSlidingHDR(this, "Head size Y", false);
    _headSizeZ      = new QtToolbox::SingleSlidingHDR(this, "Head size Z", false);
    _tailCenterX    = new QtToolbox::SingleSlidingHDR(this, "Tail center X");
    _tailCenterY    = new QtToolbox::SingleSlidingHDR(this, "Tail center Y");
    _tailCenterZ    = new QtToolbox::SingleSlidingHDR(this, "Tail center Z");
    _tailSizeX      = new QtToolbox::SingleSlidingHDR(this, "Tail size X", false);
    _tailSizeY      = new QtToolbox::SingleSlidingHDR(this, "Tail size Y", false);
    _tailSizeZ      = new QtToolbox::SingleSlidingHDR(this, "Tail size Z", false);
    _travelTime     = new QtToolbox::SingleSlidingHDR(this, "Travel time", false);

    _groupBoxGeneration->getLayout()->addWidget(_period);
    _groupBoxGeneration->getLayout()->addWidget(_maxCount);
    _groupBoxGeneration->getLayout()->addWidget(_time);
    _groupBoxGeneration->getLayout()->addWidget(_burstStart);
    _groupBoxGeneration->getLayout()->addWidget(_entropy);
    _groupBoxGeneration->getLayout()->addWidget(_fertility);
    _groupBoxGeneration->getLayout()->addWidget(_resolution);
    _groupBoxGeneration->getLayout()->addWidget(_travelTime);
    _groupBoxGeneration->getLayout()->setAlignment(Qt::AlignCenter);

    _groupBoxAnimation->getLayout()->addWidget(_textureName);
    _groupBoxAnimation->getLayout()->addWidget(_mode);
    _groupBoxAnimation->getLayout()->addWidget(_color1);
    _groupBoxAnimation->getLayout()->addWidget(_color2);
    _groupBoxAnimation->getLayout()->addWidget(_glow1);
    _groupBoxAnimation->getLayout()->addWidget(_glow2);
    _groupBoxAnimation->getLayout()->addWidget(_fadeIn);
    _groupBoxAnimation->getLayout()->addWidget(_duration);
    _groupBoxAnimation->getLayout()->addWidget(_fadeOut);
    _groupBoxAnimation->getLayout()->addWidget(_headWidth);
    _groupBoxAnimation->getLayout()->addWidget(_tailWidth);
    _groupBoxAnimation->getLayout()->addWidget(_headCenterX);
    _groupBoxAnimation->getLayout()->addWidget(_headCenterY);
    _groupBoxAnimation->getLayout()->addWidget(_headCenterZ);
    _groupBoxAnimation->getLayout()->addWidget(_headSizeX);
    _groupBoxAnimation->getLayout()->addWidget(_headSizeY);
    _groupBoxAnimation->getLayout()->addWidget(_headSizeZ);
    _groupBoxAnimation->getLayout()->addWidget(_tailCenterX);
    _groupBoxAnimation->getLayout()->addWidget(_tailCenterY);
    _groupBoxAnimation->getLayout()->addWidget(_tailCenterZ);
    _groupBoxAnimation->getLayout()->addWidget(_tailSizeX);
    _groupBoxAnimation->getLayout()->addWidget(_tailSizeY);
    _groupBoxAnimation->getLayout()->addWidget(_tailSizeZ);
    _groupBoxAnimation->getLayout()->setAlignment(Qt::AlignCenter);

    layoutGroupBox->addWidget(_restartStorm, 0, 0, Qt::AlignCenter);
    layoutGroupBox->addWidget(_pPWNode.get());
    layoutGroupBox->addWidget(_groupBoxGeneration);
    layoutGroupBox->addWidget(_groupBoxAnimation);

    _layout->addWidget(_groupBox);

    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_mode);
    getWidgetsForUndoRedo().push_back(_period);
    getWidgetsForUndoRedo().push_back(_maxCount);
    getWidgetsForUndoRedo().push_back(_time);
    getWidgetsForUndoRedo().push_back(_burstStart);
    getWidgetsForUndoRedo().push_back(_entropy);
    getWidgetsForUndoRedo().push_back(_fertility);
    getWidgetsForUndoRedo().push_back(_resolution);
    getWidgetsForUndoRedo().push_back(_travelTime);
    getWidgetsForUndoRedo().push_back(_textureName);
    getWidgetsForUndoRedo().push_back(_color1);
    getWidgetsForUndoRedo().push_back(_color2);
    getWidgetsForUndoRedo().push_back(_glow1);
    getWidgetsForUndoRedo().push_back(_glow2);
    getWidgetsForUndoRedo().push_back(_fadeIn);
    getWidgetsForUndoRedo().push_back(_duration);
    getWidgetsForUndoRedo().push_back(_fadeOut);
    getWidgetsForUndoRedo().push_back(_headWidth);
    getWidgetsForUndoRedo().push_back(_tailWidth);
    getWidgetsForUndoRedo().push_back(_headCenterX);
    getWidgetsForUndoRedo().push_back(_headCenterY);
    getWidgetsForUndoRedo().push_back(_headCenterZ);
    getWidgetsForUndoRedo().push_back(_headSizeX);
    getWidgetsForUndoRedo().push_back(_headSizeY);
    getWidgetsForUndoRedo().push_back(_headSizeZ);
    getWidgetsForUndoRedo().push_back(_tailCenterX);
    getWidgetsForUndoRedo().push_back(_tailCenterY);
    getWidgetsForUndoRedo().push_back(_tailCenterZ);
    getWidgetsForUndoRedo().push_back(_tailSizeX);
    getWidgetsForUndoRedo().push_back(_tailSizeY);
    getWidgetsForUndoRedo().push_back(_tailSizeZ);

    connect(_restartStorm, SIGNAL(clicked()), this, SLOT(restartStorm()));

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeStorm::applyNewSize()
{
    _groupBox->applyNewSize();
    setFixedWidth(_groupBox->width());
    setFixedHeight(_groupBox->height());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI