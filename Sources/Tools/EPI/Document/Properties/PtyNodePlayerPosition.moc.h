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
#ifndef PROPERTY_NODE_PLAYER_POSITION_H_
#define PROPERTY_NODE_PLAYER_POSITION_H_

#include <Core/Math/Vector.h>

#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>
#include <EPI/Document/ContentInfo/IContentRepresentation.h>

class QPushButton;

namespace Universe
{
    class World;
}

namespace QtToolbox
{
    class SingleSlidingDouble;
    class ComboBox;
    class CollapsibleWidget;
}//namespace QtToolbox



namespace EPI
{
class GuiDocument;
class CustomLine;
class PtyWidgetNodePlayerPosition;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyNodePlayerPosition : public PtyNode
{
    friend class PtyWidgetNodePlayerPosition;

public:
    PtyNodePlayerPosition(const Ptr<Universe::World>& pWorldForRepresentation,  const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title = L"NodePlayerPosition");
    PtyNodePlayerPosition(const PtyNodePlayerPosition& other);

    virtual EPropertyClassName getCalssName() const {return PTYNODE_PLAYER_POSITION;}
    virtual ~PtyNodePlayerPosition();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual bool isItMe(const Ptr<Universe::Node>& pNode);
    virtual bool isIntersecting (const Core::Rayf& ray, float& distance);
    virtual void setSelected(bool isSelected);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);

    float getPlayersDistance() const {return _playersDist;}
    const String& getModelName() const {return _modelName;}
    virtual Core::AABoxf getBoundingBox();
    virtual void setVisible(bool flag);

    virtual String getIconFileName() {return L":/icons/ptyPlayerPosition.png";}

    virtual bool isRepresented() const  {return true;}
    virtual void setRepresentationVisible(bool flag) {_pRepresentation->setVisible(flag);}

private:
    Ptr<IContentRepresentation>     _pRepresentation;
    Ptr<Universe::World>            _pWorldForRepresentation;
    float                           _playersDist;
    String                          _modelName;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetNodePlayerPosition : public PropertyWidget
{
    Q_OBJECT

    friend class PtyNodePlayerPosition;

public:
    PtyWidgetNodePlayerPosition(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetNodePlayerPosition();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    QGridLayout * _layout;
    QtToolbox::CollapsibleWidget *      _groupBox;

    Ptr<PropertyWidget> _pPWNode;

    QtToolbox::CollapsibleWidget *      _groupBoxRepresentation;
    CustomLine *                        _modelName;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_NODE_PLAYER_POSITION_H_
