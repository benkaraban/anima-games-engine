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
#include "PtyCheckPoint.moc.h"

#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/Document/ContentInfo/CheckPointRepresentation.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyCheckPoint::PtyCheckPoint(const Ptr<Universe::World>& pWorldForRepresentation, const Core::String& title):
    PropertyNode(title, false, false, CONTENT),
    _pWorld(pWorldForRepresentation),
    _time(0)
{
    _pRepresentation = Ptr<IContentRepresentation>(new CheckPointRepresentation(pWorldForRepresentation));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);

    _pPtyT = Ptr<Property> (new PtyTransformContent(L"Transform"));
    connect(_pPtyT.get(), SIGNAL(propertyChanged()), this, SLOT(ptyTransformChanged()));
}
//-----------------------------------------------------------------------------
PtyCheckPoint::PtyCheckPoint(const PtyCheckPoint& other)
{
    _time            = other._time;
    _pPtyT           = other._pPtyT->clone();
    _pRepresentation = null;//other._pRepresentation->clone();
    _pWorld          = other._pWorld;

    connect(_pPtyT.get(), SIGNAL(propertyChanged()), this, SLOT(ptyTransformChanged()));
}
//-----------------------------------------------------------------------------
PtyCheckPoint::~PtyCheckPoint()
{
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::ptyTransformChanged()
{
    updateContainer(*this);
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyCheckPoint::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    return null;
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::updateData()
{
    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::updateProperty()
{
    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyCheckPoint::clone() const
{
    Ptr<PtyCheckPoint> pRet  (new PtyCheckPoint( *this ));
   
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PropertyNode::internalResurrect(pWorld, pWorldInfoContent);
    _pWorld = pWorldInfoContent;
    LM_ASSERT(_pRepresentation==null);

    _pRepresentation = Ptr<IContentRepresentation>(new CheckPointRepresentation(pWorldInfoContent));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    updateProperty();

    connect(this, SIGNAL(updateContainer(const Property&)), (const_cast< Ptr<Property>&>(pty)).get(), SLOT(propertyContentChanged(const Property&)));
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Property& pty)
{
    PropertyNode::internalResurrect(pWorld, pWorldInfoContent);
    _pWorld = pWorldInfoContent;
    LM_ASSERT(_pRepresentation==null);
    
    _pRepresentation = Ptr<IContentRepresentation>(new CheckPointRepresentation(pWorldInfoContent));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    updateProperty();

    connect(this, SIGNAL(updateContainer(const Property&)), &pty, SLOT(propertyContentChanged(const Property&)));
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyCheckPoint> pPty = LM_DEBUG_PTR_CAST<PtyCheckPoint>(pSrc);

    _time = pPty->_time;
    _pPtyT->copy(pPty->_pPtyT);
    _pWorld = pPty->_pWorld;

    if(_pRepresentation!=null)
        _pRepresentation->updateObjectRepresentation(*this);

    updateContainer(*this);
}
//-----------------------------------------------------------------------------
bool PtyCheckPoint::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    if (id == _pPtyT->getId())
    {
        pResult = _pPtyT;
        return true;
    }

    return PropertyNode::findPtyWithId(pResult, id);
}
//-----------------------------------------------------------------------------
bool PtyCheckPoint::isTransformable() const
{
    return true;
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::copyId(const Property& pSrc)
{
    const PtyCheckPoint& other = (const PtyCheckPoint&)pSrc;

    PropertyNode::copyId(pSrc);
        _pPtyT->setId(other._pPtyT->getId());
}
//-----------------------------------------------------------------------------
bool PtyCheckPoint::isItMe(const Ptr<Universe::Node>& pNode)
{
    return _pRepresentation->isItMe(pNode);
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyCheckPoint::getPtyTransform() const
{
    return _pPtyT;
}
//-----------------------------------------------------------------------------
Ptr<Property>& PtyCheckPoint::getPtyTransform()
{
    return _pPtyT;
}
//-----------------------------------------------------------------------------
bool PtyCheckPoint::isIntersecting (const Core::Rayf& ray, float& distance)
{
    return _pRepresentation->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::setSelected(bool isSelected)
{
    if (isSelected == true)
    {
        _pRepresentation->setRepresentationMode(OBJECT_REPRESENTATION);
    }
    else
    {
        _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    }
}
//-----------------------------------------------------------------------------
Core::Vector3f PtyCheckPoint::getLocalPosition()
{
    return LM_DEBUG_PTR_CAST<PtyTransform>(getPtyTransform())->getLocalPosition();
}
//-----------------------------------------------------------------------------
Core::Quaternionf PtyCheckPoint::getLocalOrientation()
{
   // return LM_DEBUG_PTR_CAST<PtyTransform>(getPtyTransform())->getLocalOrientation();
    return Core::Quaternionf(LM_DEBUG_PTR_CAST<PtyTransform>(getPtyTransform())->getLocalAxis());
}
//-----------------------------------------------------------------------------
void PtyCheckPoint::setVisible(bool flag)
{
    _pRepresentation->setVisible(flag);
}
//-----------------------------------------------------------------------------
} // namepsace EPI
