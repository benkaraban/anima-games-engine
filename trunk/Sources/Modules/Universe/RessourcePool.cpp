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
#include <Core/Logger.h>
#include <Core/IO/Tools.h>
#include <Core/XML/XMLDocument.h>

#include <Assets/Model.h>
#include <Assets/TerrainModel.h>
#include <Assets/Texture.h>
#include <Assets/GeometricPrimitive.h>

#include <Audio/Wave.h>
#include <Audio/OggVorbis.h>

#include <Universe/RessourcePool.h>

namespace Universe
{
RessourcePool::RessourcePool(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Renderer::IRenderer> & pRenderer) :
    _pVFS(pVFS),
    _pRenderer(pRenderer)
{
    // Default mesh => sphere -------------------------------------------------

    _pDefaultModelMesh = Ptr<Assets::ModelMesh>(new Assets::ModelMesh());
    Assets::VerticeSet & vset = _pDefaultModelMesh->getLODByID(0);

    Assets::createGeoSphere(vset, 50.0, 2, true, Assets::GSP_ICOSAHEDRON);
    _pDefaultModelMesh->cleanup();

    _pDefaultMesh = _pRenderer->uploadMesh(_pDefaultModelMesh, *this);
    _pDefaultSkinMesh = _pRenderer->uploadSkinMesh(_pDefaultModelMesh, *this);


    // Default model => sphere avec anim fixe ---------------------------------

    _pDefaultModel = Ptr<Assets::Model>(new Assets::Model());

    _pDefaultModel->addMesh(L"default-mesh", *_pDefaultModelMesh);
    _pDefaultModel->addMeshNode(L"default-node", Assets::ROOT_NODE_ID, 0);
    _pDefaultModel->getSkeleton().addBone(0, Core::Matrix4f(Core::IDENTITY));
    _pDefaultModel->cleanup();

    Assets::KeyFrame key1(0, Core::Transformf(Core::IDENTITY));
    Assets::KeyFrame key2(1, Core::Transformf(Core::IDENTITY));

    Assets::Animation anim(1);
    anim.addKeyFrame(0, key1);
    anim.addKeyFrame(0, key2);

    _pDefaultModel->addAnimation(L"default-anim", anim);
}

RessourcePool::~RessourcePool()
{
    _documents.clear();
    _tunes.clear();
    _splines.clear();
    _scripts.clear();

    _terrains.clear();
    _models.clear();
    _textures.clear();
    _cubeTextures.clear();

    _meshes.clear();
    _skinMeshes.clear();

    _texturesMaps.clear();
    _cubeTexturesMaps.clear();

    _pDefaultModel = null;
    _pDefaultModelMesh = null;
    _pDefaultMesh = null;
    _pDefaultSkinMesh = null;

    _pRenderer = null;
    _pTexGen = null;
    _pVFS = null;
    _pMaster = null;
}

Ptr<Assets::TerrainModel> RessourcePool::getTerrainModel(const String & name)
{
    try
    {
        Ptr<Assets::TerrainModel> result(getLoadedTerrainModel(name));

        if(result == null)
        {
            if(_pMaster != null)
                result = _pMaster->getLoadedTerrainModel(name);

            if(result == null)
            {
                result = Ptr<Assets::TerrainModel>(new Assets::TerrainModel());
                Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

                if(pInput == null)
                    throw Core::IOException(L"Unable to open file");

                result->read(*pInput);
            }

            _terrains[name] = result;
        }

        return result;
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read terrain model '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }
}

Ptr<Assets::Model> RessourcePool::getModel(const String & name)
{
    try
    {
        Ptr<Assets::Model> result(getLoadedModel(name));

        if(result == null)
        {
            if(_pMaster != null)
                result = _pMaster->getLoadedModel(name);

            if(result == null)
            {
                result = Ptr<Assets::Model>(new Assets::Model());
                Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

                if(pInput == null)
                    throw Core::IOException(L"Unable to open file");

                result->read(*pInput);
            }

            _models[name] = result;
        }

        return result;
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read model '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }
}

void RessourcePool::addModel(const String & name, const Ptr<Assets::Model> & pMesh)
{
    if(_models[name] == null)
        _models[name] = pMesh;
}

void RessourcePool::addTexture(const String & name, const Ptr<Assets::Texture> & pTex)
{
    if(_textures[name] == null)
        _textures[name] = pTex;
}

Ptr<Assets::Texture> RessourcePool::getTextureData(const String & name)
{
    try
    {
        Ptr<Assets::Texture> result;

        if(name != String::EMPTY)
        {
            result = getLoadedTextureData(name);

            if(result == null)
            {
                if(_pMaster != null)
                    result = _pMaster->getLoadedTextureData(name);

                if(result == null)
                {
                    if(isOutsideTexture(name))
                    {
                        if(_pTexGen == null)
                            throw Core::IOException(L"On the fly texture generation off. Cannot read *outside* file " + name);

                        result = _pTexGen->generateTexture(name);

                        if(result == null)
                            throw Core::IOException(L"Unable to open *outside* file " + name);
                    }
                    else
                    {
                        result = Ptr<Assets::Texture>(new Assets::Texture());
                        Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

                        if(pInput == null)
                            throw Core::IOException(L"Unable to open file");

                        result->read(*pInput);
                    }
                }

                _textures[name] = result;
            }
        }

        return result;
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read texture '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }
}

Ptr<Assets::CubeTexture> RessourcePool::getCubeTextureData(const String & name)
{
    Ptr<Assets::CubeTexture> result;

    try
    {
        if(name != String::EMPTY)
        {
            Core::Map<String, Ptr<Assets::CubeTexture> >::const_iterator iTex = _cubeTextures.find(name);

            if(iTex !=_cubeTextures.end())
            {
                result = iTex->second;
            }
            else
            {
                result = Ptr<Assets::CubeTexture>(new Assets::CubeTexture());
                Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

                if(pInput == null)
                    throw Core::IOException(L"Unable to open file");

                result->read(*pInput);

                _cubeTextures[name] = result;
            }
        }

        return result;
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read cube texture '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }

    return result;
}

Ptr<Renderer::IMesh> RessourcePool::getMesh(const Ptr<Assets::ModelMesh> & pMesh)
{
    Ptr<Renderer::IMesh> result(getLoadedMesh(pMesh));

    if(result == null)
    {
        if(_pMaster != null)
            result = _pMaster->getLoadedMesh(pMesh);

        if(result == null)
        {
            result = _pRenderer->uploadMesh(pMesh, *this);
            _meshes[pMesh.get()] = result;
        }
    }

    return result;
}

Ptr<Renderer::ISkinMesh> RessourcePool::getSkinMesh(const Ptr<Assets::ModelMesh> & pMesh)
{
    Ptr<Renderer::ISkinMesh> result(getLoadedSkinMesh(pMesh));

    if(result == null)
    {
        if(_pMaster != null)
            result = _pMaster->getLoadedSkinMesh(pMesh);

        if(result == null)
        {
            result = _pRenderer->uploadSkinMesh(pMesh, *this);
            _skinMeshes[pMesh.get()] = result;
        }
    }

    return result;
}

Ptr<Renderer::ITextureMap> RessourcePool::getTexture(const String & name)
{
    Ptr<Renderer::ITextureMap> result;

    if(name != String::EMPTY)
    {
        result = getLoadedTexture(name);

        if(result == null)
        {
            if(_pMaster != null)
                result = _pMaster->getLoadedTexture(name);

            if(result == null)
            {
                Ptr<Assets::Texture> pTexData(getTextureData(name));
                result = _pRenderer->uploadTexture(pTexData);
                _texturesMaps[name] = result;
            }
        }
    }

    return result;
}

Ptr<Core::Trajectory> RessourcePool::getSpline(const String & name)
{
    Core::Map<String, Ptr<Spline> >::iterator iSpline = _splines.find(name);
    Ptr<Spline> pSpline;

    try
    {
        if(iSpline == _splines.end())
        {
            Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

            if(pInput == null)
                throw Core::Exception(L"file not found");

            Ptr<Core::XMLDocument> pDoc(new Core::XMLDocument());
            pDoc->loadDocument(*pInput);

            pSpline = Ptr<Spline>(new Spline());
            pSpline->date = _pVFS->getLastModificationDate(name);
            pSpline->pSpline = Ptr<Core::Trajectory>(new Core::Trajectory());
            Ptr<Core::XMLNode> pXMLNode = pDoc->firstChildNode(L"root");

            if(pXMLNode == null)
                throw Core::Exception(L"corrupted file (root)");

            pXMLNode = pXMLNode->firstChildNode(L"node");

            if(pXMLNode == null)
                throw Core::Exception(L"corrupted file");

            pSpline->pSpline->importXML(pXMLNode);

            _splines[name] = pSpline;
        }
        else
        {
            pSpline = iSpline->second;

            if(_pVFS->getLastModificationDate(name) > pSpline->date)
            {
                Ptr<Core::InputStream> pInput(_pVFS->readFile(name));
                Ptr<Core::XMLDocument> pDoc(new Core::XMLDocument());
                pDoc->loadDocument(*pInput);

                pSpline->date = _pVFS->getLastModificationDate(name);
                Ptr<Core::XMLNode> pXMLNode = pDoc->firstChildNode(L"root");

                if(pXMLNode == null)
                    throw Core::Exception(L"corrupted file (root)");

                pXMLNode = pXMLNode->firstChildNode(L"node");

                if(pXMLNode == null)
                    throw Core::Exception(L"corrupted file");

                pSpline->pSpline->importXML(pXMLNode);
            }
        }
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read spline '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }

    return pSpline->pSpline;
}

const String8 & RessourcePool::getScript(const String & name)
{
    Core::Map<String, Ptr<Script> >::iterator iScript = _scripts.find(name);
    Ptr<Script> pScript;

    try
    {
        if(iScript == _scripts.end())
        {
            Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

            if(pInput == null)
                throw Core::Exception(L"file not found");

            pScript = Ptr<Script>(new Script());
            pScript->date = _pVFS->getLastModificationDate(name);
            Core::readAllStream(pScript->source, *pInput);

            _scripts[name] = pScript;
        }
        else
        {
            pScript = iScript->second;

            if(_pVFS->getLastModificationDate(name) > pScript->date)
            {
                Ptr<Core::InputStream> pInput(_pVFS->readFile(name));
                Core::readAllStream(pScript->source, *pInput);
            }
        }

        return pScript->source;
    }
    catch(Core::Exception & e)
    {
        ERR << L"Resource manager is unable to read script '" << name << L"' : " << e.getMessage();
        return String8::EMPTY;
    }
}

Ptr<Audio::Playable> RessourcePool::getTune(const String & name)
{
    Core::Map<String, Ptr<Audio::Playable> >::iterator iPlayable = _tunes.find(name);
    Ptr<Audio::Playable> pPlayable = null;

    try
    {
        if(iPlayable == _tunes.end())
        {
            Ptr<Core::InputStream> pInput = _pVFS->readFile(name);

            if(pInput == null)
                throw Core::Exception(L"file not found");

            if(Core::System::getFileExt(name) == L"wav")
            {
                pPlayable = Ptr<Audio::Playable>(new Audio::Wave());
            }
            else if(Core::System::getFileExt(name) == L"ogg")
            {
                pPlayable = Ptr<Audio::Playable>(new Audio::OggVorbis());
            }
            else
            {
                Core::String message;
                message <<L"Unknown audio file extension ("<<Core::System::getFileExt(name)<<L")";
                throw Core::Exception(message);
            }


            pPlayable->read(*pInput);

            _tunes[name] = pPlayable;
        }
        else
        {
            pPlayable = iPlayable->second;
        }

        return pPlayable;
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read audio '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }
}

Ptr<Renderer::ITextureMap> RessourcePool::reloadTexture(const String & name)
{
    _pRenderer->lock();

    Ptr<Renderer::ITextureMap> result;
    if(name != String::EMPTY)
    {
        if(_texturesMaps.find(name) != _texturesMaps.end())
        {
            Ptr<Assets::Texture> pTexData;

            if(isOutsideTexture(name))
            {
                if(_pTexGen != null)
                {
                    pTexData = _pTexGen->generateTexture(name);
                    
                    if(pTexData != null)
                        result = _pRenderer->uploadTexture(pTexData);
                }

                if(result == null)
                    throw Core::IOException(L"Unable to open *outside* file " + name);
            }
            else
            {
                pTexData = Ptr<Assets::Texture>(new Assets::Texture());
                Ptr<Core::InputStream> pInput(_pVFS->readFile(name));
                
                if(pInput == null)
                {
                    Core::String message;
                    message << L"Reload failed : unable to open file "<<name;
                    throw Core::IOException(message);
                }

                pTexData->read(*pInput);

                result = _pRenderer->uploadTexture(pTexData);
            }

            _texturesMaps[name]->copyTexture(result);
            (*_textures[name]) = *pTexData;
        }
    }
    
    _pRenderer->unlock();

    return result;
}

Ptr<Assets::Model> RessourcePool::reloadModel(const String & name)
{
    _pRenderer->lock();

    Ptr<Assets::Model> pResult;
    Core::Map<String, Ptr<Assets::Model> >::iterator iModel = _models.find(name);

    if(name != String::EMPTY && iModel != _models.end())
    {
        Ptr<Assets::Model> pOldModel(iModel->second);
        Ptr<Assets::Model> pNewModel(new Assets::Model());
        Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

        if(pInput == null)
            throw Core::IOException(L"Unable to reload file " + name);

        pNewModel->read(*pInput);
        pResult = pNewModel;

        for(int32 iMesh=0; iMesh < pNewModel->getMeshCount(); iMesh++)
        {
            Ptr<Assets::ModelMesh> pNewMesh = pNewModel->getMeshPtr(iMesh);
            int32 oldMeshId = pOldModel->getMeshId(pNewMesh->getName());

            if(oldMeshId != Assets::UNDEFINED_MESH_ID)
            {
                Ptr<Assets::ModelMesh> pOldMesh = pOldModel->getMeshPtr(oldMeshId);

                pNewModel->replaceMeshPtr(iMesh, pOldMesh); // Après ça *pOldMesh == *pNewMesh

                if(pOldMesh->isSkin())
                {
                    Core::Map<const Assets::ModelMesh*, Ptr<Renderer::ISkinMesh> >::iterator iRenderMesh = _skinMeshes.find(pOldMesh.get());

                    if(iRenderMesh != _skinMeshes.end())
                        iRenderMesh->second->copySkinMesh(_pRenderer->uploadSkinMesh(pOldMesh, *this));
                }
                else
                {
                    Core::Map<const Assets::ModelMesh*, Ptr<Renderer::IMesh> >::iterator iRenderMesh = _meshes.find(pOldMesh.get());

                    if(iRenderMesh != _meshes.end())
                        iRenderMesh->second->copyMesh(_pRenderer->uploadMesh(pOldMesh, *this));
                }

            }
        }

        *pOldModel = *pNewModel;
    }

    _pRenderer->unlock();

    return pResult;
}

Ptr<Assets::TerrainModel> RessourcePool::reloadTerrainModel(const String & name)
{
    Ptr<Assets::TerrainModel> pResult;
    Core::Map<String, Ptr<Assets::TerrainModel> >::iterator iTerrain= _terrains.find(name);

    if(name != String::EMPTY && iTerrain != _terrains.end())
    {
        Ptr<Assets::TerrainModel> pOldTerrain(iTerrain->second);
        Ptr<Assets::TerrainModel> pNewTerrain(new Assets::TerrainModel());
        Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

        if(pInput == null)
            throw Core::IOException(L"Unable to reload file " + name);

        pNewTerrain->read(*pInput);
        pResult = pNewTerrain;

        for(int32 iMesh=0; iMesh < pNewTerrain->getMeshCount(); iMesh++)
        {
            Ptr<Assets::ModelMesh> pNewMesh = pNewTerrain->getMeshPtr(iMesh);
            int32 oldMeshId = pOldTerrain->getMeshId(pNewMesh->getName());

            if(oldMeshId != Assets::UNDEFINED_MESH_ID)
            {
                Ptr<Assets::ModelMesh> pOldMesh = pOldTerrain->getMeshPtr(oldMeshId);

                pNewTerrain->replaceMeshPtr(iMesh, pOldMesh); // Après ça *pOldMesh == *pNewMesh

                Core::Map<const Assets::ModelMesh*, Ptr<Renderer::IMesh> >::iterator iRenderMesh = _meshes.find(pOldMesh.get());

                if(iRenderMesh != _meshes.end())
                    iRenderMesh->second->copyMesh(_pRenderer->uploadMesh(pOldMesh, *this));
            }
        }

        *pOldTerrain = *pNewTerrain;
    }

    return pResult;
}

Ptr<Renderer::ICubeTextureMap> RessourcePool::getCubeTexture(const String & name)
{
    Ptr<Renderer::ICubeTextureMap> result;

    try
    {
        if(name != String::EMPTY)
        {
            Core::Map<String, Ptr<Renderer::ICubeTextureMap> >::const_iterator iCube = _cubeTexturesMaps.find(name);

            if(iCube != _cubeTexturesMaps.end())
            {
                result = iCube->second;
            }
            else
            {
                Ptr<Assets::CubeTexture> pSrcTex = getCubeTextureData(name);
                if(pSrcTex == null)
                    throw Core::Exception(L"file not opened\n");

                result = _pRenderer->uploadTexture(pSrcTex);
                _cubeTexturesMaps[name] = result;
            }
        }
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read cube texture '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }

    return result;
}

Ptr<Core::XMLDocument> RessourcePool::getDocument(const String & name)
{
    Core::Map<String, Ptr<Document> >::iterator iDoc = _documents.find(name);
    Ptr<Document> pDoc;
    Ptr<Core::XMLDocument> pResult;

    try
    {
        if(iDoc == _documents.end() || iDoc->second->date < _pVFS->getLastModificationDate(name))
        {
            Ptr<Core::InputStream> pInput(_pVFS->readFile(name));
            
            if(pInput == null)
                throw Core::IOException(L"Unable to open file");

            pDoc = Ptr<Document>(new Document());
            pDoc->pDoc = Ptr<Core::XMLDocument>(new Core::XMLDocument());
            pDoc->date = _pVFS->getLastModificationDate(name);

            pResult = pDoc->pDoc;

            _documents[name] = pDoc;
            
            if(!pResult->loadDocument(*pInput))
                throw Core::IOException(L"Corrupted file");
        }
        else
        {
            pResult = iDoc->second->pDoc;
        }
    }
    catch(Core::Exception & e)
    {
        String message(L"Resource manager is unable to read document '");
        message << name << L"' : " << e.getMessage();
        ERR << message << L"\n";
        throw Core::IOException(message);
    }


    return pResult;
}

Ptr<Assets::Model> RessourcePool::getLoadedModel(const String & name)
{
    Ptr<Assets::Model> result(null);
    Core::Map<String, Ptr<Assets::Model> >::const_iterator ipModel = _models.find(name);

    if(ipModel != _models.end())
        result = ipModel->second;

    return result;
}

Ptr<Assets::TerrainModel> RessourcePool::getLoadedTerrainModel(const String & name)
{
    Ptr<Assets::TerrainModel> result(null);
    Core::Map<String, Ptr<Assets::TerrainModel> >::const_iterator ipTerrain = _terrains.find(name);

    if(ipTerrain != _terrains.end())
        result = ipTerrain->second;

    return result;
}

Ptr<Assets::Texture> RessourcePool::getLoadedTextureData(const String & name)
{
    Ptr<Assets::Texture> result(null);
    Core::Map<String, Ptr<Assets::Texture> >::const_iterator ipTex = _textures.find(name);

    if(ipTex != _textures.end())
        result = ipTex->second;

    return result;
}

Ptr<Assets::CubeTexture> RessourcePool::getLoadedCubeTextureData(const String & name)
{
    return Ptr<Assets::CubeTexture>(null);
}

Ptr<Renderer::IMesh> RessourcePool::getLoadedMesh(const Ptr<Assets::ModelMesh> & pMesh)
{
    Ptr<Renderer::IMesh> result(null);
    Core::Map<const Assets::ModelMesh *, Ptr<Renderer::IMesh> >::const_iterator ipMesh = _meshes.find(pMesh.get());

    if(ipMesh != _meshes.end())
        result = ipMesh->second;

    return result;
}
Ptr<Renderer::ISkinMesh> RessourcePool::getLoadedSkinMesh(const Ptr<Assets::ModelMesh> & pMesh)
{
    Ptr<Renderer::ISkinMesh> result(null);
    Core::Map<const Assets::ModelMesh *, Ptr<Renderer::ISkinMesh> >::const_iterator ipMesh = _skinMeshes.find(pMesh.get());

    if(ipMesh != _skinMeshes.end())
        result = ipMesh->second;

    return result;
}
Ptr<Renderer::ITextureMap> RessourcePool::getLoadedTexture(const String & name)
{
    Ptr<Renderer::ITextureMap> result(null);
    Core::Map<String, Ptr<Renderer::ITextureMap> >::const_iterator ipTex = _texturesMaps.find(name);

    if(ipTex != _texturesMaps.end())
        result = ipTex->second;

    return result;
}
Ptr<Renderer::ICubeTextureMap> RessourcePool::getLoadedCubeTexture(const String & name)
{
    return Ptr<Renderer::ICubeTextureMap>(null);
}

Ptr<Assets::Model> RessourcePool::getDefaultModel() const
{
    return _pDefaultModel;
}

Ptr<Renderer::IMesh> RessourcePool::getDefaultMesh() const
{
    return _pDefaultMesh;
}

Ptr<Renderer::ISkinMesh> RessourcePool::getDefaultSkinMesh() const
{
    return _pDefaultSkinMesh;
}

Ptr<Assets::ModelMesh> RessourcePool::getDefaultModelMesh() const
{
    return _pDefaultModelMesh;
}

Ptr<Renderer::ITextureMap> RessourcePool::getDefaultTexture() const
{
    return _pRenderer->getDefaultTexture();
}



void RessourcePool::setMasterPool(const Ptr<RessourcePool> & pPool)
{
    _pMaster = pPool;
}

void RessourcePool::cleanPool()
{
    _terrains.clear();
    _models.clear();
    _textures.clear();
    _cubeTextures.clear();

    _meshes.clear();
    _texturesMaps.clear();
    _cubeTexturesMaps.clear();
}

bool RessourcePool::isOutsideTexture(const String & name) const
{
    return !name.startsWith(L"/");
}

void RessourcePool::setOnTheFlyTextureGenerator(const Ptr<IOnTheFlyTextureGenerator> & pTexGen)
{
    _pTexGen = pTexGen;
}

const Ptr<IOnTheFlyTextureGenerator> & RessourcePool::getOnTheFlyTextureGenerator() const
{
    return _pTexGen;
}

}

