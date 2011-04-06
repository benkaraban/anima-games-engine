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
#include <Assets/Tool.h>

#include <Core/Strings/Tools.h>
#include <Core/Logger.h>
#include <Core/Math/Color.h>
#include <Core/Math/Random.h>
#include <Core/StopWatch.h>
#include <Core/RT/Task.h>

#include <Packing/OcclusionmapGen.h>
#include <Packing/KDTriTree.h>

#include <Images/Image.h>

#include <Workflow/TexCompression.h>
#include <Workflow/ModelOptimize.h>

namespace Packing
{
//-----------------------------------------------------------------------------
class OcclusionTask;

struct OcclusionMapWIP
{
    int32                       triCount;
    int32                       pixelCount;

    Renderer::OcclusionMapInfos  genInfos;
    Ptr<Renderer::IMeshInstance> pMesh;

    Ptr<OcclusionTask>           pTask;
    Ptr<Core::IProgressObs>      pObs;

    Universe::ENodeType        type;

    // Soit
    Ptr<Universe::NodeMesh>     pNodeMesh;
    int32                       iLOD;    // uniquement si block terrain
    int32                       iBlock;  // uniquement si block terrain

    // Soit
    Ptr<Universe::NodeTerrain>              pNodeTerrain;
    Core::List<Ptr<OcclusionMapWIP> >       blocks;
    Core::List<Ptr<Assets::VerticeSet> >    vsets;
};
//-----------------------------------------------------------------------------
struct OcclusionMapTextureToPack
{
    String                     name;
    Ptr<Core::Bitmap>          pBitmap;
    int32                      pixelCount;

    Universe::ENodeType        type;

    // Soit
    Ptr<Universe::NodeMesh>    pNodeMesh;

    // Soit
    Ptr<Universe::NodeTerrain> pNodeTerrain;
    int32                      iLOD;
    int32                      iBlock;
};
//-----------------------------------------------------------------------------
struct TriSegment
{
    int32       i1;
    int32       i2;
    float       length;

    TriSegment()
    {}

    TriSegment(int32 i1, int32 i2, float length) : i1(i1), i2(i2), length(length)
    {}

    bool operator < (const TriSegment & s) const
    {
        return length > s.length;
    }
};

class TriangleMultiplicator : public Core::Object
{
public:
    TriangleMultiplicator(Assets::VerticeSet & vset, int32 texWidth, int32 texHeight, int32 res) : _vset(vset), _vert(vset.getVertices()), _ind(vset.getIndices()),
        _texWidth(float(texWidth)), _texHeight(float(texHeight)), _res(float(res))
    {
    }

    void cutSegment(int32 si1, int32 si2)
    {
        Assets::Vertex & v1 = _vert[si1];
        Assets::Vertex & v2 = _vert[si2];
        Assets::Vertex newVert;
        newVert.position = 0.5f * (v1.position + v2.position);
        newVert.normale = 0.5f * (v1.normale + v2.normale);
        newVert.tangent = 0.5f * (v1.tangent + v2.tangent);
        newVert.bitangent = 0.5f * (v1.bitangent + v2.bitangent);
        newVert.texCoords = 0.5f * (v1.texCoords + v2.texCoords);
        newVert.boneId[0] = newVert.boneId[1] = newVert.boneId[2] = newVert.boneId[3] = 0;

        int32 newInd = _vert.size();
        _vert.push_back(newVert);

        for(int32 ii=0; ii < _ind.size(); ii += 3)
        {
            for(int32 jj=0; jj < 3; jj++)
            {
                int32 i1 = _ind[ii + jj];
                int32 i2 = _ind[ii + (jj + 1) % 3];

                if((i1 == si1 && i2 == si2) || (i1 == si2 && i2 == si1))
                {
                    _ind[ii + jj] = newInd;

                    _ind.push_back(_ind[ii + 0]);
                    _ind.push_back(_ind[ii + 1]);
                    _ind.push_back(_ind[ii + 2]);

                    _ind[ii + jj] = i1;
                    _ind[ii + (jj + 1) % 3] = newInd;
                }
            }
        }
    }

    void process()
    {
        int32 maxVert = std::min(65000, std::max(_vert.size() * 4, 1000));
        Core::List<TriSegment> segments;
        bool mustContinue = true;

        while(mustContinue)
        {
            for(int32 ii=0; ii < _ind.size(); ii += 3)
            {
                Assets::Vertex & v0 = _vert[_ind[ii + 0]];
                Assets::Vertex & v1 = _vert[_ind[ii + 1]];
                Assets::Vertex & v2 = _vert[_ind[ii + 2]];

                Core::Vector2f duv0(v1.texCoords - v0.texCoords);
                Core::Vector2f duv1(v2.texCoords - v1.texCoords);
                Core::Vector2f duv2(v0.texCoords - v2.texCoords);

                duv1.x *= _texWidth;
                duv1.y *= _texHeight;

                duv2.x *= _texWidth;
                duv2.y *= _texHeight;

                duv0.x *= _texWidth;
                duv0.y *= _texHeight;

                float l1 = duv1.getLengthSafe();
                float l2 = duv2.getLengthSafe();
                float l3 = duv0.getLengthSafe();

                float area = std::max(l1*l2, std::max(l1*l3, l2*l3));

                if(area > (_res * _res))
                {
                    for(int32 jj=0; jj < 3; jj++)
                    {
                        int32 i1 = _ind[ii + jj];
                        int32 i2 = _ind[ii + (jj + 1) % 3];

                        Assets::Vertex & v1 = _vert[i1];
                        Assets::Vertex & v2 = _vert[i2];

                        Core::Vector2f duv(v2.texCoords - v1.texCoords);
                        duv.x *= _texWidth;
                        duv.y *= _texHeight;

                        float pixDist = duv.getLengthSafe();

                        if(pixDist > _res)
                            segments.push_back(TriSegment(i1, i2, pixDist));
                    }
                }
            }

            std::sort(segments.begin(), segments.end());
            segments.resize(std::min(segments.size(), std::max(0, maxVert - _vert.size())));

            mustContinue = !segments.empty();

            for(int32 ii=0; ii < segments.size(); ii++)
            {
                LM_ASSERT(segments[ii].i1 != segments[ii].i2);
                cutSegment(segments[ii].i1, segments[ii].i2);
            }

            segments.clear();
        }

        _vset.getBlendingGroups().clear();
        _vset.cleanup();
    }

protected:
    Assets::VerticeSet &            _vset;
    Core::List<Assets::Vertex> &    _vert;
    Core::List<uint16>         &    _ind;
    float                           _texWidth;
    float                           _texHeight;
    float                           _res;
};
//-----------------------------------------------------------------------------
struct VertexIndex
{
    VertexIndex(int32 m, int32 v) : iMesh(m), iVert(v)
    {};

    int32 iMesh;
    int32 iVert;
};
//-----------------------------------------------------------------------------
void mergeOcclusionForSimilarVertices(Core::List<Ptr<Assets::VerticeSet> > & vsets)
{
    for(int32 ii=0; ii < vsets.size(); ii++)
        Assets::removeUnusedVertices(*vsets[ii]);

    for(int32 iMesh=0; iMesh < vsets.size(); iMesh++)
    {
        Core::List<Assets::Vertex> & vert = vsets[iMesh]->getVertices();
        Core::List<VertexIndex> indices;

        for(int32 ii=0; ii < vert.size(); ii++)
        {
            indices.clear();
            indices.push_back(VertexIndex(iMesh, ii));
            float medValue = vert[ii].boneWeight[0];

            for(int32 jj=ii+1; jj < vert.size(); jj++)
            {
                const Assets::Vertex & v1 = vert[ii];
                const Assets::Vertex & v2 = vert[jj];

                if(vert[ii].position == vert[jj].position &&
                    vert[ii].normale == vert[jj].normale)
                {
                    indices.push_back(VertexIndex(iMesh, jj));
                    medValue += vert[jj].boneWeight[0];
                }
            }

            for(int32 iOtherMesh=iMesh+1; iOtherMesh < vsets.size(); iOtherMesh++)
            {
                Core::List<Assets::Vertex> & otherVert = vsets[iOtherMesh]->getVertices();

                for(int32 iOtherVert=0; iOtherVert < otherVert.size(); iOtherVert++)
                {
                    if(vert[ii].position == otherVert[iOtherVert].position &&
                        vert[ii].normale == otherVert[iOtherVert].normale)
                    {
                        indices.push_back(VertexIndex(iOtherMesh, iOtherVert));
                        medValue += otherVert[iOtherVert].boneWeight[0];
                    }
                }
            }

            medValue /= float(indices.size());

            for(int32 jj=0; jj < indices.size(); jj++)
            {
                Core::List<Assets::Vertex> & otherVert = vsets[indices[jj].iMesh]->getVertices();
                int32 idx = indices[jj].iVert;
                otherVert[idx].boneWeight[0] = otherVert[idx].boneWeight[1] = otherVert[idx].boneWeight[2] = otherVert[idx].boneWeight[3] = medValue;
            }
        }
    }
}
//-----------------------------------------------------------------------------
class OcclusionTask : public Core::Task
{
public:
    OcclusionTask(
        Core::List<Ptr<Assets::VerticeSet> > & vsets, 
        const Core::Matrix4f & worldMat, 
        const OcclusionMapsGenOptions & options,
        const KDTriTree & kdtree
    )
    : vsets(vsets), worldMat(worldMat), options(options), kdtree(kdtree)
    {}

    virtual void process()
    {
        int32 samplesCount = options.samplesCount;
        int32 totalTriCount = 0;
        int32 doneTriCount = 0;

        for(int32 iMesh=0; iMesh < vsets.size(); iMesh++)
            totalTriCount += vsets[iMesh]->getIndices().size() / 3;

        for(int32 iMesh=0; iMesh < vsets.size(); iMesh++)
        {
            Core::List<Assets::Vertex> & vert = vsets[iMesh]->getVertices();
            Core::List<uint16> & ind = vsets[iMesh]->getIndices();
            
            Core::List<float> occ(vert.size(), 0.0f);
            Core::List<int32> vSamples(vert.size(), 0);

            for(int32 iIndex=0; iIndex < ind.size(); iIndex += 3)
            {
                int32 i0 = ind[iIndex + 0];
                int32 i1 = ind[iIndex + 1];
                int32 i2 = ind[iIndex + 2];

                Assets::Vertex & v0 = vert[i0];
                Assets::Vertex & v1 = vert[i1];
                Assets::Vertex & v2 = vert[i2];

                Core::Vector3f t((v1.position - v0.position).getNormalizedSafe());
                Core::Vector3f b((v2.position - v1.position).getNormalizedSafe());
                Core::Vector3f n(Core::cross(t, b).getNormalizedSafe());
                b = Core::cross(n, t).getNormalizedSafe();

                Core::Matrix4f tbn(Core::MAT_AXIS, t, b, n, Core::Vector3f::ZERO);
                int32 freeCount = 0;

                for(int32 iSample=0; iSample < samplesCount; iSample++)
                {
                    // Direction <= Random point on a hemisphere
                    // http://mathworld.wolfram.com/SpherePointPicking.html
                    float u = 0.0f + Core::frand() * 1.0f;
                    float teta = f_PI_MUL_2 * Core::frand();

                    float mu = Core::L_sqrt(1 - u*u);
                    float cosTeta, sinTeta;
                    Core::L_sincos(teta, &cosTeta, &sinTeta);

                    Core::Vector3f  dir(mu * cosTeta, mu * sinTeta, u);
                    dir = tbn.apply3x3(dir);

                    // Position <= Random point on a triangle
                    // http://www.cgafaq.info/wiki/Random_Point_In_Triangle
                    float s0 = Core::frand();
                    float s1 = Core::frand();

                    if(s0 + s1 > 1.0f)
                    {
                        s0 = 1.0f - s0;
                        s1 = 1.0f - s1;
                    }

                    float s2 = 1.0f - s0 - s1;

                    Core::Vector3f pos(s0 * v0.position + s1 * v1.position + s2 * v2.position + 0.01f * n);
                    Core::Rayf ray(Core::Ray_OriginDirection, worldMat.apply(pos), worldMat.apply3x3(dir));

                    if(!kdtree.isOccluded(ray))
                        freeCount ++;
                }

                float triOccFactor = float(freeCount) / float(samplesCount);

                occ[i0] += triOccFactor;
                occ[i1] += triOccFactor;
                occ[i2] += triOccFactor;

                vSamples[i0] ++;
                vSamples[i1] ++;
                vSamples[i2] ++;

                doneTriCount ++;
            }

            for(int32 ii=0; ii < vert.size(); ii++)
            {
                if(vSamples[ii] > 0)
                    occ[ii] = occ[ii] / float(vSamples[ii]);

                vert[ii].boneWeight[0] = vert[ii].boneWeight[1] = vert[ii].boneWeight[2] = vert[ii].boneWeight[3] = occ[ii];
            }
        }
        
        mergeOcclusionForSimilarVertices(vsets);

        for(int32 iSmoothing=0; iSmoothing < 5; iSmoothing++)
        {
            for(int32 iMesh=0; iMesh < vsets.size(); iMesh++)
            {
                Core::List<Assets::Vertex> & vert = vsets[iMesh]->getVertices();
                Core::List<uint16> & ind = vsets[iMesh]->getIndices();
                
                Core::List<float> occ(vert.size(), 0.0f);
                Core::List<int32> vSamples(vert.size(), 0);

                for(int32 ii=0; ii < vert.size(); ii++)
                {
                    occ[ii] = 0.0;
                    vSamples[ii] = 0;
                }

                for(int32 iIndex=0; iIndex < ind.size(); iIndex += 3)
                {
                    int32 i0 = ind[iIndex + 0];
                    int32 i1 = ind[iIndex + 1];
                    int32 i2 = ind[iIndex + 2];

                    Assets::Vertex & v0 = vert[i0];
                    Assets::Vertex & v1 = vert[i1];
                    Assets::Vertex & v2 = vert[i2];

                    float triangleOcc = (v0.boneWeight[0] + v1.boneWeight[0] + v2.boneWeight[0]) / 3.0f;

                    occ[i0] += triangleOcc;
                    occ[i1] += triangleOcc;
                    occ[i2] += triangleOcc;

                    vSamples[i0] ++;
                    vSamples[i1] ++;
                    vSamples[i2] ++;
                }

                for(int32 ii=0; ii < vert.size(); ii++)
                {
                    if(vSamples[ii] > 0)
                        occ[ii] = occ[ii] / float(vSamples[ii]);

                    vert[ii].boneWeight[0] = vert[ii].boneWeight[1] = vert[ii].boneWeight[2] = vert[ii].boneWeight[3] = occ[ii];
                }
            }

            mergeOcclusionForSimilarVertices(vsets);
        }
    }

protected:
    Core::List<Ptr<Assets::VerticeSet> > &  vsets;
    const Core::Matrix4f &                  worldMat;
    const OcclusionMapsGenOptions &         options;
    const KDTriTree &                       kdtree;
};
//-----------------------------------------------------------------------------
void generateOcclusionMaps(const OcclusionMapsGenOptions & options)
{
    Core::StopWatch timer;

    KDTriTree kdtree;

    Core::List<Ptr<OcclusionMapWIP> > occmaps;
    Core::List<Ptr<OcclusionMapTextureToPack> > texturesToPack;

    float progressionStep = (options.compressTextures ? 0.5f : 0.9f);
    Ptr<Core::IProgressObs> pGenObs(new Core::SubProgressObs(options.pObserver, 0.0f, progressionStep));
    Ptr<Core::IProgressObs> pPakObs(new Core::SubProgressObs(options.pObserver, progressionStep, 1.0f));

    Ptr<Core::VirtualFileSystem> pVFS = options.pWorld->getRessourcesPool()->getVFS();

    const Core::List<Ptr<Universe::Node> > & allNodes = options.pWorld->getAllNodes();
    Core::List<Ptr<Universe::NodeMesh> > meshes;
    Core::List<Ptr<Universe::NodeTerrain> > terrains;

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
    }

    // Prépare les données => NODE_MESH

    int32 totalTriCount = 0;
    int32 totalPixelCount = 0;

    for(int32 ii=0; ii < meshes.size(); ii++)
    {
        const Renderer::Material & mat = meshes[ii]->getMeshInstance()->getMaterial();

        int32 width = options.defaultResolution;
        int32 height = options.defaultResolution;

        if(mat.pDiffuseMap != null)
        {
            int32 ratio = meshes[ii]->getLightmapResolutionRatio();

            width = std::max(1, mat.pDiffuseMap->getSourceTexture()->getWidth() / ratio);
            height = std::max(1, mat.pDiffuseMap->getSourceTexture()->getHeight() / ratio);
        }

        Ptr<OcclusionMapWIP> pOccMap(new OcclusionMapWIP());

        pOccMap->type = Universe::NODE_MESH;
        pOccMap->pNodeMesh = meshes[ii];
        pOccMap->pMesh = meshes[ii]->getMeshInstance();
        pOccMap->triCount = pOccMap->pMesh->getMesh()->getVerticeSet()->getIndices().size() / 3;
        pOccMap->pixelCount = width * height;
        pOccMap->genInfos.width = width;
        pOccMap->genInfos.height = height;
        pOccMap->genInfos.internalTextureBorder = false;

        occmaps.push_back(pOccMap);

        kdtree.add(meshes[ii]->getLocalToWorldMatrix(), *pOccMap->pMesh->getMesh()->getVerticeSet());

        totalTriCount += pOccMap->triCount;
        totalPixelCount += pOccMap->pixelCount;
    }

    // Prépare les données => NODE_TERRAIN

    for(int32 ii=0; ii < terrains.size(); ii++)
    {
        Ptr<OcclusionMapWIP> pOccMap(new OcclusionMapWIP());
        pOccMap->type = Universe::NODE_TERRAIN;
        pOccMap->pNodeTerrain = terrains[ii];

        int32 terrainTriCount = 0;
        int32 terrainPixelCount = 0;

        for(int32 iLOD=0; iLOD < terrains[ii]->getLODCount(); iLOD++)
        {
            for(int32 iBlock=0; iBlock < terrains[ii]->getBlockCountForLOD(iLOD); iBlock++)
            {
                Ptr<Renderer::IMeshInstance> pMesh = terrains[ii]->getBlockMesh(iLOD, iBlock);
                const Renderer::Material & mat = pMesh->getMaterial();

                int32 width = options.defaultResolution;
                int32 height = options.defaultResolution;

                if(mat.pDiffuseMap != null)
                {
                    int32 ratio = terrains[ii]->getLightmapResolutionRatio();

                    width = std::max(1, mat.pDiffuseMap->getSourceTexture()->getWidth() / ratio);
                    height = std::max(1, mat.pDiffuseMap->getSourceTexture()->getHeight() / ratio);
                }

                Ptr<OcclusionMapWIP> pBlock(new OcclusionMapWIP());
                pBlock->type = Universe::NODE_MESH;
                pBlock->pNodeTerrain = terrains[ii];
                pBlock->pMesh = pMesh;
                pBlock->triCount = pMesh->getMesh()->getVerticeSet()->getIndices().size() / 3;
                pBlock->pixelCount = width * height;
                pBlock->genInfos.width = width;
                pBlock->genInfos.height = height;
                pBlock->genInfos.internalTextureBorder = true;
                pBlock->iLOD = iLOD;
                pBlock->iBlock = iBlock;

                pOccMap->blocks.push_back(pBlock);

                kdtree.add(terrains[ii]->getLocalToWorldMatrix(), *pMesh->getMesh()->getVerticeSet());

                terrainTriCount += pBlock->triCount;
                terrainPixelCount += pBlock->pixelCount;
            }
        }

        pOccMap->triCount = terrainTriCount;
        pOccMap->pixelCount = terrainPixelCount;

        occmaps.push_back(pOccMap);

        totalTriCount += terrainTriCount;
        totalPixelCount += terrainPixelCount;
    }

    // Génère les occlusionmaps

    int32 done = 0;

    for(int32 iMap=0; iMap < occmaps.size(); iMap++)
    {
        OcclusionMapWIP & om = *occmaps[iMap];

        float start = float(done) / float(totalTriCount);
        float end = float(done + om.triCount) / float(totalTriCount);

        om.pObs = Ptr<Core::IProgressObs>(new Core::SubProgressObs(pGenObs, start, end));

        if(om.type == Universe::NODE_MESH)
        {
            Ptr<Assets::VerticeSet> pVSet(new Assets::VerticeSet(*om.pMesh->getMesh()->getVerticeSet()));
            om.vsets.push_back(pVSet);

            TriangleMultiplicator tm(*pVSet, om.genInfos.width, om.genInfos.height, options.triangleMinRes);
            tm.process();

            Ptr<OcclusionTask> pTask(new OcclusionTask(om.vsets, om.pMesh->getWorldMatrix(), options, kdtree));
            om.pTask = pTask;
        }
        else // Universe::NodeTerrain
        {
            for(int32 iSubTerrain=0; iSubTerrain < om.blocks.size(); iSubTerrain++)
            {
                Ptr<Assets::VerticeSet> pVSet(new Assets::VerticeSet(*om.blocks[iSubTerrain]->pMesh->getMesh()->getVerticeSet()));
                om.vsets.push_back(pVSet);
            }

            Ptr<OcclusionTask> pTask(new OcclusionTask(om.vsets, om.pNodeTerrain->getLocalToWorldMatrix(), options, kdtree));
            om.pTask = pTask;
        }

        done += om.triCount;
    }

    kdtree.optimize(50);

    for(int32 ii=0; ii < occmaps.size(); ii++)
        Core::Task::startTask(occmaps[ii]->pTask.get());

    for(int32 ii=0; ii < occmaps.size(); ii++)
    {
        occmaps[ii]->pTask->waitCompletion();
        occmaps[ii]->pObs->progress(1.0f);
    }

    for(int32 iMap=0; iMap < occmaps.size(); iMap++)
    {
        OcclusionMapWIP & om = *occmaps[iMap];

        float start = float(done) / float(totalTriCount);
        float end = float(done + om.triCount) / float(totalTriCount);

        Ptr<Core::IProgressObs> pObs(new Core::SubProgressObs(pGenObs, start, end));
        pObs->progress(0.0f);

        if(om.type == Universe::NODE_MESH)
        {
            Ptr<OcclusionMapTextureToPack> pTexToPack(new OcclusionMapTextureToPack());
            pTexToPack->type = Universe::NODE_MESH;
            pTexToPack->pNodeMesh = om.pNodeMesh;
            pTexToPack->pBitmap = options.pWorld->getRenderer()->generateOcclusionMap(om.genInfos, om.vsets[0]);
            pTexToPack->pixelCount = om.pixelCount;

            texturesToPack.push_back(pTexToPack);
        }
        else // Universe::NodeTerrain
        {
            for(int32 iSubTerrain=0; iSubTerrain < om.blocks.size(); iSubTerrain++)
            {
                Ptr<OcclusionMapTextureToPack> pTexToPack(new OcclusionMapTextureToPack());
                pTexToPack->type = Universe::NODE_TERRAIN;
                pTexToPack->pNodeTerrain = om.pNodeTerrain;
                pTexToPack->pBitmap = options.pWorld->getRenderer()->generateOcclusionMap(om.blocks[iSubTerrain]->genInfos, om.vsets[iSubTerrain]);
                pTexToPack->iLOD = om.blocks[iSubTerrain]->iLOD;
                pTexToPack->iBlock = om.blocks[iSubTerrain]->iBlock;
                pTexToPack->pixelCount = om.blocks[iSubTerrain]->pixelCount;

                texturesToPack.push_back(pTexToPack);
            }
        }

        done += om.triCount;
    }

    // Packe et sauvegarde le résultat

    Workflow::TextureOptions texOptions;
    texOptions.texType = Workflow::TEXTURE_LIGHTMAP;
    texOptions.quality = Workflow::QUALITY_HIGH;
    texOptions.format  = (options.compressTextures ? Assets::TEX_FORMAT_DXTC1 : Assets::TEX_FORMAT_ARGB8);
    texOptions.generateMipmap = true;

    int32 uid = 0;
    done = 0;
    INF << L"\n";

    for(int32 iMap=0; iMap < texturesToPack.size(); iMap++)
    {
        const OcclusionMapTextureToPack & map = *texturesToPack[iMap];

        float start = float(done) / float(totalPixelCount);
        float end = float(done + map.pixelCount) / float(totalPixelCount);

        INF << iMap << L" /" << Core::toString(texturesToPack.size()) << L"\n";
        String uniqueName;
        Core::trimRight(options.path, uniqueName, L"/");
        uniqueName += L"/" + options.prefix;
        uniqueName += Core::toString(uid, 4);
        uniqueName += L".tex";

        INF << L"Saving " << uniqueName << L"\n";

        Ptr<Core::IProgressObs> pObs(new Core::SubProgressObs(pPakObs, start, end));
        texOptions.pProgressObs = pObs.get();
        Ptr<Images::Image> pImage = Images::toImage(map.pBitmap);
        Ptr<Assets::Texture> pTexture = Workflow::generateTexture(texOptions, pImage);
        pVFS->write(uniqueName, *pTexture);

        if(map.type == Universe::NODE_MESH)
        {
            map.pNodeMesh->setOcclusionTexture(uniqueName, false);
        }
        else // if(lms[iMap]->type == Universe::NODE_TERRAIN)
        {
            map.pNodeTerrain->setBlockOcclusionTexture(map.iLOD, map.iBlock, uniqueName, false);
        }

        done += map.pixelCount;
        uid++;
    }

    INF << L"Elapsed time : " << Core::toString(timer.elapsed().ftotalSeconds()) << L"\n";
    INF << L"---\n";
};
//-----------------------------------------------------------------------------
}
