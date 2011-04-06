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
#include "XMLNode.h"

#include <Core/Strings/Tools.h>

namespace Core
{
XMLNode::XMLNode(const String & name, const String & delimiter)
:   _name(name),
    _delimiter(delimiter)
{
}

XMLNode::~XMLNode()
{

}

Ptr<XMLNode> XMLNode::getNode(const String & path) const
{
    String childName;
    String newPath;
    getChildNameAndNewPath(path, childName, newPath);

    Ptr<XMLNode> pChild(firstChildNode(childName));

    if(pChild == null)
    {
        return null;
    }
    else if(newPath == L"")
    {
        return pChild;
    }
    else
    {
        return pChild->getNode(newPath);
    }
}

Ptr<XMLNode> XMLNode::firstChildNode(const String & name) const
{
    Ptr<XMLNode> pResult = null;
    int32 ii = 0;

    while(ii < _children.size() && _children[ii]->getName() != name)
        ii++;

    if(ii < _children.size())
        pResult = _children[ii];

    return pResult;
}

bool XMLNode::setValue( const String & path,
                        const String & value)
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            setValue(value);
            returnCode = true;
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild == null)
            {
                pChild = Ptr<XMLNode>(new XMLNode(childName, _delimiter));
                _children.push_back(pChild);
            }
            returnCode = pChild->setValue(newPath, value);
        }
    }

    return returnCode;
}


void XMLNode::setValue(const String & value)
{
    _value = value;
}

void XMLNode::setAttribute(const String & name, const String & value)
{
    _attributes[name] = value;
}

bool XMLNode::addNode(const Ptr<XMLNode> & pChild)
{
    bool result = acceptChild();
    if(result)
        _children.push_back(pChild);
    return result;
}

const String & XMLNode::getName() const
{
    return _name;
}

void XMLNode::getValue(String & value) const
{
    value = _value;
}

bool XMLNode::getAttribute(const String & name, String & value) const
{
    Map<String, String>::const_iterator iAttribute = _attributes.find(name);
    bool result = iAttribute != _attributes.end();

    if(result)
        value = iAttribute->second;
    else
        value = String::EMPTY;

    return result;
}

void XMLNode::getNodes(const String & name, List<Ptr<XMLNode> > & children) const
{
    children.clear();

	for(List<Ptr<XMLNode> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
    {
        if((*ipChild)->getName() == name)
            children.push_back(*ipChild);
    }
}

void XMLNode::getAllNodes(const String & path, List<Ptr<XMLNode> > & children) const
{
    if(path == L"")
    {
        children = _children;
    }
    else
    {
        String childName;
        String newPath;
        getChildNameAndNewPath(path, childName, newPath);

        Ptr<XMLNode> pChild(firstChildNode(childName));

        if(pChild != null)
            pChild->getAllNodes(newPath, children);
    }
}

bool XMLNode::setValues(const String & path,
                        const List<String> & values)
{
    bool returnCode = false;

    if(isPathValid(path) && path != L"")
    {
        String childName;
        String newPath;
        getChildNameAndNewPath(path, childName, newPath);

        if(newPath == L"")
        {
			for(List<String>::const_iterator iValue = values.begin(); iValue != values.end(); ++iValue)
            {
                Ptr<XMLNode> pChild(new XMLNode(childName));
                pChild->setValue(*iValue);
                addNode(pChild);
            }
            returnCode = true;
        }
        else
        {
            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild == null)
            {
                pChild = Ptr<XMLNode>(new XMLNode(childName, _delimiter));
                _children.push_back(pChild);
            }
            returnCode = pChild->setValues(newPath, values);
        }
    }

    return returnCode;
}


bool XMLNode::setValue( const String & path,
                        int32 value)
{
    return setValue(path, Core::toString(value));
}

bool XMLNode::setValue( const String & path,
                        int64 value)
{
    return setValue(path, Core::toString(value));
}

bool XMLNode::setValue( const String & path,
                        const TimeValue & value)
{
    return setValue(path, value._time);
}

bool XMLNode::setValue( const String & path,
                        bool value)
{
    return setValue(path, int32(value ? 1 : 0));
}

bool XMLNode::setValue( const String & path,
                        double value)
{
    return setValue(path, Core::toString(value));
}

bool XMLNode::setAttribute( const String & path,
                            const String & attributeName,
                            const String & attributeValue,
                            const String & specificValue)
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            setAttribute(attributeName, attributeValue);
            returnCode = true;
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild == null)
            {
                pChild = Ptr<XMLNode>(new XMLNode(childName, _delimiter));
                _children.push_back(pChild);
            }
            returnCode = pChild->setAttribute(newPath, attributeName, attributeValue, specificValue);
        }
    }

    return returnCode;
}

bool XMLNode::setAttribute( const String & path,
                            const String & attributeName,
                            int32 attributeValue,
                            const String & specificValue)
{
    return setAttribute(path, attributeName, Core::toString(attributeValue), specificValue);
}

bool XMLNode::setAttribute( const String & path,
                            const String & attributeName,
                            double attributeValue,
                            const String & specificValue)
{
    return setAttribute(path, attributeName, Core::toString(attributeValue), specificValue);
}

bool XMLNode::getValue( const String & path,
                        String & value) const
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            getValue(value);
            returnCode = true;
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild != null)
                returnCode = pChild->getValue(newPath, value);
        }
    }

    return returnCode;
}

bool XMLNode::getValues(const String & path,
                        List<String> & values) const
{
    bool returnCode = false;

    if(isPathValid(path) && path != L"")
    {
        String childName;
        String newPath;
        getChildNameAndNewPath(path, childName, newPath);

        if(newPath == L"")
        {
            values.clear();

			for(List<Ptr<XMLNode> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
            {
                if((*ipChild)->getName() == childName)
                {
                    String value;
                    (*ipChild)->getValue(value);
                    values.push_back(value);
                }
            }
            returnCode = values.size() > 0;
        }
        else
        {
            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild != null)
                returnCode = pChild->getValues(newPath, values);
        }
    }

    return returnCode;
}


bool XMLNode::getValue( const String & path,
                        int32 & value) const
{
    String stringValue;
    bool returnCode = getValue(path, stringValue);
    if(returnCode)
        value = toInteger(stringValue);

    return returnCode;
}

bool XMLNode::getValue( const String & path,
                        int64 & value) const
{
    String stringValue;
    bool returnCode = getValue(path, stringValue);
    if(returnCode)
        value = toInteger64(stringValue);

    return returnCode;
}

bool XMLNode::getValue( const String & path,
                        TimeValue & value) const
{
    String stringValue;
    bool returnCode = getValue(path, stringValue);
    if(returnCode)
        value._time = toInteger64(stringValue);

    return returnCode;
}

bool XMLNode::getValue( const String & path,
                        bool & value) const
{
    String stringValue;
    bool returnCode = getValue(path, stringValue);
    if(returnCode)
        value = (Core::toInteger(stringValue) != 0);

    return returnCode;
}

bool XMLNode::getValue( const String & path,
                        double & value) const
{
    String stringValue;
    bool returnCode = getValue(path, stringValue);
    if(returnCode)
        value = toFloat(stringValue);

    return returnCode;
}

bool XMLNode::getValue( const String & path,
                        float & value) const
{
    String stringValue;
    bool returnCode = getValue(path, stringValue);
    if(returnCode)
        value = (float)toFloat(stringValue);

    return returnCode;
}

bool XMLNode::getAttribute( const String & path,
                            const String & attributeName,
                            String & attributeValue,
                            const String & specificValue)
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            returnCode = getAttribute(attributeName, attributeValue);
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild != null)
                returnCode = pChild->getAttribute(newPath, attributeName, attributeValue, specificValue);
        }
    }

    return returnCode;
}

bool XMLNode::getAttribute( const String & path,
                            const String & attributeName,
                            int32 & attributeValue,
                            const String & specificValue)
{
    String stringValue;
    bool returnCode = getAttribute(path, attributeName, stringValue, specificValue);
    if(returnCode)
        attributeValue = toInteger(stringValue);

    return returnCode;
}

bool XMLNode::getAttribute( const String & path,
                            const String & attributeName,
                            double & attributeValue,
                            const String & specificValue)
{
    String stringValue;
    bool returnCode = getAttribute(path, attributeName, stringValue, specificValue);
    if(returnCode)
        attributeValue = toFloat(stringValue);

    return returnCode;
}

void XMLNode::deleteNodes(const String & name)
{
    List<Ptr<XMLNode> >::iterator iChild = _children.begin();
    while(iChild != _children.end())
    {
        if((*iChild)->getName() == name)
            iChild = _children.erase(iChild);
        else
            ++iChild;
    }
}



bool XMLNode::deleteNode(   const String & path,
                            const String & nodeName)
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            deleteNodes(nodeName);
            returnCode = true;
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild != null)
            {
                returnCode = pChild->deleteNode(newPath, nodeName);
            }
        }
    }

    return returnCode;
}

bool XMLNode::deleteAttribute(const String & name)
{
    Map<String, String>::iterator iAttribute = _attributes.find(name);
    bool result = iAttribute != _attributes.end();

    if(result)
        _attributes.erase(iAttribute);

    return result;
}

bool XMLNode::deleteAttribute(  const String & path,
                                const String & attributeName,
                                const String & specificValue)
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            deleteAttribute(attributeName);
            returnCode = true;
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild != null)
            {
                returnCode = pChild->deleteAttribute(newPath, attributeName, specificValue);
            }
        }
    }
    return returnCode;
}

bool XMLNode::addNode(  const String & path,
                        const Ptr<XMLNode> & node)
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            addNode(node);
            returnCode = true;
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild == null)
            {
                pChild = Ptr<XMLNode>(new XMLNode(childName, _delimiter));
                _children.push_back(pChild);
            }
            returnCode = pChild->addNode(newPath, node);
        }
    }
    return returnCode;
}

bool XMLNode::getNodes( const String & path,
                        const String & nodeName,
                        List<Ptr<XMLNode> > & nodes) const
{
    bool returnCode = false;

    if( isPathValid(path) )
    {
        if(path == L"")//On est arrivé au noeud cible
        {
            getNodes(nodeName, nodes);
            returnCode = true;
        }
        else
        {
            String childName;
            String newPath;
            getChildNameAndNewPath(path, childName, newPath);

            Ptr<XMLNode> pChild(firstChildNode(childName));

            if(pChild != null)
            {
                returnCode = pChild->getNodes(newPath, nodeName, nodes);
            }
        }
    }

    return returnCode;
}

void XMLNode::getChildNameAndNewPath(   const String & path,
                                        String & childName,
                                        String & newPath) const
{
    int32 index = path.find(_delimiter);
    if(index == String::npos)
    {
        childName = path;
        newPath = L"";
    }
    else
    {
        childName = path.slice(0, index-1);
        newPath = path.slice(index+1);
    }
}

bool XMLNode::isPathValid(const String & path) const
{
    if(path == L"")
        return true;

    List<String> tokens;
    tokenize(path, tokens, _delimiter);

    List<String>::const_iterator iToken = tokens.begin();

    while(iToken != tokens.end() && (*iToken).length()>0 )
    {
        ++iToken;
    }
    return iToken == tokens.end();
}

bool XMLNode::acceptChild() const
{
    return true;
}

}//namespace Core
