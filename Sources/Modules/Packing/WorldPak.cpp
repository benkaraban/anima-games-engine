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
#include <Core/Strings/Tools.h>
#include <Core/Logger.h>
#include <Core/TGA.h>
#include <Core/XML/XMLMath.h>

#include <Packing/WorldPak.h>

#include <Images/Image.h>

#include <Workflow/TexCompression.h>
#include <Workflow/ZonePacking.h>

Ptr<Core::Bitmap> borderDiffuse(const Ptr<Core::Bitmap> & pSrcBMP)
{
    using Core::PixelARGB8;

    int32 width = pSrcBMP->getWidth();
    int32 height = pSrcBMP->getHeight();
    byte * pData = new byte [width * height * 4];

    PixelARGB8 * pDst = (PixelARGB8 *)pData;
    Core::PixelARGB8 * pSrc = (PixelARGB8 *)pSrcBMP->getData();

    for(int32 y1=0; y1 < height; y1++)
    {
        int32 y0 = std::max(0, y1 - 1);
        int32 y2 = std::min(height - 1, y1 + 1);

        for(int32 x1=0; x1 < width; x1++)
        {
            int32 x0 = std::max(0, x1 - 1);
            int32 x2 = std::min(width - 1, x1 + 1);

            PixelARGB8 p00 = pSrc[y0 * width + x0];
            PixelARGB8 p10 = pSrc[y1 * width + x0];
            PixelARGB8 p20 = pSrc[y2 * width + x0];

            PixelARGB8 p01 = pSrc[y0 * width + x1];
            PixelARGB8 p11 = pSrc[y1 * width + x1];
            PixelARGB8 p21 = pSrc[y2 * width + x1];
            
            PixelARGB8 p02 = pSrc[y0 * width + x2];
            PixelARGB8 p12 = pSrc[y1 * width + x2];
            PixelARGB8 p22 = pSrc[y2 * width + x2];
            
            static const int32 THRESHOLD = 0xF0;

            if(p11.a > THRESHOLD)
                pDst[y1 * width + x1] = p11;
            else if(p01.a > THRESHOLD)
                pDst[y1 * width + x1] = p01;
            else if(p21.a > THRESHOLD)
                pDst[y1 * width + x1] = p21;
            else if(p10.a > THRESHOLD)
                pDst[y1 * width + x1] = p10;
            else if(p12.a > THRESHOLD)
                pDst[y1 * width + x1] = p12;
            else if(p00.a > 0)
                pDst[y1 * width + x1] = p00;
            else if(p02.a > 0)
                pDst[y1 * width + x1] = p02;
            else if(p20.a > 0)
                pDst[y1 * width + x1] = p20;
            else if(p22.a > 0)
                pDst[y1 * width + x1] = p22;
            else
                pDst[y1 * width + x1] = p11;
        }
    }

    return Ptr<Core::Bitmap>(new Core::Bitmap(pData, width, height, Core::ARGB_U8));
}


namespace Packing
{
//-----------------------------------------------------------------------------
struct PackedMesh
{
    Ptr<Universe::NodeMesh>     pNodeMesh;
    Ptr<Assets::ModelMesh>      pModelMesh;
}; 

//-----------------------------------------------------------------------------
struct InternalLightmap
{
    Ptr<Universe::NodeMesh>     pNodeMesh;
    Ptr<Assets::ModelMesh>      pModelMesh;

    Ptr<Core::Bitmap>  pSrcBitmap;

    String          atlasName;
    String          meshName;

    float           du;
    float           dv;

    float           mu;
    float           mv;
};
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
struct StaticMesh
{
    Ptr<Universe::NodeMesh>     pNodeMesh;
    Ptr<Assets::ModelMesh>      pModelMesh;
};
//-----------------------------------------------------------------------------
void spawnTerrains(const Ptr<Universe::World> & pWorld)
{
    Ptr<Universe::RessourcePool> pResPool = pWorld->getRessourcesPool();
    const Core::List<Ptr<Universe::Node> > & allNodes = pWorld->getAllNodes();

    for(int32 ii=0; ii < allNodes.size(); ii++)
    {
        if(allNodes[ii]->getNodeType() == Universe::NODE_TERRAIN)
        {
            Ptr<Universe::NodeTerrain> pTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(allNodes[ii]);
            pTerrain->spawnMeshes(*pWorld);
        }
    }

    pWorld->update(0.0);
}
//-----------------------------------------------------------------------------
void getStaticMeshes(const Ptr<Universe::World> & pWorld, Core::List<StaticMesh> & meshes)
{
    Ptr<Universe::RessourcePool> pResPool = pWorld->getRessourcesPool();
    const Core::List<Ptr<Universe::Node> > & allNodes = pWorld->getAllNodes();

    for(int32 ii=0; ii < allNodes.size(); ii++)
    {
        StaticMesh mesh;

        if(allNodes[ii]->getNodeType() == Universe::NODE_MESH)
        {
            Ptr<Universe::NodeMesh> pMesh = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(allNodes[ii]);
            if(pMesh->isStatic() && pMesh->isVisible() && pMesh->getMeshInstance()->getMaterial().diffuse.a > 0.0f)
            {
                // Copie les vertices + le material

                Ptr<Assets::VerticeSet> pVSet(pMesh->getMeshInstance()->getMesh()->getVerticeSet());
                Ptr<Assets::ModelMesh> pModelMesh(pMesh->getModelMesh());

                mesh.pNodeMesh = pMesh;
                mesh.pModelMesh = Ptr<Assets::ModelMesh>(new Assets::ModelMesh());
                mesh.pModelMesh->getLODByID(0) = *pVSet;
                mesh.pModelMesh->getMaterial() = pModelMesh->getMaterial();

                meshes.push_back(mesh);
            }
        }
        //else if(allNodes[ii]->getNodeType() == Universe::NODE_TERRAIN)
        //{
        //    Ptr<Universe::NodeTerrain> pTerrain = LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(allNodes[ii]);
        //    if(pTerrain->isVisible())
        //        terrains.push_back(pTerrain);
        //}
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
struct AtlasInput
{
    Ptr<Core::Bitmap>           pBitmap;
    StaticMesh *                pMesh;
    Core::List<StaticMesh *>    meshes;
};
//-----------------------------------------------------------------------------
void getLightMapsForAtlas(const Ptr<Universe::RessourcePool> & pResPool, Core::List<StaticMesh> & meshes, Core::List<AtlasInput> & atlas)
{
    for(int32 iMesh=0; iMesh < meshes.size(); iMesh++)
    {
        StaticMesh & mesh = meshes[iMesh];
        String lightmapFile;

        mesh.pNodeMesh->getLightTexture(lightmapFile);

        if(lightmapFile != String::EMPTY)
        {
            Ptr<Assets::Texture> pTexture = pResPool->getTextureData(lightmapFile);
            const Assets::TextureImage & img = pTexture->getImage(0)[0];

            if(pTexture->getFormat() != Assets::TEX_FORMAT_ARGB8)
                throw Core::Exception(L"Trying to pack non ARGB8 light texture " + lightmapFile);

            AtlasInput ai;
            ai.pMesh = &mesh;
            ai.pBitmap = Ptr<Core::Bitmap>(new Core::Bitmap((byte*)img.getData(), img.getWidth(), img.getHeight(), Core::ARGB_U8, 0, false));

            atlas.push_back(ai);
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<Core::Bitmap> getUncompressedTexture(const Ptr<Universe::RessourcePool> & pResPool, const String & file)
{
    Ptr<Core::VirtualFileSystem> pVFS(pResPool->getVFS());
    Ptr<Assets::Texture> pTexture = pResPool->getTextureData(file);

    if(pTexture->getFormat() != Assets::TEX_FORMAT_ARGB8)
    {
        String metaFile(file + L".xml");

        if(!pVFS->fileExists(metaFile))
            throw Core::Exception(L"Trying to pack non ARGB8 diffuse texture with no metafile " + file);

        Ptr<Core::XMLDocument> pDoc = pResPool->getDocument(metaFile);
        
        String sourceFile;
        pDoc->getValue(L"asset.image-name", sourceFile);

        Ptr<Images::Image> pImage(new Images::Image());
        pImage->loadFile(sourceFile);

        int32 byteCount = pImage->getWidth() * pImage->getHeight() * 4;
        byte * pData = new byte[byteCount];
        memcpy(pData, pImage->getData(), byteCount);

        return Ptr<Core::Bitmap>(new Core::Bitmap(pData, pImage->getWidth(), pImage->getHeight(), Core::ARGB_U8, 0, true));
    }
    else
    {
        const Assets::TextureImage & img = pTexture->getImage(0)[0];
        return Ptr<Core::Bitmap>(new Core::Bitmap((byte*)img.getData(), img.getWidth(), img.getHeight(), Core::ARGB_U8, 0, false));
    }
}
//-----------------------------------------------------------------------------
void getDiffuseMapsForAtlas(const Ptr<Universe::RessourcePool> & pResPool, Core::List<StaticMesh> & meshes, Core::List<AtlasInput> & atlas)
{
    Core::Map<String, int32> diffuseMapToIndex;

    for(int32 iMesh=0; iMesh < meshes.size(); iMesh++)
    {
        StaticMesh & mesh = meshes[iMesh];
        String diffusemapFile;

        const Assets::Material & mat = mesh.pModelMesh->getMaterial();
        mesh.pNodeMesh->getDiffuseTexture(diffusemapFile);

        static const float THRESHOLD = 1.5f;
        bool specularOn = (std::max(mat.specular.r, std::max(mat.specular.g, mat.specular.g)) > THRESHOLD);

        if(diffusemapFile != String::EMPTY && !specularOn)
        {
            Core::Map<String, int32>::const_iterator iMap = diffuseMapToIndex.find(diffusemapFile);

            if(iMap != diffuseMapToIndex.end())
            {
                AtlasInput & ai = atlas[iMap->second];
                ai.meshes.push_back(&mesh);
            }
            else
            {
                AtlasInput ai;
                ai.pBitmap = getUncompressedTexture(pResPool, diffusemapFile);
                ai.meshes.push_back(&mesh);

                diffuseMapToIndex[diffusemapFile] = atlas.size();
                atlas.push_back(ai);
            }
        }
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
struct AtlasOutput
{
    struct AtlasEntry
    {
        AtlasInput * pInput;

        float   localDU;
        float   localDV;
        float   localMU;
        float   localMV;

        float   du;
        float   dv;

        float   mu;
        float   mv;
    };

    String                  fileName;
    Core::List<AtlasEntry>  entries;
};
//-----------------------------------------------------------------------------
void generateTextureAtlas(Core::List<AtlasInput> & input, 
                          Core::List<AtlasOutput> & output, 
                          const Ptr<Core::VirtualFileSystem> & pVFS,
                          const String & prefix, 
                          int32 textureAtlasSize,
                          int32 border,
                          bool diffuseBorder,
                          const Ptr<Core::IProgressObs> & pObs)
{
    Core::List<Workflow::Zone> zones;

    for(int32 ii=0; ii < input.size(); ii++)
    {
        const AtlasInput & ai = input[ii];
        
        Workflow::Zone zone;
        zone.width = ai.pBitmap->getWidth();
        zone.height = ai.pBitmap->getHeight();

        Ptr<Images::Image> pStretchedImage(Images::toImage(ai.pBitmap));
        pStretchedImage->resize(zone.width - 2 * border, zone.height - 2 * border);

        Ptr<Core::Bitmap> pStretchedBitmap(pStretchedImage->toBitmap());
        Core::paste(ai.pBitmap, pStretchedBitmap, border, border);

        {
            Ptr<Core::Bitmap> pCropUp(Core::crop(ai.pBitmap, 0, border, zone.width, 1));
            for(int32 y=0; y < border; y++)
                Core::paste(ai.pBitmap, pCropUp, 0, y);
        }
        {
            Ptr<Core::Bitmap> pCropDown(Core::crop(ai.pBitmap, 0, zone.height - (border + 1), zone.width, 1));
            for(int32 y=0; y < border; y++)
                Core::paste(ai.pBitmap, pCropDown, 0, zone.height - y - 1);
        }
        {
            Ptr<Core::Bitmap> pCropLeft(Core::crop(ai.pBitmap, border, 0, 1, zone.height));
            for(int32 x=0; x < border; x++)
                Core::paste(ai.pBitmap, pCropLeft, x, 0);
        }
        {
            Ptr<Core::Bitmap> pCropRight(Core::crop(ai.pBitmap, zone.width - (border + 1), 0, 1, zone.height));
            for(int32 x=0; x < border; x++)
                Core::paste(ai.pBitmap, pCropRight, zone.width - x - 1, 0);
        }

        zones.push_back(zone);
    }

    Core::List<Workflow::ZonePack> packedZones;
    Workflow::packZones(packedZones, zones, textureAtlasSize, textureAtlasSize);

    for(int32 iPack=0; iPack < packedZones.size(); iPack++)
    {
        float p1 = float(iPack + 0) / float(packedZones.size());
        float p2 = float(iPack + 1) / float(packedZones.size());
        Ptr<Core::IProgressObs> pSubObs(new Core::SubProgressObs(pObs, p1, p2));

        AtlasOutput ao;
        const Workflow::ZonePack & pack = packedZones[iPack];
        Ptr<Core::Bitmap> pAtlasBitmap(new Core::Bitmap(pack.width, pack.height, Core::ARGB_U8));

        ao.fileName = prefix + Core::toString(iPack, 4) + L".tex";

        for(int32 iZone=0; iZone < pack.zones.size(); iZone++)
        {
            const Workflow::ZonePack::PackedZone & zone = pack.zones[iZone];

            AtlasOutput::AtlasEntry entry;
            entry.pInput = &input[zone.id];

            int32 localW = zone.x2 - zone.x1 + 1;
            int32 localH = zone.y2 - zone.y1 + 1;

            entry.localDU = (float(border) + 0.5f) / float(localW);
            entry.localDV = (float(border) + 0.5f) / float(localH);

            entry.localMU = (float(localW - 2 * border) - 1.0f) / float(localW);
            entry.localMV = (float(localH - 2 * border) - 1.0f) / float(localH);
 
            entry.du = float(zone.x1) / float(pack.width);
            entry.dv = float(zone.y1) / float(pack.height);

            entry.mu = float(localW) / float(pack.width);
            entry.mv = float(localH) / float(pack.height);

            Core::paste(pAtlasBitmap, entry.pInput->pBitmap, zone.x1, zone.y1);

            ao.entries.push_back(entry);
        }

        if(diffuseBorder)
        {
            for(int32 ii=0; ii < 40; ii++)
                pAtlasBitmap = borderDiffuse(pAtlasBitmap);
        }

        Workflow::TextureOptions texOptions;
        texOptions.format = Assets::TEX_FORMAT_DXTC1;
        //texOptions.format = Assets::TEX_FORMAT_ARGB8;
        texOptions.texType = Workflow::TEXTURE_COLORMAP;
        texOptions.mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
        texOptions.pProgressObs = pSubObs.get();
        Ptr<Assets::Texture> pTexture(generateTexture(texOptions, Images::toImage(pAtlasBitmap)));
        pVFS->write(ao.fileName, *pTexture);

        static int32 sIndex = 0;
        Core::saveTGA(pAtlasBitmap, L"Local/" + Core::toString(sIndex++, 4) + L".tga");

        output.push_back(ao);
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
void applyLightMapAtlas(Core::List<AtlasOutput> & atlas)
{
    for(int32 iAtlas=0; iAtlas < atlas.size(); iAtlas++)
    {
        for(int32 iEntry=0; iEntry < atlas[iAtlas].entries.size(); iEntry++)
        {
            AtlasOutput::AtlasEntry & entry = atlas[iAtlas].entries[iEntry];
            StaticMesh & mesh = *entry.pInput->pMesh;

            Assets::VerticeSet & vset = mesh.pModelMesh->getLODByID(0);
            Core::List<Assets::Vertex> & vtx = vset.getVertices();

            for(int32 iVtx=0; iVtx < vtx.size(); iVtx++)
            {
                Assets::Vertex & v = vtx[iVtx];
                v.dummy = 1.0f;

                v.boneWeight[0] = v.texCoords.x * entry.localMU + entry.localDU;
                v.boneWeight[1] = v.texCoords.y * entry.localMV + entry.localDV;

                v.boneWeight[0] = v.boneWeight[0] * entry.mu + entry.du;
                v.boneWeight[1] = v.boneWeight[1] * entry.mv + entry.dv;
            }

            mesh.pNodeMesh->setLightTexture(atlas[iAtlas].fileName);
        }
    }
}
//-----------------------------------------------------------------------------
void applyDiffuseMapAtlas(Core::List<AtlasOutput> & atlas)
{
    for(int32 iAtlas=0; iAtlas < atlas.size(); iAtlas++)
    {
        for(int32 iEntry=0; iEntry < atlas[iAtlas].entries.size(); iEntry++)
        {
            AtlasOutput::AtlasEntry & entry = atlas[iAtlas].entries[iEntry];
            Core::List<StaticMesh *> meshes = entry.pInput->meshes;

            for(int32 iMesh=0; iMesh < meshes.size(); iMesh++)
            {
                StaticMesh & mesh = *meshes[iMesh];

                Assets::VerticeSet & vset = mesh.pModelMesh->getLODByID(0);
                Core::List<Assets::Vertex> & vtx = vset.getVertices();

                for(int32 iVtx=0; iVtx < vtx.size(); iVtx++)
                {
                    Assets::Vertex & v = vtx[iVtx];

                    v.texCoords.x = v.texCoords.x * entry.localMU + entry.localDU;
                    v.texCoords.y = v.texCoords.y * entry.localMV + entry.localDV;

                    v.texCoords.x = v.texCoords.x * entry.mu + entry.du;
                    v.texCoords.y = v.texCoords.y * entry.mv + entry.dv;
                }

                mesh.pModelMesh->getMaterial().diffuseTexName = atlas[iAtlas].fileName;
                mesh.pModelMesh->getMaterial().normalTexName = L"";
            }
        }
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
String matToString(const StaticMesh & mesh)
{
    const Assets::Material & m = mesh.pModelMesh->getMaterial();

    String result;
    mesh.pNodeMesh->getLightTexture(result);

    result << m.diffuseTexName 
           << Core::toXMLString(m.diffuse)
           << Core::toXMLString(m.specular)
           << Core::toString(m.shininess)
           << Core::toXMLString(m.emissive)
           << Core::toXMLString(m.glowColor)
           << Core::toXMLString(m.reflexions);

    return result;
}
//-----------------------------------------------------------------------------
struct UberMesh
{
    Ptr<Assets::ModelMesh>      pUberMesh;
    String                      lightmap;
    Core::List<StaticMesh>      smallMeshes;

    bool empty() const
    {
        return pUberMesh == null;
    }

    int32 getCount() const
    {
        if(pUberMesh == null)
            return 0;
        else
            return pUberMesh->getLODByID(0).getVertices().size();
    }

    void clear()
    {
        pUberMesh = null;
        smallMeshes.clear();
    }

    void addMesh(StaticMesh * pSmallMesh)
    {
        if(pUberMesh == null)
        {
            pUberMesh = Ptr<Assets::ModelMesh>(new Assets::ModelMesh());
            pUberMesh->getMaterial() = pSmallMesh->pModelMesh->getMaterial();
            pSmallMesh->pNodeMesh->getLightTexture(lightmap);
        }

        const StaticMesh & smallMesh = *pSmallMesh;
        Assets::ModelMesh & bigMesh = *pUberMesh;

        Core::List<Assets::Vertex> & dstVert = bigMesh.getLODByID(0).getVertices();
        Core::List<Assets::Vertex> & srcVert = smallMesh.pModelMesh->getLODByID(0).getVertices();

        Core::List<uint16> & dstInd = bigMesh.getLODByID(0).getIndices();
        Core::List<uint16> & srcInd = smallMesh.pModelMesh->getLODByID(0).getIndices();

        Core::Matrix4f world(smallMesh.pNodeMesh->getLocalToWorldMatrix());
        int32 startIndex = dstVert.size();

        for(int32 ii=0; ii < srcVert.size(); ii++)
        {
            Assets::Vertex v(srcVert[ii]);
            v.position = world.apply(v.position);
            v.normale = world.apply3x3(v.normale);
            v.tangent = world.apply3x3(v.tangent);
            v.bitangent = world.apply3x3(v.bitangent);

            dstVert.push_back(v);
        }

        if(world.determinant() < 0.0f)
        {
            // culling invers�

            for(int32 ii=0; ii < srcInd.size(); ii+=3)
            {
                dstInd.push_back(srcInd[ii + 0] + startIndex);
                dstInd.push_back(srcInd[ii + 2] + startIndex);
                dstInd.push_back(srcInd[ii + 1] + startIndex);
            }
        }
        else
        {
            for(int32 ii=0; ii < srcInd.size(); ii++)
                dstInd.push_back(srcInd[ii] + startIndex);
        }

        smallMeshes.push_back(*pSmallMesh);
    }
};

void mergeMeshes(Core::List<StaticMesh> & meshes, Core::List<UberMesh> & uberMeshes, int32 batchSize)
{
    Core::Map<String, Core::List<StaticMesh *> > matToMesh;

    for(int32 ii=0; ii < meshes.size(); ii++)
    {
        String mat(matToString(meshes[ii]));
        matToMesh[mat].push_back(&meshes[ii]);
    }

    for(Core::Map<String, Core::List<StaticMesh *> >::iterator iMat=matToMesh.begin(); iMat != matToMesh.end(); ++iMat)
    {
        UberMesh uberMesh;

        Core::List<StaticMesh *> & meshes = iMat->second;
        Core::Vector3f position;

        while(!meshes.empty())
        {
            if(uberMesh.empty())
            {
                position = meshes[0]->pNodeMesh->getWorldPosition();
                uberMesh.addMesh(meshes[0]);
                meshes.qerase(0);
            }

            int32 iNearest = -1;
            float minDist = FLT_MAX;

            for(int32 ii=0; ii < meshes.size(); ii++)
            {
                float dist = position.getDistance(meshes[ii]->pNodeMesh->getWorldPosition());
                if(dist < minDist)
                {
                    iNearest = ii;
                    minDist = dist;
                }
            }

            if(iNearest == -1)
            {
                LM_ASSERT(uberMesh.pUberMesh != null);
                uberMeshes.push_back(uberMesh);
                uberMesh.clear();
            }
            else
            {
                uberMesh.addMesh(meshes[iNearest]);
                meshes.qerase(iNearest);
            }

            if(uberMesh.getCount() >= batchSize)
            {
                uberMeshes.push_back(uberMesh);
                uberMesh.clear();
            }
        }

        if(!uberMesh.empty())
        {
            LM_ASSERT(uberMesh.pUberMesh != null);
            uberMeshes.push_back(uberMesh);
            uberMesh.clear();
        }
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
void packWorld(const WorldPackingOptions & options)
{
    static const float PROGRESS1 = 0.40f;
    static const float PROGRESS2 = 0.95f;

    Ptr<Core::VirtualFileSystem> pVFS(options.pWorld->getRessourcesPool()->getVFS());
    String path;
    Core::trimRight(options.path, path, L"/");

    Core::List<StaticMesh> meshes;
    spawnTerrains(options.pWorld);

    getStaticMeshes(options.pWorld, meshes);

    {
        Ptr<Core::IProgressObs> pObs(new Core::SubProgressObs(options.pObserver, 0.0f, PROGRESS1));

        Core::List<AtlasInput> lightMaps;
        getLightMapsForAtlas(options.pWorld->getRessourcesPool(), meshes, lightMaps);

        Core::List<AtlasOutput> lightAtlas;
        generateTextureAtlas(lightMaps, lightAtlas, pVFS, path + L"/light-", options.textureAtlasSize, options.border, true, pObs);

        applyLightMapAtlas(lightAtlas);
    }

    {
        Ptr<Core::IProgressObs> pObs(new Core::SubProgressObs(options.pObserver, PROGRESS1, PROGRESS2));

        Core::List<AtlasInput> diffuseMaps;
        getDiffuseMapsForAtlas(options.pWorld->getRessourcesPool(), meshes, diffuseMaps);

        Core::List<AtlasOutput> diffuseAtlas;
        generateTextureAtlas(diffuseMaps, diffuseAtlas, pVFS, path + L"/diffuse-", options.textureAtlasSize, options.border, false, pObs);

        applyDiffuseMapAtlas(diffuseAtlas);
    }

    {
        Ptr<Core::IProgressObs> pObs(new Core::SubProgressObs(options.pObserver, PROGRESS2, 1.0f));

        Core::List<UberMesh> uberMeshes;
        mergeMeshes(meshes, uberMeshes, options.batchSize);

        for(int32 ii=0; ii < uberMeshes.size(); ii++)
        {
            pObs->progress(float(ii) / float(uberMeshes.size()));

            Ptr<Assets::Model> pModel(new Assets::Model());
            pModel->addMesh(L"mesh", *uberMeshes[ii].pUberMesh);

            String meshName(path + L"/model-" + Core::toString(ii, 4) + L".mdl");
            pVFS->write(meshName, *pModel);

            for(int32 iMesh=0; iMesh < uberMeshes[ii].smallMeshes.size(); iMesh++)
                uberMeshes[ii].smallMeshes[iMesh].pNodeMesh->kill();

            Ptr<Universe::NodeMesh> pUberNodeMesh = options.pWorld->createMesh(meshName, L"mesh");
            pUberNodeMesh->setLightTexture(uberMeshes[ii].lightmap);
        }
    }

    // Sky box
    if(options.pWorld->isSkyProcedural())
    {
        Renderer::SkyboxInfos skybox;
        skybox.resolution = 512;
        skybox.pScene = options.pWorld->getScene();
        options.pWorld->getRenderer()->generateSkyBox(options.pWorld->getScene()->getSkysphereSettings(), skybox);

        Core::List<Assets::Texture> faces(Assets::ECubeFace_COUNT);

        for(int32 iFace=0; iFace < faces.size(); iFace++)
        {
            Core::List<Ptr<Core::Bitmap> > & bitmaps = skybox.bitmaps[iFace];
            Core::List<Assets::TextureImage> mipmaps(bitmaps.size());

            for(int32 iMip=0; iMip < mipmaps.size(); iMip++)
            {
                const Core::Bitmap & bmp = *bitmaps[iMip];
                int32 dataSize = bmp.getWidth() * bmp.getHeight() * 4;
                mipmaps[iMip] = Assets::TextureImage(bmp.getWidth(), bmp.getHeight(), Assets::TEX_FORMAT_ARGB8, bmp.getData(), dataSize);
            }

            faces[iFace] = Assets::Texture(mipmaps);
        }

        String cubeTexName(path + L"/skybox.ctex");

        Assets::CubeTexture cubeTex(faces);
        pVFS->write(cubeTexName, cubeTex);

        options.pWorld->setSkyTexture(cubeTexName);
    }
}
//-----------------------------------------------------------------------------
}
