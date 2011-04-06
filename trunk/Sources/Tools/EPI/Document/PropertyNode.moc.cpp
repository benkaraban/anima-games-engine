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
#include <EPI/Document/PropertyNode.moc.h>
#include <Core/List.h>
#include <Core/Logger.h>

#include <EPI/Document/Properties/PtyNode.moc.h>
namespace EPI
{
class PropertyNode::implementation
{
public:
    implementation(Core::List<Ptr<Property> > children) : _children(children) {}
    implementation()  {}
    ~implementation() {}

    const Core::List<Ptr<Property> >&   getChildren() const     {return _children;}
    Core::List<Ptr<Property> >&         getChildren()           {return _children;}

private:
    Core::List<Ptr<Property> >  _children;
};


//-----------------------------------------------------------------------------
PropertyNode::PropertyNode(const Core::List<Ptr<Property> >& children, const EPropertyType type):
    Property(type),
    _impl(new implementation(children))
{

}
//-----------------------------------------------------------------------------
PropertyNode::PropertyNode(const Core::String& name, bool isSelectable, bool isDeletable, const EPropertyType type):
    Property(type, name, isSelectable, isDeletable),
    _impl(new implementation())
{
}
//-----------------------------------------------------------------------------
PropertyNode::PropertyNode(const PropertyNode& other):
    Property(other),
    _impl(new implementation())
{
    //pour cloner les child utiliser la fonction cloneWithChild
    /*
    const int32 nbChildren = other.getNbChildren();

    Core::List<Ptr<Property> >::const_iterator iChild    = other.getChildren().begin();
    Core::List<Ptr<Property> >::const_iterator iChildEnd = other.getChildren().end();

    for (; iChild!=iChildEnd; ++iChild)
    {
        Ptr<Property> pChild = (**iChild).clone();
        addChild(pChild);
    }*/
}
//-----------------------------------------------------------------------------
PropertyNode::~PropertyNode()
{
    Core::List<Ptr<Property> >::iterator iChild = getChildren().begin();
    Core::List<Ptr<Property> >::iterator iChildEnd = getChildren().end();

    for (; iChild!=iChildEnd; ++iChild)
    {
        (*iChild)->setParent(NULL);
    }

    delete _impl;
}
//-----------------------------------------------------------------------------
void PropertyNode::removeAllChildren()
{
    for (int32 ii=0; ii<getNbChildren(); ++ii)
    {
        getChild(ii)->setParent(NULL);
    }
    getChildren().clear();
}
//-----------------------------------------------------------------------------
Ptr<Property> PropertyNode::clone() const
{
    Ptr<PropertyNode> pRet (new PropertyNode( *this ));
    return pRet;
}
//-----------------------------------------------------------------------------
void PropertyNode::copy(const Ptr<Property>& pSrc)
{
    Property::copy(pSrc);
}
//-----------------------------------------------------------------------------
void PropertyNode::internalCopy(const Ptr<Property>& pSrc)
{
    ERR << L"PropertyNode::copy not implemented ";
}
//-----------------------------------------------------------------------------
void PropertyNode::addChild(Ptr<Property> pChild, int32 indexPos)
{
    Core::List<Ptr<Property> >::iterator iChild = getChildren().begin();
    Core::List<Ptr<Property> >::iterator iChildEnd = getChildren().end();
/*
    for (; iChild!=iChildEnd; ++iChild)
    {
        //TODO gerer le cas ou l'on ajoute une propriete qui est deja contenu
        if ((**iChild) == (*pChild))
        {
            throw Core::Exception(L"PropertyNode::addChild(Ptr<Property> pChild) : Try to add an already existing child")
        }
    }*/

    pChild->setParent(this);
    pChild->setDocId(getDocId());

    if (indexPos==-1)
    {
        getChildren().push_back(pChild);
    }
    else
    {
        if (indexPos>getChildren().size())
        {
            ERR << L"PropertyNode::addChild: index trop grand : " << indexPos;
            indexPos=0;
        }

        
        Core::List<Ptr<Property> >::iterator iElt = getChildren().begin()+indexPos;
        getChildren().insert(iElt, pChild);
    }

   // pChild->updateProperty();
}
//-----------------------------------------------------------------------------
void PropertyNode::removeChild(Ptr<Property> pChild)
{
    bool childIsFind = false;

    int32 ii=0;
    for (ii=0; ii<getNbChildren(); ++ii)
    {
        if (getChild(ii) == pChild)
        {
            childIsFind = true;
            break;
        }
    }

    if (childIsFind==false)
    {
        throw Core::Exception(L"PropertyNode::removeChild(Ptr<Property> pChild) : Attempts to remove a child who doesn't exist");
    }
    else
    {
        pChild->setParent(NULL);
        getChildren().erase(getChildren().begin() + ii);
    }
}
//-----------------------------------------------------------------------------
void PropertyNode::removeChild(const int32 index)
{
    if ( (index < 0) || (index >= getNbChildren()) )
    {
        throw Core::Exception(L"PropertyNode::removeChild(const int32 index) : index isn't in range" );
    }

    Core::List<Ptr<Property> >::iterator iChild = getChildren().begin();
    for (int32 ii=0; ii<index; ii++)
    {
        ++iChild;
    }

    (*iChild)->setParent(NULL);
    getChildren().erase(iChild);
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PropertyNode::getChild(const int32 index) const
{
    if ( (index < 0) || (index >= getNbChildren()) )
    {
        throw Core::Exception(L"PropertyNode::getChild(const int32 index) : index isn't in range" );
    }
   
    return getChildren()[index];
}
//-----------------------------------------------------------------------------
Ptr<Property>& PropertyNode::getChild(const int32 index)
{
    if ( (index < 0) || (index >= getNbChildren()) )
    {
        throw Core::Exception(L"PropertyNode::getChild(const int32 index) : index isn't in range" );
    }
   
    return getChildren()[index];
}
//-----------------------------------------------------------------------------
int32 PropertyNode::getNbChildren() const
{
    return getChildren().size();
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& PropertyNode::getChildren() const
{
    return _impl->getChildren();
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Property> >& PropertyNode::getChildren()
{
    return _impl->getChildren();
}
//-----------------------------------------------------------------------------
void PropertyNode::setDocId(const int32 docId)
{
    Property::setDocId(docId);

    Core::List<Ptr<Property> >::iterator iChild = getChildren().begin();
    Core::List<Ptr<Property> >::iterator iChildEnd = getChildren().end();

    for (; iChild!=iChildEnd; ++iChild)
    {
        (*iChild)->setDocId(docId);
    }
}
//-----------------------------------------------------------------------------
bool PropertyNode::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    LM_ASSERT(getId() != id);

    bool ret = false;

    //Ptr<Property> pPty = getChildRecursive2(*this, id);
    Core::List<Ptr<Property> >::const_iterator itPy = getChildren().begin();
    Core::List<Ptr<Property> >::const_iterator itPyEnd = getChildren().end();

    while(itPy!=itPyEnd && ret==false)
    {
        if ((*itPy)->getId() == id)
        {
            pResult = (*itPy);
            ret = true;
        }
        else
        {
            ret = (*itPy)->findPtyWithId(pResult, id);
        }

        ++itPy;
    }

    return ret;
}
//-----------------------------------------------------------------------------
void PropertyNode::resurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    internalResurrect(pWorld, pWorldInfoContent, pty);
    for(int32 iChild=0; iChild<getNbChildren(); iChild++)
    {
        getChild(iChild)->resurrect(pWorld, pWorldInfoContent, pty);
        getChild(iChild)->updateData();
    }
    updateData();
}
//-----------------------------------------------------------------------------
void  PropertyNode::restart()
{
    for(int32 iChild=0; iChild<getNbChildren(); iChild++)
    {
        getChild(iChild)->restart();
        getChild(iChild)->internalRestart();
    }

    internalRestart();
}
//-----------------------------------------------------------------------------
void PropertyNode::stop()
{
    for(int32 iChild=0; iChild<getNbChildren(); iChild++)
    {
        getChild(iChild)->stop();
        getChild(iChild)->internalStop();
    }

    internalStop();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Ptr<Property>   travelProperties(Ptr<Property> pPty, int32 id)
{
    Ptr<PropertyNode> pPyNode = LM_DEBUG_PTR_CAST<PropertyNode> (pPty);

    if(pPyNode->getId() == id)
    {
        return pPyNode;
    }
    else
    {
        if(pPyNode->getNbChildren()>0)
        {
            Core::List<Ptr<Property> >::const_iterator itPy = pPyNode->getChildren().begin();
            Core::List<Ptr<Property> >::const_iterator itPyEnd = pPyNode->getChildren().end();

            while(itPy!=itPyEnd)
            {
                Ptr<Property> ret = travelProperties((*itPy), id);
                if (ret != null)
                {
                    return ret;
                }
                ++itPy;
            }
        }
        else
        {
            return null;
        }
    }

    return null;
}

//-----------------------------------------------------------------------------
LM_API_EPI Ptr<Property>  getChildRecursive2(const PropertyNode& pty, int32 id)
{
    LM_ASSERT(pty.getId() != id);   //pty ne doit pas etre la property recherche

    Core::List<Ptr<Property> >::const_iterator itPy = pty.getChildren().begin();
    Core::List<Ptr<Property> >::const_iterator itPyEnd = pty.getChildren().end();

    while(itPy!=itPyEnd)
    {
        Ptr<Property> ret = travelProperties((*itPy), id);
        if (ret != null)
        {
            return ret;
        }
        ++itPy;
    }

    return null;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void intersect_RayPropertyNodeRecursive_travel(const Ptr<Property>& pPropertyNode, const Core::Rayf& ray, float& distance, Ptr<Property>& pPtyIntersecting)
{
    float distanceTest;
    Ptr<Property> pPtyIntersectingTest;

   // if (pPropertyNode->getNbChildren()==0)
    {
        if (pPropertyNode->isIntersecting(ray, distanceTest)==true)
        {
            if (distanceTest < distance)
            {
                distance = distanceTest;
                pPtyIntersecting = pPropertyNode;
            }
        }
    }

    for (int32 iChild=0; iChild<pPropertyNode->getNbChildren(); ++iChild)
    {
        intersect_RayPropertyNodeRecursive_travel(pPropertyNode->getChild(iChild), ray, distance, pPtyIntersecting);
/*
        if (pPtyIntersectingTest!=null && distanceTest < distance)
        {
            distance = distanceTest;
            pPtyIntersecting = pPtyIntersectingTest;
        }*/
    }

}

LM_API_EPI void intersect_RayPropertyNodeRecursive(const Property& pPtyRoot, const Core::Rayf& ray, float& distance, Ptr<Property>& pPtyIntersecting)
{
    pPtyIntersecting = null;
    distance = FLT_MAX;

    for (int32 iChild=0; iChild<pPtyRoot.getNbChildren(); ++iChild)
    {
        intersect_RayPropertyNodeRecursive_travel(pPtyRoot.getChild(iChild), ray, distance, pPtyIntersecting);
    }

    if (pPtyRoot.hasFlag(FLAG_PTY_EDITABLE_CONTENT))
    {
       const Core::List<Ptr<Property> >& ptiesContent =  static_cast<const PtyNodeEditableContent&> (pPtyRoot).getPtiesContentSelectable();
       for (int32 iPty=0; iPty<ptiesContent.size(); ++iPty)
       {
            intersect_RayPropertyNodeRecursive_travel(ptiesContent[iPty], ray, distance, pPtyIntersecting);
       }
    }
}


} // namespace EPI