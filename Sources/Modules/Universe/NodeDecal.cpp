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
#include <Core/XML/XMLMath.h>
#include <Universe/NodeDecal.h>
#include <Universe/NodeCamera.h>

namespace Universe
{
//-----------------------------------------------------------------------------
NodeDecal::NodeDecal(const Ptr<Renderer::IAABox> & pBox,
                     const Ptr<Renderer::IDecal> & pDecal,
                     const Ptr<RessourcePool> & pRessourcesPool, World * pWorld) :
    Node(pBox, pWorld),
    _pDecal(pDecal),
    _pRessourcesPool(pRessourcesPool),
    _decalOrientMat(Core::IDENTITY),
    _billboardOn(false),
    _animOn(false),
    _animLoop(false),
    _animKeyFrom(0.0f),
    _animKeyTo(1.0f),
    _animDuration(0.0)
{
}
//-----------------------------------------------------------------------------
void NodeDecal::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeDecal::setTexture(const String & textureName)
{
    Ptr<Renderer::ITextureMap> pTexture;

    if(textureName == String::EMPTY)
        pTexture = _pRessourcesPool->getDefaultTexture();
    else
        pTexture = _pRessourcesPool->getTexture(textureName);

    _textureName = textureName;
    _pDecal->setTexture(pTexture);
}
//-----------------------------------------------------------------------------
void NodeDecal::setSize(float width, float height)
{
    _pDecal->setSize(width, height);
}
//-----------------------------------------------------------------------------
float NodeDecal::getWidth() const
{
    return _pDecal->getWidth();
}
//-----------------------------------------------------------------------------
float NodeDecal::getHeight() const
{
    return _pDecal->getHeight();
}
//-----------------------------------------------------------------------------
void NodeDecal::setDecalNormal(Renderer::EDecalNormal n)
{
    _pDecal->setDecalNormal(n);
}
//-----------------------------------------------------------------------------
Renderer::EDecalNormal NodeDecal::getDecalNormal() const
{
    return _pDecal->getDecalNormal();
}
//-----------------------------------------------------------------------------
void NodeDecal::setMode(Renderer::EDecalMode mode)
{
    _pDecal->setMode(mode);
}
//-----------------------------------------------------------------------------
Renderer::EDecalMode NodeDecal::getMode() const
{
    return _pDecal->getMode();
}
//-----------------------------------------------------------------------------
void NodeDecal::setSize(float size)
{
    _pDecal->setSize(size);
}
//-----------------------------------------------------------------------------
void NodeDecal::setColor(const Core::Vector4f & color)
{
    _pDecal->setColor(color);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeDecal::getColor() const
{
    return _pDecal->getColor();
}
//-----------------------------------------------------------------------------
void NodeDecal::setGlow(const Core::Vector4f & color)
{
    _pDecal->setGlow(color);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeDecal::getGlow() const
{
    return _pDecal->getGlow();
}
//-----------------------------------------------------------------------------
void NodeDecal::setNodeColor(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);
    setColor(color);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeDecal::getNodeColor() const
{
    return getColor();
}
//-----------------------------------------------------------------------------
void NodeDecal::setNodeGlow(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);
    setGlow(color);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeDecal::getNodeGlow() const
{
    return getGlow();
}
//-----------------------------------------------------------------------------
void NodeDecal::setTimeKey(float k)
{
    _pDecal->setTimeKey(k);
}
//-----------------------------------------------------------------------------
float NodeDecal::getTimeKey() const
{
    return _pDecal->getTimeKey();
}
//-----------------------------------------------------------------------------
void NodeDecal::onWorldMatrixUpdate()
{
    _pDecal->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
void NodeDecal::onVisibilityChange(bool flag)
{
    _pDecal->setVisible(flag);
}
//-----------------------------------------------------------------------------
void NodeDecal::onKill()
{
    _pDecal->kill();
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeDecal::internalGetBoundingBox() const
{
    return _pDecal->getBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeDecal::animateSprite(double duration, float keyFrom, float keyTo, bool loop)
{
    _animOn = true;
    _animDuration = std::max(duration, 0.001);
    _animKeyFrom = keyFrom;
    _animKeyTo = keyTo;
    _animLoop = loop;
    setTimeKey(_animKeyFrom);
}
//-----------------------------------------------------------------------------
double NodeDecal::getDuration() const
{
    return _animDuration;
}
//-----------------------------------------------------------------------------
float NodeDecal::getKeyFrom() const
{
    return _animKeyFrom;
}
//-----------------------------------------------------------------------------
float NodeDecal::getKeyTo() const
{
    return _animKeyTo;
}
//-----------------------------------------------------------------------------
bool NodeDecal::getLoop() const
{
    return _animLoop;
}
//-----------------------------------------------------------------------------
void NodeDecal::setBillboard(bool b)
{
    _billboardOn = b;
}
//-----------------------------------------------------------------------------
bool NodeDecal::isBillboard() const
{
    return _billboardOn;
}
//-----------------------------------------------------------------------------
void NodeDecal::update(double elapsed)
{
    Node::update(elapsed);
    
    if(_animOn)
    {
        float keyRange = _animKeyTo - _animKeyFrom;
        float alpha = (getTimeKey() - _animKeyFrom) + float(keyRange * elapsed / _animDuration);

        if(_animLoop)
        {
            alpha = float(fmod(alpha, keyRange));
        }
        else
        {
            alpha = Core::clamp(0.0f, keyRange, alpha);
            _animOn = (alpha < keyRange);
        }

        setTimeKey(_animKeyFrom + alpha);
    }
}
//-----------------------------------------------------------------------------
void NodeDecal::prepareForRendering(const Ptr<NodeCamera> & pCamera)
{
    

    if(_billboardOn)
    {
        Core::Vector3f cam((pCamera->getWorldPosition() - this->getWorldPosition()).getNormalizedSafe());

        switch(getDecalNormal())
        {
        case Renderer::DECAL_NORMAL_Y:
        {
            setUpVector(cam);
            setWorldDirection(getWorldDirection());
            break;
        }
        case Renderer::DECAL_NORMAL_Z:
        {
            setUpVector(pCamera->getLocalToWorldMatrix().getYVector());
            setWorldDirection(pCamera->getWorldDirection());
            break;
        }
        }
    }

    Node::prepareForRendering(pCamera);
}
//-----------------------------------------------------------------------------
void NodeDecal::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    String textureName;
    String mode;
    String normal;
    String strColor;
    String strGlow;
    Core::Vector4f color;
    Core::Vector4f glow;
    float  width;
    float  height;

    int32 uSpriteLayout = 1;
    int32 vSpriteLayout = 1;

    pNode->getValue(L"texture", textureName);
    pNode->getValue(L"width", width);
    pNode->getValue(L"height", height);
    pNode->getValue(L"color", strColor);
    if(!pNode->getValue(L"glow", strGlow))
        strGlow = Core::toXMLString(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    pNode->getValue(L"mode", mode);
    pNode->getValue(L"normal", normal);
    pNode->getValue(L"u-sprite-layout", uSpriteLayout);
    pNode->getValue(L"v-sprite-layout", vSpriteLayout);
    pNode->getValue(L"billboard", _billboardOn);

    setSpriteLayout(uSpriteLayout, vSpriteLayout);

    Core::fromXMLString(strColor, color);
    Core::fromXMLString(strGlow, glow);

    setTexture(textureName);
    _pDecal->setSize(width, height);
    _pDecal->setColor(color);
    _pDecal->setGlow(glow);

    Ptr<Core::XMLNode> pAnim(pNode->firstChildNode(L"anim"));

    _animOn = (pAnim != null);
    if(_animOn)
    {
        pAnim->getValue(L"loop", _animLoop);
        pAnim->getValue(L"key-from", _animKeyFrom);
        pAnim->getValue(L"key-to", _animKeyTo);
        pAnim->getValue(L"duration", _animDuration);
    }

    float zBias = 0.0f;
    float fogFactor = 1.0f;

    if(!pNode->getValue(L"zbias", zBias))
        zBias = 0.0f;

    if(!pNode->getValue(L"fog-factor", fogFactor))
        fogFactor = 1.0f;

    setZBias(zBias);
    setFogFactor(fogFactor);

    if(mode == Renderer::toString(Renderer::DECAL_LERP)) _pDecal->setMode(Renderer::DECAL_LERP);
    else if(mode == Renderer::toString(Renderer::DECAL_ADD)) _pDecal->setMode(Renderer::DECAL_ADD);
    else if(mode == Renderer::toString(Renderer::DECAL_REFRAC)) _pDecal->setMode(Renderer::DECAL_REFRAC);
    else
        throw Core::Exception(L"Unknown decal mode : " + mode);

    if(normal == Renderer::toString(Renderer::DECAL_NORMAL_Y)) _pDecal->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    else _pDecal->setDecalNormal(Renderer::DECAL_NORMAL_Z);
}
//-----------------------------------------------------------------------------
void NodeDecal::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"texture", _textureName);
    pNode->setValue(L"width", _pDecal->getWidth());
    pNode->setValue(L"height", _pDecal->getHeight());
    pNode->setValue(L"color", Core::toXMLString(_pDecal->getColor()));
    pNode->setValue(L"glow", Core::toXMLString(_pDecal->getGlow()));
    pNode->setValue(L"mode", Renderer::toString(_pDecal->getMode()));
    pNode->setValue(L"normal", Renderer::toString(_pDecal->getDecalNormal()));
    pNode->setValue(L"u-sprite-layout", getUSpriteLayout());
    pNode->setValue(L"v-sprite-layout", getVSpriteLayout());
    pNode->setValue(L"billboard", _billboardOn);
    pNode->setValue(L"zbias", getZBias());
    pNode->setValue(L"fog-factor", getFogFactor());

    if(_animOn)
    {
        pNode->setValue(L"anim.loop", _animLoop);
        pNode->setValue(L"anim.key-from", _animKeyFrom);
        pNode->setValue(L"anim.key-to", _animKeyTo);
        pNode->setValue(L"anim.duration", _animDuration);
    }
}
//-----------------------------------------------------------------------------
bool NodeDecal::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{
    //code temporaire pour le moment on test uniquement le bbox
    return isIntersectingBoundingBox(ray, distance);
}
//-----------------------------------------------------------------------------
}
