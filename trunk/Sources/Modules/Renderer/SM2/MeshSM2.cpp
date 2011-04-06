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
#include <Renderer/SM2/MeshSM2.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
MaterialData::MaterialData() :
    isUpdatingReflexion(false),
    isHidden(false)
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
MeshInstanceSM2::MeshInstanceSM2(const Ptr<IMesh> & pMesh, const Ptr<GPUResourceLib> & pResLib) : MeshInstance(pMesh), _pResLib(pResLib)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
MeshInstanceSM2::~MeshInstanceSM2()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void MeshInstanceSM2::setMaterial(const Renderer::Material & material)
{
    MeshInstance::setMaterial(material);

    if(material.reflexions.isZero(0.0) && _matData.pReflexionsCube != null)
    {
        _matData.pReflexionsCube = null;
        for(int32 face=Gfx::ECubeMapFace_COUNT - 1; face >= 0; face--)
            _matData.pRenderTarget[face] = null;
    }
}
//-----------------------------------------------------------------------------
void MeshInstanceSM2::onDeviceLost()
{
     for(int32 ii=0; ii < Gfx::ECubeMapFace_COUNT; ii++)
        _matData.pRenderTarget[ii] = null;
     _matData.pReflexionsCube = null;
}
//-----------------------------------------------------------------------------
void MeshInstanceSM2::onDeviceReset()
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
SkinMeshInstanceSM2::SkinMeshInstanceSM2(const Ptr<ISkinMesh> & pMesh, const Ptr<ISkeletonInstance> & pSkeleton, const Ptr<GPUResourceLib> & pResLib) :
    SkinMeshInstance(pMesh, pSkeleton),
    _pResLib(pResLib)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
SkinMeshInstanceSM2::~SkinMeshInstanceSM2()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void SkinMeshInstanceSM2::setMaterial(const Renderer::Material & material)
{
    SkinMeshInstance::setMaterial(material);

    if(material.reflexions.isZero(0.0) && _matData.pReflexionsCube != null)
    {
        _matData.pReflexionsCube = null;
        for(int32 face=Gfx::ECubeMapFace_COUNT - 1; face >= 0; face--)
            _matData.pRenderTarget[face] = null;
    }
}
//-----------------------------------------------------------------------------
void SkinMeshInstanceSM2::onDeviceLost()
{
    _matData.pReflexionsCube = null;

    for(int32 ii=0; ii < Gfx::ECubeMapFace_COUNT; ii++)
        _matData.pRenderTarget[ii] = null;
}
//-----------------------------------------------------------------------------
void SkinMeshInstanceSM2::onDeviceReset()
{
}
//-----------------------------------------------------------------------------
}
