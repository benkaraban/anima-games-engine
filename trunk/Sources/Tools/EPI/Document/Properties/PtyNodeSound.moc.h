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
#ifndef PTY_NODE_SOUND_H_
#define PTY_NODE_SOUND_H_

#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/ContentInfo/IContentRepresentation.h>
#include <Universe/NodeSoundSource.h>


namespace QtToolbox
{
    class SingleSlidingHDR;
    class SingleSlidingInt;
    class CollapsibleWidget;
    class QuickColorPicker;
    class ComboBox;
    class QCheckBox;
}//namespace QtToolbox

class QGridLayout;

namespace EPI
{
class CustomLine;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyNodeSound : public PtyNode
{
    friend class PtyWidgetNodeSound;

public:
    PtyNodeSound( const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeSoundSource> & pNodeSound,
                  const Core::String& title = L"Node Sound");
    PtyNodeSound(const PtyNodeSound& other);

    virtual EPropertyClassName getCalssName() const {return PTYNODE_SOUND;}

    virtual ~PtyNodeSound();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual String getIconFileName() {return L":/icons/ptySound.png";}

    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);

    virtual bool isItMe(const Ptr<Universe::Node>& pNode);
    virtual bool isIntersecting  (const Core::Rayf& ray, float& distance);
    virtual bool isRepresented()  const {return true;} 
    virtual void setRepresentationVisible(bool flag) {_pRepresentation->setVisible(flag);}

private:
    String                          _soundName;

    float                           _gain;
    bool                            _isStatic;
    Ptr<IContentRepresentation>     _pRepresentation;
    Ptr<Universe::World>            _pWorldForRepresentation;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetNodeSound : public PropertyWidget
{
    friend class PtyNodeSound;

    Q_OBJECT

public:
    PtyWidgetNodeSound(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetNodeSound();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();


private Q_SLOTS:
    void play();

private:
    QGridLayout *                       _layout;

    QtToolbox::CollapsibleWidget *      _groupBox;
    QtToolbox::CollapsibleWidget *      _groupBoxNodeSound;

    CustomLine *                        _soundName;
    QtToolbox::SingleSlidingDouble *    _gain;
    QCheckBox *                         _isStatic;
    QPushButton *                       _play;

    Ptr<PropertyWidget>                 _pPWNode;
    Ptr<Universe::NodeSoundSource>      _pNodeSoundDemo;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_NODE_SOUND_H_*/
