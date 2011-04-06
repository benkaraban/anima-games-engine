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
#include <Core/logger.h>
#include <Core/XML/XMLMath.h>
#include <Universe/NodeMesh.h>

namespace Universe
{
//-----------------------------------------------------------------------------
NodeMesh::NodeMesh(const Ptr<Renderer::IAABox> & pBox,
                   const Ptr<Renderer::IMeshInstance> & pMesh,
                   const Ptr<RessourcePool> & pRessourcesPool,
                   World * pWorld) :
    Node(pBox, pWorld),
    _pMesh(pMesh),
    _pRessourcesPool(pRessourcesPool)
{
}
//-----------------------------------------------------------------------------
const Ptr<Renderer::IMeshInstance> & NodeMesh::getMeshInstance() const
{
    return _pMesh;
}
//-----------------------------------------------------------------------------
void NodeMesh::setMesh(const String & modelName, const String & meshName)
{
    Ptr<Renderer::IMesh> pMesh;

    if(modelName == String::EMPTY)
    {
        _modelName = String::EMPTY;
        _meshName = String::EMPTY;
        _pModelMesh = _pRessourcesPool->getDefaultModelMesh();
        pMesh = _pRessourcesPool->getDefaultMesh();
    }
    else
    {
       Ptr<Assets::Model> pModel(_pRessourcesPool->getModel(modelName));

       int32 meshId = pModel->getMeshId(meshName);

        if(meshId == Assets::UNDEFINED_MESH_ID)
        {
            String message(L"Unable to create mesh, '");
            message << meshName << L"' not found in model '" << modelName <<L"'";
            ERR << message << L"\n";
            throw Core::Exception(message);
        }

        _modelName = modelName;
        _meshName = meshName;

        _pModelMesh = pModel->getMeshPtr(meshId);
        pMesh = _pRessourcesPool->getMesh(_pModelMesh);
    }

    const Assets::Material & mat = _pModelMesh->getMaterial();
    _diffuseTexName = mat.diffuseTexName;
    _specularTexName = mat.specularTexName;
    _normalTexName = mat.normalTexName;
    _lightTexName = String::EMPTY;
    _occlusionTexName = String::EMPTY;
    _customTexName0 = mat.texture0Name;
    _customTexName1 = mat.texture1Name;
    _customTexName2 = mat.texture2Name;

    _pMesh->setMesh(pMesh);
    _pMesh->setMaterial(pMesh->getMaterial());
    onWorldMatrixUpdate();
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeMesh::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeMesh::setDiffuseTexture(const String & texName)
{
    Renderer::Material material = _pMesh->getMaterial();

    if(texName == String::EMPTY)
    {
        material.pDiffuseMap = null;
    }
    else
    {
        material.pDiffuseMap = _pRessourcesPool->getTexture(texName);
    }

    _diffuseTexName = texName;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::setSpecularTexture(const String & texName)
{
    Renderer::Material material = _pMesh->getMaterial();

    if(texName == String::EMPTY)
    {
        material.pSpecularMap = null;
    }
    else
    {
        material.pSpecularMap = _pRessourcesPool->getTexture(texName);
    }

    _specularTexName = texName;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::setNormalTexture(const String & texName)
{
    Renderer::Material material = _pMesh->getMaterial();

    if(texName == String::EMPTY)
    {
        material.pNormalMap = null;
    }
    else
    {
        material.pNormalMap = _pRessourcesPool->getTexture(texName);
    }

    _normalTexName = texName;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::setLightTexture(const String & texName)
{
    Renderer::Material material = _pMesh->getMaterial();

    if(texName == String::EMPTY)
    {
        material.pLightMap = null;
        material.staticLighting = false;
    }
    else
    {
        material.pLightMap = _pRessourcesPool->getTexture(texName);
        material.staticLighting = true;
    }

    _lightTexName = texName;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::setOcclusionTexture(const String & texName, bool updateMaterial)
{
    _occlusionTexName = texName;

    if(updateMaterial)
    {
        Renderer::Material material = _pMesh->getMaterial();

        if(texName == String::EMPTY)
            material.pOcclusionMap = null;
        else
            material.pOcclusionMap = _pRessourcesPool->getTexture(texName);

        _pMesh->setMaterial(material);
    }
}
//-----------------------------------------------------------------------------
void NodeMesh::setCustomTexture0(const String & texName)
{
    Renderer::Material material = _pMesh->getMaterial();

    if(texName == String::EMPTY)
        material.pCustomMap0 = null;
    else
        material.pCustomMap0 = _pRessourcesPool->getTexture(texName);

    _customTexName0 = texName;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::setCustomTexture1(const String & texName)
{
    Renderer::Material material = _pMesh->getMaterial();

    if(texName == String::EMPTY)
        material.pCustomMap1 = null;
    else
        material.pCustomMap1 = _pRessourcesPool->getTexture(texName);

    _customTexName1 = texName;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::setCustomTexture2(const String & texName)
{
    Renderer::Material material = _pMesh->getMaterial();

    if(texName == String::EMPTY)
        material.pCustomMap2 = null;
    else
        material.pCustomMap2 = _pRessourcesPool->getTexture(texName);

    _customTexName2 = texName;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::getCustomTexture0(String & texName) const
{
    texName = _customTexName0;
}
//-----------------------------------------------------------------------------
void NodeMesh::getCustomTexture1(String & texName) const
{
    texName = _customTexName1;
}
//-----------------------------------------------------------------------------
void NodeMesh::getCustomTexture2(String & texName) const
{
    texName = _customTexName2;
}
//-----------------------------------------------------------------------------
void NodeMesh::getDiffuseTexture(String & texName)
{
    texName = _diffuseTexName;
}
//-----------------------------------------------------------------------------
void NodeMesh::getSpecularTexture(String & texName)
{
    texName = _specularTexName;
}
//-----------------------------------------------------------------------------
void NodeMesh::getNormalTexture(String & texName)
{
    texName = _normalTexName;
}
//-----------------------------------------------------------------------------
void NodeMesh::getLightTexture(String & texName)
{
    texName = _lightTexName;
}
//-----------------------------------------------------------------------------
void NodeMesh::getOcclusionTexture(String & texName)
{
    texName = _occlusionTexName;
}
//-----------------------------------------------------------------------------
void NodeMesh::onWorldMatrixUpdate()
{
    _pMesh->setWorldMatrix(_localToWorldMatrix);

    Renderer::Material material = _pMesh->getMaterial();
    material.invertedCulling = (_localToWorldMatrix.determinant() < 0.0f);
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
void NodeMesh::onKill()
{
    _pMesh->kill();
}
//-----------------------------------------------------------------------------
void NodeMesh::onVisibilityChange(bool flag)
{
    _pMesh->setVisible(flag);
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeMesh::internalGetBoundingBox() const
{
    return _pMesh->getBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeMesh::setNodeColor(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);

    Renderer::Material material(_pMesh->getMaterial());
    if(material.matType == Assets::MAT_STANDARD)
        material.diffuse = color;
    else
        material.customColor.a = color.a;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeMesh::getNodeColor() const
{
    return _pMesh->getMaterial().diffuse;
}
//-----------------------------------------------------------------------------
void NodeMesh::setNodeGlow(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);

    Renderer::Material material(_pMesh->getMaterial());
    material.glowColor = color;
    _pMesh->setMaterial(material);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeMesh::getNodeGlow() const
{
    return _pMesh->getMaterial().glowColor;
}
//-----------------------------------------------------------------------------
void NodeMesh::getMaterial(Assets::Material & mat)
{
    const Renderer::Material & material = _pMesh->getMaterial();

    mat.diffuseTexName  = _diffuseTexName;
    mat.specularTexName = _specularTexName;
    mat.normalTexName   = _normalTexName;

    mat.emissive    = material.emissive;
    mat.diffuse     = material.diffuse;
    mat.specular    = material.specular;
    mat.shininess   = material.shininess;
    
    mat.glowColor   = material.glowColor;
    mat.glowExtent  = material.glowExtent;
    mat.reflexions  = material.reflexions;

    mat.glowColor   = material.glowColor;
    mat.glowExtent  = material.glowExtent;
    mat.reflexions  = material.reflexions;

    mat.refracColor = material.refracColor;
    mat.refracScale = material.refracScale;
    mat.refracIndex = material.refracIndex;

    mat.texture0Name = _customTexName0;
    mat.texture1Name = _customTexName1;
    mat.texture2Name = _customTexName2;

    mat.customMode  = material.customMode;
    mat.customColor = material.customColor;
    mat.colorFactor = material.colorFactor;
    mat.glowFactor  = material.glowFactor;

    mat.opColor01   = material.opColor01;
    mat.opColor12   = material.opColor12;
    mat.duv0        = material.duv0;
    mat.duv1        = material.duv1;
    mat.duv2        = material.duv2;

    mat.type        = material.matType;
    mat.flags       = 0;

    if(material.transDiffuseTex)
        mat.flags |= Assets::MAT_FLAG_TRANSLUCID_TEXTURE;

    if(material.clampTextureBorder)
        mat.flags |= Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER;
}
//-----------------------------------------------------------------------------
void NodeMesh::setMaterial(const Assets::Material & material, bool recursive)
{
    Node::setMaterial(material, recursive);

    setDiffuseTexture(material.diffuseTexName);
    setSpecularTexture(material.specularTexName);
    setNormalTexture(material.normalTexName);

    setCustomTexture0(material.texture0Name);
    setCustomTexture1(material.texture1Name);
    setCustomTexture2(material.texture2Name);

    Renderer::Material mat = _pMesh->getMaterial();
    mat.matType     = material.type;

    mat.emissive    = material.emissive;
    mat.diffuse     = material.diffuse;
    mat.specular    = material.specular;
    mat.shininess   = material.shininess;
    
    mat.glowColor   = material.glowColor;
    mat.glowExtent  = material.glowExtent;
    mat.reflexions  = material.reflexions;

    mat.refracColor = material.refracColor;
    mat.refracScale = material.refracScale;
    mat.refracIndex = material.refracIndex;

    mat.customMode  = material.customMode;
    mat.customColor = material.customColor;
    mat.colorFactor = material.colorFactor;
    mat.glowFactor  = material.glowFactor;

    mat.opColor01   = material.opColor01;
    mat.opColor12   = material.opColor12;
    mat.duv0        = material.duv0;
    mat.duv1        = material.duv1;
    mat.duv2        = material.duv2;

    mat.transDiffuseTex = ( (material.flags & Assets::MAT_FLAG_TRANSLUCID_TEXTURE) != 0);
    mat.clampTextureBorder = ( (material.flags & Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER) != 0);

    _pMesh->setMaterial(mat);
}
//-----------------------------------------------------------------------------
void NodeMesh::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    String modelName;
    String meshName;
    String diffuseTexName;
    String specularTexName;
    String normalTexName;
    String lightTexName;
    String occlusionTexName;
    String customTexName0;
    String customTexName1;
    String customTexName2;

    pNode->getValue(L"model", modelName);
    pNode->getValue(L"mesh",  meshName);
    pNode->getValue(L"diffuse-texture",  diffuseTexName);
    pNode->getValue(L"specular-texture",  specularTexName);
    pNode->getValue(L"normal-texture",  normalTexName);
    pNode->getValue(L"light-texture",  lightTexName);
    pNode->getValue(L"occlusion-texture",  occlusionTexName);
    pNode->getValue(L"custom-texture0",  customTexName0);
    pNode->getValue(L"custom-texture1",  customTexName1);
    pNode->getValue(L"custom-texture2",  customTexName2);

    setMesh(modelName, meshName);
    setDiffuseTexture(diffuseTexName);
    setSpecularTexture(specularTexName);
    setNormalTexture(normalTexName);
    setLightTexture(lightTexName);
    setOcclusionTexture(occlusionTexName);
    setCustomTexture0(customTexName0);
    setCustomTexture1(customTexName1);
    setCustomTexture2(customTexName2);

    Renderer::Material mat = _pMesh->getMaterial();

    Core::getXMLValue(*pNode, L"emissive", mat.emissive);
    Core::getXMLValue(*pNode, L"diffuse", mat.diffuse);
    Core::getXMLValue(*pNode, L"specular", mat.specular);

    pNode->getValue(L"shininess", mat.shininess);
    pNode->getValue(L"is-static", _isStatic);
    pNode->getValue(L"lm-resolution-ratio", _lightmapResRatio);

    _pMesh->setMaterial(mat);
}
//-----------------------------------------------------------------------------
void NodeMesh::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    Renderer::Material material = _pMesh->getMaterial();

    pNode->setValue(L"model", _modelName);
    pNode->setValue(L"mesh",  _meshName);
    pNode->setValue(L"diffuse-texture",  _diffuseTexName);
    pNode->setValue(L"specular-texture",  _specularTexName);
    pNode->setValue(L"normal-texture",  _normalTexName);
    pNode->setValue(L"light-texture",  _lightTexName);
    pNode->setValue(L"occlusion-texture",  _occlusionTexName);
    pNode->setValue(L"custom-texture0",  _customTexName0);
    pNode->setValue(L"custom-texture1",  _customTexName1);
    pNode->setValue(L"custom-texture2",  _customTexName2);

    pNode->setValue(L"emissive",  Core::toXMLString(material.emissive));
    pNode->setValue(L"diffuse",   Core::toXMLString(material.diffuse));
    pNode->setValue(L"specular",  Core::toXMLString(material.specular));
    pNode->setValue(L"shininess", material.shininess);
    pNode->setValue(L"is-static", _isStatic);
    pNode->setValue(L"lm-resolution-ratio", _lightmapResRatio);
}
//-----------------------------------------------------------------------------
bool NodeMesh::isRayOccluded(const Core::Rayf& ray) const
{
    bool result = false;

    if(_isStatic)
    {
        float uselessDistance = 0.0f;

        if(isIntersectingBoundingBox(ray, uselessDistance))
        {   
            Core::Matrix4f mat(this->getWorldToLocalMatrix());
            Core::Rayf rayLocal = mat * ray;

            Core::Vector3f posIntersec;
            result = intersect_RayVerticeSet(rayLocal, *(_pMesh->getMesh()->getVerticeSet()), posIntersec);
        }
    }

    return result;
}
//-----------------------------------------------------------------------------
bool NodeMesh::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{
    bool ret = false;

    if (isVisible() == true)
    {
        if (isIntersectingBoundingBox(ray, distance)==true)
        {   
            Core::Matrix4f mat(this->getWorldToLocalMatrix());
            Core::Rayf rayLocal = mat * ray;

            Core::Vector3f posIntersec;
            ret = intersect_RayVerticeSet(rayLocal, *(_pMesh->getMesh()->getVerticeSet()), posIntersec);

            if(ret)
            {
                Core::Matrix4f matWL (this->getLocalToWorldMatrix());
                Core::Vector3f posW = matWL.apply(posIntersec);

                distance = ray.origin.getDistance(posW);
            }
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
}
