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
#ifndef RENDERERSM2_MESH_H_
#define RENDERERSM2_MESH_H_

#include <Renderer/Common/Mesh.h>
#include <Renderer/Common/GPUResource.h>
#include <Renderer/Common/Light.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
struct MaterialData
{
    Ptr<Gfx::ITextureCube>        pReflexionsCube;
    Ptr<Gfx::IRenderTargetView>   pRenderTarget[Gfx::ECubeMapFace_COUNT];
    bool                          isUpdatingReflexion;
    bool                          isHidden;

    MaterialData();
};

//-----------------------------------------------------------------------------
struct LightingData
{
    Core::List<PointLightInfluence> pointLights;
};

//-----------------------------------------------------------------------------
class IRenderable
{
public:
    virtual ~IRenderable() {};

    virtual Core::Vector3f       getPosition() const = 0;
    virtual const Core::AABoxf & getAABoundingBox() const = 0;

    virtual const Material     & getRenderableMaterial() const = 0;

    virtual       MaterialData & getMatData() = 0;
    virtual const MaterialData & getMatData() const = 0;

    virtual       LightingData & getLightingData() = 0;
    virtual const LightingData & getLightingData() const = 0;
};

//-----------------------------------------------------------------------------
class MeshInstanceSM2 : public MeshInstance, public IRenderable, public IGPUResource
{
public:
    MeshInstanceSM2(const Ptr<IMesh> & pMesh, const Ptr<GPUResourceLib> & pResLib);
    ~MeshInstanceSM2();

    virtual void setMaterial(const Renderer::Material & material);

    virtual Core::Vector3f       getPosition() const { return getWorldMatrix().getTVector(); };
    virtual const Core::AABoxf & getAABoundingBox() const { return _box; };

    virtual const Material     & getRenderableMaterial() const { return _material; };

    virtual       MaterialData & getMatData()       { return _matData; };
    virtual const MaterialData & getMatData() const { return _matData; };

    virtual       LightingData & getLightingData()       { return _lightingData; };
    virtual const LightingData & getLightingData() const { return _lightingData; };

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:
    Ptr<GPUResourceLib> _pResLib;
    MaterialData        _matData;
    LightingData        _lightingData;
};
//-----------------------------------------------------------------------------
class SkinMeshInstanceSM2 : public SkinMeshInstance, public IRenderable, public IGPUResource
{
public:
    SkinMeshInstanceSM2(const Ptr<ISkinMesh> & pMesh, const Ptr<ISkeletonInstance> & pSkeleton, const Ptr<GPUResourceLib> & pResLib);
    ~SkinMeshInstanceSM2();

    virtual void setMaterial(const Renderer::Material & material);

    virtual Core::Vector3f       getPosition() const { return _pSkeleton->getWorldMatrix().getTVector(); };
    virtual const Core::AABoxf & getAABoundingBox() const { return _box; };

    virtual const Material     & getRenderableMaterial() const { return _material; };

    virtual       MaterialData & getMatData()       { return _matData; };
    virtual const MaterialData & getMatData() const { return _matData; };

    virtual       LightingData & getLightingData()       { return _lightingData; };
    virtual const LightingData & getLightingData() const { return _lightingData; };

    virtual void onDeviceLost();
    virtual void onDeviceReset();

protected:
    Ptr<GPUResourceLib> _pResLib;
    MaterialData        _matData;
    LightingData        _lightingData;
};
}

#endif /* MESH_H_ */
