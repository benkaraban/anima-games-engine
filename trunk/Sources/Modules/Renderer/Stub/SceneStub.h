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
#ifndef RENDERER_SCENESTUB_H_
#define RENDERER_SCENESTUB_H_

#include <Renderer/IScene.h>

namespace Renderer
{
class LM_API_RDS SceneStub : public Core::Object, public IScene
{
public:
    SceneStub() : _skySettings(null, 0.0), _waterLevel(0.0) {};

    virtual Ptr<IPerspectiveCamera> createPerspectiveCamera(float fov, float ratio,
                                                            float near, float far);

    virtual Ptr<IOrthoCamera> createOrthoCamera(float left, float right,
                                                float bottom, float top,
                                                float near, float far);

    virtual Ptr<IMeshInstance> createMeshInstance(const Ptr<IMesh> & pMesh);
    virtual Ptr<ISkeletonInstance> createSkeletonInstance(const Ptr<Assets::Skeleton> & pSkeleton);
    virtual Ptr<ISkinMeshInstance> createSkinMeshInstance(const Ptr<ISkinMesh> & pMesh,
                                                          const Ptr<ISkeletonInstance> & pSkeleton);

    virtual Ptr<IAABox> createAABox();
    virtual Ptr<ISpline> createSpline();

    virtual Ptr<IDirLight> getDirLight();
    virtual Ptr<IPointLight> createPointLight();

    virtual const MainLightSettings & getLightSettings() const;
    virtual void setLightSettings(const MainLightSettings & settings);

    virtual Ptr<IGrassCluster> createGrassCluster(const Core::List<GrassPatch> & patches,
                                                  const Ptr<ITextureMap> & pTexture);

    virtual Ptr<IBillboardGroup> createBillboardGroup(int32 maxSize,
                                                      const Ptr<ITextureMap> & pTexture,
                                                      EBillboardMode mode);

    virtual Ptr<IDecal> createDecal(const Ptr<ITextureMap> & pTexture, EDecalMode mode);

    virtual Ptr<IFreeForm> createFreeForm(const Ptr<ITextureMap> & pTexture, EFreeFormMode mode, bool isLongLife = false);

    virtual Ptr<IWaterPatch> createWaterPatch(float width, float height);
    virtual float getWaterLevel() const { return _waterLevel; };
    virtual void setWaterLevel(float level) { _waterLevel = level; };

    virtual const SkysphereSettings & getSkysphereSettings();
    virtual void setSkysphereSettings(const SkysphereSettings & settings);

    virtual const FogSettings & getFogSettings() { return _fogSettings; };
    virtual void setFogSettings(const FogSettings & settings) { _fogSettings = settings; };

    virtual void setWindSettings(int32 windId, const WindSettings & settings);

    virtual void setPostFX(const PostFX & fx) { _postFX = fx; };
    virtual const PostFX & getPostFX() const { return _postFX; };

    virtual void update(double elapsed);

    virtual void setLightProbe(const Ptr<ICubeTextureMap> & pProbe) {};
    virtual Ptr<ICubeTextureMap> getLightProbe() const { return null; };

    WindSettings         _windSettings[10];
    MainLightSettings    _mainLightSettings;
    SkysphereSettings    _skySettings;
    FogSettings          _fogSettings;
    float                _waterLevel;
    PostFX               _postFX;
};
//-----------------------------------------------------------------------------
}
#endif
