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
#include <Universe/NodeWaterPatch.h>

namespace Universe
{

NodeWaterPatch::NodeWaterPatch( const Ptr<Renderer::IAABox> & pBox,
                                const Ptr<Renderer::IWaterPatch> & pWaterPatch,
                                const Ptr<RessourcePool> & pRessourcesPool,
                                World * pWorld)
:   Node(pBox, pWorld),
    _pWaterPatch(pWaterPatch),
    _pRessourcesPool(pRessourcesPool)
{}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setLayerTexture(int32 id, const String & textureName)
{
    Ptr<Renderer::ITextureMap> pTexture = _pRessourcesPool->getTexture(textureName);
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    layer.pNormalMap = pTexture;
    _pWaterPatch->setLayer(id, layer);

    _textureNames[id] = textureName;
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setLayerScaleU(int32 id, float scaleU)
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    layer.scaleU = scaleU;
    _pWaterPatch->setLayer(id, layer);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setLayerScaleV(int32 id, float scaleV)
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    layer.scaleV = scaleV;
    _pWaterPatch->setLayer(id, layer);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setLayerSpeedU(int32 id, float speedU)
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    layer.speedU = speedU;
    _pWaterPatch->setLayer(id, layer);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setLayerSpeedV(int32 id, float speedV)
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    layer.speedV = speedV;
    _pWaterPatch->setLayer(id, layer);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setLayerAmplitude(int32 id, float amplitude)
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    layer.amplitude = amplitude;
    _pWaterPatch->setLayer(id, layer);
}
//-----------------------------------------------------------------------------
const String & NodeWaterPatch::getLayerTexture(int32 id) const
{
    return _textureNames[id];
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getLayerScaleU(int32 id) const
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    return layer.scaleU;
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setDiffuseTexture(const String & name)
{
    Ptr<Renderer::ITextureMap> pTexture = _pRessourcesPool->getTexture(name);
    _pWaterPatch->setDiffuseTexture(pTexture);
    _diffuseTexName = name;
}
//-----------------------------------------------------------------------------
const String & NodeWaterPatch::getDiffuseTexture() const
{
    return _diffuseTexName;
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getLayerScaleV(int32 id) const
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    return layer.scaleV;
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getLayerSpeedU(int32 id) const
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    return layer.speedU;
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getLayerSpeedV(int32 id) const
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    return layer.speedV;
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getLayerAmplitude(int32 id) const
{
    Renderer::WaterLayer layer;
    _pWaterPatch->getLayer(id, layer);
    return layer.amplitude;
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getWidth() const
{
    return _pWaterPatch->getWidth();
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getHeight() const
{
    return _pWaterPatch->getHeight();
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setSize(float width, float height)
{
    _pWaterPatch->setSize(width, height);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setDiffuse(const Core::Vector4f & color)
{
    _pWaterPatch->setDiffuse(color);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setSpecular(const Core::Vector4f & color)
{
    _pWaterPatch->setSpecular(color);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setShininess(float shininess)
{
    _pWaterPatch->setShininess(shininess);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setReflectionScale(float scale)
{
    _pWaterPatch->setReflectionScale(scale);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setRefractionScale(float scale)
{
    _pWaterPatch->setRefractionScale(scale);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setFresnelMin(float minReflection)
{
    _pWaterPatch->setFresnelMin(minReflection);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setFresnelPower(float power)
{
    _pWaterPatch->setFresnelPower(power);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeWaterPatch::getDiffuse() const
{
    return _pWaterPatch->getDiffuse();
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeWaterPatch::getSpecular() const
{
    return _pWaterPatch->getSpecular();
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getShininess() const
{
    return _pWaterPatch->getShininess();
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getReflectionScale() const
{
    return _pWaterPatch->getReflectionScale();
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getRefractionScale() const
{
    return _pWaterPatch->getRefractionScale();
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getFresnelMin() const
{
    return _pWaterPatch->getFresnelMin();
}
//-----------------------------------------------------------------------------
float NodeWaterPatch::getFresnelPower() const
{
    return _pWaterPatch->getFresnelPower();
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::onWorldMatrixUpdate()
{
    Core::Vector3f pos(getWorldPosition());
    _pWaterPatch->setPosition(Core::Vector2f(pos.x, pos.z));
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::onVisibilityChange(bool flag)
{
    _pWaterPatch->setVisible(flag);
}
//-----------------------------------------------------------------------------
bool NodeWaterPatch::isRefractive() const
{
    return _pWaterPatch->isRefractive();
}
//-----------------------------------------------------------------------------
bool NodeWaterPatch::isReflective() const
{
    return _pWaterPatch->isReflective();
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setRefractive(bool refractive)
{
    _pWaterPatch->setRefractive(refractive);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::setReflective(bool reflective)
{
    _pWaterPatch->setReflective(reflective);
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::onKill()
{
    _pWaterPatch->kill();
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeWaterPatch::internalGetBoundingBox() const
{
    return _pWaterPatch->getBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::importXML(const Ptr<Core::XMLNode> & pNode)
{
    bool success = true;

    Node::importXML(pNode);
    float width;
    float height;
    float shininess;
    float reflectionScale;
    float refractionScale;
    float fresnelMin;
    float fresnelPower;
    bool  refractive;
    bool  reflective;

    String strDiffuse;
    String strSpecular;
    String diffuseTexName;

    pNode->getValue(L"width",               width);
    pNode->getValue(L"height",              height);
    pNode->getValue(L"diffuse-texture",     diffuseTexName);
    pNode->getValue(L"diffuse",             strDiffuse);
    pNode->getValue(L"specular",            strSpecular);
    pNode->getValue(L"shininess",           shininess);
    pNode->getValue(L"reflection-scale",    reflectionScale);
    pNode->getValue(L"refraction-scale",    refractionScale);
    pNode->getValue(L"fresnel-min",         fresnelMin);
    pNode->getValue(L"fresnel-power",       fresnelPower);
    pNode->getValue(L"is-refractive",       refractive);
    pNode->getValue(L"is-reflective",       reflective);

    Core::Vector4f diffuse;
    Core::Vector4f specular;

    Core::fromXMLString(strDiffuse, diffuse);
    Core::fromXMLString(strSpecular, specular);

    setSize(width, height);
    setShininess(shininess);
    setReflectionScale(reflectionScale);
    setRefractionScale(refractionScale);
    setFresnelMin(fresnelMin);
    setFresnelPower(fresnelPower);
    setDiffuse(diffuse);
    setSpecular(specular);
    setRefractive(refractive);
    setReflective(reflective);
    setDiffuseTexture(diffuseTexName);

    Core::List<Ptr<Core::XMLNode> > layers;
    success &= pNode->getNodes(L"layers", L"layer", layers);

    if(layers.size() != Renderer::WATER_LAYER_COUNT)
    {
        Core::String message = L"The water patch layers in the XML file isn't right!\nExpecting : ";
        message << Renderer::WATER_LAYER_COUNT
                << L"\nFound :"
                << layers.size();
        throw Core::Exception(message);
    }

    for(int32 ii=0; ii<Renderer::WATER_LAYER_COUNT; ++ii)
    {
        String texture;
        float scaleU;
        float scaleV;
        float speedU;
        float speedV;
        float amplitude;
        Core::Ptr<Core::XMLNode> pXMLLayer = layers[ii];
        success &= pXMLLayer->getValue(L"texture", texture);
        success &= pXMLLayer->getValue(L"scale-u", scaleU);
        success &= pXMLLayer->getValue(L"scale-v", scaleV);
        success &= pXMLLayer->getValue(L"speed-u", speedU);
        success &= pXMLLayer->getValue(L"speed-v", speedV);
        success &= pXMLLayer->getValue(L"amplitude", amplitude);

        setLayerTexture(ii, texture);
        setLayerScaleU(ii, scaleU);
        setLayerScaleV(ii, scaleV);
        setLayerSpeedU(ii, speedU);
        setLayerSpeedV(ii, speedV);
        setLayerAmplitude(ii, amplitude);
    }

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML NodeWaterPatch values.");
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeWaterPatch::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"width",               getWidth());
    pNode->setValue(L"height",              getHeight());
    pNode->setValue(L"diffuse-texture",     _diffuseTexName);
    pNode->setValue(L"diffuse",             Core::toXMLString(getDiffuse()));
    pNode->setValue(L"specular",            Core::toXMLString(getSpecular()));
    pNode->setValue(L"shininess",           getShininess());
    pNode->setValue(L"is-refractive",       isRefractive());
    pNode->setValue(L"is-reflective",       isReflective());
    pNode->setValue(L"reflection-scale",    getReflectionScale());
    pNode->setValue(L"refraction-scale",    getRefractionScale());
    pNode->setValue(L"fresnel-min",         getFresnelMin());
    pNode->setValue(L"fresnel-power",       getFresnelPower());

    for(int32 ii=0; ii<Renderer::WATER_LAYER_COUNT; ++ii)
    {
        Core::Ptr<Core::XMLNode> pXMLLayer(new Core::XMLNode(L"layer"));
        pXMLLayer->setValue(L"texture", getLayerTexture(ii));
        pXMLLayer->setValue(L"scale-u", getLayerScaleU(ii));
        pXMLLayer->setValue(L"scale-v", getLayerScaleV(ii));
        pXMLLayer->setValue(L"speed-u", getLayerSpeedU(ii));
        pXMLLayer->setValue(L"speed-v", getLayerSpeedV(ii));
        pXMLLayer->setValue(L"amplitude", getLayerAmplitude(ii));
        pNode->addNode(L"layers", pXMLLayer);
    }
}
//-----------------------------------------------------------------------------
bool NodeWaterPatch::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{
    return isIntersectingBoundingBox(ray, distance);
}
//-----------------------------------------------------------------------------
}//namespace Universe
