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
#include <Workflow/COLLADA/COLLADAMeshCvt.h>
#include <Workflow/COLLADA/COLLADATools.h>
#include <Core/Map.h>

#include <COLLADAFWTriangles.h>

namespace Workflow
{
//-----------------------------------------------------------------------------
struct COLLADATuple
{
    int32   positionIndex;
    int32   normalIndex;
    int32   texcoordsIndex;

    COLLADATuple() : positionIndex(-1), normalIndex(-1), texcoordsIndex(-1)
    {
    };

    bool operator < (const COLLADATuple & t) const
    {
        if(positionIndex == t.positionIndex)
            if(normalIndex == t.normalIndex)
                return texcoordsIndex < t.texcoordsIndex;
            else
                return normalIndex < t.normalIndex;
        else
            return positionIndex < t.positionIndex;
    };
};
//-----------------------------------------------------------------------------
class COLLADAMeshConverter : public Core::Object
{
public:
    COLLADAMeshConverter(const COLLADAFW::Mesh * pMesh, bool swapYZ, float scale);
    Ptr<MeshData> convert();

protected:
    const COLLADAFW::Mesh *            _pMesh;

    const COLLADAFW::MeshVertexData &  _positions;
    const COLLADAFW::MeshVertexData &  _normals;
    const COLLADAFW::MeshVertexData &  _texcoords;

    bool                               _swapYZ;
    float                              _scale;

    Ptr<MeshData>                      _pResult;
    Ptr<Assets::VerticeSet>            _pVertices;

    Core::Map<COLLADATuple, int32>     _tupleToVertexIndex;

    void convertPrimitive(const COLLADAFW::MeshPrimitive * pPrim);
    void addTuple(const COLLADATuple & tuple);
};
//-----------------------------------------------------------------------------
COLLADAMeshConverter::COLLADAMeshConverter(const COLLADAFW::Mesh * pMesh, bool swapYZ, float scale) :
    _pMesh(pMesh),
    _positions(pMesh->getPositions()),
    _normals(pMesh->getNormals()),
    _texcoords(pMesh->getUVCoords()),
    _swapYZ(swapYZ),
    _scale(scale)
{
}
//-----------------------------------------------------------------------------
Ptr<MeshData> COLLADAMeshConverter::convert()
{
    _pResult = Ptr<MeshData>(new MeshData());
    _pResult->pMesh = Ptr<Assets::ModelMesh>(new Assets::ModelMesh());
    _pVertices = _pResult->pMesh->getLOD(Assets::LOD_HIGH);

    const COLLADAFW::MeshPrimitiveArray & prims = _pMesh->getMeshPrimitives();

    for(int32 iPrim=0; iPrim < int32(prims.getCount()); iPrim++)
        convertPrimitive(prims[iPrim]);

    Core::List<uint16> & ind = _pVertices->getIndices();

    if(_swapYZ)
    {
        for(int32 ii=0; ii < ind.size(); ii += 3)
            std::swap(ind[ii + 1], ind[ii + 2]);
    }

    return _pResult;
}
//-----------------------------------------------------------------------------
void COLLADAMeshConverter::convertPrimitive(const COLLADAFW::MeshPrimitive * pPrim)
{
    const COLLADAFW::UIntValuesArray & indPositions = pPrim->getPositionIndices();
    const COLLADAFW::UIntValuesArray & indNormals   = pPrim->getNormalIndices();
    const COLLADAFW::UIntValuesArray * pIndTexcoords = 0;
    const COLLADAFW::IndexListArray &  indListTexcoords = pPrim->getUVCoordIndicesArray();

    bool hasNormals = !indNormals.empty();
    bool hasTexcoords = !indListTexcoords.empty();

    if(hasTexcoords)
    {
        pIndTexcoords = &indListTexcoords[0]->getIndices();
        hasTexcoords = !pIndTexcoords->empty();
    }

    const COLLADAFW::UIntValuesArray & indTexcoords = *pIndTexcoords;

    LM_ASSERT(!hasNormals || (indNormals.getCount() == indPositions.getCount()));
    LM_ASSERT(!hasTexcoords || (indTexcoords.getCount() == indPositions.getCount()));

    switch(pPrim->getPrimitiveType())
    {
    case COLLADAFW::MeshPrimitive::TRIANGLES:
    {
        for(int32 ii=0; ii < int32(indPositions.getCount()); ii++)
        {
            COLLADATuple tuple;

            tuple.positionIndex = indPositions[ii];

            if(hasNormals) tuple.normalIndex = indNormals[ii];
            if(hasTexcoords) tuple.texcoordsIndex = indTexcoords[ii];

            addTuple(tuple);
        }
        break;
    }
    default:
        throw Core::Exception(L"COLLADAMeshConverter unsupported mesh primitive type");
    }
}
//-----------------------------------------------------------------------------
void COLLADAMeshConverter::addTuple(const COLLADATuple & tuple)
{
    Core::List<Assets::Vertex> & vert = _pVertices->getVertices();
    Core::List<uint16> & ind = _pVertices->getIndices();

    Core::Map<COLLADATuple, int32>::const_iterator iTuple = _tupleToVertexIndex.find(tuple);

    if(iTuple != _tupleToVertexIndex.end())
    {
        ind.push_back(iTuple->second);
    }
    else
    {
        Assets::Vertex v;

        if (_positions.getType() == COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE)
        {
            const double * pPositions = _positions.getDoubleValues()->getData();
            pPositions += 3 * tuple.positionIndex;

            if(_swapYZ) v.position = _scale * Core::Vector3f(float(-pPositions[0]), float(pPositions[2]), float(-pPositions[1]));
            else        v.position = _scale * Core::Vector3f(float(pPositions[0]), float(pPositions[1]), float(pPositions[2]));
        }
        else
        {
            const float * pPositions = _positions.getFloatValues()->getData();
            pPositions += 3 * tuple.positionIndex;

            if(_swapYZ) v.position = _scale * Core::Vector3f(-pPositions[0], pPositions[2], -pPositions[1]);
            else        v.position = _scale * Core::Vector3f(pPositions[0], pPositions[1], pPositions[2]);
        }

        if(tuple.normalIndex >= 0)
        {
            if (_normals.getType() == COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE)
            {
                const double * pNormals = _normals.getDoubleValues()->getData();
                pNormals += 3 * tuple.normalIndex;

                if(_swapYZ) v.normale = Core::Vector3f(float(-pNormals[0]), float(pNormals[2]), float(-pNormals[1]));
                else        v.normale = Core::Vector3f(float(pNormals[0]), float(pNormals[1]), float(pNormals[2]));
            }
            else
            {
                const float * pNormals = _normals.getFloatValues()->getData();
                pNormals += 3 * tuple.normalIndex;

                if(_swapYZ) v.normale = Core::Vector3f(-pNormals[0], pNormals[2], -pNormals[1]);
                else        v.normale = Core::Vector3f(pNormals[0], pNormals[1], pNormals[2]);
            }
        }

        if(tuple.texcoordsIndex >= 0)
        {
            if (_texcoords.getType() == COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE)
            {
                const double * pTexcoords = _texcoords.getDoubleValues()->getData();
                pTexcoords += _texcoords.getStride(0) * tuple.texcoordsIndex;
                v.texCoords = Core::Vector2f(float(pTexcoords[0]), 1.0f - float(pTexcoords[1]));
            }
            else
            {
                const float * pTexcoords = _texcoords.getFloatValues()->getData();
                pTexcoords += _texcoords.getStride(0) * tuple.texcoordsIndex;
                v.texCoords = Core::Vector2f(pTexcoords[0], 1.0f - pTexcoords[1]);
            }
        }

        int32 index = vert.size();

        ind.push_back(index);
        _tupleToVertexIndex[tuple] = index;
        vert.push_back(v);
        _pResult->newToOldIndex.push_back(tuple.positionIndex);
    }
}
//-----------------------------------------------------------------------------
Ptr<MeshData> convertMesh(const COLLADAFW::Mesh * pMesh, bool swapYZ, float scale)
{
    COLLADAMeshConverter cvt(pMesh, swapYZ, scale);
    return cvt.convert();
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
Ptr<SkinData> convertSkin(const COLLADAFW::SkinControllerData * pSkinData, bool swapYZ, float scale)
{
    Ptr<SkinData> pResult(new SkinData());
    Core::List<SkinData::VertexData> & influ = pResult->bonesInfluences;

    influ.resize(pSkinData->getVertexCount());

    for(int32 ii=0; ii < influ.size(); ii++)
    {
        for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
        {
            influ[ii].boneIndex[iBone] = 0;
            influ[ii].boneWeight[iBone] = 0.0f;
        }
    }

    int32 ind = 0;

    const COLLADAFW::FloatOrDoubleArray & weights = pSkinData->getWeights();
    const COLLADAFW::UIntValuesArray & jointsPerVertex = pSkinData->getJointsPerVertex();
    const COLLADAFW::UIntValuesArray & weightInd = pSkinData->getWeightIndices();
    const COLLADAFW::IntValuesArray & jointInd = pSkinData->getJointIndices();

    Core::List<BoneFactor> bones;

    for(int32 iVertex=0; iVertex < int32(pSkinData->getVertexCount()); iVertex++)
    {
        bones.resize(jointsPerVertex[iVertex]);

        for(int32 iJoint=0; iJoint < bones.size(); iJoint++)
        {
            bones[iJoint].id = jointInd[ind];
            LM_ASSERT(jointInd[ind] < int32(pSkinData->getJointsCount()));

            if(weights.getType() == COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE)
            {
                const double * pWeights = weights.getDoubleValues()->getData();
                bones[iJoint].weight = float(pWeights[weightInd[ind]]);
            }
            else
            {
                const float * pWeights = weights.getFloatValues()->getData();
                bones[iJoint].weight = pWeights[weightInd[ind]];
            }

            ind ++;
        }

        // Ne garde que les Assets::MAX_BONES_PER_VERTEX plus influents
        std::sort(bones.begin(), bones.end()); 
        bones.resize(std::min(bones.size(), Assets::MAX_BONES_PER_VERTEX));

        // Renormalise le r�sultat
        float weightSum = 0.0f;

        for(int32 iJoint=0; iJoint < bones.size(); iJoint++)
            weightSum += bones[iJoint].weight;

        if(weightSum > 0.0f)
        {
            for(int32 iJoint=0; iJoint < bones.size(); iJoint++)
                bones[iJoint].weight /= weightSum;
        }

        for(int32 iJoint=0; iJoint < bones.size(); iJoint++)
        {
            influ[iVertex].boneIndex[iJoint] = bones[iJoint].id;
            influ[iVertex].boneWeight[iJoint] = bones[iJoint].weight;
        }
    }

    LM_ASSERT(ind == weightInd.getCount());
    LM_ASSERT(ind == jointInd.getCount());

    pResult->bindShapeMatrix = cvtMat4(pSkinData->getBindShapeMatrix(), swapYZ, scale);
    const COLLADAFW::Matrix4Array & bindMatrices = pSkinData->getInverseBindMatrices();

    for(int32 ii=0; ii < int32(bindMatrices.getCount()); ii++)
    {
        Core::Matrix4f bindMat(cvtMat4(bindMatrices[ii], swapYZ, scale));
        pResult->offsetMatrices.push_back(bindMat);
    }

    return pResult;
}
//-----------------------------------------------------------------------------
Ptr<SkeletonData> convertSkeleton(const COLLADAFW::SkinController * pCtrl)
{
    Ptr<SkeletonData> pResult(new SkeletonData());

    for(int32 ii=0; ii < int32(pCtrl->getJoints().getCount()); ii++)
        pResult->bonesUID.push_back(pCtrl->getJoints()[ii]);

    pResult->dataUID = pCtrl->getSkinControllerData();
    pResult->meshUID = pCtrl->getSource();

    return pResult;
}
//-----------------------------------------------------------------------------
void applySkinToVertices(Assets::VerticeSet & vset, const MeshData & meshData, const SkinData & skinData)
{
    const Core::List<int32> & newToOldIndex = meshData.newToOldIndex;
    Core::List<Assets::Vertex> & vert = vset.getVertices();

    LM_ASSERT(vert.size() == newToOldIndex.size());

    for(int32 iVert=0; iVert < vert.size(); iVert++)
    {
        int32 oldIndex = newToOldIndex[iVert];
        LM_ASSERT(oldIndex < skinData.bonesInfluences.size());

        for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
        {
            vert[iVert].boneId[iBone] = skinData.bonesInfluences[oldIndex].boneIndex[iBone];
            vert[iVert].boneWeight[iBone] = skinData.bonesInfluences[oldIndex].boneWeight[iBone];
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<AnimationData> convertAnimData(const COLLADAFW::AnimationCurve * pAnimCurve, bool swapYZ, float scale)
{
    Ptr<AnimationData> pResult(new AnimationData());
    Core::List<AnimationData::Key> & keys = pResult->keys;

    if(pAnimCurve->getInPhysicalDimension() != COLLADAFW::PHYSICAL_DIMENSION_TIME)
        throw Core::Exception(L"Unsupported animation input physical dimension");

    if(pAnimCurve->getOutDimension() != 16)
        throw Core::Exception(L"Unsupported animation output dimension");

    keys.resize(pAnimCurve->getKeyCount());
    pResult->startIndex = 0;
    
    const COLLADAFW::FloatOrDoubleArray & inputData = pAnimCurve->getInputValues();
    const COLLADAFW::FloatOrDoubleArray & outputData = pAnimCurve->getOutputValues();

    LM_ASSERT(inputData.getValuesCount() == keys.size());
    LM_ASSERT(outputData.getValuesCount() == (keys.size() * 16));

    for(int32 ii=0; ii < keys.size(); ii++)
    {
        if(inputData.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE)
        {
            const double * pInputData = reinterpret_cast<const double *>(inputData.getDoubleValues()->getData());
            keys[ii].time = float(pInputData[ii]);
        }
        else
        {
            const float * pInputData = reinterpret_cast<const float *>(inputData.getFloatValues()->getData());
            keys[ii].time = pInputData[ii];
        }

        Core::Matrix4f matrix;

        if(outputData.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE)
        {
            const double * pOutputData = reinterpret_cast<const double *>(outputData.getDoubleValues()->getData());
            pOutputData += 16 * ii;
            matrix = Core::Matrix4f(float(pOutputData[ 0]), float(pOutputData[ 1]), float(pOutputData[ 2]), float(pOutputData[ 3]),
                                    float(pOutputData[ 4]), float(pOutputData[ 5]), float(pOutputData[ 6]), float(pOutputData[ 7]),
                                    float(pOutputData[ 8]), float(pOutputData[ 9]), float(pOutputData[10]), float(pOutputData[11]),
                                    float(pOutputData[12]), float(pOutputData[13]), float(pOutputData[14]), float(pOutputData[15]));
            matrix = cvtMat4(matrix, swapYZ, scale);
        }
        else
        {
            const float * pOutputData = reinterpret_cast<const float *>(outputData.getFloatValues()->getData());
            pOutputData += 16 * ii;
            matrix = Core::Matrix4f(pOutputData[ 0], pOutputData[ 1], pOutputData[ 2], pOutputData[ 3],
                                    pOutputData[ 4], pOutputData[ 5], pOutputData[ 6], pOutputData[ 7],
                                    pOutputData[ 8], pOutputData[ 9], pOutputData[10], pOutputData[11],
                                    pOutputData[12], pOutputData[13], pOutputData[14], pOutputData[15]);
            matrix = cvtMat4(matrix, swapYZ, scale);
        }

        keys[ii].transform = Core::Transformf(matrix);
    }

    if(keys.size() > 0)
    {
        float minTime = keys[0].time;
        float maxTime = keys[keys.size() - 1].time;
        float timeMul = 1000.0f / std::max(maxTime, 1.0f);

        for(int32 iKey=0; iKey < keys.size(); iKey++)
            keys[iKey].time *= timeMul;

        pResult->duration = maxTime - minTime;
        pResult->startIndex = int32(Core::L_round(minTime / (pResult->duration / float(keys.size()))));
    }

    return pResult;
}
//-----------------------------------------------------------------------------
}
