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
#ifndef RENDERER_SCENESM2_H_
#define RENDERER_SCENESM2_H_

#include <Renderer/IScene.h>
#include <Renderer/StreamBuffer.h>
#include <Renderer/Common/Box.h>
#include <Renderer/Common/Decal.h>
#include <Renderer/Common/Light.h>
#include <Renderer/Common/Skysphere.h>
#include <Renderer/Common/GrassCluster.h>
#include <Renderer/Common/FreeForm.h>
#include <Renderer/Common/WaterPatch.h>
#include <Renderer/Common/Spline.h>
#include <Renderer/SM2/MeshSM2.h>
#include <Renderer/SM2/BillboardGroup.h>
#include <Gfx/IDevice.h>

namespace Renderer
{

class SceneSM2 : public IScene
{
public:
    SceneSM2(const Ptr<Gfx::IDevice> & pDevice, const Ptr<GPUResourceLib> & pResLib);
    virtual Ptr<IPerspectiveCamera> createPerspectiveCamera(float fov, float ratio,
                                                            float near, float far);
    virtual Ptr<IOrthoCamera> createOrthoCamera(float left, float right,
                                                float bottom, float top,
                                                float near, float far);

    virtual Ptr<IMeshInstance> createMeshInstance(const Ptr<IMesh> & pMesh);


    virtual const MainLightSettings & getLightSettings() const;
    virtual void setLightSettings(const MainLightSettings & settings);

    virtual Ptr<ISkeletonInstance> createSkeletonInstance(const Ptr<Assets::Skeleton> & pSkeleton);
    virtual Ptr<ISkinMeshInstance> createSkinMeshInstance(const Ptr<ISkinMesh> & pMesh,
                                                          const Ptr<ISkeletonInstance> & pSkeleton);

    virtual Ptr<IDecal> createDecal(const Ptr<ITextureMap> & pTexture, EDecalMode mode);

    virtual Ptr<IFreeForm> createFreeForm(const Ptr<ITextureMap> & pTexture, EFreeFormMode mode, bool isLongLife = false);

    virtual Ptr<IGrassCluster> createGrassCluster(const Core::List<GrassPatch> & patches,
                                                  const Ptr<ITextureMap> & pTexture);

    virtual Ptr<IBillboardGroup> createBillboardGroup(int32 maxSize,
                                                      const Ptr<ITextureMap> & pTexture,
                                                      EBillboardMode mode);

    virtual Ptr<IAABox> createAABox();

    virtual Ptr<ISpline> createSpline();

    virtual Ptr<IPointLight> createPointLight();

    virtual Ptr<IWaterPatch> createWaterPatch(float width, float height);
    virtual float getWaterLevel() const;
    virtual void setWaterLevel(float level);

    virtual const SkysphereSettings & getSkysphereSettings();
    virtual void setSkysphereSettings(const SkysphereSettings & settings);

    virtual const FogSettings & getFogSettings();
    virtual void setFogSettings(const FogSettings & settings);

    virtual void update(double elapsed);

    // PACKAGE

    virtual void cleanup();

    virtual void getAllMeshes(Core::List<MeshInstanceSM2 *> & meshes);
    virtual void getMeshes(Core::List<MeshInstanceSM2 *> & meshes, const Core::Frustumf & frustum);
    virtual void getSkinMeshes(Core::List<SkinMeshInstanceSM2 *> & meshes, const Core::Frustumf & frustum);
    virtual void getDecals(Core::List<Decal *> & decals, const Core::Frustumf & frustum);
    virtual void getFreeForms(Core::List<FreeForm *> & freeForms, const Core::Frustumf & frustum);
    virtual void getBillboardGroups(Core::List<BillboardGroup *> & bills, const Core::Frustumf & frustum);
    virtual void getGrassClusters(Core::List<GrassCluster *> & grass, const Core::Frustumf & frustum);
    virtual void getWaterPatches(Core::List<WaterPatch *> & water, const Core::Frustumf & frustum);
    virtual void getPointLights(Core::List<PointLight *> & lights, const Core::Frustumf & frustum);
    virtual void getAABoxes(Core::List<AABox *> & boxes, const Core::Frustumf & frustum);
    virtual void getSplines(Core::List<Spline *> & splines, const Core::Frustumf & frustum);
    virtual const Skysphere & getSkysphere() const;

    virtual bool mustUpdateReflexions() const;
    virtual void globalReflexionsUpdated();

    virtual void setWindSettings(int32 windId, const WindSettings & settings);
    virtual const WindSettings & getWindSettings(int32 windId) const;

    virtual double getTime() const;

    virtual void getAABox(Core::AABoxf & box) const;

protected:
    Ptr<Gfx::IDevice>   _pDevice;
    Ptr<GPUResourceLib> _pResLib;

    MainLightSettings   _lightSettings;
    FogSettings         _fogSettings;
    PostFX              _postFX;

    Ptr<StreamBuffer>   _pStreamFreeForm;
    Ptr<Skysphere>      _pSkysphere;

    Core::List<Ptr<SkeletonInstance> >    _skeletons;
    Core::List<Ptr<MeshInstanceSM2> >     _meshes;
    Core::List<Ptr<SkinMeshInstanceSM2> > _skinMeshes;
    Core::List<Ptr<Decal> >               _decals;
    Core::List<Ptr<FreeForm> >            _freeForms;
    Core::List<Ptr<BillboardGroup> >      _bills;
    Core::List<Ptr<GrassCluster> >        _grass;
    Core::List<Ptr<WaterPatch> >          _water;
    Core::List<Ptr<AABox> >               _boxes;
    Core::List<Ptr<Spline> >              _splines;

    Core::List<Ptr<PointLight> >          _pointLights;
    float                                 _waterLevel;

    WindSettings        _windSettings[WIND_COUNT];

    bool        _mustUpdateReflexions;
    double      _time;
};

}

#endif /* SCENESM2_H_ */
