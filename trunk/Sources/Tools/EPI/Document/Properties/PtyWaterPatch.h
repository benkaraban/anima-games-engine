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
#ifndef PROPERTY_WATER_PATCH_H_
#define PROPERTY_WATER_PATCH_H_

#include <EPI/Document/Properties/PtyNode.moc.h>

#include <Universe/NodeWaterPatch.h>

class QGridLayout;
class QCheckBox;

namespace QtToolbox
{
    class CollapsibleWidget;
    class SingleSlidingDouble;
    class QuickColorPicker;
    class SingleSlidingHDR;
}//namespace QtToolbox

namespace EPI
{
class CustomLine;

class PtyWidgetWaterPatch;
class PtyWidgetWaterLayer;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWaterLayer : public PropertyNode
{
    friend class PtyWidgetWaterLayer;

public:
    PtyWaterLayer(const Ptr<Universe::NodeWaterPatch>& pNodeWP, int32 idLayer, const Core::String& title = L"WaterLayer");
    virtual ~PtyWaterLayer();

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual void updateData();
    void updateProperty();

    virtual String getIconFileName() {return L":/icons/ptyWater.png";}
    virtual EPropertyClassName getCalssName() const {return PTYNODE_WATER_LAYER;}
private:
    Ptr<Universe::NodeWaterPatch> _pNodeWP;

    int32       _idLayer;

    String      _layer;
    double      _scaleU;
    double      _scaleV;
    double      _speedU;
    double      _speedV;
    double      _amplitude;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetWaterLayer : public PropertyWidget
{
    friend class PtyWaterLayer;

public:
    PtyWidgetWaterLayer(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetWaterLayer();
    
    void setupUi();

    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    
private:
    QGridLayout *                   _layout;
    //QtToolbox::SmartGroupBox *      _groupBox;

    CustomLine*                     _pLayer;
    QtToolbox::SingleSlidingHDR*    _pScaleU;
    QtToolbox::SingleSlidingHDR*    _pScaleV;
    QtToolbox::SingleSlidingHDR*    _pSpeedU;
    QtToolbox::SingleSlidingHDR*    _pSpeedV;
    QtToolbox::SingleSlidingDouble* _pAmplitude;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWaterPatch : public PtyNode
{
    friend class PtyWidgetWaterPatch;

public:
    PtyWaterPatch(const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeWaterPatch>& pNodeW, const Core::String& title = L"WaterPatch");
    virtual ~PtyWaterPatch();

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    Ptr<Property> getPtyLayer(int32 id);

    virtual void updateData();
    void updateProperty();
    virtual EPropertyClassName getCalssName() const {return PTYNODE_WATER_PATCH;}
    virtual String getIconFileName() {return L":/icons/ptyWater.png";}

private:
    double           _width;
    double           _height;
    Core::Vector4f  _diffuse;
    Core::Vector4f  _specular;
    double           _shininess;
    bool            _isRefractive;
    bool            _isReflective;
    double           _reflectionScale;
    double           _refractionScale;
    double           _fresnelMin;
    double           _FresnelPower;
    String           _diffuseTex;

    int32 _indexChildLayer [Renderer::WATER_LAYER_COUNT];
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetWaterPatch : public PropertyWidget
{
    friend class PtyWaterPatch;

public:
    PtyWidgetWaterPatch(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetWaterPatch();
    
    void setupUi();

    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);

private:
    QGridLayout *                   _layout;
    QtToolbox::CollapsibleWidget *  _groupBox;

    Ptr<PropertyWidget>             _pPWNode;

    QtToolbox::SingleSlidingHDR*    _width;
    QtToolbox::SingleSlidingHDR*    _height;
    QtToolbox::QuickColorPicker *   _diffuse;
    QtToolbox::QuickColorPicker *   _specular;
    QtToolbox::SingleSlidingDouble* _shininess;
    QCheckBox *                     _isRefractive;
    QCheckBox *                     _isReflective;
    QtToolbox::SingleSlidingHDR*    _reflectionScale;
    QtToolbox::SingleSlidingHDR*    _refractionScale;
    QtToolbox::SingleSlidingDouble* _fresnelMin;
    QtToolbox::SingleSlidingHDR*    _FresnelPower;
    CustomLine*                     _pDiffuseTex;

    QtToolbox::CollapsibleWidget *  _groupBoxPlane;
    QtToolbox::CollapsibleWidget *  _groupBoxMaterial;
    QtToolbox::CollapsibleWidget *  _groupBoxLayer;

    Ptr<PtyWidgetWaterLayer>        _tabLayerWidget [Renderer::WATER_LAYER_COUNT];
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

#endif //PROPERTY_WATER_PATCH_H_
