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
#include "PtySun.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/Slider2D/Slider2D.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <Universe/World.h>
#include <EPI/Gui/Viewport/Tools/SunViewportTool.moc.h>
#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Document/DocumentBase.moc.h>



namespace EPI
{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtySun::PtySun(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorld, const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _guiDoc(guiDoc),
    _pWorld(pWorld),
    _mainLightSettings(_pWorld->getMainLightSettings())
{
    LOG_FCT_NAME

    updateProperty();
}
//-----------------------------------------------------------------------------
PtySun::~PtySun()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtySun::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    LOG_FCT_NAME

    Ptr<PtyWidgetSun> pPW (new PtyWidgetSun(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtySun::updateData()
{
    LOG_FCT_NAME

    _pWorld->setMainLightSettings(_mainLightSettings);
}
//-----------------------------------------------------------------------------
void PtySun::updateProperty()
{
    LOG_FCT_NAME

    _mainLightSettings = _pWorld->getMainLightSettings();
    emit propertyChanged();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtySun::clone() const
{
    LOG_FCT_NAME

    return Ptr<Property>(new PtySun( *this ));
}
//-----------------------------------------------------------------------------
void PtySun::internalCopy(const Ptr<Property>& pSrc)
{
    LOG_FCT_NAME

    Ptr<PtySun> pPty = LM_DEBUG_PTR_CAST<PtySun>(pSrc);

    _mainLightSettings = pPty->_mainLightSettings;
    _pWorld = pPty->_pWorld;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetSun::PtyWidgetSun(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    LOG_FCT_NAME

    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetSun::~PtyWidgetSun()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
void PtyWidgetSun::setupUi()
{
    LOG_FCT_NAME

    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Sun", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    QGridLayout* layoutGroupBox = new QGridLayout(NULL); 
        _groupBox->setLayout(layoutGroupBox);

    _pSunVTool  = new QPushButton (QIcon(":/icons/sun.png"), "Sun", _groupBox);
        _pSunVTool->setCheckable(true);
        _pSunVTool->setFixedSize(80,30);
        connect(_pSunVTool, SIGNAL(clicked (bool)), this, SLOT(sunVToolClicked(bool)));

    _groupBoxLighting       = new QtToolbox::CollapsibleWidget(this, "Lighting");
    _groupBoxStaticShadow   = new QtToolbox::CollapsibleWidget(this, "Static shadows");
    _groupBoxBloom          = new QtToolbox::CollapsibleWidget(this, "Bloom");
    _groupBoxOcclusion      = new QtToolbox::CollapsibleWidget(this, "Occlusion");
    _groupBoxDynamicShadow  = new QtToolbox::CollapsibleWidget(this, "Dynamic shadows");

    _position   = new QtToolbox::Slider2D(  _groupBox,  "Position", 
                                            "Azimuth",  "Elevation", 
                                            200,        100,
                                            0.0,        Core::rad2deg(d_PI_MUL_2),
                                            0.0,        Core::rad2deg(d_PI),
                                            0.0,        0.0);

    _ambient            = new QtToolbox::QuickColorPicker(_groupBox, "Ambient");
    _diffuse0           = new QtToolbox::QuickColorPicker(_groupBox, "Diffuse front (key)");
    _diffuse1           = new QtToolbox::QuickColorPicker(_groupBox, "Diffuse mid");
    _diffuse2           = new QtToolbox::QuickColorPicker(_groupBox, "Diffuse back (fill)");
    _specular           = new QtToolbox::QuickColorPicker(_groupBox, "Specular", Qt::white, false);
    _mulIntensity       = new QtToolbox::SingleSlidingDouble(_groupBox, "Intensity factor", 1.0, 5.0);
    
    _staticShadowBias         = new QtToolbox::SingleSlidingHDR(_groupBox, "Shadow Bias", false);
    _staticShadowOpacity      = new QtToolbox::SingleSlidingHDR(_groupBox, "Shadow opacity", false);
    _staticShadowDiffusion    = new QtToolbox::SingleSlidingHDR(_groupBox, "Diffusion", false);

    _dynamicShadowOpacity      = new QtToolbox::SingleSlidingHDR(_groupBox, "Shadow opacity", false);
    _dynamicShadowMiddle       = new QtToolbox::SingleSlidingHDR(_groupBox, "Shadow middle", false);
    _dynamicShadowMiddleBlend  = new QtToolbox::SingleSlidingHDR(_groupBox, "Middle blend", false);
    _dynamicShadowEnd          = new QtToolbox::SingleSlidingHDR(_groupBox, "Shadow end", false);
    _dynamicShadowEndBlend     = new QtToolbox::SingleSlidingHDR(_groupBox, "End blend", false);

    _bloomThreshold     = new QtToolbox::SingleSlidingHDR(_groupBox, "Bloom threshold", false);
    _bloomFactor        = new QtToolbox::SingleSlidingHDR(_groupBox, "Bloom factor", false);

    _occlusionContrast   = new QtToolbox::SingleSlidingHDR(_groupBox, "Occlusion contrast", true);
    _occlusionLuminosity = new QtToolbox::SingleSlidingHDR(_groupBox, "Occlusion luminosity", true);

    layoutGroupBox->addWidget(_position);
    layoutGroupBox->addWidget(_pSunVTool);
    
    layoutGroupBox->addWidget(_groupBoxLighting);
    _groupBoxLighting->getLayout()->addWidget(_ambient);
    _groupBoxLighting->getLayout()->addWidget(_diffuse0);
    _groupBoxLighting->getLayout()->addWidget(_diffuse1);
    _groupBoxLighting->getLayout()->addWidget(_diffuse2);
    _groupBoxLighting->getLayout()->addWidget(_specular);
    _groupBoxLighting->getLayout()->addWidget(_mulIntensity);

    layoutGroupBox->addWidget(_groupBoxStaticShadow);
    _groupBoxStaticShadow->getLayout()->addWidget(_staticShadowBias);
    _groupBoxStaticShadow->getLayout()->addWidget(_staticShadowOpacity);
    _groupBoxStaticShadow->getLayout()->addWidget(_staticShadowDiffusion);

    layoutGroupBox->addWidget(_groupBoxBloom);
    _groupBoxBloom->getLayout()->addWidget(_bloomThreshold);
    _groupBoxBloom->getLayout()->addWidget(_bloomFactor);

    layoutGroupBox->addWidget(_groupBoxOcclusion);
    _groupBoxOcclusion->getLayout()->addWidget(_occlusionContrast);
    _groupBoxOcclusion->getLayout()->addWidget(_occlusionLuminosity);

    layoutGroupBox->addWidget(_groupBoxDynamicShadow);
    _groupBoxDynamicShadow->getLayout()->addWidget(_dynamicShadowOpacity);
    _groupBoxDynamicShadow->getLayout()->addWidget(_dynamicShadowMiddle);
    _groupBoxDynamicShadow->getLayout()->addWidget(_dynamicShadowMiddleBlend);
    _groupBoxDynamicShadow->getLayout()->addWidget(_dynamicShadowEnd);
    _groupBoxDynamicShadow->getLayout()->addWidget(_dynamicShadowEndBlend);

    layoutGroupBox->setAlignment(Qt::AlignCenter);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_position);
    getWidgetsForUndoRedo().push_back(_ambient);
    getWidgetsForUndoRedo().push_back(_diffuse0);
    getWidgetsForUndoRedo().push_back(_diffuse1);
    getWidgetsForUndoRedo().push_back(_diffuse2);
    getWidgetsForUndoRedo().push_back(_specular);
    getWidgetsForUndoRedo().push_back(_mulIntensity);

    getWidgetsForUndoRedo().push_back(_staticShadowBias);
    getWidgetsForUndoRedo().push_back(_staticShadowOpacity);
    getWidgetsForUndoRedo().push_back(_staticShadowDiffusion);

    getWidgetsForUndoRedo().push_back(_dynamicShadowOpacity);
    getWidgetsForUndoRedo().push_back(_dynamicShadowMiddle);
    getWidgetsForUndoRedo().push_back(_dynamicShadowMiddleBlend);
    getWidgetsForUndoRedo().push_back(_dynamicShadowEnd);
    getWidgetsForUndoRedo().push_back(_dynamicShadowEndBlend);

    getWidgetsForUndoRedo().push_back(_bloomThreshold);
    getWidgetsForUndoRedo().push_back(_bloomFactor);
    
    getWidgetsForUndoRedo().push_back(_occlusionContrast);
    getWidgetsForUndoRedo().push_back(_occlusionLuminosity);
    
    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetSun::readProperty()
{
    LOG_FCT_NAME

    Ptr<PtySun> pPtySun = LM_DEBUG_PTR_CAST<PtySun>(getDataProxy()->getProperty());

    const Renderer::MainLightSettings & lightSettings = pPtySun->_mainLightSettings;

    _position->setValue(Core::rad2deg(lightSettings.sunAngleToNorth), Core::rad2deg(lightSettings.sunAngleToZenith));

    _ambient->setColorLinear(lightSettings.ambient);
    _diffuse0->setColorLinear(lightSettings.diffuse0);
    _diffuse1->setColorLinear(lightSettings.diffuse1);
    _diffuse2->setColorLinear(lightSettings.diffuse2);
    _specular->setColorLinear(lightSettings.specular);
    _mulIntensity->setSingleValue(lightSettings.mulIntensity);

    _staticShadowBias->setSingleValue(lightSettings.staticShadowBias);
    _staticShadowOpacity->setSingleValue(lightSettings.staticShadowOpacity);
    _staticShadowDiffusion->setSingleValue(lightSettings.staticShadowDiffusion);
    
    _dynamicShadowOpacity->setSingleValue(lightSettings.dynamicShadowOpacity);
    _dynamicShadowMiddle->setSingleValue(lightSettings.dynamicShadowMiddle);
    _dynamicShadowMiddleBlend->setSingleValue(lightSettings.dynamicShadowMiddleBlend);
    _dynamicShadowEnd->setSingleValue(lightSettings.dynamicShadowEnd);
    _dynamicShadowEndBlend->setSingleValue(lightSettings.dynamicShadowEndBlend);

    _bloomThreshold->setSingleValue(lightSettings.bloomThreshold);
    _bloomFactor->setSingleValue(lightSettings.bloomFactor);

    _occlusionContrast->setSingleValue(lightSettings.occlusionContrast);
    _occlusionLuminosity->setSingleValue(lightSettings.occlusionLuminosity);
}
//-----------------------------------------------------------------------------
void PtyWidgetSun::writeProperty(QWidget* pWidget)
{
    LOG_FCT_NAME

    Ptr<PtySun> pPtySun = LM_DEBUG_PTR_CAST<PtySun>(getDataProxy()->getProperty());

    Renderer::MainLightSettings & lightSettings = pPtySun->_mainLightSettings;

    double sunAngleToNorth = 0.0;
    double sunAngleToZenith = 0.0;
    _position->getValue(sunAngleToNorth, sunAngleToZenith);
    lightSettings.sunAngleToNorth = Core::deg2rad(sunAngleToNorth);
    lightSettings.sunAngleToZenith = Core::deg2rad(sunAngleToZenith);

    _ambient->getColorLinear(lightSettings.ambient);
    _diffuse0->getColorLinear(lightSettings.diffuse0);
    _diffuse1->getColorLinear(lightSettings.diffuse1);
    _diffuse2->getColorLinear(lightSettings.diffuse2);
    _specular->getColorLinear(lightSettings.specular);
    _mulIntensity->getSingleValue(lightSettings.mulIntensity);

    _staticShadowBias->getSingleValue(lightSettings.staticShadowBias);
    _staticShadowOpacity->getSingleValue(lightSettings.staticShadowOpacity);
    _staticShadowDiffusion->getSingleValue(lightSettings.staticShadowDiffusion);

    _dynamicShadowOpacity->getSingleValue(lightSettings.dynamicShadowOpacity);
    _dynamicShadowMiddle->getSingleValue(lightSettings.dynamicShadowMiddle);
    _dynamicShadowMiddleBlend->getSingleValue(lightSettings.dynamicShadowMiddleBlend);
    _dynamicShadowEnd->getSingleValue(lightSettings.dynamicShadowEnd);
    _dynamicShadowEndBlend->getSingleValue(lightSettings.dynamicShadowEndBlend);

    _bloomThreshold->getSingleValue(lightSettings.bloomThreshold);
    _bloomFactor->getSingleValue(lightSettings.bloomFactor);

    _occlusionContrast->getSingleValue(lightSettings.occlusionContrast);
    _occlusionLuminosity->getSingleValue(lightSettings.occlusionLuminosity);
}
//-----------------------------------------------------------------------------
void PtyWidgetSun::sunVToolClicked(bool checked)
{
    Ptr<PtySun> pPtySun = LM_DEBUG_PTR_CAST<PtySun>(getDataProxy()->getProperty());

    if (pPtySun->_guiDoc.getViewportTool()!=null && pPtySun->_guiDoc.getViewportTool()->getType()==SUN_TOOL)
    {
        pPtySun->_guiDoc.setViewportTool(null);
    }
    else
    {
        Ptr<IViewportTool> pVTool;

        pVTool = Ptr<IViewportTool>(new SunViewportTool(pPtySun->_guiDoc.getDocument()->getStateRecorder(), pPtySun));
                    
/*
        connect(pGDoc->getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                    pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
        */
        connect(pPtySun->_guiDoc.getDocument().get(), SIGNAL(documentContentUpdated()),
                pVTool.get(), SLOT(updateViewportTool()));

        pPtySun->_guiDoc.setViewportTool(pVTool);
    }
}
//-----------------------------------------------------------------------------

} // namespace EPI