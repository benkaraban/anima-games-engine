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
#ifndef ABSTRACT_RENDERER_H_
#define ABSTRACT_RENDERER_H_

#include <Core/Standard.h>
#include <Core/IProgressObs.h>
#include <Core/Bitmap.h>

#include <Assets/Model.h>

#include <Gfx/GfxType.h>

#include <Renderer/ITexture.h>
#include <Renderer/IMesh.h>
#include <Renderer/IScene.h>
#include <Renderer/IHUD.h>
#include <Renderer/IRenderView.h>

namespace Renderer
{
class LM_API_RDR RendererException : public Core::Exception
{
public:
    RendererException(const String & message) : Exception(message) {};
};

struct LM_API_RDR RadiosityQuery
{
    RadiosityQuery();

    Ptr<IScene>                  pScene;
    Core::List<Core::Vector3f>   positions;
    Core::List<Core::Vector3f>   directions;
    float                        angle;

    bool                         screenShots;
};

struct LM_API_RDR LayerMapInfos
{
    struct LayerInfo
    {
        LayerInfo() : angle(0.f), color(1.0f, 1.0f, 1.0f, 1.0f), maskContrast(0.f), normalStrength(1.f), useDisplacementMap(false)
        {};

        Core::Vector4f   generateU; // texture u = dot(generateU, float3(worldX, worldY, worldZ, 1.0))
        Core::Vector4f   generateV; // texture v = dot(generateV, float3(worldX, worldY, worldZ, 1.0))

        Core::Vector4f   color;
        float            maskContrast;
        float            angle;
        float            normalStrength;
        bool             useDisplacementMap;
        Ptr<ITextureMap> pSourceTexture;
        Ptr<ITextureMap> pMaskTexture;
    };

    Core::List<LayerInfo> layers;

    Core::AABoxf    box;

    int32           width;
    int32           height;
    bool            isNormalMap;

    bool            useDetailLayer;
    LayerInfo       detailLayer;
};

struct LM_API_RDR OcclusionMapInfos
{
    OcclusionMapInfos() : superSampleUV(true), internalTextureBorder(false)
    {}

    int32                   width;
    int32                   height;
    bool                    internalTextureBorder;
    bool                    superSampleUV;
};

struct LM_API_RDR SkyboxInfos
{
    int32   resolution;
    Ptr<IScene>     pScene;
    Core::List<Ptr<Core::Bitmap> > bitmaps[Assets::ECubeFace_COUNT];
};

struct LM_API_RDR LightMapInfos
{
    LightMapInfos() : superSampleUV(true), internalTextureBorder(false)
    {}

    Ptr<IScene>             pScene;
    int32                   width;
    int32                   height;
    Ptr<Core::IProgressObs> pObs;
    bool                    internalTextureBorder;
    bool                    superSampleUV;

    bool   debugPositionBitmap;
    bool   debugNormalBitmap;

    Ptr<Core::Bitmap> pPositionBitmap;
    Ptr<Core::Bitmap> pNormalBitmap;
};

/**
 * Cette interface n'est pas une classe abstraite implémentée par les différentes implémentations
 * de Renderer. Elle fait office de callback pour permettre au Renderer s'obtenir les ressources
 * dont il a besoin. Typiquement, cette interface va être implémentée par le manager de ressources.
 */
class LM_API_RDR ITextureGrabber
{
public:
    virtual ~ITextureGrabber() {};
    virtual Ptr<ITextureMap> getTexture(const String & name) = 0;
    virtual Ptr<ICubeTextureMap> getCubeTexture(const String & name) = 0;
};

/**
 * La classe Renderer est le point d'entrée du module.
 * C'est la seule classe dont il est nécessaire de connaître une implémentation pour
 * utiliser le module. En effet, toutes les instances des autres classes du modules sont créées par
 * le Renderer ou par des instances qu'il a lui même créées.
 *
 * En particulier, le Renderer génère les ressources de rendu et la classe IScene permet de remplir
 * les scènes avec des entités de rendu.
 */
class LM_API_RDR IRenderer
{
public:
    virtual ~IRenderer() {};
    virtual bool initialise() = 0;

    /**
     * Charge un mesh dans le renderer. Le renderer retourne un IMesh qui peut être
     * utilisé pour créer des IMeshInstance. Si le mesh nécessite des textures, le Renderer
     * appelle les méthodes getTexture et getCubeTexture du ITextureGrabber pour les obtenir.
     * Attention : que ce soit au niveau des textures ou des mesh, le Renderer ne fait aucune
     * gestion intelligente des ressources : il charge tout ce qu'on lui dit de charger,
     * c'est à l'utilisateur d'éviter les doublons.
     */
    virtual Ptr<IMesh> uploadMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber) = 0;
    virtual Ptr<ISkinMesh> uploadSkinMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber) = 0;
    virtual Ptr<ITextureMap> uploadTexture(const Ptr<Assets::Texture> & pTexture) = 0;
    virtual Ptr<ICubeTextureMap> uploadTexture(const Ptr<Assets::CubeTexture> & pTexture) = 0;

    virtual Ptr<ITextureMap> getDefaultTexture() const = 0;

    virtual Ptr<IFont> createFont(const String & name, int32 height) = 0;
    virtual Ptr<IPictureBank> uploadPictureBank(const Ptr<Assets::PictureBank> & pPicBank) = 0;


    /**
     * Crée une scène. C'est ensuite dans cette cette que seront créés toutes les entités à rendre.
     */
    virtual Ptr<IScene> createScene() = 0;
    virtual Ptr<IHUD> createHUD() = 0;

    virtual Ptr<IRenderView> getDefaultView() const = 0;
    virtual Ptr<IRenderView> createView(int32 width, int32 height, int32 bufferCount, void * pHandle) = 0;

    virtual void renderScene(const Ptr<IRenderView> & pView, const Ptr<IScene> & pScene, const Ptr<ICamera> & pCamera) = 0;
    virtual void renderScenes(const Ptr<IRenderView> & pView, const Core::List<Ptr<IScene> > & scenes, const Ptr<ICamera> & pCamera) = 0;

    virtual void renderHUD(const Ptr<IRenderView> & pView, const Ptr<IHUD> & pHUD) = 0;

    virtual Ptr<Core::Bitmap> generateLightMap(const LightMapInfos & lm, const Ptr<IMeshInstance> & pMesh) = 0;
    virtual Ptr<Core::Bitmap> generateLayerMap(const LayerMapInfos & lm, const Ptr<IMeshInstance> & pMesh) = 0;
    virtual Ptr<Core::Bitmap> generateOcclusionMap(const OcclusionMapInfos & lm, const Ptr<Assets::VerticeSet> & vset) = 0;
    virtual void generateSkyBox(const SkysphereSettings & settings, SkyboxInfos & skybox) = 0;

    virtual void setRenderSettings(const RendererSettings & settings) = 0;
    virtual const RendererSettings & getRenderSettings() const = 0;
    virtual const RendererSettings & getMaxRenderSettings() const = 0;

    virtual void setDebugSettings(const DebugSettings & settings) = 0;
    virtual const DebugSettings & getDebugSettings() const = 0;

    virtual String getRenderStats() const = 0;
    virtual int32 getVRAMUsage() const = 0;

    virtual const Core::List<Resolution> & getAllResolutions() const = 0;
    virtual const Core::List<Resolution> & getPreferredResolutions() const = 0;

    virtual Gfx::EDeviceState getDeviceState() = 0;

    virtual void lock() = 0;
    virtual void unlock() = 0;

    virtual void onDeviceLost() = 0;
    virtual void onDeviceReset() = 0;
};
}

#endif /*ABSTRACT_RENDERER_H_*/
