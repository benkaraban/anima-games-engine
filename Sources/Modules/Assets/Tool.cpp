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
#include <Assets/NVMeshMender.h>
#include "Tool.h"
#include <Assets/geometricPrimitive.h>
#include <Core/Math/Random.h>
#include <Core/Math/Operation.h>
#include <Core/Logger.h>


namespace Assets
{
LM_API_ASS void removeUnusedVertices(Assets::VerticeSet & vset)
{
    Core::List<Assets::Vertex> & vert = vset.getVertices();
    Core::List<uint16> & ind = vset.getIndices();
    Core::List<bool> isUsed;

    isUsed.resize(vert.size(), false);

    for(Core::List<uint16>::const_iterator iInd = ind.begin(); iInd != ind.end(); ++iInd)
        isUsed[*iInd] = true;

    Core::List<uint16> remap;
    Core::List<Assets::Vertex> newVert;

    for(int32 ii=0; ii < vert.size(); ii++)
    {
        if(isUsed[ii])
        {
            remap.push_back(newVert.size());
            newVert.push_back(vert[ii]);
        }
        else
        {
            // La valeur n'a pas d'importance
            remap.push_back(0);
        }
    }

    Core::List<uint16> newInd;
    newInd.resize(ind.size());

    for(int32 ii=0; ii < newInd.size(); ii++)
    {
        LM_ASSERT(isUsed[ind[ii]]);
        newInd[ii] = remap[ind[ii]];
    }

    ind.swap(newInd);
    vert.swap(newVert);
}

LM_API_ASS void removeDegeneratedTriangles(Assets::VerticeSet & vset)
{
    const Core::List<Assets::Vertex> & vert = vset.getVertices();
    Core::List<uint16> & oldInd = vset.getIndices();
    Core::List<uint16> newInd;

    for(int32 ii=0; ii < oldInd.size(); ii += 3)
    {
        const Vertex & v1 = vert[oldInd[ii + 0]];
        const Vertex & v2 = vert[oldInd[ii + 1]];
        const Vertex & v3 = vert[oldInd[ii + 2]];

        if(v1.position != v2.position &&
            v1.position != v3.position &&
            v2.position != v3.position)
        {
            newInd.push_back(oldInd[ii + 0]);
            newInd.push_back(oldInd[ii + 1]);
            newInd.push_back(oldInd[ii + 2]);
        }
    }

    oldInd.swap(newInd);
}

LM_API_ASS void makeTBNBasis(Assets::VerticeSet & vset, const bool computeNormals, const Ptr<Core::IProgressObs> & pObs,
  const float minNormalsCreaseCosAngle, const float minTangentsCreaseCosAngle, const float minBinormalsCreaseCosAngle,
  const float weightNormalsByArea)
{
#if DO_NOT_USE_NV_MENDER
    // Voir si MeshMender peut fonctionner correctement ?
    makeTBN(vset);
    return;
#endif

    removeUnusedVertices(vset);
    removeDegeneratedTriangles(vset);

    MeshMender mender;

    //param

    MeshMender::NormalCalcOption MM_computeNormals = MeshMender::CALCULATE_NORMALS;
    const MeshMender::ExistingSplitOption respectExistingSplits = MeshMender::RESPECT_SPLITS;
    const MeshMender::CylindricalFixOption fixCylindricalWrapping = MeshMender::DONT_FIX_CYLINDRICAL;

    if (computeNormals == false)
    {
        MM_computeNormals = MeshMender::DONT_CALCULATE_NORMALS;
    }
    else
    {
        MM_computeNormals = MeshMender::CALCULATE_NORMALS;
    }

    //Etape 1 :
    std::vector< MeshMender::Vertex > menderVtx;
    std::vector< unsigned int > menderInd;

    Core::List<Assets::Vertex> & vsetVtx  = vset.getVertices();
    Core::List<uint16> & vsetInd = vset.getIndices();

    menderVtx.resize(vsetVtx.size());
    for(int32 iVtx=0; iVtx<vsetVtx.size(); iVtx++)
    {
        menderVtx[iVtx].pos.x = vsetVtx[iVtx].position.x;
        menderVtx[iVtx].pos.y = vsetVtx[iVtx].position.y;
        menderVtx[iVtx].pos.z = vsetVtx[iVtx].position.z;
        
        menderVtx[iVtx].normal.x = vsetVtx[iVtx].normale.x;
        menderVtx[iVtx].normal.y = vsetVtx[iVtx].normale.y;
        menderVtx[iVtx].normal.z = vsetVtx[iVtx].normale.z;

        LM_ASSERT(!vsetVtx[iVtx].normale.isZero());

        menderVtx[iVtx].tangent.x = 0.0f;
        menderVtx[iVtx].tangent.y = 0.0f;
        menderVtx[iVtx].tangent.z = 0.0f;

        menderVtx[iVtx].binormal.x = 0.0f;
        menderVtx[iVtx].binormal.y = 0.0f;
        menderVtx[iVtx].binormal.z = 0.0f;

        menderVtx[iVtx].s = vsetVtx[iVtx].texCoords.x;
        menderVtx[iVtx].t = vsetVtx[iVtx].texCoords.y;
    }

    menderInd.resize(vsetInd.size());
    for(int32 iInd=0; iInd<vsetInd.size(); iInd+=3)
    {
        menderInd[iInd+0] = vsetInd[iInd+0];
        menderInd[iInd+1] = vsetInd[iInd+1];
        menderInd[iInd+2] = vsetInd[iInd+2];
    }



    //mappingNewToOldVert - this should be passed in as an empty vector. after mending
    //                it will contain a mapping of newvertexindex -> oldvertexindex
    //                so it could be used to map any per vertex data you had in your original
    //                mesh to the new mesh like so:
    //
    //                    for each new vertex index
    //                        newVert[index]->myData = oldVert[ mappingNewToOldVert[index]]->myData;
    //
    //                where myData is some custom vertex data in your original mesh
    std::vector< unsigned int > mappingNewToOldVert;


    mender.Mend(menderVtx,
                menderInd,
                mappingNewToOldVert,
                minNormalsCreaseCosAngle,
                minTangentsCreaseCosAngle,
                minBinormalsCreaseCosAngle,
                weightNormalsByArea,
                MM_computeNormals,
                respectExistingSplits,
                fixCylindricalWrapping,
                pObs);


    //Etape2
    Core::List<Assets::Vertex> oldVtx = vsetVtx;

    vsetVtx.resize(menderVtx.size());
    for(int32 iVtx=0; iVtx<int32(menderVtx.size()); iVtx++)
    {
        vsetVtx[iVtx] = oldVtx[mappingNewToOldVert[iVtx]];

        vsetVtx[iVtx].position.x = menderVtx[iVtx].pos.x;
        vsetVtx[iVtx].position.y = menderVtx[iVtx].pos.y;
        vsetVtx[iVtx].position.z = menderVtx[iVtx].pos.z;

        vsetVtx[iVtx].normale.x = menderVtx[iVtx].normal.x;
        vsetVtx[iVtx].normale.y = menderVtx[iVtx].normal.y;
        vsetVtx[iVtx].normale.z = menderVtx[iVtx].normal.z;

        vsetVtx[iVtx].tangent.x = menderVtx[iVtx].tangent.x;
        vsetVtx[iVtx].tangent.y = menderVtx[iVtx].tangent.y;
        vsetVtx[iVtx].tangent.z = menderVtx[iVtx].tangent.z;

        vsetVtx[iVtx].bitangent.x = menderVtx[iVtx].binormal.x;
        vsetVtx[iVtx].bitangent.y = menderVtx[iVtx].binormal.y;
        vsetVtx[iVtx].bitangent.z = menderVtx[iVtx].binormal.z;

        vsetVtx[iVtx].texCoords.x = menderVtx[iVtx].s;
        vsetVtx[iVtx].texCoords.y = menderVtx[iVtx].t;
    }


    vsetInd.resize(menderInd.size());
    for(int32 iInd=0; iInd<int32(menderInd.size()); iInd++)
    {
        LM_ASSERT(menderInd[iInd] < Core::UINT16_MAX);
        vsetInd[iInd] = menderInd[iInd];
        LM_ASSERT(vsetInd[iInd] < vsetVtx.size());
    }

    removeUnusedVertices(vset);
}


struct TriangleTBN
{
    int32   i0;
    int32   i1;
    int32   i2;

    Core::Vector3f  normal;
    Core::Vector3f  tangent;
    Core::Vector3f  bitangent;
};

LM_API_ASS void makeTBN(Assets::VerticeSet & vset)
{
    Core::List<TriangleTBN>  tris;
    Core::List<Vertex> & vert = vset.getVertices();
    Core::List<uint16> & ind = vset.getIndices();

    for(int32 ii=0; ii < ind.size(); ii += 3)
    {
        TriangleTBN tri;

        tri.i0 = ind[ii + 0];
        tri.i1 = ind[ii + 1];
        tri.i2 = ind[ii + 2];

        const Vertex & v0 = vert[tri.i0];
        const Vertex & v1 = vert[tri.i1];
        const Vertex & v2 = vert[tri.i2];

        Core::Vector3f vec1(v1.position - v0.position);
        Core::Vector3f vec2(v2.position - v1.position);

        vec1.normalizeSafe();
        vec2.normalizeSafe();

        tri.normal = Core::cross(vec1, vec2).getNormalizedSafe();

        if(tri.normal.isZero())
            tri.normal = Core::Vector3f(0.0f, 0.0f, 1.0f);

        tri.tangent = Core::Vector3f(0.0f, 0.0f, 0.0f);
        tri.bitangent = Core::Vector3f(0.0f, 0.0f, 0.0f);

        //using Eric Lengyel's approach with a few modifications
        //from Mathematics for 3D Game Programmming and Computer Graphics
        // want to be able to trasform a vector in Object Space to Tangent Space
        // such that the x-axis cooresponds to the 's' direction and the
        // y-axis corresponds to the 't' direction, and the z-axis corresponds
        // to <0,0,1>, straight up out of the texture map

        //let P = v1 - v0
        Core::Vector3f P = v1.position - v0.position;
        //let Q = v2 - v0
        Core::Vector3f Q = v2.position - v0.position;
        float s1 = v1.texCoords.x - v0.texCoords.x;
        float t1 = v1.texCoords.y - v0.texCoords.y;
        float s2 = v2.texCoords.x - v0.texCoords.x;
        float t2 = v2.texCoords.y - v0.texCoords.y;

        //we need to solve the equation
        // P = s1*T + t1*B
        // Q = s2*T + t2*B
        // for T and B


        //this is a linear system with six unknowns and six equatinos, for TxTyTz BxByBz
        //[px,py,pz] = [s1,t1] * [Tx,Ty,Tz]
        // qx,qy,qz     s2,t2     Bx,By,Bz

        //multiplying both sides by the inverse of the s,t matrix gives
        //[Tx,Ty,Tz] = 1/(s1t2-s2t1) *  [t2,-t1] * [px,py,pz]
        // Bx,By,Bz                      -s2,s1        qx,qy,qz

        //solve this for the unormalized T and B to get from tangent to object space

        float tmp = 0.0f;
        if(fabsf(s1*t2 - s2*t1) <= 0.0001f)
        {
            tmp = 1.0f;
        }
        else
        {
            tmp = 1.0f/(s1*t2 - s2*t1 );
        }

        tri.tangent.x = (t2*P.x - t1*Q.x);
        tri.tangent.y = (t2*P.y - t1*Q.y);
        tri.tangent.z = (t2*P.z - t1*Q.z);

        tri.tangent = tmp * tri.tangent;

        tri.bitangent.x = (s1*Q.x - s2*P.x);
        tri.bitangent.y = (s1*Q.y - s2*P.y);
        tri.bitangent.z = (s1*Q.z - s2*P.z);

        tri.bitangent = tmp * tri.bitangent;

        if(tri.tangent.isZero())
        {
            if(tri.bitangent.isZero())
            {
                if(tri.normal.x < tri.normal.y)
                    if(tri.normal.x < tri.normal.z) // Y min
                        tri.tangent = Core::Vector3f(1.0f, 0.0f, 0.0f);
                    else // Z Min
                        tri.tangent = Core::Vector3f(0.0f, 0.0f, 1.0f);
                else
                    if(tri.normal.y < tri.normal.z) // Y min
                        tri.tangent = Core::Vector3f(0.0f, 1.0f, 0.0f);
                    else // Z min
                        tri.tangent = Core::Vector3f(0.0f, 0.0f, 1.0f);
            }
            else
            {
                tri.bitangent.normalizeSafe();
            }

            tri.tangent = Core::cross(tri.bitangent, tri.normal).getNormalizedSafe();
        }
        else
        {
            tri.tangent.normalizeSafe();
        }

        tri.bitangent = Core::cross(tri.normal, tri.tangent).getNormalizedSafe();
        tri.tangent = Core::cross(tri.bitangent, tri.normal).getNormalizedSafe();

        tris.push_back(tri);
    }

    for(int32 ii=0; ii < vert.size(); ii++)
    {
        vert[ii].tangent = Core::Vector3f::ZERO;
        vert[ii].bitangent = Core::Vector3f::ZERO;
    }

    for(int32 ii=0; ii < tris.size(); ii++)
    {
        const TriangleTBN & tri = tris[ii];

        vert[tri.i0].tangent += tri.tangent;
        vert[tri.i1].tangent += tri.tangent;
        vert[tri.i2].tangent += tri.tangent;

        vert[tri.i0].bitangent += tri.bitangent;
        vert[tri.i1].bitangent += tri.bitangent;
        vert[tri.i2].bitangent += tri.bitangent;
    }

    for(int32 ii=0; ii < vert.size(); ii++)
    {
        vert[ii].tangent.normalizeSafe();
        vert[ii].bitangent.normalizeSafe();
    }
}

}  // namespace Assets
