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
#include <Renderer/Stub/RendererStub.h>
#include <Renderer/Stub/RenderViewStub.h>
#include <Renderer/Stub/SceneStub.h>
#include <Renderer/Stub/MeshStub.h>
#include <Renderer/Stub/TextureStub.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
RendererStub::RendererStub() :
    _pDefaultView(new RenderViewStub(100, 100)),
    _pDefaultTexture(new TextureMapStub(null))
{
}
//-----------------------------------------------------------------------------
bool RendererStub::initialise()
{
    return true;
}
//-----------------------------------------------------------------------------
Ptr<IMesh> RendererStub::uploadMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber)
{
    return Ptr<IMesh>(new MeshStub(pMesh));
}
//-----------------------------------------------------------------------------
Ptr<ISkinMesh> RendererStub::uploadSkinMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber)
{
    return Ptr<ISkinMesh>(new SkinMeshStub(pMesh));
}
//-----------------------------------------------------------------------------
Ptr<ITextureMap> RendererStub::uploadTexture(const Ptr<Assets::Texture> & pTexture)
{
    return Ptr<ITextureMap>(new TextureMapStub(pTexture));
}
//-----------------------------------------------------------------------------
Ptr<ICubeTextureMap> RendererStub::uploadTexture(const Ptr<Assets::CubeTexture> & pTexture)
{
    return Ptr<ICubeTextureMap>(null);
}
//-----------------------------------------------------------------------------
Ptr<IScene> RendererStub::createScene()
{
    return Ptr<IScene>(new SceneStub());
}
//-----------------------------------------------------------------------------
Ptr<ITextureMap> RendererStub::getDefaultTexture() const
{
    return _pDefaultTexture;
}
//-----------------------------------------------------------------------------
void RendererStub::renderScene(const Ptr<IRenderView> & pView, const Ptr<IScene> & pScene, const Ptr<ICamera> & pCamera)
{
}
//-----------------------------------------------------------------------------
Ptr<IRenderView> RendererStub::getDefaultView() const
{
    return _pDefaultView;
}
//-----------------------------------------------------------------------------
Ptr<IRenderView> RendererStub::createView(int32 width, int32 height, int32 bufferCount, void * pHandle)
{
    return Ptr<IRenderView>(new RenderViewStub(width, height));
}
//-----------------------------------------------------------------------------

}
