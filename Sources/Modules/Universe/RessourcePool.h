/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef UNIVERSE_RESSOURCEPOOL_H_
#define UNIVERSE_RESSOURCEPOOL_H_

#include <Assets/TerrainModel.h>
#include <Core/XML/XMLDocument.h>
#include <Core/Math/Splines/Trajectory.h>
#include <Core/VFS/VirtualFileSystem.h>
#include <Renderer/IRenderer.h>
#include <Audio/Playable.h>

namespace Assets
{
class Texture;
class Model;
class TerrainModel;
}

namespace Universe
{
class LM_API_UNI IOnTheFlyTextureGenerator
{
public:
    virtual ~IOnTheFlyTextureGenerator() {};
    virtual Ptr<Assets::Texture> generateTexture(const String & name) = 0;
};

class LM_API_UNI RessourcePool : public Core::Object, public Renderer::ITextureGrabber
{
public:
    RessourcePool(const Ptr<Core::VirtualFileSystem> & pVFS,
                  const Ptr<Renderer::IRenderer> & pRenderer);

    virtual ~RessourcePool();

    virtual void                      addModel          (const String & name, const Ptr<Assets::Model> & pMesh);
    virtual void                      addTexture        (const String & name, const Ptr<Assets::Texture> & pTex);

    virtual Ptr<Assets::TerrainModel> getTerrainModel   (const String & name);
    virtual Ptr<Assets::Model>        getModel          (const String & name);
    virtual Ptr<Assets::Texture>      getTextureData    (const String & name);
    virtual Ptr<Assets::CubeTexture>  getCubeTextureData(const String & name);

    virtual Ptr<Renderer::IMesh>           getMesh       (const Ptr<Assets::ModelMesh> & pMesh);
    virtual Ptr<Renderer::ISkinMesh>       getSkinMesh   (const Ptr<Assets::ModelMesh> & pMesh);
    virtual Ptr<Renderer::ITextureMap>     getTexture    (const String & name);
    virtual Ptr<Renderer::ICubeTextureMap> getCubeTexture(const String & name);

    /**
     * Les scripts sont rechargés si le source a été modifié depuis le dernier chargement.
     */
    virtual const String8 &                getScript(const String & name);

    virtual Ptr<Audio::Playable>           getTune(const String & name);

    virtual Ptr<Core::Trajectory>          getSpline(const String & name);

    /**
     * Méthode permettant de recharger une texture si elle a déjà été chargée!
     */
    virtual Ptr<Renderer::ITextureMap>     reloadTexture(const String & name);
    virtual Ptr<Assets::Model>             reloadModel(const String & name);
    virtual Ptr<Assets::TerrainModel>      reloadTerrainModel(const String & name);

    virtual Ptr<Core::XMLDocument>         getDocument(const String & name);

    virtual Ptr<Assets::Model>             getDefaultModel() const;
    virtual Ptr<Renderer::IMesh>           getDefaultMesh() const;
    virtual Ptr<Renderer::ISkinMesh>       getDefaultSkinMesh() const;
    virtual Ptr<Assets::ModelMesh>         getDefaultModelMesh() const;
    virtual Ptr<Renderer::ITextureMap>     getDefaultTexture() const;

    void setMasterPool(const Ptr<RessourcePool> & pPool);
    void cleanPool();

    void setOnTheFlyTextureGenerator(const Ptr<IOnTheFlyTextureGenerator> & pTexGen);
    const Ptr<IOnTheFlyTextureGenerator> & getOnTheFlyTextureGenerator() const;

    const Ptr<Core::VirtualFileSystem> & getVFS() const { return _pVFS; };

protected:
    Ptr<Assets::TerrainModel> getLoadedTerrainModel   (const String & name);
    Ptr<Assets::Model>        getLoadedModel          (const String & name);
    Ptr<Assets::Texture>      getLoadedTextureData    (const String & name);
    Ptr<Assets::CubeTexture>  getLoadedCubeTextureData(const String & name);

    Ptr<Renderer::IMesh>           getLoadedMesh       (const Ptr<Assets::ModelMesh> & pMesh);
    Ptr<Renderer::ISkinMesh>       getLoadedSkinMesh   (const Ptr<Assets::ModelMesh> & pMesh);
    Ptr<Renderer::ITextureMap>     getLoadedTexture    (const String & name);
    Ptr<Renderer::ICubeTextureMap> getLoadedCubeTexture(const String & name);

    bool isOutsideTexture(const String & name) const;

    struct Script : public Core::Object
    {
        String8          source;
        Core::TimeValue  date;
    };

    struct Spline : public Core::Object
    {
        Ptr<Core::Trajectory>   pSpline;
        Core::TimeValue         date;
    };

    struct Document : public Core::Object
    {
        Ptr<Core::XMLDocument>  pDoc;
        Core::TimeValue         date;
    };

    Ptr<Core::VirtualFileSystem>    _pVFS;
    Ptr<Renderer::IRenderer>        _pRenderer;
    Ptr<RessourcePool>              _pMaster;
    Ptr<IOnTheFlyTextureGenerator>  _pTexGen;

    Ptr<Assets::Model>              _pDefaultModel;
    Ptr<Assets::ModelMesh>          _pDefaultModelMesh;
    Ptr<Renderer::IMesh>            _pDefaultMesh;
    Ptr<Renderer::ISkinMesh>        _pDefaultSkinMesh;

    Core::Map<String, Ptr<Assets::TerrainModel> > _terrains;
    Core::Map<String, Ptr<Assets::Model> >        _models;
    Core::Map<String, Ptr<Assets::Texture> >      _textures;
    Core::Map<String, Ptr<Assets::CubeTexture> >  _cubeTextures;

    Core::Map<const Assets::ModelMesh*, Ptr<Renderer::IMesh> >        _meshes;
    Core::Map<const Assets::ModelMesh*, Ptr<Renderer::ISkinMesh> >    _skinMeshes;
    Core::Map<String, Ptr<Renderer::ITextureMap> >               _texturesMaps;
    Core::Map<String, Ptr<Renderer::ICubeTextureMap> >           _cubeTexturesMaps;

    Core::Map<String, Ptr<Script> >             _scripts;
    Core::Map<String, Ptr<Spline> >             _splines;

    Core::Map<String, Ptr<Audio::Playable> >    _tunes;

    Core::Map<String, Ptr<Document> >           _documents;
};
}

#endif /* RESSOURCEPOOL_H_ */
