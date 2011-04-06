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
#include <Workflow/TerrainModelGen.h>
#include <Workflow/ModelOptimize.h>
#include <Core/Logger.h>
#include <Images/Operator.h>

namespace Workflow
{
//l'ensemble du terrain est compris sur [0, 1]
//une forte valeur de blend donne la priorite a valueScale
//une faible valeur de blend donne la priorite a valueScaleDown
float computeBlendfactor(int32 iLevel, int32 levelCount, float blendFactor, float posX, float posZ)
{
    //exemple :
    //si nbLvel == 3 pour
    //iLevel = 0, levelWidth = 25% (0.25) de la taille du terrain
    //iLevel = 1, levelWidth = 50% (0.5) de la taille du terrain
    //iLevel = 2, levelWidth = 100%(1) de la taille du terrain
    float levelWidth = Core::L_pow(2.f, float(iLevel+1)) / Core::L_pow(2.f, float(levelCount));

    //les position minimum et maximum du level sur le terrain
    float minLevelPos = -levelWidth/2.f;
    float maxLevelPos = levelWidth/2.f;

    float centerX = posX - 0.5f;

    float centerZ = posZ - 0.5f;


    if (centerX<minLevelPos || centerX>maxLevelPos ||
        centerZ<minLevelPos || centerZ>maxLevelPos)
    {
        ERR << L"error computeBlendfactor blendind";
    }
    //on calcul le blend en colonne et en ligne
        //on normalise sur la zone du level
    float blendX = (centerX - minLevelPos) / (maxLevelPos-minLevelPos);
    float blendZ = (centerZ - minLevelPos) / (maxLevelPos-minLevelPos);
        //on passe sur [-0.5, 0.5]


    blendX = blendX - 0.5f;
    blendZ = blendZ - 0.5f;

    blendX = Core::L_abs(blendX) * 2;
    blendZ = Core::L_abs(blendZ) * 2;


    float seuil = blendFactor;
    if (blendX>=seuil)
    {
        blendX = (blendX-seuil) / (1-seuil);
    }
    else
    {
        blendX = 0;
    }
    if (blendZ>=seuil)
    {
        blendZ = (blendZ-seuil) / (1-seuil);
    }
    else
    {
        blendZ = 0;
    }

    float blend = 0.f;

    //carre
    //blend = (blendX>blendZ) ? blendX : blendZ;
    //circulaire
    blend = blendX*blendX + blendZ*blendZ;
    if (blend>0)
        blend = Core::L_sqrt(blend);

    blend = Core::clamp(0.f, 1.f, blend);

    blend = 1-blend;

    return blend;
}


/**
 * Calcule les positions des vertex en x et en z de la grille.
 * les sommets de la grille sont compris dans [-0.5, 0.5]
 */
void makeBlock(Assets::VerticeSet & grid, int32 gridWidth, const Core::Vector3f & blockCenter, float scale)
{
    Core::List<Assets::Vertex> & gridVtx  = grid.getVertices();
    Core::List<uint16> & gridInd = grid.getIndices();

    gridVtx.resize(0);
    gridInd.resize(0);

    int32 midle = gridWidth / 2;
    float* tabPos = new float [gridWidth];

    for (int32 ii=0; ii < gridWidth; ii++)
       tabPos[ii] = (ii - midle) / float(gridWidth - 1);

    int32 precision = 1;

    // Positions
    for (int32 iZ=0; iZ < gridWidth; iZ++)
    {
        for (int32 iX=0; iX < gridWidth; iX++)
        {
            Assets::Vertex v;
            v.position.x       = tabPos[iX] * scale + blockCenter.x;
            v.position.z       = tabPos[iZ] * scale + blockCenter.z;

            float s = float(iX) / float(gridWidth - 1);
            float t = float(iZ) / float(gridWidth - 1);

            v.texCoords.x      = s;
            v.texCoords.y      = t;

            gridVtx.push_back(v);
        }
    }

    // Indices
    for (int32 iZ=1; iZ < gridWidth; iZ++)
    {
        for (int32 iX=1; iX < gridWidth; iX++)
        {
            int32 iVertex = iX + iZ * gridWidth;

            gridInd.push_back(iVertex - gridWidth - 1);
            gridInd.push_back(iVertex - 1);
            gridInd.push_back(iVertex - gridWidth);

            gridInd.push_back(iVertex - 1);
            gridInd.push_back(iVertex);
            gridInd.push_back(iVertex - gridWidth);
        }
    }

    delete [] tabPos;
}
//-----------------------------------------------------------------------------
void makeLevelBlocks(int32 iLevel, Core::List<Assets::VerticeSet> & blocks, int32 blockWidth)
{
    Core::Vector3f levelCenter(0, 0, 0);

    float bScale = 100.0f * Core::L_pow(2, std::max(1, iLevel));
    float bt = 0.5f * bScale;

    int32 incBlock = 0;
    int32 middle = 2;

    if(iLevel == 0)
    {
        blocks.resize(4);

        makeBlock(blocks[0], blockWidth, Core::Vector3f( -bt, 0.0f,  -bt), bScale);
        makeBlock(blocks[1], blockWidth, Core::Vector3f(  bt, 0.0f,  -bt), bScale);
        makeBlock(blocks[2], blockWidth, Core::Vector3f(  bt, 0.0f,   bt), bScale);
        makeBlock(blocks[3], blockWidth, Core::Vector3f( -bt, 0.0f,   bt), bScale);
    }
    else
    {
        blocks.resize(12);

        Core::Vector3f blockCenter(0.0f, 0.0f, 0.0f);

        //ligne nord (block 1,2,3,4 du commentaire)
        blockCenter.z = -1.5f * bScale;
        for (int32 iBlock=0; iBlock < 4; iBlock++)
        {
            blockCenter.x = bScale * (iBlock + 0.5f - middle);
            makeBlock(blocks[incBlock++], blockWidth, blockCenter, bScale);
        }

        //ligne est (block 5,6 du commentaire)
        //blockCenter.x = blockCenter.x //on reprend l'abscisse du dernier block
        for (int32 iBlock=0; iBlock < 2; iBlock++)
        {
            blockCenter.z = blockCenter.z + bScale;
            makeBlock(blocks[incBlock++], blockWidth, blockCenter, bScale);
        }

        //ligne sud (block 7, 8, 9, 10 du commentaire)
        blockCenter.z = 1.5f * bScale;
        blockCenter.x = blockCenter.x + bScale;
        for (int32 iBlock=0; iBlock < 4; iBlock++)
        {
            blockCenter.x -= bScale;
            makeBlock(blocks[incBlock++], blockWidth, blockCenter, bScale);
        }

        //ligne ouest (block 11, 12 du commentaire)
        //blockCenter.x = blockCenter.x //on reprend l'abscisse du dernier block
        for (int32 iBlock=0; iBlock < 2; iBlock++)
        {
            blockCenter.z = blockCenter.z - bScale;
            makeBlock(blocks[incBlock++], blockWidth, blockCenter, bScale);
        }
    }
}
//-----------------------------------------------------------------------------
void generateBlockHeight(Assets::VerticeSet & vset, 
                 const Core::AABoxf & bbox,
                 const Core::Vector3f & size,
                 Images::Pixel minY, 
                 Images::Pixel maxY,
                 int32 iLevel, 
                 int32 levelCount,
                 float blendFactor,
                 Ptr<Images::Image> pHeightMap, 
                 Ptr<Images::Image> pHeightMapScaleDown,
                 Ptr<Images::Image> pNormalMap, 
                 Ptr<Images::Image> pNormalMapScaleDown)
{
    Core::List<Assets::Vertex> & vertices = vset.getVertices();
    Core::Vector3f boxSize(bbox.getSize());

    for(Core::List<Assets::Vertex>::iterator iVtx = vertices.begin(); iVtx != vertices.end(); ++iVtx)
    {
        float u = (iVtx->position.x - bbox.min.x) / boxSize.x;
        float v = 1.0f - (iVtx->position.z - bbox.min.z) / boxSize.z;

        float height = pHeightMap->getSubPixel(Images::COORD_NORM, u, v).as_grayF32.i;
        Images::rgbU8 normal = pNormalMap->getSubPixel(Images::COORD_NORM, u, v).as_rgbU8;

        float blend = computeBlendfactor(iLevel, levelCount, blendFactor, u, 1.0f - v);

        if(pHeightMapScaleDown != null)
        {
            float heightDown = pHeightMapScaleDown->getSubPixel(Images::COORD_NORM, u, v).as_grayF32.i;
            height = blend * height + (1.0f - blend) * heightDown;
        }

        if(pHeightMapScaleDown != null)
        {
            Images::rgbU8 normalDown = pNormalMapScaleDown->getSubPixel(Images::COORD_NORM, u, v).as_rgbU8;
            normal = blend * normal + (1.0f - blend) * normalDown;
        }

        iVtx->position.x = size.x * (u - 0.5f);
        iVtx->position.z = size.z * ((1.0f - v) - 0.5f);

        iVtx->position.y = size.y * ((height - minY.as_grayF32.i) / (maxY.as_grayF32.i - minY.as_grayF32.i));

        Core::Vector3f n(Assets::unpackUInt8UN(normal.r), Assets::unpackUInt8UN(normal.g), Assets::unpackUInt8UN(normal.b));
        n -= Core::Vector3f(0.5f, 0.5f, 0.5f);
        n.normalizeSafe();

        iVtx->normale.x = n.x;
        iVtx->normale.y = n.z;
        iVtx->normale.z = n.y;
    }
}
//-----------------------------------------------------------------------------
//Les sommet doivent etre ordonnee comme a la creation sur la grille
void removeTJunctionLevelBlock(Assets::VerticeSet & vset, int32 iLevel, int32 iBlock, int32 blockWidth)
{
    int32 n = blockWidth;
    Core::List<Assets::Vertex> & vert = vset.getVertices();

    //les 12 block peuvent etre ramené à 8 cas
    switch(iBlock)
    {
        case 0:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iZ == 0 && iX%2 != 0)   //ligne Nord
                    {
                        vert[iZ*n+iX] = vert[iZ*n+(iX-1)];
                    }
                    else if (iX == 0 && iZ%2 != 0)  //colonne Est
                    {
                        vert[iZ*n+iX] = vert[(iZ-1)*n+iX];
                    }
                }
        }
        break;

        case 1:
        case 2:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iZ == 0 && iX%2 != 0)   //ligne Nord
                    {
                        vert[iZ*n+iX] = vert[iZ*n+(iX-1)];
                    }
                }
        }
        break;

        case 3:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iZ == 0 && iX%2 != 0)   //ligne Nord
                    {
                        vert[iZ*n+iX] = vert[iZ*n+(iX-1)];
                    }
                    else if (iX == (n-1) && iZ%2 != 0)  //colonne Ouest
                    {
                        vert[iZ*n+iX] = vert[(iZ+1)*n+iX];
                    }
                }
        }
        break;

        case 4:
        case 5:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iX == (n-1) && iZ%2 != 0)  //colonne Ouest
                    {
                        vert[iZ*n+iX] = vert[(iZ+1)*n+iX];
                    }
                }
        }
        break;

        case 6:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iZ == (n-1) && iX%2 != 0)   //ligne Sud
                    {
                        vert[iZ*n+iX] = vert[iZ*n+(iX+1)];
                    }
                    else if (iX == (n-1) && iZ%2 != 0)  //colonne Ouest
                    {
                        vert[iZ*n+iX] = vert[(iZ+1)*n+iX];
                    }
                }
        }
        break;

        case 7:
        case 8:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iZ == (n-1) && iX%2 != 0)   //ligne Sud
                    {
                        vert[iZ*n+iX] = vert[iZ*n+(iX+1)];
                    }
                }
        }
        break;

        case 9:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iZ == (n-1) && iX%2 != 0)   //ligne Sud
                    {
                        vert[iZ*n+iX] = vert[iZ*n+(iX+1)];
                    }
                    else if (iX == 0 && iZ%2 != 0)  //colonne Est
                    {
                        vert[iZ*n+iX] = vert[(iZ-1)*n+iX];
                    }
                }
        }
        break;

        case 10:
        case 11:
        {
            for (int32 iZ=0; iZ<n; iZ++)
                for (int32 iX=0; iX<n; iX++)
                {
                    if (iX == 0 && iZ%2 != 0)  //colonne Est
                    {
                        vert[iZ*n+iX] = vert[(iZ-1)*n+iX];
                    }
                }
        }
        break;
    }
}
//-----------------------------------------------------------------------------
Ptr<Images::Image> generateNormalMap(const Ptr<Images::Image> & pHeightMap, const Core::Vector3f & size)
{
    Images::Pixel minPixelY, maxPixelY;
    Images::minMax(pHeightMap, minPixelY, maxPixelY);

    float minY = minPixelY.as_grayF32.i;
    float maxY = maxPixelY.as_grayF32.i;

    float stepXSize = size.x / float(pHeightMap->getWidth() - 1); 
    float stepYSize = size.z / float(pHeightMap->getHeight() - 1);
    float height = size.y / (maxY - minY);

    LM_ASSERT(pHeightMap->getImgType() == Core::GRAY_F32);
    Ptr<Images::Image> pNormalMap(new Images::Image(Core::RGB_U8, pHeightMap->getWidth(), pHeightMap->getHeight()));

    byte * pNormalData = pNormalMap->getData();
    byte * pHeightData = pHeightMap->getData();
    
    for(int32 y1=0; y1 < pNormalMap->getHeight(); y1++)
    {
        int32 y0 = std::max(0, y1 - 1);
        int32 y2 = std::min(pHeightMap->getHeight() - 1, y1 + 1);
        float yStep = stepYSize * float(y2 - y0);

        byte *  pNormalLine  = pNormalData + y1 * pNormalMap->getRowStep();
        float * pHeightLine0 = reinterpret_cast<float*>(pHeightData + y0 * pHeightMap->getRowStep());
        float * pHeightLine1 = reinterpret_cast<float*>(pHeightData + y1 * pHeightMap->getRowStep());
        float * pHeightLine2 = reinterpret_cast<float*>(pHeightData + y2 * pHeightMap->getRowStep());

        for(int32 x1=0; x1 < pNormalMap->getWidth(); x1++)
        {
            int32 x0 = std::max(0, x1 - 1);
            int32 x2 = std::min(pHeightMap->getWidth() - 1, x1 + 1);
            float xStep = stepXSize * float(x2 - x0);


            Core::Vector3f vl( 0.0f, (pHeightLine1[x0] - minY) * height, 0.0f); // left
            Core::Vector3f vr(xStep, (pHeightLine1[x2] - minY) * height, 0.0f); // right

            Core::Vector3f vu( 0.0f, (pHeightLine0[x1] - minY) * height,   0.0f); // up
            Core::Vector3f vd( 0.0f, (pHeightLine2[x1] - minY) * height, -yStep); // down

            Core::Vector3f v1(vr - vl);
            Core::Vector3f v2(vd - vu);

            Core::Vector3f n(Core::cross(v1, v2).getNormalizedSafe());

            byte * pNormalPixel = &pNormalLine[3 * x1];
            Assets::packUInt8Vec3(n, pNormalPixel);

            pNormalPixel[0] = Assets::packUInt8N(n.y); // B
            pNormalPixel[1] = Assets::packUInt8N(n.z); // G
            pNormalPixel[2] = Assets::packUInt8N(n.x); // R
        }
    }

    return pNormalMap;
}
//-----------------------------------------------------------------------------
Ptr<Assets::TerrainModel> generateTerrainModel(TMGenInput & gen)
{
    Ptr<Assets::TerrainModel> pModel(new Assets::TerrainModel());

    Core::Vector3f size(gen.terrainSize.x, gen.maxHeight, gen.terrainSize.y);
    float blendScaleFactor = Core::clamp(0.0f, 1.0f, gen.blendScaleFactor);
    int32 blockWidth = std::max(2, gen.blockWidth);
    int32 LODCount = gen.LODCount;

    if(blockWidth % 2 == 0)
        blockWidth ++;

    Ptr<Images::Image> pHeightMap;
    Ptr<Images::Image> pNormalMap;

    if(gen.pHeightMap->getImgType() != Core::GRAY_F32)
    {
        throw Core::Exception(L"generateTerrainModel error : pHeightMap not a gray F32");
    }
    else
    {
        pHeightMap = gen.pHeightMap;
    }

    if(gen.pNormalMap == null)
    {
        pNormalMap = generateNormalMap(pHeightMap, size);
    }
    else if(gen.pNormalMap->getImgType() != Core::RGB_U8)
    {
        throw Core::Exception(L"generateTerrainModel error : pNormalMap is not a RGBU8 but :" + gen.pNormalMap->getImgType());
    }
    else
    {
        pNormalMap = gen.pNormalMap;
    }

    Core::List<Core::List<Assets::VerticeSet> > blocks;
    blocks.resize(LODCount);

    for(int32 iLOD=0; iLOD < blocks.size(); iLOD++)
        makeLevelBlocks(iLOD, blocks[iLOD], blockWidth);

    Core::AABoxf bbox;

    for(int32 iLevel=0; iLevel < blocks.size(); iLevel++)
    {
        for(int32 iBlock=0; iBlock < blocks[iLevel].size(); iBlock++)
        {
            const Core::List<Assets::Vertex> & vert = blocks[iLevel][iBlock].getVertices();

            for(int32 ii=0; ii < vert.size(); ii++)
                bbox.addPoint(vert[ii].position);
        }
    }

    int32 terrainWidth = (2 * (blockWidth - 1)) + 1;

    for (int32 iLevel=1; iLevel < LODCount; iLevel++)
        terrainWidth += (2 * (blockWidth - 1)) * Core::L_pow(2, iLevel - 1);

    // Height / normales
    {
        Ptr<Images::Image> pHeight(new Images::Image(Core::GRAY_F32, terrainWidth, terrainWidth));
        Ptr<Images::Image> pHeightDown(new Images::Image(Core::GRAY_F32, terrainWidth, terrainWidth));

        Ptr<Images::Image> pNormal(new Images::Image(Core::RGB_U8, terrainWidth, terrainWidth));
        Ptr<Images::Image> pNormalDown(new Images::Image(Core::RGB_U8, terrainWidth, terrainWidth));

        Images::Pixel minY, maxY;
        Images::resizeByStep(pHeightMap, pHeight);
        Images::resizeByStep(pNormalMap, pNormal);
        Images::minMax(pHeight, minY, maxY);

        for(int32 iBlock=0; iBlock < blocks[0].size(); iBlock++)
            generateBlockHeight(blocks[0][iBlock], bbox, size, minY, maxY, 0, LODCount, blendScaleFactor, pHeight, null, pNormal, null);

        for(int32 iLevel=1; iLevel < blocks.size(); iLevel++)
        {
            Images::blur(pHeight, pHeightDown, iLevel, Images::BLUR_GAUSSIAN);
            Images::blur(pNormal, pNormalDown, iLevel, Images::BLUR_GAUSSIAN);

            for(int32 iBlock=0; iBlock < blocks[iLevel].size(); iBlock++)
                generateBlockHeight(blocks[iLevel][iBlock], bbox, size, minY, maxY, iLevel, LODCount, blendScaleFactor, pHeight, pHeightDown, pNormal, pNormalDown);

            Ptr<Images::Image> tmp(pHeight);
            pHeight = pHeightDown;
            pHeightDown = tmp;

            tmp = pNormal;
            pNormal = pNormalDown;
            pNormalDown = tmp;
        }
    }

    for (int32 iLevel=1; iLevel < LODCount; iLevel++)
    {
        for(int32 iBlock=0; iBlock < blocks[iLevel].size(); iBlock++)
            removeTJunctionLevelBlock(blocks[iLevel][iBlock], iLevel, iBlock, blockWidth);
    }

    pModel->setLODCount(blocks.size());

    for(int32 iLevel=0; iLevel < blocks.size(); iLevel++)
    {
        Assets::Material  mat;
        if     (iLevel == 0) mat.diffuse = Core::Vector4f(1.0f, 0.3f, 0.3f, 1.0f);
        else if(iLevel == 1) mat.diffuse = Core::Vector4f(0.3f, 1.0f, 0.3f, 1.0f);
        else if(iLevel == 2) mat.diffuse = Core::Vector4f(0.0f, 1.0f, 1.0f, 1.0f);
        else if(iLevel == 3) mat.diffuse = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

        for(int32 iBlock=0; iBlock < blocks[iLevel].size(); iBlock++)
        {
            String name(L"level-" + Core::toString(iLevel) + L"-block-" + Core::toString(iBlock, 2));
            Assets::ModelMesh mesh;
            mesh.getMaterial() = mat;
            *mesh.getLOD(Assets::LOD_HIGH) = blocks[iLevel][iBlock];
            int32 meshId = pModel->addMesh(L"mesh-" + name, mesh);
            int32 nodeId = pModel->addMeshNode(L"node-" + name, Assets::ROOT_NODE_ID, meshId);
            pModel->addNodeRef(iLevel, nodeId);
        }
    }

    if(gen.pNormalMap == null)
        gen.pNormalMap = pNormalMap;

    pModel->cleanup();

    return pModel;
}
//-----------------------------------------------------------------------------
}
