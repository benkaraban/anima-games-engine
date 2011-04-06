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
#ifndef NODEMESH_H_
#define NODEMESH_H_

#include <Renderer/IMesh.h>
#include <Universe/Node.h>
#include <Universe/RessourcePool.h>

namespace Universe
{
class LM_API_UNI NodeMesh : public Node
{
public:
    NodeMesh(const Ptr<Renderer::IAABox> & pBox,
             const Ptr<Renderer::IMeshInstance> & pMesh,
             const Ptr<RessourcePool> & pRessourcesPool,
             World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_MESH; };

    virtual void setMesh(const String & modelName, const String & meshName);
    virtual void setDiffuseTexture(const String & texName);
    virtual void setSpecularTexture(const String & texName);
    virtual void setNormalTexture(const String & texName);
    virtual void setLightTexture(const String & texName);
    virtual void setOcclusionTexture(const String & texName, bool updateMaterial = true);
    virtual void setStatic(bool isStatic) { _isStatic = isStatic; };
    virtual void setLightmapResolutionRatio(int32 ratio) { _lightmapResRatio = ratio; };

    virtual void setCustomTexture0(const String & texName);
    virtual void setCustomTexture1(const String & texName);
    virtual void setCustomTexture2(const String & texName);
    virtual void getCustomTexture0(String & texName) const;
    virtual void getCustomTexture1(String & texName) const;
    virtual void getCustomTexture2(String & texName) const;

    virtual void getDiffuseTexture(String & texName);
    virtual void getSpecularTexture(String & texName);
    virtual void getNormalTexture(String & texName);
    virtual void getLightTexture(String & texName);
    virtual void getOcclusionTexture(String & texName);
    virtual bool isStatic() const { return _isStatic; };
    virtual int32 getLightmapResolutionRatio() const { return _lightmapResRatio; };

    virtual void getMaterial(      Assets::Material & material);
    virtual void setMaterial(const Assets::Material & material, bool recursive = false);

    virtual void setNodeColor(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeColor() const;

    virtual void setNodeGlow(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeGlow() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    virtual inline const String & getModelName() const { return _modelName; }
    virtual inline const String & getMeshName() const { return _meshName; }

    // ??? KEEP ???

    const Ptr<Renderer::IMeshInstance> & getMeshInstance() const;

    virtual bool isRayOccluded(const Core::Rayf& ray) const;

    Ptr<Assets::ModelMesh> getModelMesh() const { return _pModelMesh; };

protected:
    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);
    virtual void internalPrepareForPacking();

    Ptr<Renderer::IMeshInstance> _pMesh;
    Ptr<Assets::ModelMesh>       _pModelMesh;
    Ptr<RessourcePool>           _pRessourcesPool;
    String                       _modelName;
    String                       _meshName;
    String                       _diffuseTexName;
    String                       _specularTexName;
    String                       _normalTexName;
    String                       _lightTexName;
    String                       _occlusionTexName;
    String                       _customTexName0;
    String                       _customTexName1;
    String                       _customTexName2;
};
}

#endif /* NODEMESH_H_ */
