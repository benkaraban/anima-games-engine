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
#ifndef PTY_NODE_EMITTER_H_
#define PTY_NODE_EMITTER_H_

#include <EPI/Document/Properties/PtyNode.moc.h>

#include <Universe/NodeEmitter.h>

#include <Renderer/IBillboard.h>

namespace QtToolbox
{
    class RangedSlidingInt;
    class RangedSlidingDouble;
    class RangedSlidingTimeValue;
    class RangedSlidingHDR;
    class SingleSlidingHDR;
    class CollapsibleWidget;
    class QuickColorPicker;
    class ComboBox;
    class LineEdit;
}//namespace QtToolbox

class QGridLayout;
class QCheckBox;
class QPushButton;
class QHBoxLayout;

namespace EPI
{
class CustomLine;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyNodeEmitter : public PtyNode
{
    Q_OBJECT
    friend class PtyWidgetNodeEmitter;

public:
    PtyNodeEmitter( const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeEmitter> & pNodeEmitter,
                    const Core::String& title = L"Node Emitter");
protected:
    PtyNodeEmitter(const PtyNodeEmitter& other);

public:
    virtual ~PtyNodeEmitter();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    //universeData -> property
    virtual void updateData();

    //property -> universeData
    virtual void updateProperty();

    virtual void setUniverseNode(const Ptr<Universe::Node>& pNode);
    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);

    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    Ptr<Property>   getAllEffets();

    virtual String getIconFileName() {return L":/icons/ptyEmitter.png";}

    virtual void            copyId(const Property& pSrc);

    virtual EPropertyClassName getCalssName() const {return PTYNODE_EMITTER;}

private Q_SLOTS:
    void makeUpdateEffect();

private:
    void addEffect(const Core::String & effectName, Ptr<StateRecorder>& pStateRecorder);
    void removeEffect(int32 iEffect, Ptr<StateRecorder>& pStateRecorder);
    void updateShape(Ptr<StateRecorder> pStateRecorder = null);
    void updateRepresentation(Ptr<StateRecorder> pStateRecorder = null);

private:
    // Generation
    float   _minPeriod; 
    float   _maxPeriod;
    float   _minTimeMul;
    float   _maxTimeMul;
    int32   _maxCount;

    double  _startTime;
    double  _endTime;

    bool    _burstStart;
    float   _minBurst;
    float   _maxBurst;

    bool    _stickyParts;

    String                          _addEffectType;
    Universe::EShape                _shape;
    Universe::EPartRepresentation   _representation;

    Ptr<Property>   _pPtyShape;
    Ptr<Property>   _pPtyRepresentation;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetNodeEmitter : public PropertyWidget
{
    Q_OBJECT
    friend class PtyNodeEmitter;

public:
    PtyWidgetNodeEmitter(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetNodeEmitter();

    void setupUi();

    //property -> propertyWidget
    void readProperty();
    //propertyWidget -> property
    void writeProperty(QWidget* pWidget = NULL);

private:
    void updatePtyWidgetShape();
    void updatePtyWidgetRepresentation();

public Q_SLOTS:
    void deletePtyWidgetEffect(QWidget * pEffect);

private Q_SLOTS:
    void addEffect();
    void restartEmitter();

private:
    void updatePtyWidgetNodeEmitter();

private:
    QGridLayout * _layout;
    QPushButton * _restartEmitter;

    QtToolbox::CollapsibleWidget *      _groupBox;
    QtToolbox::CollapsibleWidget *      _groupBoxShape;
    QtToolbox::CollapsibleWidget *      _groupBoxRepresentation;
    QtToolbox::CollapsibleWidget *      _groupBoxGeneration;

    // Generation
    QtToolbox::RangedSlidingHDR *       _frequency;
    QtToolbox::RangedSlidingHDR *       _timeMul;
    QtToolbox::SingleSlidingInt *       _maxCount;
    QtToolbox::RangedSlidingTimeValue * _generationTime;
    QtToolbox::RangedSlidingHDR *       _burst;
    QCheckBox *                         _burstStart;
    QCheckBox *                         _stickyParts;

    QtToolbox::ComboBox *               _shape;
    QtToolbox::ComboBox *               _representation;

    Ptr<PropertyWidget>                 _ptyWidgetShape;
    Ptr<PropertyWidget>                 _ptyWidgetRepresentation;

    QWidget *                           _addEffectBar;
    QHBoxLayout *                       _addEffectLayout;
    QPushButton *                       _addEffectButton;
    QtToolbox::ComboBox *               _addEffectType;

    Core::List<Ptr<PropertyWidget> >    _effects;
    Ptr<PropertyWidget>                 _pPWNode;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_NODE_EMITTER_H_*/
