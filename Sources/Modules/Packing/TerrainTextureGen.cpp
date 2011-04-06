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
#include <Core/Logger.h>
#include <Packing/TerrainTextureGen.h>
#include <Workflow/TexCompression.h>

namespace Packing
{
//-----------------------------------------------------------------------------
void convert(const Core::List<Universe::NodeTerrain::Layer> & layers1,
             Core::List<Renderer::LayerMapInfos::LayerInfo> & layers2,
             const Ptr<Universe::RessourcePool> & pPool,
             bool isNormal)
{
    layers2.clear();

    for(int32 iLayer=0; iLayer < layers1.size(); iLayer++)
    {
        const Universe::NodeTerrain::Layer & layer = layers1[iLayer];
        const String & texName = (isNormal ? layer.normalTexName : layer.diffuseTexName);

        if(texName != String::EMPTY)
        {
            Renderer::LayerMapInfos::LayerInfo newLayer;

            Core::Vector4f u(1.0f / layer.width, 0.0f, 0.0f,  0.0f);
            Core::Vector4f v(0.0f, 0.0f, -1.0f / layer.height, 0.0f);

            if(layer.angle != 0.0)
            {
                Core::Matrix4f rotZ(Core::ROTATION, -layer.angle, Core::ROT_Y_YAW);
                newLayer.generateU = rotZ.apply(u);
                newLayer.generateV = rotZ.apply(v);
            }
            else
            {
                newLayer.generateU = u;
                newLayer.generateV = v;
            }
            
            newLayer.angle              = layer.angle;
            newLayer.pSourceTexture     = pPool->getTexture(texName);
            newLayer.pMaskTexture       = (layer.maskTexName != String::EMPTY ? pPool->getTexture(layer.maskTexName) : null);
            newLayer.color              = layer.color;
            newLayer.maskContrast       = layer.contrast;
            newLayer.normalStrength     = layer.normalStrength;
            if (isNormal == true)   newLayer.useDisplacementMap = layer.useDisplacementMap;
            else                    newLayer.useDisplacementMap = false;
            layers2.push_back(newLayer);
        }
    }
}
//-----------------------------------------------------------------------------
void generateTerrainTexture(const TerrainTextureGenOptions & options)
{
    if(options.resolution.empty())
        throw Core::Exception(L"generateTerrainTexture error : no resolution specified");

    // Noms textures

    String prefix = options.path + L"/" + options.prefix;

    const Universe::NodeTerrain & terrain = *options.pTerrain;

    Core::List<Core::List<String> >  diffuseTextures;
    Core::List<Core::List<String> >  normalTextures;

    diffuseTextures.resize(terrain.getLODCount());
    normalTextures.resize(terrain.getLODCount());

    // Estimate time/size

    int64 totalSize = 0;
    int64 generatedSize = 0;

    for(int32 iLOD=0; iLOD < terrain.getLODCount(); iLOD++)
    {
        int32 blockCount = terrain.getBlockCountForLOD(iLOD);
        int32 resolution = options.resolution[std::min(options.resolution.size() - 1, iLOD)];
        totalSize += (blockCount * resolution * resolution);
    }

    // Box

    Core::AABoxf box;
    Ptr<Assets::TerrainModel> pTerrainModel(options.pPool->getTerrainModel(terrain.getTerrain()));

    for(int32 ii=0; ii < pTerrainModel->getMeshCount(); ii++)
        box.setUnion(pTerrainModel->getMesh(ii).getBoundingBox());

    // Layers

    Renderer::LayerMapInfos infosDiffuse, infosNormal;
    
    infosDiffuse.box = box;
    infosNormal.box  = box;

    convert(terrain.getLayers(), infosDiffuse.layers, options.pPool, false);
    convert(terrain.getLayers(), infosNormal.layers,  options.pPool, true);

    infosDiffuse.isNormalMap = false;
    infosNormal.isNormalMap  = true;

    if(terrain.getGlobalNormalTexture() != String::EMPTY)
    {
        float sx = infosNormal.box.getSize().x;
        float sz = infosNormal.box.getSize().z;

        infosNormal.useDetailLayer = true;
        infosNormal.detailLayer.pSourceTexture = options.pPool->getTexture(terrain.getGlobalNormalTexture());

        float dx = 2.0f / float(infosNormal.detailLayer.pSourceTexture->getSourceTexture()->getWidth());
        float dy = 2.0f / float(infosNormal.detailLayer.pSourceTexture->getSourceTexture()->getHeight());

        infosNormal.detailLayer.generateU          = Core::Vector4f(1.0f / sx, 0.000f, 0.000f,  0.5f + dx);
        infosNormal.detailLayer.generateV          = Core::Vector4f(0.000f, 0.000f, -1.0f / sz,  0.5f + dy);
        infosNormal.detailLayer.color              = Core::Vector4f(1.0f, 1.0f, 1.0f, terrain.getGlobalNormalWeight());
        infosNormal.detailLayer.maskContrast       = 0.f;
        infosNormal.detailLayer.useDisplacementMap = false;
        infosNormal.detailLayer.normalStrength     = 1.f;
    }
    else
    {
        infosNormal.useDetailLayer = false;
    }


    // Compression textures

    Workflow::TextureOptions optionsDiffuse, optionsNormal;
    optionsDiffuse.texType = Workflow::TEXTURE_COLORMAP;
    optionsNormal.texType  = Workflow::TEXTURE_NORMALMAP;

    optionsDiffuse.format  = (options.compressTextures ? Assets::TEX_FORMAT_DXTC1 : Assets::TEX_FORMAT_ARGB8);
    optionsNormal.format   = (options.compressTextures ? Assets::TEX_FORMAT_DXTC5 : Assets::TEX_FORMAT_ARGB8);

    optionsDiffuse.quality = Workflow::QUALITY_HIGH;
    optionsNormal.quality  = Workflow::QUALITY_HIGH;

    for(int32 iLOD=0; iLOD < terrain.getLODCount(); iLOD++)
    {
        int32 blockCount = terrain.getBlockCountForLOD(iLOD);

        diffuseTextures[iLOD].resize(blockCount);
        normalTextures[iLOD].resize(blockCount);

        int32 resolution = options.resolution[std::min(options.resolution.size() - 1, iLOD)];
        infosDiffuse.width = infosDiffuse.height = resolution;
        infosNormal.width = infosNormal.height = resolution;

        for(int32 iBlock=0; iBlock < blockCount; iBlock++)
        {
            int32 blockSize = resolution * resolution;
            float progressStart  = float(generatedSize) / float(totalSize);
            float progressEnd    = float(generatedSize + blockSize) / float(totalSize);
            float progressMiddle = 0.5f * (progressStart + progressEnd);

            String base = prefix + L"lod-" + Core::toString(iLOD) + L"-block-" + Core::toString(iBlock, 2);
            diffuseTextures[iLOD][iBlock] = base + L"-diffuse.tex";
            normalTextures[iLOD][iBlock]  = base + L"-normal.tex";

            Ptr<Renderer::IMeshInstance> pMesh = terrain.getBlockMesh(iLOD, iBlock);

            {
                Ptr<Core::IProgressObs> pProg(new Core::SubProgressObs(options.pObserver, progressStart, progressMiddle));
                optionsDiffuse.pProgressObs = pProg.get();

                Ptr<Core::Bitmap> pBitmap = options.pRenderer->generateLayerMap(infosDiffuse, pMesh);

                Ptr<Assets::Texture> pTexDiffuse(generateTexture(optionsDiffuse, Images::toImage(pBitmap)));
                options.pVFS->write(diffuseTextures[iLOD][iBlock], *pTexDiffuse);
            }

            {
                Ptr<Core::IProgressObs> pProg(new Core::SubProgressObs(options.pObserver, progressMiddle, progressEnd));
                optionsNormal.pProgressObs = pProg.get();

                Ptr<Core::Bitmap> pBitmap = options.pRenderer->generateLayerMap(infosNormal, pMesh);

                Ptr<Assets::Texture> pTexNormal(generateTexture(optionsNormal, Images::toImage(pBitmap)));
                options.pVFS->write(normalTextures[iLOD][iBlock], *pTexNormal);
            }

            generatedSize += blockSize;
        }
    }

    for(int32 iLOD=0; iLOD < terrain.getLODCount(); iLOD++)
    {
        int32 blockCount = terrain.getBlockCountForLOD(iLOD);
        for(int32 iBlock=0; iBlock < blockCount; iBlock++)
        {
            options.pPool->reloadTexture(diffuseTextures[iLOD][iBlock]);
            options.pPool->reloadTexture(normalTextures[iLOD][iBlock]);
        }
    }

    options.pTerrain->setBlockTextures(diffuseTextures, normalTextures);
}
//-----------------------------------------------------------------------------
}
