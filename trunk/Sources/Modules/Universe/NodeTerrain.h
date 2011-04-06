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
#ifndef UNIVERSE_NODETERRAIN_H_
#define UNIVERSE_NODETERRAIN_H_

#include <Assets/TerrainModel.h>
#include <Universe/Node.h>
#include <Universe/NodeMesh.h>
#include <Universe/RessourcePool.h>

namespace Universe
{

class LM_API_UNI NodeTerrain : public Node
{
public:
    struct Layer
    {
        Layer() : angle(0.f), contrast(0.f), useDisplacementMap(false), normalStrength(1.f), color(1.0f, 1.0f, 1.0f, 1.0f)
        {};

        String          name;

        String          diffuseTexName;
        String          normalTexName;
        String          maskTexName;

        float           width;
        float           height;
        float           angle;
        float           contrast;

        bool            useDisplacementMap;
        float           normalStrength;

        Core::Vector4f  color;
    };

    NodeTerrain(const Ptr<Renderer::IAABox> & pBox,
                const Ptr<Renderer::IScene> & pScene,
                const Ptr<RessourcePool> & pRessourcesPool,
                World * pWorld);

    void setTerrain(const String & terrainName);
    const String & getTerrain() const { return _terrainName; };

    virtual ENodeType getNodeType() const { return NODE_TERRAIN; };

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    const Core::Vector4f & getEmissive() const { return _emissive; };
    const Core::Vector4f & getDiffuse()  const { return _diffuse; };
    const Core::Vector4f & getSpecular() const { return _specular; };
    const Core::Vector4f & getGlow() const { return _glow; };
    float getShininess() const { return _shininess; };
    int32 getLightmapResolutionRatio() const { return _lightmapResRatio; };

    void setEmissive(const Core::Vector4f & c);
    void setDiffuse(const Core::Vector4f & c);
    void setSpecular(const Core::Vector4f & c);
    void setGlow(const Core::Vector4f & c);
    void setShininess(float s);
    void setLightmapResolutionRatio(int32 ratio) { _lightmapResRatio = ratio; };


    const String & getGlobalNormalTexture() const { return _globalNormalTex; };
    float getGlobalNormalWeight() const { return _globalNormalWeight; };

          Core::List<Layer> & getLayers()       { return _layers; };
    const Core::List<Layer> & getLayers() const { return _layers; };

    // EDITION

    int32 getLODCount() const { return _blocks.size(); };
    int32 getBlockCountForLOD(int32 LOD) const { return _blocks[LOD].size(); };

    Ptr<Renderer::IMeshInstance> getBlockMesh(int32 LOD, int32 index) const { return _blocks[LOD][index].pMesh; };
    const String & getDiffuseTexName(int32 LOD, int32 index) const { return _blocks[LOD][index].diffuseTex; };

    void setGlobalNormalTexture(const String & tex) { _globalNormalTex = tex; };
    void setGlobalNormalWeight(float w) { _globalNormalWeight = w; };

    // [LOD][index]
    void setBlockTextures(const Core::List<Core::List<String> > & diffuseTextures,
                          const Core::List<Core::List<String> > & normalTextures);
    void getBlockTextures(Core::List<Core::List<String> > & diffuseTextures,
                          Core::List<Core::List<String> > & normalTextures);

    void setBlockLightTexture(int32 LOD, int32 index, const String & lightTexture, bool updateMaterial = true);
    void setBlockLightTextures(const Core::List<Core::List<String> > & lightTextures);
    void clearBlockLightTextures();

    void setBlockOcclusionTexture(int32 LOD, int32 index, const String & occlusionTexture, bool updateMaterial = true);
    void setBlockOcclusionTextures(const Core::List<Core::List<String> > & occlusionTextures);
    void clearBlockOcclusionTextures();

    virtual void prepareForPacking();
    virtual void spawnMeshes(World & world);

    virtual bool isRayOccluded(const Core::Rayf& ray) const;

protected:
    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual void onMaterialUpdate();
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);
    virtual void internalPrepareForPacking();

    Ptr<RessourcePool>                        _pRessourcesPool;
    Ptr<Renderer::IScene>                     _pScene;
    String                                    _terrainName;
    Core::List<Layer>                         _layers;

    struct BlockInfos
    {
        String                         diffuseTex;
        String                         normalTex;
        String                         lightTex;
        String                         occlusionTex;
        Ptr<Renderer::IMeshInstance>   pMesh;
    };

    String                              _globalNormalTex;
    float                               _globalNormalWeight;

    Core::List<Core::List<BlockInfos> > _blocks;

    Core::Vector4f      _emissive;
    Core::Vector4f      _diffuse;
    Core::Vector4f      _specular;
    Core::Vector4f      _glow;
    float               _shininess;
    int32               _lightmapResRatio;
};

}

#endif /* NODE_H_ */
