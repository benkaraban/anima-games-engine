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
#include <EPI/Document/Property.moc.h>

#include <EPI/ImportInfo/ImportInfo.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

namespace EPI
{

const int32 FLAG_PTY_NODE_VALUE       = 0x00000001;
const int32 FLAG_PTY_EDITABLE_CONTENT_VALUE       = 0x00000002;
/**
* Defini l'interface de base des proprietes du document
*/
class LM_API_EPI Property::implementation
{
public:
    implementation(const implementation& other);
    implementation(const EPropertyType type);
    implementation(const EPropertyType type, const Core::String name, bool isSelectable = false, bool isDeletable = true);

    ~implementation();


    bool isIntersecting     (const Core::Rayf& ray, float& distance);
    bool isTransformable    ()  const;
    bool isEditable         ()       const   {return false;}
    bool isSelectable       ()             {return _isSelectable;}
    bool isDeletable        ()              {return _isDeletable;}

    virtual void copy(const Property::implementation& pSrc);

    int32           getId       () const;
    void            setId       (int32 id);
    void            setDocId    (const int32 docId);
    const int32&    getDocId    () const;

    void                    addChild        (Ptr<Property> pChild, int32 indexPos);
    void                    removeChild     (Ptr<Property> pChild);
    void                    removeChild     (const int32 index);
    const Ptr<Property>&    getChild        (const int32 index) const;
    Ptr<Property>&          getChild        (const int32 index);
    int32                   getNbChildren   () const;
    const Property*         getParent       () const;
    Property*               getParent       ();
    void                    setParent       (Property* parent);

    const Core::String& getName () const;
    EPropertyType       getType () const;

    void updateData         (){;}
    void updateProperty     (){;}

    bool isItMe(const Ptr<Universe::Node>& pNode);

    void resurrect();
    void internalResurrect();

    bool findPtyWithId(Ptr<Property>& pResult, int32 id);

private:
    implementation& operator=( const implementation& other );

Q_SIGNALS:
    void generate(const Ptr<ImportInfo> & pImportInfo);
    void propertyChanged();

public:
    int32           _id;
    Property*       _pParent;
    Core::String    _name;
    bool            _isSelectable;
    bool            _isDeletable;
    int32           _docId;
    EPropertyType   _type;
    int32           flags;

    static int32 countProperty; //compte le nombre de propriete cree depuis le lancement du programme
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Ptr<Property> _pBidon;
//-----------------------------------------------------------------------------
int32 Property::implementation::countProperty = 0;
//-----------------------------------------------------------------------------
Property::implementation::implementation(const EPropertyType type):
    _pParent(NULL),
    _id(countProperty++),
    _name(L"noName"),
    _isSelectable(false),
    _docId(-1),
    _type(type),
    _isDeletable(true),
    flags(0)
{

}
//-----------------------------------------------------------------------------
Property::implementation::implementation(const EPropertyType type, const Core::String name, bool isSelectable, bool isDeletable):
    _pParent(NULL),
    _id(countProperty++),
    _name(name),
    _isSelectable(isSelectable),
    _docId(-1),
    _type(type),
    _isDeletable(isDeletable),
    flags(0)
{

}
//-----------------------------------------------------------------------------
Property::implementation::implementation(const implementation& other):
    _pParent(NULL),
    _id(countProperty++),
    _name(other._name),
    _isSelectable(other._isSelectable),
    _docId(-1),
    _type(other._type),
    _isDeletable(other._isDeletable),
    flags(0)
{
    flags = other.flags;
}
//-----------------------------------------------------------------------------
Property::implementation::~implementation()
{

}
//-----------------------------------------------------------------------------
Property::implementation& Property::implementation::operator=( const Property::implementation& other )
{
    LM_ASSERT(false);
    /*
   if (_pParent != NULL)
   {
      _pParent->removeChild(Ptr<Property>(this));
   }

   _pParent = NULL;
   _id = countProperty++;
*/
   return *this;
}
//-----------------------------------------------------------------------------
int32 Property::implementation::getId() const
{
    return _id;
}
//-----------------------------------------------------------------------------
void Property::implementation::setId(int32 id)
{
    _id = id;
}
//-----------------------------------------------------------------------------
void Property::implementation::resurrect()
{
    ERR << L"Try to resurrect a property non-composite";
}
//-----------------------------------------------------------------------------
void Property::implementation::internalResurrect()
{
}
//-----------------------------------------------------------------------------
void Property::implementation::addChild(Ptr<Property> pChild, int32 indexPos)
{
    ERR << L"Try to add a child has a property non-composite";
}
//-----------------------------------------------------------------------------
void Property::implementation::removeChild(Ptr<Property> pChild)
{
    ERR << L"Try to remove a child has a property non-composite";
}
//-----------------------------------------------------------------------------
void Property::implementation::removeChild(const int32 index)
{
    ERR << L"Try to remove a child has a property non-composite";
}
//-----------------------------------------------------------------------------
const Ptr<Property>& Property::implementation::getChild(const int32 index) const
{
    ERR << L"Attempts to access a child from a non-composite property";
    return _pBidon;
}
//-----------------------------------------------------------------------------
Ptr<Property>& Property::implementation::getChild(const int32 index)
{
    ERR << L"Attempts to access a child from a non-composite property";
    return _pBidon;
}
//-----------------------------------------------------------------------------
bool Property::implementation::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    ERR << L"findPtyWithId is apply from a non-composite property";
    return false;
}
//-----------------------------------------------------------------------------
int32 Property::implementation::getNbChildren() const
{
    ERR << L"Tries to know the number of children a property non-composite";
    return 0;
}
//-----------------------------------------------------------------------------
const Property* Property::implementation::getParent() const
{
    return _pParent;
}
//-----------------------------------------------------------------------------
Property* Property::implementation::getParent()
{
    return _pParent;
}
//-----------------------------------------------------------------------------
void Property::implementation::setParent(Property* parent)
{
    _pParent = parent;
}
//-----------------------------------------------------------------------------
void Property::implementation::setDocId(const int32 docId)
{
    _docId = docId;
}
//-----------------------------------------------------------------------------
const int32& Property::implementation::getDocId() const
{
    return _docId;
}
//-----------------------------------------------------------------------------
const Core::String& Property::implementation::getName() const
{
    return _name;
}
//-----------------------------------------------------------------------------
void Property::implementation::copy(const Property::implementation& pSrc)
{
/*
    int32           _id;
*/
    _pParent        = NULL;
    _docId          = -1;

    _isSelectable   = pSrc._isSelectable;
    _isDeletable    = pSrc._isDeletable;
    _name           = pSrc._name;
    _type           = pSrc._type;

    
}
//-----------------------------------------------------------------------------
EPropertyType Property::implementation::getType() const
{
    return _type;
}
//-----------------------------------------------------------------------------
bool Property::implementation::isItMe(const Ptr<Universe::Node>& pNode)
{
    return false;
}
//-----------------------------------------------------------------------------
bool Property::implementation::isIntersecting(const Core::Rayf& ray, float& distance)
{
    return false;
}
//-----------------------------------------------------------------------------
bool Property::implementation::isTransformable() const
{
    return false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Property::Property(const Property& other) :
    _impl(new implementation(*(other._impl)))
{
}
//-----------------------------------------------------------------------------
Property::Property(const EPropertyType type) :
    _impl(new implementation(type))
{
}
//-----------------------------------------------------------------------------
Property::Property(const EPropertyType type, const Core::String name, bool isSelectable, bool isDeletable) :
    _impl(new implementation(type, name, isSelectable, isDeletable))
{
}
//-----------------------------------------------------------------------------
Property::~Property()
{
    delete _impl;
}
//-----------------------------------------------------------------------------
void Property::copy(const Ptr<Property>& pSrc)
{
    _impl->copy(*(pSrc->_impl));

    internalCopy(pSrc);

    emit propertyChanged();
}
//-----------------------------------------------------------------------------
int32 Property::getId() const
{
    return _impl->getId();
}
//-----------------------------------------------------------------------------
void Property::setId(int32 id)
{
    _impl->setId(id);
}
//-----------------------------------------------------------------------------
void Property::setDocId(const int32 docId)
{
    _impl->setDocId(docId);
}
//-----------------------------------------------------------------------------
const int32& Property::getDocId() const
{
    return _impl->getDocId();
}
//-----------------------------------------------------------------------------
bool Property::isItMe(const Ptr<Universe::Node>& pNode)
{
    return _impl->isItMe(pNode);
}
//-----------------------------------------------------------------------------
void Property::updateData()
{
    _impl->updateData();
}
//-----------------------------------------------------------------------------
void Property::updateProperty()
{
    _impl->updateProperty();
}
//-----------------------------------------------------------------------------
void Property::resurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    _impl->resurrect();
}
//-----------------------------------------------------------------------------
void Property::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    _impl->internalResurrect();
}
//-----------------------------------------------------------------------------
bool Property::isIntersecting(const Core::Rayf& ray, float& distance)
{
    return _impl->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
bool Property::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    return _impl->findPtyWithId(pResult, id);
}
//-----------------------------------------------------------------------------
bool Property::isSelectable() const
{
    return _impl->isSelectable();
}
//-----------------------------------------------------------------------------
void Property::setSelectable(bool isSelectable)
{
    _impl->_isSelectable = isSelectable;
}
//-----------------------------------------------------------------------------
void Property::setDeletable(bool flag)
{
    _impl->_isDeletable = flag;
}
//-----------------------------------------------------------------------------
bool Property::isDeletable() const
{
    return _impl->isDeletable();
}
//-----------------------------------------------------------------------------
bool Property::isEditable() const
{
    return _impl->isEditable();
}
//-----------------------------------------------------------------------------
bool Property::isTransformable () const
{
    return _impl->isTransformable();
}
//-----------------------------------------------------------------------------
EPropertyType Property::getType() const
{
    return _impl->getType();
}
//-----------------------------------------------------------------------------
const Core::String& Property::getName() const
{
    return _impl->getName();
}
//-----------------------------------------------------------------------------
void Property::setName(const Core::String& name)
{
    _impl->_name = name;
}
//-----------------------------------------------------------------------------
void Property::addChild(Ptr<Property> pChild, int32 indexPos)
{
    _impl->addChild(pChild, indexPos);
}
//-----------------------------------------------------------------------------
void Property::removeChild(Ptr<Property> pChild)
{
    _impl->removeChild(pChild);
}
//-----------------------------------------------------------------------------
void Property::removeAllChildren()
{
    ERR << L"Try to removeAllChildren in a property non-composite";
}
//-----------------------------------------------------------------------------
void Property::removeChild(const int32 index)
{
    _impl->removeChild(index);
}
//-----------------------------------------------------------------------------
int32 Property::getNbChildren() const
{
    return _impl->getNbChildren();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& Property::getChild(const int32 index) const
{
    return _impl->getChild(index);
}
//-----------------------------------------------------------------------------
Ptr<Property>& Property::getChild(const int32 index)
{
    return _impl->getChild(index);
}
//-----------------------------------------------------------------------------
const Property* Property::getParent() const
{
    return _impl->getParent();
}
//-----------------------------------------------------------------------------
Property* Property::getParent()
{
    return _impl->getParent();
}
//-----------------------------------------------------------------------------
void Property::setParent(Property* parent)
{
    _impl->setParent(parent);
} 
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> Property::createPropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PropertyWidget> pRet = internalCreatePropertyWidget(pDtaProxy, parent);
    connect(this, SIGNAL(propertyChanged()), pRet.get(), SLOT(updatePropertyWidget()));

    return pRet;
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> Property::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PropertyWidget> pRet;
    return pRet;
}
//-----------------------------------------------------------------------------
void Property::launchSignalProeprtyChanged(bool callParent)
{
    emit propertyChanged();

    if (callParent==true && getParent() != NULL)
    {
        getParent()->launchSignalProeprtyChanged();
    }
}
//-----------------------------------------------------------------------------
const Ptr<Property>& Property::getPtyTransform() const
{
    return _pBidon;
}
//-----------------------------------------------------------------------------
Ptr<Property>& Property::getPtyTransform()
{
    return _pBidon;
}
//-----------------------------------------------------------------------------
const Ptr<Property>& Property::getPtySelection() const
{
    return _pBidon;
}
//-----------------------------------------------------------------------------
Ptr<Property>& Property::getPtySelection()
{
    return _pBidon;
}
//-----------------------------------------------------------------------------
int32 Property::getFlags()
{
    return _impl->flags;
}
//-----------------------------------------------------------------------------
void Property::addFlag(EPtyFlagType  flag)
{
    switch(flag)
    {
    case FLAG_PTY_NODE: _impl->flags |= FLAG_PTY_NODE_VALUE;    break;
    case FLAG_PTY_EDITABLE_CONTENT: _impl->flags |= FLAG_PTY_EDITABLE_CONTENT_VALUE; break;
    case FLAG_PTY_TRANSFORM: _impl->flags |= FLAG_PTY_TRANSFORM;    break;
    }
}
//-----------------------------------------------------------------------------
bool Property::hasFlag(EPtyFlagType flag) const
{
    switch(flag)
    {
    case FLAG_PTY_NODE:             return (_impl->flags & FLAG_PTY_NODE_VALUE);    break;
    case FLAG_PTY_EDITABLE_CONTENT: return (_impl->flags & FLAG_PTY_EDITABLE_CONTENT_VALUE); break;
    case FLAG_PTY_TRANSFORM:        return (_impl->flags & FLAG_PTY_TRANSFORM); break;
    }

    return false;
}
//-----------------------------------------------------------------------------
String Property::toString() const
{
    String str;

    str << _impl->_id << L" " << _impl->_name;

    return str;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI bool ptiesAreInstance(const Core::List<Ptr<Property> >& pties)
{
    for(int32 ii=0; ii<pties.size(); ++ii)
    {
        if (pties[ii]->getCalssName() != PTYNODE_INSTANCE) return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
LM_API_EPI bool ptiesAreDeletable(const Core::List<Ptr<Property> >& pties)
{
    for(int32 ii=0; ii<pties.size(); ++ii)
    {
        if (pties[ii]->isDeletable() == false) return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI void cloneWithChild(Ptr<Property>& pPtyDst, const Property& ptySrc, const bool copyId)
{
    pPtyDst = ptySrc.clone();

    if (copyId == true)
    {
        //pPtyDst->setId(ptySrc.getId());
        pPtyDst->copyId(ptySrc);
    }

    for (int32 iChild=0; iChild<ptySrc.getNbChildren(); ++iChild)
    {
        Ptr<Property>   pChild;
        cloneWithChild(pChild, *ptySrc.getChild(iChild), copyId);
        pPtyDst->addChild(pChild);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI  Core::List<Ptr<Property> > removePropertyChildren(Core::List<Ptr<Property> >& pties)
{
    Core::List<Ptr<Property> > ret;

    for (int32 ii=0; ii<pties.size(); ++ii)
    {
        bool isPresent = false;
        for (int32 ij=0; ij<pties.size(); ++ij)
        {
            if (ii != ij)
            {
                if(pties[ii]->getParent() == pties[ij].get())
                {
                    isPresent = true;
                }
            }
            
        }

        if (isPresent==false)
        {
            ret.push_back(pties[ii]);
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
} // EPI