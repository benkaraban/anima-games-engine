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
#include <Core/Strings/Tools.h>
#include <Core/Logger.h>

#include <Packing/LightmapGen.h>

#include <Images/Image.h>

#include <Workflow/TexCompression.h>

namespace Packing
{
//-----------------------------------------------------------------------------
struct LightMapWIP
{
    String                     name;
    Ptr<Core::Bitmap>          pBitmap;
    int32                      pixelCount;

    Renderer::LightMapInfos      genInfos;
    Ptr<Renderer::IMeshInstance> pMesh;

    Universe::ENodeType        type;

    // Soit
    Ptr<Universe::NodeMesh>    pNodeMesh;

    // Soit
    Ptr<Universe::NodeTerrain> pNodeTerrain;
    int32                      iLOD;
    int32                      iBlock;
};
//-----------------------------------------------------------------------------
void generateLightmaps(const LightmapsGenOptions & options)
{
    // Pour chaque texture diffuse, plusieurs textures lightmaps sont envisageables
    Core::Map<String, Core::List<Ptr<LightMapWIP> > > lightmaps;

    float progressionStep = (options.compressTextures ? 0.5f : 0.9f);
    Ptr<Core::IProgressObs> pGenObs(new Core::SubProgressObs(options.pObserver, 0.0f, progressionStep));
    Ptr<Core::IProgressObs> pPakObs(new Core::SubProgressObs(options.pObserver, progressionStep, 1.0f));

    Ptr<Core::VirtualFileSystem> pVFS = options.pWorld->getRessourcesPool()->getVFS();

    const Core::List<Ptr<Universe::Node> > & allNodes = options.pWorld->getAllNodes();
    Core::List<Ptr<Universe::NodeMesh> > meshes;
    Core::List<Ptr<Universe::NodeTerrain> > terrains;
    Core::List<Ptr<Universe::NodePointLight> > lights;
    String texName;

    Ptr<Renderer::IRenderer> pRenderer(options.pWorld->getRenderer());
    Ptr<Renderer::IScene> pSourceScene(options.pWorld->getScene());
    Ptr<Renderer::IScene> pStaticScene(pRenderer->createScene());

    pStaticScene->setLightSettings(pSourceScene->getLightSettings());
    pStaticScene->setFogSettings(pSourceScene->getFogSettings());

    // Filtres les nodes sur lesquels on va travailler

    for(int32 ii=0; ii < allNodes.size(); ii++)
    {
        if(options.tag == String::EMPTY || allNodes[ii]->hasTag(options.tag))
        {
            if(allNodes[ii]->getNodeType() == Universe::NODE_MESH)
            {
                Ptr<Universe::NodeMesh> pMesh = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(allNodes[ii]);
                if(pMesh->isStatic() && pMesh->isVisible() && pMesh->getMeshInstance()->getMaterial().diffuse.a > 0.0f)
                    meshes.push_back(pMesh);
            }
            else if(allNodes[ii]->getNodeType() == Universe::NODE_TERRAIN)
            {
                Ptr<Universe::NodeTerrain> pTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(allNodes[ii]);
                if(pTerrain->isVisible())
                    terrains.push_back(pTerrain);
            }
        }

        if(allNodes[ii]->getNodeType() == Universe::NODE_POINT_LIGHT)
        {
            Ptr<Universe::NodePointLight> pLight = LM_DEBUG_PTR_CAST<Universe::NodePointLight>(allNodes[ii]);
            if(pLight->isVisible() && pLight->isStatic())
                lights.push_back(pLight);
        }
    }


    // Prépare les données => NODE_POINT_LIGHT

    for(int32 ii=0; ii < lights.size(); ii++)
    {
        Universe::NodePointLight & light = *lights[ii];

        Ptr<Renderer::IPointLight> pStaticLight = pStaticScene->createPointLight();
        pStaticLight->setAmbientColor(light.getAmbientColor());
        pStaticLight->setDiffuseColor(light.getDiffuseColor());
        pStaticLight->setSpecularColor(light.getSpecularColor());
        pStaticLight->setPosition(light.getWorldPosition());
        pStaticLight->setRadius(light.getRadius());
        pStaticLight->setShadowCasting(light.getShadowCasting());

        light.kill();
    }

    // Prépare les données => NODE_MESH

    int32 totalSize = 0;

    for(int32 ii=0; ii < meshes.size(); ii++)
    {
        Ptr<Renderer::IMeshInstance> pSourceMesh(meshes[ii]->getMeshInstance());
        Ptr<Renderer::IMeshInstance> pStaticMesh = pStaticScene->createMeshInstance(pSourceMesh->getMesh());
        pStaticMesh->setMaterial(pSourceMesh->getMaterial());
        pStaticMesh->setWorldMatrix(pSourceMesh->getWorldMatrix());

        const Renderer::Material & mat = pSourceMesh->getMaterial();

        int32 width = options.defaultResolution;
        int32 height = options.defaultResolution;

        if(mat.pDiffuseMap != null)
        {
            int32 ratio = meshes[ii]->getLightmapResolutionRatio();
            width = std::max(1, mat.pDiffuseMap->getSourceTexture()->getWidth() / ratio);
            height = std::max(1, mat.pDiffuseMap->getSourceTexture()->getHeight() / ratio);
        }

        Ptr<LightMapWIP> pLightmap(new LightMapWIP());

        pLightmap->type = Universe::NODE_MESH;
        pLightmap->pNodeMesh = meshes[ii];
        pLightmap->pMesh = meshes[ii]->getMeshInstance();
        pLightmap->pixelCount = width * height;
        pLightmap->genInfos.width = width;
        pLightmap->genInfos.height = height;
        pLightmap->genInfos.internalTextureBorder = false;
        pLightmap->genInfos.pScene = pStaticScene;

        meshes[ii]->getDiffuseTexture(texName);
        lightmaps[texName].push_back(pLightmap);

        totalSize += pLightmap->pixelCount;
    }

    // Prépare les données => NODE_TERRAIN

    for(int32 ii=0; ii < terrains.size(); ii++)
    {
        for(int32 iLOD=0; iLOD < terrains[ii]->getLODCount(); iLOD++)
        {
            for(int32 iBlock=0; iBlock < terrains[ii]->getBlockCountForLOD(iLOD); iBlock++)
            {
                Ptr<Renderer::IMeshInstance> pMesh = terrains[ii]->getBlockMesh(iLOD, iBlock);
                Ptr<Renderer::IMeshInstance> pStaticMesh = pStaticScene->createMeshInstance(pMesh->getMesh());
                pStaticMesh->setMaterial(pMesh->getMaterial());
                pStaticMesh->setWorldMatrix(pMesh->getWorldMatrix());

                const Renderer::Material & mat = pMesh->getMaterial();

                int32 width = options.defaultResolution;
                int32 height = options.defaultResolution;

                if(mat.pDiffuseMap != null)
                {
                    int32 ratio = terrains[ii]->getLightmapResolutionRatio();
                    width = std::max(1, mat.pDiffuseMap->getSourceTexture()->getWidth() / ratio);
                    height = std::max(1, mat.pDiffuseMap->getSourceTexture()->getHeight() / ratio);
                }

                Ptr<LightMapWIP> pLightmap(new LightMapWIP());

                pLightmap->type = Universe::NODE_TERRAIN;
                pLightmap->pNodeTerrain = terrains[ii];
                pLightmap->iLOD = iLOD;
                pLightmap->iBlock = iBlock;
                pLightmap->pMesh = pMesh;
                pLightmap->pixelCount = width * height;
                pLightmap->genInfos.width = width;
                pLightmap->genInfos.height = height;
                pLightmap->genInfos.internalTextureBorder = true;
                pLightmap->genInfos.pScene = pStaticScene;

                String texName(terrains[ii]->getDiffuseTexName(iLOD, iBlock));
                lightmaps[texName].push_back(pLightmap);

                totalSize += pLightmap->pixelCount;
            }
        }
    }

    // Génère les lightmaps

    int32 done = 0;

    for(Core::Map<String, Core::List<Ptr<LightMapWIP> > >::iterator iLM=lightmaps.begin(); iLM != lightmaps.end(); ++iLM)
    {
        Core::List<Ptr<LightMapWIP> > & lms = iLM->second;

        for(int32 iMap=0; iMap < lms.size(); iMap++)
        {
            float start = float(done) / float(totalSize);
            float end = float(done + lms[iMap]->pixelCount) / float(totalSize);

            lms[iMap]->genInfos.pObs = Ptr<Core::IProgressObs>(new Core::SubProgressObs(pGenObs, start, end));
            lms[iMap]->genInfos.pObs->progress(0.0f);
            lms[iMap]->pBitmap = options.pWorld->getRenderer()->generateLightMap(lms[iMap]->genInfos, lms[iMap]->pMesh);

            done += lms[iMap]->pixelCount;
        }
    }


    // Sauvegarde le résultat

    Workflow::TextureOptions texOptions;
    texOptions.texType = Workflow::TEXTURE_LIGHTMAP;
    texOptions.quality = Workflow::QUALITY_HIGH;
    texOptions.format  = (options.compressTextures ? Assets::TEX_FORMAT_DXTC1 : Assets::TEX_FORMAT_ARGB8);
    texOptions.generateMipmap = true;

    int32 uid = 0;
    done = 0;
    INF << L"\n";

    for(Core::Map<String, Core::List<Ptr<LightMapWIP> > >::iterator iLM=lightmaps.begin(); iLM != lightmaps.end(); ++iLM)
    {
        Core::List<Ptr<LightMapWIP> > & lms = iLM->second;

        for(int32 iMap=0; iMap < lms.size(); iMap++)
        {
            float start = float(done) / float(totalSize);
            float end = float(done + lms[iMap]->pixelCount) / float(totalSize);

            String uniqueName;
            Core::trimRight(options.path, uniqueName, L"/");
            uniqueName += L"/" + options.prefix;
            uniqueName += Core::toString(uid, 4);
            uniqueName += L".tex";

            INF << L"Saving " << uniqueName << L"\n";

            Ptr<Core::IProgressObs> pObs(new Core::SubProgressObs(pPakObs, start, end));
            texOptions.pProgressObs = pObs.get();
            Ptr<Images::Image> pImage = Images::toImage(lms[iMap]->pBitmap);
            Ptr<Assets::Texture> pTexture = Workflow::generateTexture(texOptions, pImage);
            pVFS->write(uniqueName, *pTexture);

            if(options.debugCopy)
                pImage->saveFile(L"Local/LM/" + Core::toString(uid, 4) + L".tga");

            if(lms[iMap]->type == Universe::NODE_MESH)
            {
                lms[iMap]->pNodeMesh->setLightTexture(uniqueName);
                lms[iMap]->pNodeMesh->setOcclusionTexture(L"");
            }
            else // if(lms[iMap]->type == Universe::NODE_TERRAIN)
            {
                lms[iMap]->pNodeTerrain->setBlockLightTexture(lms[iMap]->iLOD, lms[iMap]->iBlock, uniqueName);
                lms[iMap]->pNodeTerrain->setBlockOcclusionTexture(lms[iMap]->iLOD, lms[iMap]->iBlock, L"");
            }

            done += lms[iMap]->pixelCount;
            uid++;
        }
    }

    INF << L"\n";

    // TODO => générer la skybox
    //// Génère la light probe
    //{
    //    Renderer::LightProbeInfos lp;
    //    lp.pScene = options.pWorld->getScene();
    //    lp.size = 16;
    //    Ptr<Assets::CubeTexture> pLightProbe = options.pWorld->getRenderer()->generateLightProbe(lp);

    //    String name;
    //    Core::trimRight(options.path, name, L"/");
    //    name << L"/" << options.prefix << L"probe.ctex";

    //    pVFS->write(name, *pLightProbe);
    //    options.pWorld->setLightProbe(name);
    //}
};
//-----------------------------------------------------------------------------
}
