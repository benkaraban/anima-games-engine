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
#ifndef EPI_PROPERTY_SUN_H_
#define EPI_PROPERTY_SUN_H_

#include <QWidget>
#include <QGridLayout>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/World.h>

namespace QtToolbox
{
    class Slider2D;
    class QuickColorPicker;
    class SingleSlidingDouble;
    class CollapsibleWidget;
    class ComboBox;
    class SingleSlidingHDR;
}//namespace QtToolbox

namespace EPI
{
class PtyWidgetSun;
class GuiDocument;
//-----------------------------------------------------------------------------
class LM_API_EPI PtySun : public PropertyNode
{
    Q_OBJECT

    friend class PtyWidgetSun;

public:
    PtySun(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorld, const Core::String& title = L"Sun");
    virtual ~PtySun();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual bool            isLeaf () const {return true;}

    const Renderer::MainLightSettings& getMainLightSetting()    {return _mainLightSettings;}

    virtual String getIconFileName() {return L":/icons/ptySun.png";}

private:
    Ptr<Universe::World>        _pWorld;
    Renderer::MainLightSettings _mainLightSettings;
    GuiDocument&                 _guiDoc;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetSun : public PropertyWidget
{
    Q_OBJECT

    friend class PtySun;

public:
    PtyWidgetSun(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetSun();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void sunVToolClicked ( bool checked );

private:
    QPushButton*    _pSunVTool;
    QGridLayout * _layout;
    QtToolbox::CollapsibleWidget *  _groupBox;
    QtToolbox::CollapsibleWidget *  _groupBoxLighting;
    QtToolbox::CollapsibleWidget *  _groupBoxStaticShadow;
    QtToolbox::CollapsibleWidget *  _groupBoxDynamicShadow;
    QtToolbox::CollapsibleWidget *  _groupBoxBloom;
    QtToolbox::CollapsibleWidget *  _groupBoxOcclusion;
    
    QtToolbox::Slider2D *           _position;

    QtToolbox::QuickColorPicker *   _ambient;
    QtToolbox::QuickColorPicker *   _diffuse0;
    QtToolbox::QuickColorPicker *   _diffuse1;
    QtToolbox::QuickColorPicker *   _diffuse2;
    QtToolbox::QuickColorPicker *   _specular;
    QtToolbox::SingleSlidingDouble * _mulIntensity;

    QtToolbox::SingleSlidingHDR * _staticShadowBias;
    QtToolbox::SingleSlidingHDR * _staticShadowOpacity;
    QtToolbox::SingleSlidingHDR * _staticShadowDiffusion;

    QtToolbox::SingleSlidingHDR * _dynamicShadowOpacity;
    QtToolbox::SingleSlidingHDR * _dynamicShadowMiddle;
    QtToolbox::SingleSlidingHDR * _dynamicShadowMiddleBlend;
    QtToolbox::SingleSlidingHDR * _dynamicShadowEnd;
    QtToolbox::SingleSlidingHDR * _dynamicShadowEndBlend;

    QtToolbox::SingleSlidingHDR * _bloomThreshold;
    QtToolbox::SingleSlidingHDR * _bloomFactor;

    QtToolbox::SingleSlidingHDR * _occlusionContrast;
    QtToolbox::SingleSlidingHDR * _occlusionLuminosity;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif /*EPI_PROPERTY_SUN_H_*/
