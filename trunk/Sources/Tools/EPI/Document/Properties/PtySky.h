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
#ifndef PROPERTY_SKY_H_
#define PROPERTY_SKY_H_

#include <Core/Math/Vector.h>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>


namespace Universe
{
    class World;
}

namespace QtToolbox
{
    class QuickColorPicker;
    class SingleSlidingDouble;
    class CollapsibleWidget;
    class ComboBox;
}//namespace QtToolbox

class QGridLayout;
class QCheckBox;

namespace EPI
{
class CustomLine;

class PtyWidgetSky;
//-----------------------------------------------------------------------------
class LM_API_EPI PtySky : public PropertyNode
{
    friend class PtyWidgetSky;

public:
    PtySky(const Ptr<Universe::World>& pWorld, const Core::String& title = L"Sky");
    virtual ~PtySky();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual void updateData();
    void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual bool            isLeaf () const {return true;}

    virtual String getIconFileName() {return L":/icons/ptySky.png";}
private:
    Core::String            _skyTexture;
    Core::Vector4f          _color;
    Core::Vector4f          _glow;
    double                  _horizon;
    double                  _angle;
    double                  _roof;
    double                  _sphericity;
    bool                    _atInfinity;
    bool                    _isProcedural;
    Renderer::ELightingModel _model;
    Core::Vector4f          _skyColor;
    Core::Vector4f          _sunColor;
    double                  _intensity;

    Ptr<Universe::World>    _pWorld;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetSky : public PropertyWidget
{
    friend class PtySky;

public:
    PtyWidgetSky(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetSky();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *          _groupBox;
    CustomLine *                        _skyTexture;
    QtToolbox::QuickColorPicker *       _color;
    QtToolbox::QuickColorPicker *       _glow;
    QtToolbox::SingleSlidingDouble *    _horizon;
    QtToolbox::SingleSlidingDouble *    _angle;
    QtToolbox::SingleSlidingDouble *    _roof;
    QtToolbox::SingleSlidingDouble *    _sphericity;
    QCheckBox *                         _atInfinity;
    QCheckBox *                         _isProcedural;
    QtToolbox::ComboBox *               _model;
    QtToolbox::QuickColorPicker *       _skyColor;
    QtToolbox::QuickColorPicker *       _sunColor;
    QtToolbox::SingleSlidingDouble *    _intensity;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_SKY_H_
