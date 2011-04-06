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
#ifndef PTY_EMITTER_REP_GROUP_H_
#define PTY_EMITTER_REP_GROUP_H_

#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterRep.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/Particles/PartRepresentationGroup.h>

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
}

class QGridLayout;
class QCheckBox;
class QPushButton;
class QHBoxLayout;

namespace EPI
{
class CustomLine;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyEmitterRepGroup : public PtyEmitterRep
{
    friend class PtyWidgetEmitterRepGroup;

public:
    PtyEmitterRepGroup(   const Ptr<Universe::NodeEmitter>& pNodeE,
                              const Ptr<Universe::PartRepresentationGroup>& pRep,
                              const Core::String& title = L"Representation group");
    PtyEmitterRepGroup(   const Ptr<Universe::NodeEmitter>& pNodeE,
                              const Core::String& title =L"Representation group");

    virtual Universe::EPartRepresentation       getRepresentationType() const { return Universe::PR_GROUP; };

    virtual ~PtyEmitterRepGroup();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    //universeData <= property
    virtual void updateData();

    //property <= universeData
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);

private:
    String          _groupName;
    bool            _alignOnDir;
    bool            _restartGroups;
    bool            _affectSize;
    bool            _affectRoll;
    bool            _glowAsColor;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetEmitterRepGroup : public PropertyWidget
{
    friend class PtyEmitterRepGroup;

public:
    PtyWidgetEmitterRepGroup(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetEmitterRepGroup();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    QGridLayout * _layout;

    CustomLine *            _groupName;
    QCheckBox *             _alignOnDir;
    QCheckBox *             _restartGroups;
    QCheckBox *             _affectSize;
    QCheckBox *             _affectRoll;
    QCheckBox *             _glowAsColor;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_EMITTER_SHAPE_CONE_H_*/
