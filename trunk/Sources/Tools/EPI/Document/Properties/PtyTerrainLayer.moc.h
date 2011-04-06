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
#ifndef EPI_PTY_TERRAIN_LAYER_H_
#define EPI_PTY_TERRAIN_LAYER_H_

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/NodeTerrain.h>

class QGridLayout;
class QCheckBox;

namespace QtToolbox
{
class CollapsibleWidget;
class LineEdit;
class QuickColorPicker;
class SingleSlidingDouble;
}

namespace EPI
{
class CustomLine;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyTerrainLayer : public PropertyNode
{
    friend class PtyWidgetTerrainLayer;

public:
    PtyTerrainLayer(const Core::String& title = L"Terrain layer");

    PtyTerrainLayer(const Universe::NodeTerrain::Layer & layer);
    virtual ~PtyTerrainLayer();

    void setLayer(const Universe::NodeTerrain::Layer & layer);
    void getLayer(Universe::NodeTerrain::Layer & layer);

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);

    virtual void updateData();
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    inline const Core::String & getName() const { return _name; }
    inline void setName(const Core::String & name) { PropertyNode::setName(name); _name = name; }

    inline const Core::String & getDiffuseTexName() const { return _diffuseTexName; }
    inline const Core::String & getNormalTexName() const { return _normalTexName; }
    inline const Core::String & getMaskTexName() const { return _maskTexName; }

    inline int32 getWidth() const { return _width; }
    inline int32 getHeight() const { return _height; }
    inline float getAngle() const { return _angle; }

    inline const Core::Vector4f & getColor() const { return _color; }

    virtual String getIconFileName() {return L":/icons/ptyTerrainLayer.png";}

private:
    Core::String    _name;
    
    Core::String    _diffuseTexName;
    Core::String    _normalTexName;
    Core::String    _maskTexName;
    int32           _width;
    int32           _height;
    float           _angle;
    float           _contrast;
    Core::Vector4f  _color;
    bool            _useDisplacementMap;
    float           _normalStrength;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetTerrainLayer : public PropertyWidget
{
    Q_OBJECT
    friend class PtyTerrainLayer;
public:
    PtyWidgetTerrainLayer(  const Ptr<PropertyWidgetDataProxy>& data,
                            QWidget * parent = 0);

    
    virtual ~PtyWidgetTerrainLayer();

    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

    void setProperty(const Ptr<PtyTerrainLayer> & pPtyTerrainLayer);


private:
    QGridLayout *                       _layout;
    QtToolbox::CollapsibleWidget *      _groupBox;
    CustomLine *                        _diffuse;
    CustomLine *                        _normal;
    CustomLine *                        _mask;
    QtToolbox::LineEdit *               _width;
    QtToolbox::LineEdit *               _height;
    QtToolbox::LineEdit *               _angle;
    QtToolbox::QuickColorPicker *       _color;
    QtToolbox::SingleSlidingDouble *    _contrast;
    QtToolbox::SingleSlidingDouble*     _normalStrength;
    QCheckBox *                         _useDisplacementMap;
};
//-----------------------------------------------------------------------------

}//namespace EPI

#endif/*EPI_PTY_TERRAIN_LAYER_H_*/