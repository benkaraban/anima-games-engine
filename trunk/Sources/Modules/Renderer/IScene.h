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
#ifndef ABSTRACT_ISCENE_H_
#define ABSTRACT_ISCENE_H_

#include <Core/Standard.h>
#include <Core/Math/Vector.h>

#include <Renderer/IBox.h>
#include <Renderer/ICamera.h>
#include <Renderer/IMesh.h>
#include <Renderer/ILight.h>
#include <Renderer/IBillboard.h>
#include <Renderer/IDecal.h>
#include <Renderer/IFreeForm.h>
#include <Renderer/ITexture.h>
#include <Renderer/IGrassCluster.h>
#include <Renderer/ISpline.h>
#include <Renderer/IWaterPatch.h>
#include <Renderer/Settings.h>

namespace Renderer
{

/**
 * Cette classe représente une scène. Une scène est comme un micro-univers. Les objets
 * de scènes différentes ne peuvent pas intéragir. Le renderer peut avoir à gérer plusieurs
 * scènes. Typiquement, on utilise une scène pour représenter l'univers de jeu et une scène
 * pour l'interface, mais d'autres possibilités sont envisageables. Par exemple on peut
 * imaginer créer une scène qui représente ce qui est vu par une télévision dans une autre
 * scène (il faudrait pour cela pouvoir afficher une scène dans une texture, ce qui n'est pas
 * géré pour l'instant).
 */
class LM_API_RDR IScene
{
public:
    virtual ~IScene() {};
    virtual Ptr<IPerspectiveCamera> createPerspectiveCamera(float fov, float ratio,
                                                            float near, float far) = 0;

    virtual Ptr<IOrthoCamera> createOrthoCamera(float left, float right,
                                                float bottom, float top,
                                                float near, float far) = 0;

    virtual Ptr<IMeshInstance> createMeshInstance(const Ptr<IMesh> & pMesh) = 0;
    virtual Ptr<ISkeletonInstance> createSkeletonInstance(const Ptr<Assets::Skeleton> & pSkeleton) = 0;
    virtual Ptr<ISkinMeshInstance> createSkinMeshInstance(const Ptr<ISkinMesh> & pMesh,
                                                          const Ptr<ISkeletonInstance> & pSkeleton) = 0;

    virtual Ptr<IAABox> createAABox() = 0;

    virtual Ptr<ISpline> createSpline() = 0;

    virtual Ptr<IPointLight> createPointLight() = 0;

    virtual const MainLightSettings & getLightSettings() const = 0;
    virtual void setLightSettings(const MainLightSettings & settings) = 0;

    virtual Ptr<IGrassCluster> createGrassCluster(const Core::List<GrassPatch> & patches,
                                                  const Ptr<ITextureMap> & pTexture) = 0;

    virtual Ptr<IBillboardGroup> createBillboardGroup(int32 maxSize,
                                                      const Ptr<ITextureMap> & pTexture,
                                                      EBillboardMode mode) = 0;

    virtual Ptr<IDecal> createDecal(const Ptr<ITextureMap> & pTexture, EDecalMode mode) = 0;

    virtual Ptr<IFreeForm> createFreeForm(const Ptr<ITextureMap> & pTexture, EFreeFormMode mode, bool isLongLife = false) = 0;

    virtual Ptr<IWaterPatch> createWaterPatch(float width, float height) = 0;
    virtual float getWaterLevel() const = 0;
    virtual void setWaterLevel(float level) = 0;

    virtual const SkysphereSettings & getSkysphereSettings() = 0;
    virtual void setSkysphereSettings(const SkysphereSettings & settings) = 0;

    virtual const FogSettings & getFogSettings() = 0;
    virtual void setFogSettings(const FogSettings & settings) = 0;

    virtual void setWindSettings(int32 windId, const WindSettings & settings) = 0;

    virtual void update(double elapsed) = 0;
};
//-----------------------------------------------------------------------------
}
#endif /*ABSTRACT_ISCENE_H_*/
