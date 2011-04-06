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
#include <Core/Math/MathTools.h>
#include <Core/Math/Random.h>
#include <Universe/Particles/EmitterShapeModel.h>

namespace Universe
{
//-----------------------------------------------------------------------------
EmitterShapeModel::EmitterShapeModel(const Ptr<RessourcePool> & pResPool) :
    _pResPool(pResPool)
{
}
//-----------------------------------------------------------------------------
void EmitterShapeModel::setModel(const String & model, const String & mesh)
{
    _modelName = model;
    _meshName = mesh;

    _pModel = _pResPool->getModel(model);

    if(mesh == String::EMPTY)
        _pMesh = _pModel->getMeshPtr(0);
    else
        _pMesh = _pModel->getMeshPtr(_pModel->getMeshId(mesh));

    Ptr<Assets::VerticeSet> pVset = _pMesh->getLOD(Assets::LOD_HIGH);
    const Core::List<Assets::Vertex> & vert = pVset->getVertices();
    const Core::List<uint16> & ind = pVset->getIndices();

    _tris.clear();

    _psum = 0.0;

    for(int32 ii=0; ii < ind.size(); ii += 3)
    {
        const Assets::Vertex & v0 = vert[ind[ii + 0]];
        const Assets::Vertex & v1 = vert[ind[ii + 1]];
        const Assets::Vertex & v2 = vert[ind[ii + 2]];

        TriangleInfos   tri;
        tri.o = v0.position;
        tri.u = v1.position - v0.position;
        tri.v = v2.position - v0.position;
        tri.n = Core::cross(-tri.v, tri.u).getNormalizedSafe();

        _psum += double(tri.u.getLengthSafe()) * double(tri.u.getLengthSafe());
        
        tri.psum = _psum;
        _tris.push_back(tri);
    }
}
//-----------------------------------------------------------------------------
void EmitterShapeModel::createParticle(Particle & part)
{
    if(_tris.size() == 0)
    {
        part.position = Core::Vector3f::ZERO;
        part.speed = Core::Vector3f::ZERO;
    }
    else
    {
        int32 ii = 0;
        double p = Core::drand(0.0, _psum);

        while(ii < _tris.size() && p > _tris[ii].psum)
            ii++;

        LM_ASSERT(ii < _tris.size());

        if(ii < _tris.size())
        {
            float u = Core::frand();
            float v = Core::frand();
            
            if((u + v) > 1.0f)
            {
                if(u > v)
                    u = 1.0f - u;
                else
                    v = 1.0f - v;
            }

            const TriangleInfos & tri = _tris[ii];
            part.position = tri.o + u * tri.u + v * tri.v;
            part.speed = Core::Vector3f::ZERO;
        }
    }
}
//-----------------------------------------------------------------------------
void EmitterShapeModel::updateParticles(Core::List<Particle> & particles, double elapsedTime)
{
}
//-----------------------------------------------------------------------------
void EmitterShapeModel::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"shape.model", _modelName);
    pXMLNode->setValue(L"shape.mesh", _meshName);
}
//-----------------------------------------------------------------------------
void EmitterShapeModel::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    String model, mesh;

    pXMLNode->getValue(L"shape.model", model);
    pXMLNode->getValue(L"shape.mesh", mesh);

    setModel(model, mesh);
}
//-----------------------------------------------------------------------------
}
