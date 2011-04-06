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
#ifndef EPI_PROPERTY_CHECKPOINT_H_
#define EPI_PROPERTY_CHECKPOINT_H_


#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/ContentInfo/IContentRepresentation.h>

namespace EPI
{



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI PtyCheckPoint : public PropertyNode
{
    Q_OBJECT

public:
    PtyCheckPoint(const Ptr<Universe::World>& pWorldForRepresentation, const Core::String& title = L"CheckPoint");
    PtyCheckPoint(const PtyCheckPoint& other);
    virtual ~PtyCheckPoint();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);
    virtual bool isTransformable() const;
    virtual void            copyId(const Property& pSrc);
    virtual bool isItMe(const Ptr<Universe::Node>& pNode);
    
    virtual const Ptr<Property>& getPtyTransform() const;
    virtual Ptr<Property>& getPtyTransform();

    virtual bool            isIntersecting  (const Core::Rayf& ray, float& distance);
    virtual void setSelected(bool isSelected);

    float getTime() const {return _time;}
    void setTime(float t) {_time = t;}
    Core::Vector3f getLocalPosition();
    Core::Quaternionf getLocalOrientation();

    void setVisible(bool flag);

    virtual void internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Property& pty);

    Ptr<Universe::Node> getUniverseNodeRepresentation() { return _pRepresentation->getNodeRepresentation();}
    virtual bool isRepresented() const  {return true;}
    virtual void setRepresentationVisible(bool flag) {_pRepresentation->setVisible(flag);}
private Q_SLOTS:
    void ptyTransformChanged();

Q_SIGNALS:
    void updateContainer(const Property& pty);
private:
    float _time;
    Ptr<Property>           _pPtyT;
    Ptr<Universe::World>    _pWorld;
    Ptr<IContentRepresentation>   _pRepresentation;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

#endif /*EPI_PROPERTY_CHECKPOINT_H_*/
