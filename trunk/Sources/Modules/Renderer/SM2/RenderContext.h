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
#ifndef RENDERER_RENDERCONTEXT_H_
#define RENDERER_RENDERCONTEXT_H_

#include <Core/List.h>
#include <Gfx/IDevice.h>
#include <Renderer/Common/Box.h>
#include <Renderer/Common/Decal.h>
#include <Renderer/Common/Spline.h>
#include <Renderer/Common/WaterPatch.h>
#include <Renderer/ICamera.h>
#include <Renderer/SM2/IRenderTarget.h>
#include <Renderer/SM2/SceneSM2.h>
#include <Renderer/SM2/MeshSM2.h>
#include <Renderer/SM2/BillboardGroup.h>
#include <Renderer/SM2/InternalSettings.h>

namespace Renderer
{
const int32 STANDARD_CONTEXT        = 0x00000000;
const int32 WATER_CONTEXT           = 0x00000001;

const int32 CTX_DYNAMIC_SHADOW_ON   = 0x00000002;

LM_ENUM_1(EStaticShadowContext, STATIC_SHADOW_CTX);
LM_ENUM_1(EDynamicShadowContext, DYNAMIC_SHADOW_CTX);
LM_ENUM_1(ELightmapGenerationContext, LIGHTMAP_GENERATION_CTX);

class RenderContext : public Core::Object
{
public:
    RenderContext(
            const Ptr<IRenderTarget> & pTarget,
            const Core::List<Ptr<SceneSM2> > & scenes,
            const Ptr<ICamera> & pCamera,
            int32 width,
            int32 height,
            float time,
            int32 flags
            );

    RenderContext(
        ELightmapGenerationContext ctx, 
        const Ptr<IRenderTarget> & pTarget,
        const Core::List<Ptr<SceneSM2> > & scenes,
        const Ptr<ICamera> & pCamera,
        const Ptr<IMeshInstance> & pMesh,
        int32 width,
        int32 height,
        float time
    );

    RenderContext(EStaticShadowContext ctx, const RenderContext & context, const Core::List<Ptr<SceneSM2> > & scenes);
    RenderContext(EDynamicShadowContext ctx, const RenderContext & context, const Core::List<Ptr<SceneSM2> > & scenes, int32 split);

    const Core::Matrix4f & getLightWorld() const { return _lightWorld; };
    const Core::Matrix4f & getLightView() const { return _lightView; };
    const Core::Matrix4f & getLightProj0() const { return _lightProj0; };
    const Core::Matrix4f & getLightProj1() const { return _lightProj1; };
    const Core::Matrix4f & getLightViewProj0() const { return _lightViewProj0; };
    const Core::Matrix4f & getLightViewProj1() const { return _lightViewProj1; };

    const Core::Matrix4f & getStaticLightProj() const { return _staticLightProj; };
    const Core::Matrix4f & getStaticLightViewProj() const { return _staticLightViewProj; };

    float getDynamicShadowSplitSize0() const { return _dynamicShadowSplitSize0; };
    float getDynamicShadowSplitSize1() const { return _dynamicShadowSplitSize1; };

    const Core::Matrix4f & getWorld() const { return _world; };
    const Core::Matrix4f & getView() const { return _view; };
    const Core::Matrix4f & getProj() const { return _proj; };
    const Core::Matrix4f & getViewProj() const { return _viewProj; };

    const Core::Vector3f & getEye() const { return _eye; };
    const Core::Vector3f & getEyeDir() const { return _eyeDir; };

    const Core::Frustumf & getFrustum() const { return _frustum; };

    const InternalMainLightSettings & getLight() const { return _light; };
    const InternalFogSettings & getFog() const { return _fog; };

    const Skysphere * getSkysphere() const { return _pSky; };
    const Core::List<MeshInstanceSM2 *> & getMeshes() const { return _meshes; };
    const Core::List<MeshInstanceSM2 *> & getCustomMeshes() const { return _customMeshes; };
    const Core::List<SkinMeshInstanceSM2 *> & getSkinMeshes() const { return _skinMeshes; };
    const Core::List<SkinMeshInstanceSM2 *> & getCustomSkinMeshes() const { return _customSkinMeshes; };
    const Core::List<Decal *> & getDecals() const { return _decals; };
    const Core::List<FreeForm *> & getFreeForms() const { return _freeForms; };
    const Core::List<BillboardGroup *> & getBills() const { return _bills; };
    const Core::List<GrassCluster *> & getGrassClusters() const { return _grass; };
    const Core::List<WaterPatch *> & getWaterPatches() const { return _water; };
    const Core::List<AABox *> & getAABoxes() const { return _boxes; };
    const Core::List<Spline *> & getSplines() const { return _splines; };
    float getTime() const { return _time; };

    IRenderTarget * getRenderTarget() const { return _pRenderTarget; };

    int32 getViewportWidth() const { return _viewportWidth; };
    int32 getViewportHeight() const { return _viewportHeight; };

    const WindSettings & getWindSettings(int32 windId) const { return _windSettings[windId]; };
    float getWaterLevel() const { return _waterLevel; };

    bool isReflectionMapNeeded() const { return _isReflectionMapNeeded; };
    bool isDynamicShadowMapNeeded() const { return _isDynamicShadowMapNeeded; };
    bool isDynamicShadowOn() const { return _isDynamicShadowOn; };

    const String & getStats() const { return _stats; };

protected:
    IRenderTarget *     _pRenderTarget;

    int32               _viewportWidth;
    int32               _viewportHeight;

    Core::Matrix4f      _lightWorld;
    Core::Matrix4f      _lightView;
    Core::Matrix4f      _lightProj0;
    Core::Matrix4f      _lightProj1;
    Core::Matrix4f      _lightViewProj0;
    Core::Matrix4f      _lightViewProj1;

    Core::Matrix4f      _staticLightProj;
    Core::Matrix4f      _staticLightViewProj;

    float               _dynamicShadowSplitSize0;
    float               _dynamicShadowSplitSize1;

    Core::Matrix4f      _world;
    Core::Matrix4f      _view;
    Core::Matrix4f      _proj;
    Core::Matrix4f      _viewProj;

    Core::Vector3f      _eye;
    Core::Vector3f      _eyeDir;

    Core::Frustumf      _frustum;

    InternalMainLightSettings _light;
    InternalFogSettings _fog;

    float               _time;

    WindSettings        _windSettings[WIND_COUNT];
    float               _waterLevel;

    bool                _isReflectionMapNeeded;
    bool                _isDynamicShadowMapNeeded;
    bool                _isDynamicShadowOn;

    String              _stats;

    const Skysphere *                 _pSky;
    Core::List<MeshInstanceSM2 *>     _meshes;
    Core::List<MeshInstanceSM2 *>     _customMeshes;
    Core::List<SkinMeshInstanceSM2 *> _skinMeshes;
    Core::List<SkinMeshInstanceSM2 *> _customSkinMeshes;
    Core::List<Decal *>               _decals;
    Core::List<FreeForm *>            _freeForms;
    Core::List<BillboardGroup *>      _bills;
    Core::List<GrassCluster *>        _grass;
    Core::List<WaterPatch *>          _water;
    Core::List<AABox *>               _boxes;
    Core::List<Spline *>              _splines;

    void setStats();
    void getShadowSplit(Core::Matrix4f & lightProj, float & splitSize, float near, float far, const Ptr<ICamera> & pCamera);
};
}

#endif /* RENDERCONTEXT_H_ */
