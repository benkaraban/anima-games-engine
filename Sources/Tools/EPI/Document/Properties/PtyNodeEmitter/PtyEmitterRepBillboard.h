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
#ifndef PTY_EMITTER_REP_BILLBOARD_H_
#define PTY_EMITTER_REP_BILLBOARD_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterRep.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/PartRepresentationBillboard.h>

#include <QGridLayout>
#include <QCheckBox>

namespace QtToolbox
{
    class RangedSlidingHDR;
    class SingleSlidingHDR;
    class CollapsibleWidget;
    class QuickColorPicker;
    class ComboBox;
    class LineEdit;
    class SmartGroupBox;
}//namespace QtToolbox

//namespace QtToolbox
//{
//    class RangedSlidingInt;
//    class RangedSlidingDouble;
//    class RangedSlidingTimeValue;
//    class RangedSlidingHDR;
//    class SingleSlidingHDR;
//    class CollapsibleWidget;
//    class QuickColorPicker;
//    class ComboBox;
//    class LineEdit;
//}//namespace QtToolbox

class QGridLayout;
class QCheckBox;
class QPushButton;
class QHBoxLayout;

namespace EPI
{
class CustomLine;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyEmitterRepBillboard : public PtyEmitterRep
{
    friend class PtyWidgetEmitterRepBillboard;

public:
    PtyEmitterRepBillboard(   const Ptr<Universe::NodeEmitter>& pNodeE,
                              const Ptr<Universe::PartRepresentationBillboard>& pRep,
                              const Core::String& title = L"Representation billboard");
    PtyEmitterRepBillboard(   const Ptr<Universe::NodeEmitter>& pNodeE,
                              const Core::String& title =L"Representation billboard");

    PtyEmitterRepBillboard(const PtyEmitterRepBillboard & other);

    virtual Universe::EPartRepresentation       getRepresentationType() const { return Universe::PR_BILLBOARD; };

    virtual ~PtyEmitterRepBillboard();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    //universeData <= property
    virtual void updateData();

    //property <= universeData
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);

private:
    Core::String                _textureName;
    Renderer::EBillboardMode    _billboardMode;

    Renderer::EBillboardAlign   _billboardAlign;
    Core::Vector3f              _alignVector;

    int32                       _uSpriteLayout;
    int32                       _vSpriteLayout;
    bool                        _spriteSmooth;

    float   _minWidth; 
    float   _maxWidth;
    float   _minHeight;
    float   _maxHeight;
    bool    _squareParts;   
    bool    _normSize;

    float   _minAngle;
    float   _maxAngle;
    float   _minT;
    float   _maxT;
    Core::Vector4f _color1;
    Core::Vector4f _color2;
    float   _colorFactor;
    float   _glowFactor;

    float           _zBias;
    float           _fogFactor;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetEmitterRepBillboard : public PropertyWidget
{
    friend class PtyEmitterRepBillboard;

public:
    PtyWidgetEmitterRepBillboard(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetEmitterRepBillboard();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    QGridLayout * _layout;

    CustomLine *                        _textureName;
    QtToolbox::ComboBox *               _billboardMode;
    QtToolbox::ComboBox *               _billboardAlign;
    QtToolbox::SingleSlidingDouble *    _alignVectorX;
    QtToolbox::SingleSlidingDouble *    _alignVectorY;
    QtToolbox::SingleSlidingDouble *    _alignVectorZ;
    QtToolbox::LineEdit *               _uSpriteLayout;
    QtToolbox::LineEdit *               _vSpriteLayout;
    QCheckBox *                         _spriteSmooth;
    QtToolbox::SmartGroupBox *          _groupColors;
    QtToolbox::QuickColorPicker *       _color1;
    QtToolbox::QuickColorPicker *       _color2;
    QtToolbox::RangedSlidingHDR *       _width;
    QtToolbox::RangedSlidingHDR *       _height;
    QtToolbox::RangedSlidingDouble *    _angle;
    QtToolbox::RangedSlidingDouble *    _time;
    QtToolbox::SingleSlidingDouble *    _colorFactor;
    QtToolbox::SingleSlidingDouble *    _glowFactor;
    QCheckBox *                         _squareParts;
    QCheckBox *                         _normSize;
    QtToolbox::SingleSlidingHDR *       _zBias;
    QtToolbox::SingleSlidingDouble *    _fogFactor;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_EMITTER_SHAPE_CONE_H_*/
