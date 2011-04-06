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

#include <Renderer/Common/WaterPatch.h>
#include <Renderer/Common/Camera.h>
#include <Renderer/Common/Light.h>
#include <Renderer/Common/Decal.h>
#include <Renderer/Common/Box.h>
#include <Renderer/Common/Spline.h>

#include <Renderer/Stub/MeshStub.h>
#include <Renderer/Stub/BillboardStub.h>
#include <Renderer/Stub/GrassClusterStub.h>
#include <Renderer/Stub/SceneStub.h>

namespace Renderer
{
class FreeFormStub : public Core::Object, public IFreeForm
{
public:
    FreeFormStub(EFreeFormMode mode)
    {
        _mode = mode;
    };
    virtual void setGlowFlag(bool glow) {};
    virtual bool getGlowFlag() const { return false; };
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) { _worldMatrix = matrix; };
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const { matrix = _worldMatrix; };

    virtual void setMax(int32 maxVert, int32 maxIndices) {};
    virtual void getMax(int32 & maxVert, int32 & maxIndices) {};

    virtual void setMode(EFreeFormMode mode) { _mode = mode; };
    virtual EFreeFormMode getMode() const { return _mode; };

    virtual FreeFormData & beginUpdate() { return _data; };
    virtual void endUpdate() {};

    virtual void setPrecomputedAABox(const Core::AABoxf & box) {};
    virtual const Core::AABoxf & getPrecomputedAABox() const { return _box; };

    virtual void setWorldSpaceCoords(bool flag) {}
    virtual bool isWorldSpaceCoords() const {return false;}

    virtual void unsetPrecomputedAABox() {};
    virtual bool usePrecomputedAABox() const { return false; };

    virtual void setTexture(const Ptr<ITextureMap> & pTexture) { _pTexture = pTexture; };
    virtual Ptr<ITextureMap> getTexture() const { return _pTexture; };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

protected:
    bool                    _isAlive;
    bool                    _isVisible;

    int32                   _maxVertices;
    int32                   _maxIndices;

    EFreeFormMode           _mode;
    Core::Matrix4f          _worldMatrix;
    Core::Vector3f          _position;
    Ptr<ITextureMap>        _pTexture;

    Core::AABoxf            _localBox;
    Core::AABoxf            _box;

    FreeFormData            _data;

    int32                   _vCount;
    int32                   _iCount;
    mutable bool            _isDirty;
};

//-----------------------------------------------------------------------------
Ptr<IPerspectiveCamera> SceneStub::createPerspectiveCamera(float fov, float ratio,
                                                        float near, float far)
{
    return Ptr<IPerspectiveCamera>(new PerspectiveCamera(fov, ratio, near, far));
}
//-----------------------------------------------------------------------------
Ptr<IOrthoCamera> SceneStub::createOrthoCamera(float left, float right,
                                            float bottom, float top,
                                            float near, float far)
{
    return Ptr<IOrthoCamera>(new OrthoCamera(left, right, bottom, top, near, far));
}
//-----------------------------------------------------------------------------
Ptr<IMeshInstance> SceneStub::createMeshInstance(const Ptr<IMesh> & pMesh)
{
    return Ptr<IMeshInstance>(new MeshInstanceStub(pMesh));
}
//-----------------------------------------------------------------------------
Ptr<ISkeletonInstance> SceneStub::createSkeletonInstance(const Ptr<Assets::Skeleton> & pSkeleton)
{
    return Ptr<ISkeletonInstance>(new SkeletonInstanceStub(pSkeleton));
}
//-----------------------------------------------------------------------------
Ptr<ISkinMeshInstance> SceneStub::createSkinMeshInstance(const Ptr<ISkinMesh> & pMesh,
                                                      const Ptr<ISkeletonInstance> & pSkeleton)
{
    return Ptr<ISkinMeshInstance>(new SkinMeshInstanceStub(pMesh, pSkeleton));
}
//-----------------------------------------------------------------------------
Ptr<IAABox> SceneStub::createAABox()
{
    return Ptr<IAABox>(new AABox());
}
//-----------------------------------------------------------------------------
Ptr<ISpline> SceneStub::createSpline()
{
    return Ptr<ISpline>(new Spline());
}
//-----------------------------------------------------------------------------
Ptr<IDirLight> SceneStub::getDirLight()
{
    return Ptr<IDirLight>(null);
}
//-----------------------------------------------------------------------------
Ptr<IPointLight> SceneStub::createPointLight()
{
    return Ptr<IPointLight>(new PointLight());
}
//-----------------------------------------------------------------------------
const MainLightSettings & SceneStub::getLightSettings() const
{
    return _mainLightSettings;
}
//-----------------------------------------------------------------------------
void SceneStub::setLightSettings(const MainLightSettings & settings)
{
    _mainLightSettings = settings;
}
//-----------------------------------------------------------------------------
Ptr<IBillboardGroup> SceneStub::createBillboardGroup(int32 maxSize,
                                                     const Ptr<ITextureMap> & pTexture,
                                                     EBillboardMode mode)
{
    return Ptr<IBillboardGroup>(new BillboardGroupStub(pTexture, mode));
}
//-----------------------------------------------------------------------------
Ptr<IDecal> SceneStub::createDecal(const Ptr<ITextureMap> & pTexture, EDecalMode mode)
{
    return Ptr<IDecal>(new Decal(pTexture, mode));
}
//-----------------------------------------------------------------------------
Ptr<IFreeForm> SceneStub::createFreeForm(const Ptr<ITextureMap> & pTexture, EFreeFormMode mode, bool isLongLife)
{
    return Ptr<IFreeForm>(new FreeFormStub(mode));
}
//-----------------------------------------------------------------------------
const SkysphereSettings & SceneStub::getSkysphereSettings()
{
    return _skySettings;
}
//-----------------------------------------------------------------------------
void SceneStub::setSkysphereSettings(const SkysphereSettings & settings)
{
    _skySettings = settings;
}
//-----------------------------------------------------------------------------
Ptr<IGrassCluster> SceneStub::createGrassCluster(const Core::List<GrassPatch> & patches,
                                                const Ptr<ITextureMap> & pTexture)
{
    return Ptr<IGrassCluster>(new GrassClusterStub());
}
//-----------------------------------------------------------------------------
void SceneStub::setWindSettings(int32 windId, const WindSettings & settings)
{
    _windSettings[windId] = settings;
}
//-----------------------------------------------------------------------------
void SceneStub::update(double elapsed)
{
}
//-----------------------------------------------------------------------------
Ptr<IWaterPatch> SceneStub::createWaterPatch(float width, float height)
{
    return Ptr<IWaterPatch>(new WaterPatch(width, height));
}
//-----------------------------------------------------------------------------
}

