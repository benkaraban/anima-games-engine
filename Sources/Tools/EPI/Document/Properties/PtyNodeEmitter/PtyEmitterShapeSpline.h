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
#ifndef PTY_EMITTER_SHAPE_SPLINE_H_
#define PTY_EMITTER_SHAPE_SPLINE_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterShape.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/EmitterShapeSpline.h>

#include <QGridLayout>
#include <QCheckBox>

namespace QtToolbox
{
    class RangedSlidingHDR;
    class RangedSlidingDouble;
    class SmartGroupBox;
    class ComboBox;
}//namespace QtToolbox

namespace EPI
{
class CustomLine;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyEmitterShapeSpline : public PtyEmitterShape
{
    friend class PtyWidgetEmitterShapeSpline;

public:
    PtyEmitterShapeSpline(   const Ptr<Universe::NodeEmitter>& pNodeE,
                            const Ptr<Universe::RessourcePool> & pResPool,
                            const Ptr<Universe::EmitterShapeSpline>& pShape,
                            const Core::String& title = L"Shape Spline");
    PtyEmitterShapeSpline(   const Ptr<Universe::NodeEmitter>& pNodeE,
                            const Ptr<Universe::RessourcePool> & pResPool,
                            const Core::String& title =L"Shape Spline");

    virtual Universe::EShape getShapeType() const {return Universe::SHAPE_SPLINE;}

    virtual ~PtyEmitterShapeSpline();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateProperty();
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Property>& pty = null);

private:
    String                      _splineName;
    Universe::ESplineShapeMode  _mode;

    float           _uStartMin;
    float           _uStartMax;

    float           _uSpeedMin;
    float           _uSpeedMax;

    Ptr<Universe::RessourcePool> _pResPool;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetEmitterShapeSpline : public PropertyWidget
{
    friend class PtyEmitterShapeSpline;

public:
    PtyWidgetEmitterShapeSpline(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetEmitterShapeSpline();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    QGridLayout * _layout;

    CustomLine *                        _splineName;
    QtToolbox::ComboBox *               _mode;
    QtToolbox::RangedSlidingDouble *    _uStart;
    QtToolbox::RangedSlidingHDR *       _uSpeed;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_EMITTER_SHAPE_CONE_H_*/
