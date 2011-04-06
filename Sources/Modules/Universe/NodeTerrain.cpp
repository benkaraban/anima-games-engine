/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include <Universe/NodeTerrain.h>
#include <Universe/World.h>
#include <Universe/Tools.h>

#include <Core/XML/XMLMath.h>

namespace Universe
{
//-----------------------------------------------------------------------------
NodeTerrain::NodeTerrain(const Ptr<Renderer::IAABox> & pBox,
                         const Ptr<Renderer::IScene> & pScene,
                         const Ptr<RessourcePool> & pRessourcesPool,
                         World * pWorld) :
    Node(pBox, pWorld),
    _pRessourcesPool(pRessourcesPool),
    _pScene(pScene),
    _globalNormalWeight(0.5f),
    _emissive(0.0f, 0.0f, 0.0f, 0.0f),
    _diffuse(1.0f, 1.0f, 1.0f, 1.0f),
    _specular(0.0f, 0.0f, 0.0f, 1.0f),
    _glow(0.0f, 0.0f, 0.0f, 1.0f),
    _shininess(22.0f),
    _lightmapResRatio(2)
{
}
//-----------------------------------------------------------------------------
void NodeTerrain::prepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeTerrain::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeTerrain::setTerrain(const String & terrainName)
{
    _terrainName = terrainName;

    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            _blocks[iLOD][iBlock].pMesh->kill();

    _blocks.clear();

    if(_terrainName != String::EMPTY)
    {
        Ptr<Assets::TerrainModel> pTerrain = _pRessourcesPool->getTerrainModel(terrainName);

        _blocks.resize(pTerrain->getLODCount());

        for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        {
            _blocks[iLOD].resize(pTerrain->getNodeCountForLOD(iLOD));

            for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            {
                Ptr<Assets::ModelNode> pNode = pTerrain->getNodeByLOD(iLOD, iBlock);
                Ptr<Assets::ModelMesh> pMesh = pTerrain->getMeshPtr(pNode->getMeshId());
                Ptr<Renderer::IMesh> pRenderMesh = _pRessourcesPool->getMesh(pMesh);

                _blocks[iLOD][iBlock].pMesh = _pScene->createMeshInstance(pRenderMesh);
            }
        }

        onMaterialUpdate();
        onWorldMatrixUpdate();
        updateBoundingBox();
    }
}
//-----------------------------------------------------------------------------
void NodeTerrain::onWorldMatrixUpdate()
{
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            _blocks[iLOD][iBlock].pMesh->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeTerrain::internalGetBoundingBox() const
{
    Core::AABoxf box;

    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            box.setUnion(_blocks[iLOD][iBlock].pMesh->getBoundingBox());

    return box;
}
//-----------------------------------------------------------------------------
void NodeTerrain::onKill()
{
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            _blocks[iLOD][iBlock].pMesh->kill();

    _blocks.clear();
}
//-----------------------------------------------------------------------------
void NodeTerrain::onVisibilityChange(bool flag)
{
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            _blocks[iLOD][iBlock].pMesh->setVisible(flag);
}
//-----------------------------------------------------------------------------
void NodeTerrain::setEmissive(const Core::Vector4f & c)
{
    _emissive = c;
    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::setDiffuse(const Core::Vector4f & c)
{
    _diffuse = c;
    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::setSpecular(const Core::Vector4f & c)
{
    _specular = c;
    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::setGlow(const Core::Vector4f & c)
{
    _glow = c;
    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::setShininess(float s)
{
    _shininess = s;
    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::spawnMeshes(World & world)
{
    Ptr<Assets::TerrainModel> pTerrain = _pRessourcesPool->getTerrainModel(_terrainName);

    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
        {
            const BlockInfos & block = _blocks[iLOD][iBlock];

            Ptr<Assets::ModelNode> pNode = pTerrain->getNodeByLOD(iLOD, iBlock);
            Ptr<Assets::ModelMesh> pMesh = pTerrain->getMeshPtr(pNode->getMeshId());

            Ptr<NodeMesh> pNodeMesh = world.createMesh(_terrainName, pMesh->getName());

            pMesh = pNodeMesh->getModelMesh();

            Assets::Material & amat = pMesh->getMaterial();
            amat.emissive   = _emissive;
            amat.diffuse    = _diffuse;
            amat.specular   = _specular;
            amat.specular   = _glow;
            amat.shininess  = _shininess;
            amat.flags      = Assets::MAT_FLAG_DYNAMIC_SHADOW_RECEIVER;

            pNodeMesh->setDiffuseTexture(block.diffuseTex);
            pNodeMesh->setLightTexture(block.lightTex);
            pNodeMesh->setStatic(true);
            Renderer::Material mat = pNodeMesh->getMeshInstance()->getMaterial();
            mat.emissive        = _emissive;
            mat.diffuse         = _diffuse;
            mat.specular        = _specular;
            mat.glowColor       = _glow;
            mat.shininess       = _shininess;
            mat.dynamicShadowReceiver = true;
            mat.dynamicShadowCaster   = false;
            mat.clampTextureBorder    = true;
            mat.internalTextureBorder = true;
            
            pNodeMesh->getMeshInstance()->setMaterial(mat);
        }
    }

    kill();
}
//-----------------------------------------------------------------------------
void NodeTerrain::onMaterialUpdate()
{
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
        {
            const BlockInfos & block = _blocks[iLOD][iBlock];
            Renderer::Material mat = block.pMesh->getMaterial();

            mat.pDiffuseMap     = (block.diffuseTex != String::EMPTY) ? _pRessourcesPool->getTexture(block.diffuseTex) : null;
            mat.pNormalMap      = (block.normalTex != String::EMPTY) ? _pRessourcesPool->getTexture(block.normalTex) : null;
            mat.pLightMap       = (block.lightTex != String::EMPTY) ? _pRessourcesPool->getTexture(block.lightTex) : null;
            mat.pOcclusionMap   = (block.occlusionTex != String::EMPTY) ? _pRessourcesPool->getTexture(block.occlusionTex) : null;
            mat.emissive        = _emissive;
            mat.diffuse         = _diffuse;
            mat.specular        = _specular;
            mat.glowColor       = _glow;
            mat.shininess       = _shininess;
            mat.dynamicShadowReceiver = true;
            mat.dynamicShadowCaster   = false;
            mat.clampTextureBorder    = true;
            mat.internalTextureBorder = true;
            mat.staticLighting        = (mat.pLightMap != null);

            _blocks[iLOD][iBlock].pMesh->setMaterial(mat);
        }
    }
}
//-----------------------------------------------------------------------------
void NodeTerrain::setBlockTextures(const Core::List<Core::List<String> > & diffuseTextures,
                                   const Core::List<Core::List<String> > & normalTextures)
{
    LM_ASSERT(diffuseTextures.size() == _blocks.size());
    LM_ASSERT(normalTextures.size() == _blocks.size());

    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        LM_ASSERT(diffuseTextures[iLOD].size() == _blocks[iLOD].size());
        LM_ASSERT(normalTextures[iLOD].size() == _blocks[iLOD].size());

        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
        {
            _blocks[iLOD][iBlock].diffuseTex = diffuseTextures[iLOD][iBlock];
            _blocks[iLOD][iBlock].normalTex  = normalTextures[iLOD][iBlock];
        }
    }

    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::getBlockTextures( Core::List<Core::List<String> > & diffuseTextures,
                                    Core::List<Core::List<String> > & normalTextures)
{
    diffuseTextures.resize(_blocks.size());
    normalTextures.resize(_blocks.size());
    
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        diffuseTextures[iLOD].resize(_blocks[iLOD].size());
        normalTextures[iLOD].resize(_blocks[iLOD].size());

        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
        {
            diffuseTextures[iLOD][iBlock] = _blocks[iLOD][iBlock].diffuseTex;
            normalTextures[iLOD][iBlock]  = _blocks[iLOD][iBlock].normalTex;
        }
    }
}
//-----------------------------------------------------------------------------
void NodeTerrain::setBlockLightTexture(int32 LOD, int32 index, const String & lightTexture, bool updateMaterial)
{
    _blocks[LOD][index].lightTex = lightTexture;
    if(updateMaterial)
        onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::setBlockLightTextures(const Core::List<Core::List<String> > & lightTextures)
{
    LM_ASSERT(lightTextures.size() == _blocks.size());
 
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        LM_ASSERT(lightTextures[iLOD].size() == _blocks[iLOD].size());

        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
        {
            _blocks[iLOD][iBlock].lightTex = lightTextures[iLOD][iBlock];
        }
    }

    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::setBlockOcclusionTexture(int32 LOD, int32 index, const String & occlusionTexture, bool updateMaterial)
{
    _blocks[LOD][index].occlusionTex = occlusionTexture;
    if(updateMaterial)
        onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::setBlockOcclusionTextures(const Core::List<Core::List<String> > & occlusionTextures)
{
    LM_ASSERT(occlusionTextures.size() == _blocks.size());
 
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        LM_ASSERT(occlusionTextures[iLOD].size() == _blocks[iLOD].size());

        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
        {
            _blocks[iLOD][iBlock].occlusionTex = occlusionTextures[iLOD][iBlock];
        }
    }

    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::clearBlockLightTextures()
{
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            _blocks[iLOD][iBlock].lightTex = String::EMPTY;

    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::clearBlockOcclusionTextures()
{
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
            _blocks[iLOD][iBlock].occlusionTex = String::EMPTY;

    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    String terrainName;
    pNode->getValue(L"terrain-model", terrainName);

    Core::getXMLValue(*pNode, L"emissive", _emissive);
    Core::getXMLValue(*pNode, L"diffuse", _diffuse);
    Core::getXMLValue(*pNode, L"specular", _specular);
    Core::getXMLValue(*pNode, L"glow", _glow);
    Core::getXMLValue(*pNode, L"shininess", _shininess);
    Core::getXMLValue(*pNode, L"global-normal-texture", _globalNormalTex);
    Core::getXMLValue(*pNode, L"global-normal-weight", _globalNormalWeight);

    // Crée la géométrie avant de mettre les textures
    setTerrain(terrainName);

    // Layers
    Core::List<Ptr<Core::XMLNode> > layers;
    pNode->getNodes(L"layer", layers);
    
    _layers.clear();
    _layers.resize(layers.size());

    for(int32 iLayer=0; iLayer < _layers.size(); iLayer++)
    {
        Core::getXMLValue(*layers[iLayer], L"name", _layers[iLayer].name);

        Core::getXMLValue(*layers[iLayer], L"diffuse-texture", _layers[iLayer].diffuseTexName);
        Core::getXMLValue(*layers[iLayer], L"normal-texture", _layers[iLayer].normalTexName);
        Core::getXMLValue(*layers[iLayer], L"mask-texture", _layers[iLayer].maskTexName);

        Core::getXMLValue(*layers[iLayer], L"width", _layers[iLayer].width);
        Core::getXMLValue(*layers[iLayer], L"height", _layers[iLayer].height);
        Core::getXMLValue(*layers[iLayer], L"angle", _layers[iLayer].angle);
        Core::getXMLValue(*layers[iLayer], L"contrast", _layers[iLayer].contrast);
        Core::getXMLValue(*layers[iLayer], L"normal-strength", _layers[iLayer].normalStrength);
        Core::getXMLValue(*layers[iLayer], L"use-displacement-map", _layers[iLayer].useDisplacementMap);
        Core::getXMLValue(*layers[iLayer], L"color", _layers[iLayer].color);
    }

    // Blocks
    Core::List<Ptr<Core::XMLNode> > LODs;

    pNode->getNodes(L"LOD", LODs);
   // LM_ASSERT(LODs.size() == _blocks.size());

    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        Core::List<Ptr<Core::XMLNode> > blocks;
        
        LODs[iLOD]->getNodes(L"block", blocks);
        LM_ASSERT(blocks.size() == _blocks[iLOD].size());

        for(int32 iBlock=0; iBlock < blocks.size(); iBlock++)
        {
            blocks[iBlock]->getValue(L"diffuse-texture", _blocks[iLOD][iBlock].diffuseTex);
            blocks[iBlock]->getValue(L"normal-texture", _blocks[iLOD][iBlock].normalTex);
            blocks[iBlock]->getValue(L"light-texture", _blocks[iLOD][iBlock].lightTex);
            blocks[iBlock]->getValue(L"occlusion-texture", _blocks[iLOD][iBlock].occlusionTex);
        }
    }

    onMaterialUpdate();
}
//-----------------------------------------------------------------------------
void NodeTerrain::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"terrain-model", _terrainName);

    pNode->setValue(L"emissive", Core::toXMLString(_emissive));
    pNode->setValue(L"diffuse", Core::toXMLString(_diffuse));
    pNode->setValue(L"specular", Core::toXMLString(_specular));
    pNode->setValue(L"glow", Core::toXMLString(_glow));
    pNode->setValue(L"shininess", _shininess);
    pNode->setValue(L"lm-resolution-ratio", _lightmapResRatio);
    pNode->setValue(L"global-normal-texture", _globalNormalTex);
    pNode->setValue(L"global-normal-weight", _globalNormalWeight);

    // Layers
    for(int32 iLayer=0; iLayer < _layers.size(); iLayer++)
    {
        Ptr<Core::XMLNode> pLayer(new Core::XMLNode(L"layer"));

        pLayer->setValue(L"name", _layers[iLayer].name);

        pLayer->setValue(L"diffuse-texture", _layers[iLayer].diffuseTexName);
        pLayer->setValue(L"normal-texture", _layers[iLayer].normalTexName);
        pLayer->setValue(L"mask-texture", _layers[iLayer].maskTexName);
        pLayer->setValue(L"width", _layers[iLayer].width);
        pLayer->setValue(L"height", _layers[iLayer].height);
        pLayer->setValue(L"angle", _layers[iLayer].angle);
        pLayer->setValue(L"contrast", _layers[iLayer].contrast);
        pLayer->setValue(L"normal-strength", _layers[iLayer].normalStrength);
        pLayer->setValue(L"use-displacement-map", _layers[iLayer].useDisplacementMap);
        pLayer->setValue(L"color", Core::toXMLString(_layers[iLayer].color));

        pNode->addNode(pLayer);
    }

    // Blocks
    for(int32 iLOD=0; iLOD < _blocks.size(); iLOD++)
    {
        Ptr<Core::XMLNode> pLOD(new Core::XMLNode(L"LOD"));

        for(int32 iBlock=0; iBlock < _blocks[iLOD].size(); iBlock++)
        {
            Ptr<Core::XMLNode> pBlock(new Core::XMLNode(L"block"));

            pBlock->setValue(L"diffuse-texture", _blocks[iLOD][iBlock].diffuseTex);
            pBlock->setValue(L"normal-texture", _blocks[iLOD][iBlock].normalTex);
            pBlock->setValue(L"light-texture", _blocks[iLOD][iBlock].lightTex);
            pBlock->setValue(L"occlusion-texture", _blocks[iLOD][iBlock].occlusionTex);

            pLOD->addNode(pBlock);
        }

        pNode->addNode(pLOD);
    }
}
//-----------------------------------------------------------------------------
bool NodeTerrain::isRayOccluded(const Core::Rayf& ray) const
{
    bool ret = false;
    float distanceTest;

    if(isVisible())
    {
        Core::Matrix4f mat(this->getWorldToLocalMatrix());
        Core::Rayf rayLocal = mat * ray;

        for(int32 iLOD=0; iLOD<getLODCount();++iLOD)
        {
            for(int32 iIndex=0; iIndex<getBlockCountForLOD(iLOD); ++iIndex)
            {
                Ptr<Renderer::IMeshInstance>  blockMesh = getBlockMesh(iLOD, iIndex);

                const Core::AABoxf& box = blockMesh->getBoundingBox();

                if(intersect_RayAABox(ray, box, distanceTest))
                {
                    Core::Vector3f posIntersec;
                    ret = intersect_RayVerticeSet(rayLocal, *(blockMesh->getMesh()->getVerticeSet()), posIntersec);

                    if(ret)
                        return true;
                }
            }
        }
    }

    return false;
}
//-----------------------------------------------------------------------------
bool NodeTerrain::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{
    bool ret = false;
    float distanceTest;

    if (isVisible() == true)
    {
        for (int32 iLOD=0; iLOD<getLODCount();++iLOD)
        {
            for (int32 iIndex=0; iIndex<getBlockCountForLOD(iLOD); ++iIndex)
            {
                Ptr<Renderer::IMeshInstance>  blockMesh = getBlockMesh(iLOD, iIndex);

                const Core::AABoxf& box = blockMesh->getBoundingBox();

                if (intersect_RayAABox(ray, box, distanceTest) == true)
                {
                    Core::Matrix4f mat(this->getWorldToLocalMatrix());
                    Core::Rayf rayLocal = mat * ray;

                    Core::Vector3f posIntersec;
                    ret = intersect_RayVerticeSet(rayLocal, *(blockMesh->getMesh()->getVerticeSet()), posIntersec);

                    if(ret)
                    {
                        Core::Matrix4f matWL (this->getLocalToWorldMatrix());
                        Core::Vector3f posW = matWL.apply(posIntersec);

                        distance = ray.origin.getDistance(posW);
                    }
                }
            }
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
}
