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
#include <Universe/NodePointLight.h>
#include <Core/XML/XMLMath.h>

namespace Universe
{
//------------------------------------------------------------------------------
NodePointLight::NodePointLight(const Ptr<Renderer::IAABox> & pBox,
                               const Ptr<Renderer::IPointLight> & pLight,
                               World * pWorld) :
    Node(pBox, pWorld),
    _pLight(pLight),
    _mainColor(Core::Vector4f::ONE),
    _mainGlow(Core::Vector4f::ONE),
    _turnOnLatency(0.5f),
    _turnOffLatency(0.5f),
    _burst(false),
    _isStatic(false),
    _stopProgrammed(false),
    _colorChangeInProgress(false),
    _color1(Core::Vector4f::ONE),
    _color2(Core::Vector4f::ONE),
    _elapsed(0.0f),
    _colorChangeTime(0.0f),
    _remainingLifeTime(0.0f)
{
}
//------------------------------------------------------------------------------
void NodePointLight::onWorldMatrixUpdate()
{
    _pLight->setPosition(getWorldPosition());
}
//-----------------------------------------------------------------------------
void NodePointLight::onVisibilityChange(bool flag)
{
    _pLight->setVisible(flag);
}
//-----------------------------------------------------------------------------
void NodePointLight::onKill()
{
    _pLight->kill();
}
//-----------------------------------------------------------------------------
void NodePointLight::setRadius(float radius)
{
    _radius = radius;
    updateLight();
}
//-----------------------------------------------------------------------------
void NodePointLight::setAmbientColor(const Core::Vector4f & color) 
{ 
    _ambient = color;
    updateLight();
}
//-----------------------------------------------------------------------------
void NodePointLight::setDiffuseColor(const Core::Vector4f & color)
{
    _diffuse = color;
    updateLight();
}
//-----------------------------------------------------------------------------
void NodePointLight::setSpecularColor(const Core::Vector4f & color)
{ 
    _specular = color;
    updateLight();
};
//-----------------------------------------------------------------------------
void NodePointLight::updateLight()
{
    _pLight->setAmbientColor(Core::mul(_mainColor, _ambient));
    _pLight->setDiffuseColor(Core::mul(_mainColor, _diffuse));
    _pLight->setSpecularColor(Core::mul(_mainColor, _specular));
    _pLight->setRadius(_radius);
}
//-----------------------------------------------------------------------------
void NodePointLight::setNodeColor(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);
    _mainColor = color;
    updateLight();
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodePointLight::getNodeColor() const
{
    return _mainColor;
}
//-----------------------------------------------------------------------------
void NodePointLight::restart(bool recursive)
{
    Node::restart(recursive);

    _color1 = Core::Vector4f::ZERO;
    _color2 = Core::Vector4f::ONE;
    _colorChangeTime = _turnOnLatency;
    _elapsed = 0.0f;
    _stopProgrammed = false;
    _colorChangeInProgress = true;
    updateLifeTime(0.0);
}
//-----------------------------------------------------------------------------
void NodePointLight::stop(bool recursive, float inTime)
{
    Node::stop(recursive, inTime);

    _stopProgrammed = true;
    _remainingLifeTime = inTime;
    updateLifeTime(0.0);
}
//-----------------------------------------------------------------------------
void NodePointLight::updateLifeTime(double elapsed)
{
    if(_stopProgrammed)
    {
        _remainingLifeTime -= float(elapsed);
        
        if(_remainingLifeTime < 0.0f)
        {
            _color1 = Core::Vector4f::ONE;
            _color2 = Core::Vector4f::ZERO;
            _colorChangeTime = _turnOffLatency;
            _elapsed = 0.0f;
            _colorChangeInProgress = true;
            _stopProgrammed = false;
        }
    }

    if(_colorChangeInProgress)
    {
        _elapsed += float(elapsed);
        float a = Core::clamp(0.0f, 1.0f, _elapsed / _colorChangeTime);
        _mainColor.setLerp(_color1, _color2, a);
        updateLight();

        if(a >= 1.0f)
        {
            _colorChangeInProgress = false;

            if(_burst && _color2 == Core::Vector4f::ONE)
                stop(false, 0.0);
        }
    }
}
//-----------------------------------------------------------------------------
void NodePointLight::update(double elapsed)
{
    Node::update(elapsed);
    updateLifeTime(elapsed);
}
//-----------------------------------------------------------------------------
void NodePointLight::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodePointLight::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    bool shadowCast = false;

    _mainColor = _mainGlow = Core::Vector4f::ONE;
    _stopProgrammed = false;
    _colorChangeInProgress = false;
    _color1 = Core::Vector4f::ONE;
    _color2 = Core::Vector4f::ONE;
    _elapsed = 0.0f;
    _colorChangeTime = 0.0f;
    _remainingLifeTime = 0.0f;
    _isStatic = false;

    Core::getXMLValue(*pNode, L"shadow-cast", shadowCast);
    Core::getXMLValue(*pNode, L"ambient", _ambient);
    Core::getXMLValue(*pNode, L"diffuse", _diffuse);
    Core::getXMLValue(*pNode, L"specular", _specular);
    Core::getXMLValue(*pNode, L"radius", _radius);
    Core::getXMLValue(*pNode, L"turn-on-latency", _turnOnLatency);
    Core::getXMLValue(*pNode, L"turn-off-latency", _turnOffLatency);
    Core::getXMLValue(*pNode, L"is-burst", _burst);
    Core::getXMLValue(*pNode, L"is-static", _isStatic);
    
    restart();
    _pLight->setShadowCasting(shadowCast);
    updateLight();
}
//-----------------------------------------------------------------------------
void NodePointLight::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"shadow-cast", getShadowCasting());
    pNode->setValue(L"ambient", Core::toXMLString(getAmbientColor()));
    pNode->setValue(L"diffuse", Core::toXMLString(getDiffuseColor()));
    pNode->setValue(L"specular", Core::toXMLString(getSpecularColor()));
    pNode->setValue(L"is-static", _isStatic);
    pNode->setValue(L"radius", getRadius());
    pNode->setValue(L"turn-on-latency", _turnOnLatency);
    pNode->setValue(L"turn-off-latency", _turnOffLatency);
    pNode->setValue(L"is-burst", _burst);
}
//-----------------------------------------------------------------------------
}
