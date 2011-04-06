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
#include "PtyNodeSound.moc.h"


#include <QGridLayout>
#include <QCheckbox>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <EPI/GUI/Widget/CustomLine.moc.h>
#include <Renderer/IFreeForm.h>
#include <EPI/Document/ContentInfo/SoundRepresentation.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    PtyNodeSound::PtyNodeSound( const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeSoundSource>& pNodeSound, 
                            const Core::String& title)
:   PtyNode(pWorld, pNodeSound, title),
    _pWorldForRepresentation(pWorldForRepresentation)
{
    _pRepresentation = Ptr<IContentRepresentation>(new SoundRepresentation(_pWorldForRepresentation, getUniverseNode()));
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeSound::PtyNodeSound(const PtyNodeSound& other):
    PtyNode(other)
{
    _pWorldForRepresentation    = null;
    _pRepresentation            = null;
    _soundName                  = other._soundName;
    _gain                       = other._gain;      
    _isStatic                   = other._isStatic;
}
//-----------------------------------------------------------------------------
PtyNodeSound::~PtyNodeSound()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeSound::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodeSound> pPW (new PtyWidgetNodeSound(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodeSound::updateProperty()
{
    Ptr<Universe::NodeSoundSource>  pNode = LM_DEBUG_PTR_CAST<Universe::NodeSoundSource>(getUniverseNode());


    Core::List<Universe::NodeSoundSource::SoundSourceInfo> infos = pNode->getSoundsSourceInfos();

    if (infos.size()>0)
    {
        _soundName = infos[0].name;
    }
    else
    {
        _soundName = L"";
    }

    _gain                 = pNode->getGain();
    _isStatic             = pNode->isStaticSound();

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
void PtyNodeSound::updateData()
{
    

    Ptr<Universe::NodeSoundSource>  pNode = LM_DEBUG_PTR_CAST<Universe::NodeSoundSource>(getUniverseNode());

    pNode->eraseAllSounds();
    if (_soundName!=L"")
    {
        pNode->enqueue(_soundName, true);
    }
    


    pNode->setGain(_gain);
    pNode->setStaticSound(_isStatic);
    PtyNode::updateData();
    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeSound::clone() const
{
    Ptr<PtyNodeSound> ret (new PtyNodeSound( *this ));

    return ret;
}
//-----------------------------------------------------------------------------
void PtyNodeSound::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeSound> pPty = LM_DEBUG_PTR_CAST<PtyNodeSound>(pSrc);

    _soundName             = pPty->_soundName;
    _gain                  = pPty->_gain;
    _isStatic              = pPty->_isStatic;

    updateData();
}
//-----------------------------------------------------------------------------
bool PtyNodeSound::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (_pRepresentation->isItMe(pNode) == true)
        return true;
    else
        return PtyNode::isItMe(pNode);
}
//-----------------------------------------------------------------------------
bool PtyNodeSound::isIntersecting  (const Core::Rayf& ray, float& distance)
{
    bool flag = PtyNode::isIntersecting(ray, distance);

    if (flag == false)
    {
        flag = _pRepresentation->isIntersecting(ray, distance);
    }

    return flag;
}
//-----------------------------------------------------------------------------
void PtyNodeSound::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PtyNode::internalResurrect(pWorld, pWorldInfoContent, pty);
    getUniverseNode()->restart(false);
    _pWorldForRepresentation = pWorldInfoContent;

    LM_ASSERT(_pRepresentation==null);

    _pRepresentation = Ptr<IContentRepresentation>(new SoundRepresentation(_pWorldForRepresentation, getUniverseNode()));

    updateProperty();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeSound::PtyWidgetNodeSound(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeSound::~PtyWidgetNodeSound()
{
    if (_pNodeSoundDemo!=null)
    {
        _pNodeSoundDemo->kill();
        _pNodeSoundDemo = null;
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeSound::readProperty()
{
    Ptr<PtyNodeSound> pP = LM_DEBUG_PTR_CAST<PtyNodeSound>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();

    _soundName->setText(pP->_soundName);
    
    double valGain = pP->_gain;
    _gain->setSingleValue(valGain);
    _isStatic->setChecked(pP->_isStatic);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeSound::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeSound> pP = LM_DEBUG_PTR_CAST<PtyNodeSound>(getDataProxy()->getProperty());

    
    _soundName->getText(pP->_soundName);

    if (pWidget == NULL || pWidget ==_gain)
    {
        _gain->getSingleValue(pP->_gain);

        if (_pNodeSoundDemo!=null)
        {
            _pNodeSoundDemo->setGain(pP->_gain);
        }
    }

    if (pWidget == NULL || pWidget ==_isStatic)
    {
        pP->_isStatic = _isStatic->isChecked();
    }
    
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeSound::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);


    _groupBox       = new QtToolbox::CollapsibleWidget(this, "Sound", false);
    _groupBox->setStyle(QtToolbox::CW_TITLE_1);
    _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupBoxNodeSound = new QtToolbox::CollapsibleWidget(this, "Parameter");

    _soundName    = new CustomLine(this, "Sound name");
        _soundName->pushAuthorizedDropMimeData("asset/sound");
 
    _gain = new QtToolbox::SingleSlidingDouble(this, "gain", 0, 1);
    _isStatic = new QCheckBox("static sound", this);

    _play = new QPushButton(QIcon(":/icons/play.png"), "", this);
        _play->setFixedSize(35, 25);
        _play->hide();

    _groupBoxNodeSound->getLayout()->addWidget(_play);
    _groupBoxNodeSound->getLayout()->addWidget(_soundName);
    _groupBoxNodeSound->getLayout()->addWidget(_gain);
    _groupBoxNodeSound->getLayout()->addWidget(_isStatic);
   

    _groupBox->getLayout()->addWidget(_pPWNode.get());
    _groupBox->getLayout()->addWidget(_groupBoxNodeSound);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_soundName);
    getWidgetsForUndoRedo().push_back(_gain);
    getWidgetsForUndoRedo().push_back(_isStatic);

    connect (_play, SIGNAL(clicked()),  this, SLOT(play()));  

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeSound::play()
{
    Ptr<PtyNodeSound> pP = LM_DEBUG_PTR_CAST<PtyNodeSound>(getDataProxy()->getProperty());

    if (_pNodeSoundDemo!=null)
        _pNodeSoundDemo->kill();

    _pNodeSoundDemo = pP->_pWorldForRepresentation->createSoundSource();
    
    if (_pNodeSoundDemo!=null)
    {
        _pNodeSoundDemo->enqueue(pP->_soundName, false);
        _pNodeSoundDemo->setGain(pP->_gain);
        _pNodeSoundDemo->killAtEnd();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI