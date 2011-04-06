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
#include <d3d9.h>
#include <d3dx9.h>

#include <algorithm>
#include <iostream>

#include <winSafe.h>

#include <Core/Set.h>
#include <Core/Logger.h>
#include <Core/System/Files.h>

#include <Assets/VerticeSet.h>
#include <Assets/Vertex.h>
#include <Assets/Model.h>
#include <Assets/Tool.h>

#include <Workflow/ModelOptimize.h>

namespace Workflow
{
//-----------------------------------------------------------------------------
int32 getTotalVertices(const Assets::Model & model)
{
    int32 result = 0;

    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
        result += model.getMesh(iMesh).getVertexCount();

    return std::max(result, 1);
}
//-----------------------------------------------------------------------------
void removeBlendingGroups(Assets::VerticeSet & set)
{
    const Core::List<uint16> & ind = set.getIndices();
    Core::List<Assets::Vertex> & vert = set.getVertices();
    Core::List<Assets::BlendingGroup> & groups = set.getBlendingGroups();

    for(int32 iGroup=0; iGroup < groups.size(); iGroup++)
    {
        const Assets::BlendingGroup & group = groups[iGroup];
        const Core::List<int32> & palette = group.getMatrixPalette();

        int32 iInd = group.getStartIndice();

        for(int32 ii=0; ii < group.getIndiceCount(); ii++)
        {
            vert[ind[iInd]].blendingGroup = iGroup;
            iInd ++;
        }
    }

    for(int32 iVert=0; iVert < vert.size(); iVert++)
    {
        int32 iGroup = vert[iVert].blendingGroup;

        if(iGroup < groups.size())
        {
            const Core::List<int32> & palette = groups[iGroup].getMatrixPalette();

            for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
            {
                int32 oldBoneId = vert[iVert].boneId[iBone];

                if(oldBoneId < palette.size())
                    vert[iVert].boneId[iBone] = palette[oldBoneId];
            }
        }
    }

    groups.clear();
    set.cleanup();
}
//-----------------------------------------------------------------------------
void removeBlendingGroups(Assets::Model & model)
{
    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            removeBlendingGroups(mesh.getLODByID(iLOD));
    }
}
//-----------------------------------------------------------------------------
void createBlendingGroups(Assets::VerticeSet & set, int32 maxPaletteSize, const Ptr<Core::IProgressObs> & pObs)
{
    if(maxPaletteSize >= Core::UINT8_MAX)
        throw Core::Exception(L"Unable to subdivise VerticeSet : palette size is too high");

    Core::List<Assets::BlendingGroup> & groups = set.getBlendingGroups();
    groups.clear();

    Core::List<Assets::Vertex> & vert = set.getVertices();
    Core::List<uint16> indices = set.getIndices();
    Core::List<uint16> newIndices;
    Core::List<uint16> remainingInd;

    int32 total = indices.size();
    int32 iterCount = 5;
    int32 maxBoneId = 0;

    for(Core::List<Assets::Vertex>::const_iterator iVert = vert.begin(); iVert != vert.end(); ++iVert)
    {
        for(int32 ii=0; ii < Assets::MAX_BONES_PER_VERTEX; ii++)
        {
            maxBoneId = std::max(maxBoneId, iVert->boneId[ii]);
        }
    }

    int32 boneCount = maxBoneId + 1;

    do
    {
        int32 startIndice = newIndices.size();
        Core::List<int32>   palette;
        Core::List<bool>    isBoneInPalette;
        Core::List<bool>    isVertexInGroup;
        Core::List<uint16>  vertexDuplicate;
        Core::List<bool>    isTriangleInGroup;

        isBoneInPalette.resize(boneCount, false);
        isVertexInGroup.resize(vert.size(), false);
        vertexDuplicate.resize(vert.size(), 0);
        isTriangleInGroup.resize(indices.size() / 3, false);

        int32 iTriangle = 0;

        for(int32 ii=0; ii < indices.size(); ii += 3)
        {
            uint16 indice[3];

            indice[0] = indices[ii + 0];
            indice[1] = indices[ii + 1];
            indice[2] = indices[ii + 2];

            Core::Set<int32> bones;

            for(int32 iIndice=0; iIndice < 3; iIndice++)
                for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
                    if(vert[indice[iIndice]].boneWeight[iBone] > 0.0)
                        bones.insert(vert[indice[iIndice]].boneId[iBone]);

            for(Core::Set<int32>::const_iterator iBone = bones.begin(); iBone != bones.end(); ++iBone)
            {
                int32 bone = *iBone;
                if(!isBoneInPalette[bone] && int32(palette.size()) < maxPaletteSize)
                {
                    palette.push_back(bone);
                    isBoneInPalette[bone] = true;
                }
            }

            bool areAllBonesInPalette = true;
            for(Core::Set<int32>::const_iterator iBone = bones.begin(); iBone != bones.end(); ++iBone)
            {
                if(!isBoneInPalette[*iBone])
                    areAllBonesInPalette = false;
            }

            if(areAllBonesInPalette)
            {
                for(int32 iIndice=0; iIndice < 3; iIndice++)
                {
                    int32 iVert = indice[iIndice];
                    isVertexInGroup[iVert] = true;
                    newIndices.push_back(iVert);
                }

                isTriangleInGroup[iTriangle] = true;
            }
            else
            {
                isTriangleInGroup[iTriangle] = false;
            }

            iTriangle ++;
        }

        iTriangle = 0;

        for(int32 ii=0; ii < indices.size(); ii += 3)
        {
            if(!isTriangleInGroup[iTriangle])
            {
                uint16 indice[3];

                indice[0] = indices[ii + 0];
                indice[1] = indices[ii + 1];
                indice[2] = indices[ii + 2];
                
                for(int32 iIndice=0; iIndice < 3; iIndice++)
                {
                    int32 ind = indice[iIndice];

                    if(isVertexInGroup[ind])
                    {
                        int32 duplicate = vertexDuplicate[ind];
                        if(duplicate == 0)
                        {
                            duplicate = vertexDuplicate[ind] = vert.size();
                            vert.push_back(vert[ind]);
                        }
                        remainingInd.push_back(duplicate);
                    }
                    else
                    {
                        remainingInd.push_back(ind);
                    }
                }
            }

            iTriangle ++;
        }

        groups.push_back(Assets::BlendingGroup(startIndice, newIndices.size() - startIndice, palette));
        if(indices.size() <= remainingInd.size() && (indices.size() > 0))
            throw Core::Exception(L"Unable to subdivise VerticeSet : palette size is too low");
        indices = remainingInd;
        remainingInd.clear();

        if(pObs != null)
            pObs->progress(1.0f - (float(indices.size()) / float(std::max(1, total))));

    } while(indices.size() > 0 && --iterCount > 0);

    set.getIndices() = newIndices;

    // Remapping des boneId globaux en boneId locaux
    Core::List<bool>    isVertexRemapped;
    Core::List<int32>   matrixToPalette;
    matrixToPalette.resize(boneCount, 0);
    isVertexRemapped.resize(vert.size(), false);
    int32 groupIndex = 0;

    for(int32 ii=0; ii < vert.size(); ii++)
        vert[ii].blendingGroup = -1;

    for(Core::List<Assets::BlendingGroup>::const_iterator iGroup = groups.begin(); iGroup != groups.end(); ++iGroup)
    {
        const Core::List<int32> & palette = iGroup->getMatrixPalette();
        for(int32 ii=0; ii < palette.size(); ii++)
            matrixToPalette[palette[ii]] = ii;

        int32 iStart = iGroup->getStartIndice();
        int32 iEnd = iStart + iGroup->getIndiceCount();

        for(int32 ii=iStart; ii < iEnd; ii++)
        {
            int32 vertexIndice = newIndices[ii];
            if(!isVertexRemapped[vertexIndice])
            {
                Assets::Vertex & vertex = vert[vertexIndice];

                LM_ASSERT(vertex.blendingGroup == -1);
                vertex.blendingGroup = groupIndex;

                for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
                {
                    if(vertex.boneWeight[iBone] > 0.0)
                    {
                        vertex.boneId[iBone] = matrixToPalette[vertex.boneId[iBone]];
                    }
                }
                isVertexRemapped[vertexIndice] = true;
            }
            else
            {
                const Assets::Vertex & vertex = vert[vertexIndice];
                LM_ASSERT(vertex.blendingGroup == groupIndex);
            }
        }

        groupIndex++;
    }

    for(int32 ii=0; ii < vert.size(); ii++)
        LM_ASSERT(vert[ii].blendingGroup != -1);
}
//-----------------------------------------------------------------------------
struct BoneFactor
{
    int32 id;
    float weight;

    bool operator < (const BoneFactor & bone) const
    { return weight > bone.weight; };
};
//-----------------------------------------------------------------------------
void normalizeBonesWeights(Assets::Vertex & v, int32 maxBones)
{
    BoneFactor bones[Assets::MAX_BONES_PER_VERTEX];

    for(int32 ii=0; ii < Assets::MAX_BONES_PER_VERTEX; ii++)
    {
        bones[ii].id = v.boneId[ii];
        bones[ii].weight = v.boneWeight[ii];
    }

    std::sort(&bones[0], &bones[Assets::MAX_BONES_PER_VERTEX]);

    for(int32 ii=0; ii < maxBones; ii++)
    {
        v.boneId[ii] = bones[ii].id;
        v.boneWeight[ii] = bones[ii].weight;
    }

    for(int32 ii=maxBones; ii < Assets::MAX_BONES_PER_VERTEX; ii++)
    {
        v.boneId[ii] = 0;
        v.boneWeight[ii] = 0.0;
    }

    float weightSum = 0.0;
    for(int32 ii=0; ii < maxBones; ii++)
        weightSum += v.boneWeight[ii];

    if(weightSum > 0.0)
    {
        for(int32 ii=0; ii < maxBones; ii++)
            v.boneWeight[ii] /= weightSum;
    }
}
//-----------------------------------------------------------------------------
void normalizeBonesWeights(Assets::Model & model, int32 maxBones)
{
    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
        {
            Core::List<Assets::Vertex> & vertices = mesh.getLODByID(iLOD).getVertices();
            for(Core::List<Assets::Vertex>::iterator iVertex = vertices.begin(); iVertex != vertices.end(); ++iVertex)
                normalizeBonesWeights(*iVertex, maxBones);
        }
    }
}
//-----------------------------------------------------------------------------
void optimizeVerticesForCache(Assets::VerticeSet & vset)
{
    Assets::removeUnusedVertices(vset);
    Assets::removeDegeneratedTriangles(vset);

    Core::List<Assets::BlendingGroup> & groups = vset.getBlendingGroups();
    Core::List<Assets::Vertex> & vert = vset.getVertices();
    Core::List<uint16> & ind = vset.getIndices();

    if(groups.empty())
    {
        int32 maxBone = 0;

        for(int32 ii=0; ii < vert.size(); ii++)
            for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
                maxBone = std::max(maxBone, vert[ii].boneId[iBone]);

        Core::List<int32> palette;
        palette.resize(maxBone + 1);

        for(int32 ii=0; ii < palette.size(); ii++)
            palette[ii] = ii;

        groups.push_back(Assets::BlendingGroup(0, ind.size(), palette));
    }

    Core::List<uint16> newInd;
    newInd.resize(ind.size());

    for(int32 iGroup=0; iGroup < groups.size(); iGroup++)
    {
        const Assets::BlendingGroup & group = groups[iGroup];
        int32 faceCount = group.getIndiceCount() / 3;

        Core::List<int32> faceRemap;
        faceRemap.resize(faceCount);
        HRESULT hr = D3DXOptimizeFaces(&ind[group.getStartIndice()], faceCount, vert.size(), FALSE, (DWORD*)faceRemap.data());

        if(hr != D3D_OK)
            throw Core::Exception(L"Error in D3DXOptimizeFaces");

        int32 iNew = group.getStartIndice();

        for(Core::List<int32>::const_iterator iFace = faceRemap.begin(); iFace != faceRemap.end(); ++iFace)
        {
            int32 startIndice = *iFace * 3 + group.getStartIndice();
            newInd[iNew++] = ind[startIndice + 0];
            newInd[iNew++] = ind[startIndice + 1];
            newInd[iNew++] = ind[startIndice + 2];
        }
    }

    Core::List<int32> vertexRemap;
    vertexRemap.resize(vert.size());
    HRESULT hr = D3DXOptimizeVertices(newInd.data(), ind.size() / 3, vert.size(), FALSE, (DWORD*)vertexRemap.data());

    if(hr != D3D_OK)
        throw Core::Exception(L"Error in D3DXOptimizeVertices");

    Core::List<Assets::Vertex> newVert;
    Core::List<int32> oldIndToNew;
    oldIndToNew.resize(vert.size());
    newVert.reserve(vert.size());

    int32 ii =0;

    for(int32 iVertex=0; iVertex < vertexRemap.size(); iVertex++)
    {
        int32 index = vertexRemap[iVertex];
        LM_ASSERT(index >= 0);
        LM_ASSERT(index < int32(vert.size()));
        oldIndToNew[index] = ii++;
        newVert.push_back(vert[index]);
    }

    for(Core::List<uint16>::iterator iInd = newInd.begin(); iInd != newInd.end(); ++iInd)
        *iInd = oldIndToNew[*iInd];

    ind.swap(newInd);
    vert.swap(newVert);
}
//-----------------------------------------------------------------------------
void optimizeVerticesForCache(Assets::Model & model)
{
    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            optimizeVerticesForCache(mesh.getLODByID(iLOD));
    }
}
//-----------------------------------------------------------------------------
void generateTBN(Assets::Model & model, bool generateNormals, const Ptr<Core::IProgressObs> & pObs)
{
    int32 processed = 0;
    int32 total = getTotalVertices(model);

    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        int32 vcount = mesh.getVertexCount();
        float start = float(processed) / float(total);
        float end = float(processed + vcount) / float(total);

        Ptr<Core::IProgressObs> pSubObs(new Core::SubProgressObs(pObs, start, end));

        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            Assets::makeTBNBasis(mesh.getLODByID(iLOD), generateNormals, pSubObs);

        processed += vcount;
    }
}
//-----------------------------------------------------------------------------
void removeUnusedVertices(Assets::Model & model)
{
    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            Assets::removeUnusedVertices(mesh.getLODByID(iLOD));
    }
}
//-----------------------------------------------------------------------------
void removeDegeneratedTriangles(Assets::Model & model)
{
    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            Assets::removeDegeneratedTriangles(mesh.getLODByID(iLOD));
    }
}
//-----------------------------------------------------------------------------
void createBlendingGroups(Assets::Model & model, int32 maxPaletteSize, const Ptr<Core::IProgressObs> & pObs)
{
    int32 processed = 0;
    int32 total = getTotalVertices(model);

    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        int32 vcount = mesh.getVertexCount();
        float start = float(processed) / float(total);
        float end = float(processed + vcount) / float(total);

        Ptr<Core::IProgressObs> pSubObs(new Core::SubProgressObs(pObs, start, end));

        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            createBlendingGroups(mesh.getLODByID(iLOD), maxPaletteSize, pSubObs);

        processed += vcount;
    }
}
//-----------------------------------------------------------------------------
bool isIdentical(const Assets::Vertex & v1, const Assets::Vertex & v2)
{
    bool result = (v1.position == v2.position &&
                   v1.normale == v2.normale &&
                   v1.tangent == v2.tangent &&
                   v1.bitangent == v2.bitangent &&
                   v1.texCoords == v2.texCoords &&
                   v1.blendingGroup == v2.blendingGroup &&
                   v1.boneId[0] == v2.boneId[0] &&
                   v1.boneId[1] == v2.boneId[1] &&
                   v1.boneId[2] == v2.boneId[2] &&
                   v1.boneId[3] == v2.boneId[3] &&
                   v1.boneWeight[0] == v2.boneWeight[0] &&
                   v1.boneWeight[1] == v2.boneWeight[1] &&
                   v1.boneWeight[2] == v2.boneWeight[2] &&
                   v1.boneWeight[3] == v2.boneWeight[3]);

    return result;
}
//-----------------------------------------------------------------------------
void mergeIdenticalVertices(Assets::VerticeSet & vset, const Ptr<Core::IProgressObs> & pObs)
{
    Core::List<Assets::Vertex> & oldVert = vset.getVertices();
    
    Core::List<Assets::Vertex> newVert;
    newVert.reserve(oldVert.size());

    Core::List<uint16>  toNewInd;
    toNewInd.resize(oldVert.size());

    Core::List<bool> isUnique;
    isUnique.resize(oldVert.size(), true);

    int32 processed = 0;
    int32 total = (oldVert.size() * (oldVert.size() + 1)) / 2;

    for(int32 ii=0; ii < oldVert.size(); ii++)
    {
        if(isUnique[ii])
        {
            toNewInd[ii] = newVert.size();
            newVert.push_back(oldVert[ii]);

            for(int32 jj=(ii + 1); jj < oldVert.size(); jj++)
            {
                if(isIdentical(oldVert[ii], oldVert[jj]))
                {
                    isUnique[jj] = false;
                    toNewInd[jj] = toNewInd[ii];
                }
            }
        }

        processed += (oldVert.size() - ii);

        if(pObs != null)
            pObs->progress(float(processed) / float(total));
    }

    LM_ASSERT(processed == total);

    Core::List<uint16> & ind = vset.getIndices();

    for(int32 ii=0; ii < ind.size(); ii++)
        ind[ii] = toNewInd[ind[ii]];

    oldVert.swap(newVert);
}
//-----------------------------------------------------------------------------
void mergeIdenticalVertices(Assets::Model & model, const Ptr<Core::IProgressObs> & pObs)
{
    int32 processed = 0;
    int32 total = getTotalVertices(model);

    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        int32 vcount = mesh.getVertexCount();
        float start = float(processed) / float(total);
        float end = float(processed + vcount) / float(total);

        Ptr<Core::IProgressObs> pSubObs(new Core::SubProgressObs(pObs, start, end));

        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            mergeIdenticalVertices(mesh.getLODByID(iLOD), pSubObs);

        processed += vcount;
    }
}
//-----------------------------------------------------------------------------
void flipFaces(Assets::VerticeSet & vset)
{
    Core::List<uint16> & ind = vset.getIndices();
    for(int32 ii=0; ii < ind.size(); ii += 3)
        std::swap(ind[ii + 1], ind[ii + 2]);
}
//-----------------------------------------------------------------------------
void flipFaces(Assets::Model & model)
{
    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);

        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            flipFaces(mesh.getLODByID(iLOD));
    }
}
//-----------------------------------------------------------------------------
void autoFlipInvertedFaces(Assets::VerticeSet & vset)
{
    const Core::List<Assets::Vertex> & vert = vset.getVertices();
    Core::List<uint16> & ind = vset.getIndices();

    int32 flippedFaces = 0;
    int32 totalFaces = ind.size() / 3;

    for(int32 ii=0; ii < ind.size(); ii += 3)
    {
        const Assets::Vertex & v1 = vert[ind[ii + 0]];
        const Assets::Vertex & v2 = vert[ind[ii + 2]];
        const Assets::Vertex & v3 = vert[ind[ii + 1]];

        Core::Vector3f v1v2(v2.position - v1.position);
        Core::Vector3f v2v3(v3.position - v2.position);
        
        Core::Vector3f normFace(Core::cross(v1v2, v2v3).getNormalizedSafe());
        Core::Vector3f invNormFace(-normFace);
        Core::Vector3f normVtx((v1.normale + v2.normale + v3.normale).getNormalizedSafe());

        if(Core::dot(invNormFace, normVtx) >= Core::dot(normFace, normVtx))
            flippedFaces ++;
    }

    if((float(flippedFaces) / float(totalFaces)) > 0.9f)
    {
        for(int32 ii=0; ii < ind.size(); ii += 3)
            std::swap(ind[ii + 1], ind[ii + 2]);
    }
}
//-----------------------------------------------------------------------------
void autoFlipInvertedFaces(Assets::Model & model, const Ptr<Core::IProgressObs> & pObs)
{
    int32 processed = 0;
    int32 total = getTotalVertices(model);

    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        int32 vcount = mesh.getVertexCount();

        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            autoFlipInvertedFaces(mesh.getLODByID(iLOD));

        if(pObs != null)
            pObs->progress(float(processed + vcount) / float(total));

        processed += vcount;
    }
}
//-----------------------------------------------------------------------------
void packModel(const ModelPackingOptions & options, Assets::Model & model)
{
    removeBlendingGroups(model);
    normalizeBonesWeights(model, std::max(1, std::min(Assets::MAX_BONES_PER_VERTEX, options.maxBonesPerVertex)));
    removeUnusedVertices(model);
    removeDegeneratedTriangles(model);

    float step1, step2, step3;

    if(options.generateNormals || options.generateTangentBasis)
    {
        step1 = 0.40f;
        step2 = 0.80f;
        step3 = 0.90f;

        Ptr<Core::IProgressObs> pObsPreMerge(new Core::SubProgressObs(options.pObs, 0.0f, step1));
        Ptr<Core::IProgressObs> pObsTBN(new Core::SubProgressObs(options.pObs, step1, step2));

        mergeIdenticalVertices(model, pObsPreMerge);
        removeUnusedVertices(model);
        removeDegeneratedTriangles(model);
        generateTBN(model, options.generateNormals, pObsTBN);
    }
    else
    {
        step2 = 0.00f;
        step3 = 0.90f;
    }

    Ptr<Core::IProgressObs> pObsMerge(new Core::SubProgressObs(options.pObs, step2, step3));
    Ptr<Core::IProgressObs> pObsGroup(new Core::SubProgressObs(options.pObs, step3, 1.0f));

    mergeIdenticalVertices(model, pObsMerge);
    removeUnusedVertices(model);
    removeDegeneratedTriangles(model);
    createBlendingGroups(model, std::max(4, std::min(options.maxBonesPerBlendingGroup, 70)), pObsGroup);

    if(options.optimizeForCache)
        optimizeVerticesForCache(model);

    if(options.pObs != null)
        options.pObs->progress(1.0f);
}
//-----------------------------------------------------------------------------
void generateNormals(Assets::VerticeSet & vset)
{
    Core::List<Assets::Vertex> & vert = vset.getVertices();
    Core::List<uint16> & ind = vset.getIndices();

    for(int32 ii=0; ii < vert.size(); ii++)
        vert[ii].normale.setZero();

    for(int32 ii=0; ii < ind.size(); ii += 3)
    {
        Assets::Vertex & v1 = vert[ind[ii + 0]];
        Assets::Vertex & v2 = vert[ind[ii + 2]];
        Assets::Vertex & v3 = vert[ind[ii + 1]];

        Core::Vector3f v1v2(v2.position - v1.position);
        Core::Vector3f v2v3(v3.position - v2.position);
        
        Core::Vector3f normFace(Core::cross(v2v3, v1v2).getNormalizedSafe());

        v1.normale += normFace;
        v2.normale += normFace;
        v3.normale += normFace;
    }

    for(int32 ii=0; ii < vert.size(); ii++)
        vert[ii].normale.normalizeSafe();
}
//-----------------------------------------------------------------------------
void generateNormals(Assets::Model & model)
{
    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);

        for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
            generateNormals(mesh.getLODByID(iLOD));
    }
}
//-----------------------------------------------------------------------------
void setupTextures(Assets::Model & model, const Ptr<Core::VirtualFileSystem> & pVFS, const String & baseName)
{
    String path(Core::System::getPath(baseName));

    for(int32 iMesh=0; iMesh < model.getMeshCount(); iMesh++)
    {
        Assets::ModelMesh & mesh = model.getMesh(iMesh);
        Assets::Material & mat = mesh.getMaterial();

        if(!mat.diffuseTexName.empty())
        {
            String diffuse(path + mat.diffuseTexName + L".tex");

            if(pVFS->fileExists(diffuse))
                mat.diffuseTexName = diffuse;
            else
                mat.diffuseTexName = L"";

            if(diffuse.endsWith(L"-diffuse.tex"))
            {
                String normal(diffuse.slice(0, diffuse.size() - 13) + L"-normal.tex");

                if(pVFS->fileExists(normal))
                    mat.normalTexName = normal;
                else
                    mat.normalTexName = L"";
            }
        }
        else
        {
            mat.diffuseTexName = L"";
            mat.normalTexName = L"";
            mat.specularTexName = L"";
        }
    }
}
//-----------------------------------------------------------------------------
}
