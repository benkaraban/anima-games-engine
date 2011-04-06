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
#include <Core/IO/Tools.h>
#include <Core/Math/MathTools.h>
#include <Core/Math/random.h>
#include "geometricPrimitive.h"
#include <Core/Logger.h>
namespace Assets
{

//-----------------------------------------------------------------------------
namespace Detail
{
void createOctahedron(Assets::VerticeSet& octahedron);
void createIcosahedron(Assets::VerticeSet& icosahedron);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
LM_API_ASS void textureWrapping(Core::List<Assets::Vertex>& vtx)
{
    const float limit = 0.5f;

    for (int32 ii=0; ii<vtx.size(); ii+=3)
    {
        if (vtx[ii].texCoords.x - vtx[ii+1].texCoords.x > limit)
           vtx[ii+1].texCoords.x += 1.f;
        if (vtx[ii+1].texCoords.x - vtx[ii].texCoords.x > limit)
           vtx[ii].texCoords.x += 1.f;
        if (vtx[ii].texCoords.x - vtx[ii+2].texCoords.x > limit)
           vtx[ii+2].texCoords.x += 1.f;
        if (vtx[ii+2].texCoords.x - vtx[ii].texCoords.x > limit)
           vtx[ii].texCoords.x += 1.f;
        if (vtx[ii+1].texCoords.x - vtx[ii+2].texCoords.x > limit)
           vtx[ii+2].texCoords.x += 1.f;
        if (vtx[ii+2].texCoords.x - vtx[ii+1].texCoords.x > limit)
           vtx[ii+1].texCoords.x += 1.f;

        if (vtx[ii].texCoords.y - vtx[ii+1].texCoords.y > limit)
           vtx[ii+1].texCoords.y += 1.f;
        if (vtx[ii+1].texCoords.y - vtx[ii].texCoords.y > limit)
           vtx[ii].texCoords.y += 1.f;
        if (vtx[ii].texCoords.y - vtx[ii+2].texCoords.y > limit)
           vtx[ii+2].texCoords.y += 1.f;
        if (vtx[ii+2].texCoords.y - vtx[ii].texCoords.y > limit)
           vtx[ii].texCoords.y += 1.f;
        if (vtx[ii+1].texCoords.y - vtx[ii+2].texCoords.y > limit)
           vtx[ii+2].texCoords.y += 1.f;
        if (vtx[ii+2].texCoords.y - vtx[ii+1].texCoords.y > limit)
           vtx[ii+1].texCoords.y += 1.f;
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void textureCvtScaleV(Core::List<Assets::Vertex>& vtx, const float minV, const float maxV)
{
    float actualMinV = FLT_MAX;
    float actualMaxV = FLT_MIN;

    for (int32 ii=0; ii<vtx.size(); ii++)
    {
        if (vtx[ii].texCoords.y < actualMinV)    actualMinV = vtx[ii].texCoords.y;
        if (vtx[ii].texCoords.y > actualMaxV)    actualMaxV = vtx[ii].texCoords.y;
    }

    for (int32 ii=0; ii<vtx.size(); ii++)
    {
        vtx[ii].texCoords.y = ((vtx[ii].texCoords.y - actualMinV) / (actualMaxV - actualMinV)) * (maxV - minV) + minV;
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void createSkySphere(Assets::VerticeSet& vset, const int32 subdivisionLevel, const float radius, const float minV, const float maxV, float angle)
{
    createUVSphere(vset, radius, subdivisionLevel, 2 * subdivisionLevel, false, angle);
    textureCvtScaleV(vset.getVertices(), minV, maxV);
}
//-----------------------------------------------------------------------------
static int32 getIndice(int32 x, int32 y, int32 meshDivCount)
{
    return y * meshDivCount + x;
}

LM_API_ASS void createPlane(Assets::VerticeSet & vset, float width, float height, int32 widthSubdiv, int32 heightSubdiv, float texScale, float randDepth)
{
    LM_ASSERT(widthSubdiv >= 1);
    LM_ASSERT(heightSubdiv >= 1);

    int32 vertexCount = widthSubdiv * heightSubdiv;

    LM_ASSERT(vertexCount <= Core::UINT16_MAX);

    Core::List<Assets::Vertex> & vertices = vset.getVertices();
    Core::List<uint16> & indices = vset.getIndices();

    vertices.resize(vertexCount);
    for(int32 y=0; y < heightSubdiv; y++)
    {
        float v = float(y) / float(heightSubdiv - 1);
        float ypos = height * (v - 0.5f);
        for(int32 x=0; x < widthSubdiv; x++)
        {
            int32 indice = getIndice(x, y, widthSubdiv);
            LM_ASSERT(indice < (int32)(vertices.size()));
            float u = float(x) / float(widthSubdiv - 1);
            float xpos = width * (u - 0.5f);
            vertices[indice].position  = Core::Vector3f(xpos, Core::frand() * randDepth, ypos);
            vertices[indice].normale   = Core::Vector3f(0.0, 1.0, 0.0);
            vertices[indice].tangent   = Core::Vector3f(1.0, 0.0, 0.0);
            vertices[indice].bitangent = Core::Vector3f(0.0, 0.0, 1.0);
            vertices[indice].texCoords = texScale * Core::Vector2f(u, v);
        }
    }

    indices.clear();

    for(int32 y=0; y < heightSubdiv - 1; y++)
    {
        for(int32 x=0; x < widthSubdiv - 1; x++)
        {
            indices.push_back(getIndice(x + 0, y + 0, widthSubdiv));
            indices.push_back(getIndice(x + 1, y + 1, widthSubdiv));
            indices.push_back(getIndice(x + 1, y + 0, widthSubdiv));

            indices.push_back(getIndice(x + 0, y + 0, widthSubdiv));
            indices.push_back(getIndice(x + 0, y + 1, widthSubdiv));
            indices.push_back(getIndice(x + 1, y + 1, widthSubdiv));
        }
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void createUVSphere(Assets::VerticeSet& sphere, const float radius, const int32 nRings, const int32 nSegments, const bool faceIsCounterClock, float maxAngle)
{
    Core::List<Assets::Vertex> & sphereVtx = sphere.getVertices();
    Core::List<uint16> & sphereInd = sphere.getIndices();

    sphereInd.clear();
    sphereVtx.resize((nRings + 1) * (nSegments+1));

    int32 wVerticeIndex = 0 ;
    int32 wVerticeIndex2 = 0 ;
    int32 u = 0 ;

    for(int32 ring=0; ring<=nRings; ring++)
    {
        float s = float(ring) / float(nRings);
        float r0 = 1 * sinf (maxAngle * s);
        float y0 = 1 * cosf (maxAngle * s);

        for(int32 seg = 0; seg<=nSegments; seg++)
        {
            float t = float(seg) / float(nSegments);
            float x0 = r0 * sinf(2.0f * f_PI * t);
            float z0 = r0 * cosf(2.0f * f_PI * t);

            sphereVtx[wVerticeIndex].position.set( x0,  y0, z0);
            sphereVtx[wVerticeIndex].position.normalizeSafe();
            sphereVtx[wVerticeIndex].normale = sphereVtx[wVerticeIndex].position;

            sphereVtx[wVerticeIndex].texCoords.x = t;
            sphereVtx[wVerticeIndex].texCoords.y = s;

            if(ring < nRings && seg < nSegments)
            {
                sphereInd.push_back(u + nSegments + 2);
                sphereInd.push_back(u + nSegments + 1);
                sphereInd.push_back(u);

                sphereInd.push_back(u + nSegments + 2);
                sphereInd.push_back(u);
                sphereInd.push_back(u + 1);
            }

            u ++;

            wVerticeIndex++;
        }
    }

    // on applique le rayon demandé
    for(int32 ii=0; ii<sphereVtx.size(); ++ii)
    {
        sphereVtx[ii].position = sphereVtx[ii].position * radius * 0.5f;
    }

    //si l'on souhaite rendre l'interieur de la sphere
    if (faceIsCounterClock == false)
    {
        for(int32 ii=0; ii<sphereInd.size(); ii+=3)
        {
            int32 tmp = sphereInd[ii+0];
            sphereInd[ii+0] = sphereInd[ii+2];
            sphereInd[ii+2] = tmp;
        }
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void createGeoSphere(Assets::VerticeSet & sphere, const float radius, const int32 levelSubdivide, const bool faceIsCounterClock, const EGeoSpherePolyhedron polyhedron)
{
    Assets::VerticeSet polyhedronSet;
    Core::List<Assets::Vertex> & polyhedronVtx = polyhedronSet.getVertices();
    Core::List<uint16> & polyhedronInd = polyhedronSet.getIndices();
    Core::List<Assets::Vertex> & sphereVtx = sphere.getVertices();
    Core::List<uint16> & sphereInd = sphere.getIndices();

    //initialisation du polyhedre
    switch (polyhedron)
    {
    case GSP_OCTAHEDRON: Detail::createOctahedron(polyhedronSet);break;
    case GSP_ICOSAHEDRON: Detail::createIcosahedron(polyhedronSet); break;
    }

    //on subdivide les faces
    sphereVtx = polyhedronVtx;
    sphereInd = polyhedronInd;
    for(int32 iLevel=0; iLevel<levelSubdivide; iLevel++)
    {
        Core::List<uint16> indexSource = sphereInd;
        Core::List<Assets::Vertex> vertexSource = sphereVtx;
        sphereInd.resize( 0 );
        sphereVtx.resize( 0 );

        for(int32 i=0; i<indexSource.size(); i+=3)
        {
            //calcule des nouveaux sommets
            int i0 = indexSource[i+0];
            int i1 = indexSource[i+1];
            int i2 = indexSource[i+2];

            const Vertex& vtx0 = vertexSource[i0];
            const Vertex& vtx1 = vertexSource[i1];
            const Vertex& vtx2 = vertexSource[i2];

            Vertex vtx01, vtx12, vtx20;
            vtx01.position = (vtx0.position + vtx1.position); vtx01.position.normalizeSafe();
            vtx12.position = (vtx1.position + vtx2.position); vtx12.position.normalizeSafe();
            vtx20.position = (vtx2.position + vtx0.position); vtx20.position.normalizeSafe();

            vtx01.normale = vtx01.position;
            vtx12.normale = vtx12.position;
            vtx20.normale = vtx20.position;

            sphereVtx.push_back(vtx0);  sphereVtx.push_back(vtx01); sphereVtx.push_back(vtx20);
            sphereVtx.push_back(vtx01); sphereVtx.push_back(vtx1);  sphereVtx.push_back(vtx12);
            sphereVtx.push_back(vtx01); sphereVtx.push_back(vtx12); sphereVtx.push_back(vtx20);
            sphereVtx.push_back(vtx20); sphereVtx.push_back(vtx12); sphereVtx.push_back(vtx2);

            //on place les nouvelles faces
            int32 iIndice = sphereInd.size();
            sphereInd.push_back( iIndice++);  sphereInd.push_back( iIndice++ ); sphereInd.push_back( iIndice++ );
            sphereInd.push_back( iIndice++ ); sphereInd.push_back( iIndice++ ); sphereInd.push_back( iIndice++ );
            sphereInd.push_back( iIndice++ ); sphereInd.push_back( iIndice++ ); sphereInd.push_back( iIndice++ );
            sphereInd.push_back( iIndice++ ); sphereInd.push_back( iIndice++ ); sphereInd.push_back( iIndice++ );
        }
    }

    //texture
    for(int32 ii=0; ii<sphereVtx.size(); ++ii)
    {
        sphereVtx[ii].texCoords.x = atan2(sphereVtx[ii].position.x, sphereVtx[ii].position.z) / (2.0f * f_PI) + 0.5f;
        sphereVtx[ii].texCoords.y = 1.0f - (asin(sphereVtx[ii].position.y) / f_PI_DIV_2);
    }

    textureWrapping(sphereVtx);

    for (int32 ii=0; ii < sphereVtx.size(); ii+=3)
    {
        if (sphereVtx[ii].position.y == -1 || sphereVtx[ii].position.y == 1)
            sphereVtx[ii].texCoords.x = (sphereVtx[ii+1].texCoords.x + sphereVtx[ii+2].texCoords.x)/2.0f;

        if (sphereVtx[ii+1].position.y == -1 || sphereVtx[ii+1].position.y == 1)
            sphereVtx[ii+1].texCoords.x = (sphereVtx[ii].texCoords.x + sphereVtx[ii+2].texCoords.x)/2.0f;

        if (sphereVtx[ii+2].position.y == -1 || sphereVtx[ii+2].position.y == 1)
            sphereVtx[ii+2].texCoords.x = (sphereVtx[ii].texCoords.x + sphereVtx[ii+1].texCoords.x)/2.0f;
    }

    // on applique le rayon
    for( int32 ii=0; ii<sphereVtx.size(); ++ii)
    {
        sphereVtx[ii].position = sphereVtx[ii].position * radius * 0.5f;
    }

    //si l'on souhaite rendre l'interieur de la sphere
    if (faceIsCounterClock == false)
    {
        for(int32 ii=0; ii<sphereInd.size(); ii+=3)
        {
            int32 tmp = sphereInd[ii+0];
            sphereInd[ii+0] = sphereInd[ii+2];
            sphereInd[ii+2] = tmp;
        }
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void createCube(Assets::VerticeSet& cube, const Core::Vector3f& size)
{
    Core::List<Assets::Vertex> & cubeVtx = cube.getVertices();
    Core::List<uint16> & cubeInd = cube.getIndices();

    Vertex vtx;

    /*0*/vtx.position = Core::Vector3f(-size.x/2,  size.y/2, -size.z/2);    cubeVtx.push_back(vtx);
    /*1*/vtx.position = Core::Vector3f( size.x/2,  size.y/2, -size.z/2);    cubeVtx.push_back(vtx);
    /*2*/vtx.position = Core::Vector3f( size.x/2, -size.y/2, -size.z/2);    cubeVtx.push_back(vtx);
    /*3*/vtx.position = Core::Vector3f(-size.x/2, -size.y/2, -size.z/2);    cubeVtx.push_back(vtx);
    /*4*/vtx.position = Core::Vector3f(-size.x/2,  size.y/2,  size.z/2);    cubeVtx.push_back(vtx);
    /*5*/vtx.position = Core::Vector3f( size.x/2,  size.y/2,  size.z/2);    cubeVtx.push_back(vtx);
    /*6*/vtx.position = Core::Vector3f( size.x/2, -size.y/2,  size.z/2);    cubeVtx.push_back(vtx);
    /*7*/vtx.position = Core::Vector3f(-size.x/2, -size.y/2,  size.z/2);    cubeVtx.push_back(vtx);
    
    cubeInd.push_back(0); cubeInd.push_back(1); cubeInd.push_back(3);
    cubeInd.push_back(1); cubeInd.push_back(2); cubeInd.push_back(3);
    cubeInd.push_back(1); cubeInd.push_back(5); cubeInd.push_back(2);
    cubeInd.push_back(5); cubeInd.push_back(6); cubeInd.push_back(2);
    cubeInd.push_back(5); cubeInd.push_back(4); cubeInd.push_back(7);
    cubeInd.push_back(5); cubeInd.push_back(7); cubeInd.push_back(6);
    cubeInd.push_back(4); cubeInd.push_back(0); cubeInd.push_back(7);
    cubeInd.push_back(0); cubeInd.push_back(3); cubeInd.push_back(7);
    cubeInd.push_back(4); cubeInd.push_back(5); cubeInd.push_back(1);
    cubeInd.push_back(4); cubeInd.push_back(1); cubeInd.push_back(0);
    cubeInd.push_back(3); cubeInd.push_back(2); cubeInd.push_back(7);
    cubeInd.push_back(2); cubeInd.push_back(6); cubeInd.push_back(7);
}
//-----------------------------------------------------------------------------
LM_API_ASS void createCubeWithNormals(Assets::VerticeSet& cube, const Core::Vector3f& size)
{
    Core::List<Assets::Vertex> & cubeVtx = cube.getVertices();
    Core::List<uint16> & cubeInd = cube.getIndices();

    float hx = 0.5f * size.x;
    float hy = 0.5f * size.y;
    float hz = 0.5f * size.z;


    Vertex v;

    Core::Vector3f  up   ( 0.0f,  1.0f,  0.0f);
    Core::Vector3f  down ( 0.0f, -1.0f,  0.0f);
    Core::Vector3f  left (-1.0f,  0.0f,  0.0f);
    Core::Vector3f  right( 1.0f,  0.0f,  0.0f);
    Core::Vector3f  front( 0.0f,  0.0f, -1.0f);
    Core::Vector3f  back ( 0.0f,  0.0f,  1.0f);

    cubeInd.reserve(6 * 2 * 3);
    cubeVtx.resize(4 * 6);

    Core::Vector3f  f1(-hx,  hy, -hz);
    Core::Vector3f  f2( hx,  hy, -hz);
    Core::Vector3f  f3( hx, -hy, -hz);
    Core::Vector3f  f4(-hx, -hy, -hz);

    Core::Vector3f  b1(-hx,  hy,  hz);
    Core::Vector3f  b2( hx,  hy,  hz);
    Core::Vector3f  b3( hx, -hy,  hz);
    Core::Vector3f  b4(-hx, -hy,  hz);

    //     B1     B2
    //     +------+
    //    /      /|
    //F1 +------+ | 
    //   |    F2| +B3
    //   |      |/
    //   +------+
    //  F4     F3
    {
        v.normale = up;
        v.position = f1; cubeVtx[ 0] = v;
        v.position = f2; cubeVtx[ 1] = v;
        v.position = b2; cubeVtx[ 2] = v;
        v.position = b1; cubeVtx[ 3] = v;

        cubeInd.push_back(0); cubeInd.push_back(3); cubeInd.push_back(1);
        cubeInd.push_back(1); cubeInd.push_back(3); cubeInd.push_back(2);
    }
    {
        v.normale = front;
        v.position = f1; cubeVtx[ 4] = v;
        v.position = f4; cubeVtx[ 5] = v;
        v.position = f3; cubeVtx[ 6] = v;
        v.position = f2; cubeVtx[ 7] = v;

        cubeInd.push_back(4); cubeInd.push_back(7); cubeInd.push_back(5);
        cubeInd.push_back(5); cubeInd.push_back(7); cubeInd.push_back(6);
    }
    {
        v.normale = right;
        v.position = f2; cubeVtx[ 8] = v;
        v.position = f3; cubeVtx[ 9] = v;
        v.position = b3; cubeVtx[10] = v;
        v.position = b2; cubeVtx[11] = v;

        cubeInd.push_back(8); cubeInd.push_back(11); cubeInd.push_back(9);
        cubeInd.push_back(9); cubeInd.push_back(11); cubeInd.push_back(10);
    }
    {
        v.normale = back;
        v.position = b2; cubeVtx[12] = v;
        v.position = b1; cubeVtx[13] = v;
        v.position = b4; cubeVtx[14] = v;
        v.position = b3; cubeVtx[15] = v;

        cubeInd.push_back(12); cubeInd.push_back(13); cubeInd.push_back(15);
        cubeInd.push_back(13); cubeInd.push_back(14); cubeInd.push_back(15);
    }
    {
        v.normale = left;
        v.position = f1; cubeVtx[16] = v;
        v.position = f4; cubeVtx[17] = v;
        v.position = b4; cubeVtx[18] = v;
        v.position = b1; cubeVtx[19] = v;

        cubeInd.push_back(16); cubeInd.push_back(17); cubeInd.push_back(19);
        cubeInd.push_back(17); cubeInd.push_back(18); cubeInd.push_back(19);
    }
    {
        v.normale = down;
        v.position = f3; cubeVtx[20] = v;
        v.position = f4; cubeVtx[21] = v;
        v.position = b4; cubeVtx[22] = v;
        v.position = b3; cubeVtx[23] = v;

        cubeInd.push_back(20); cubeInd.push_back(23); cubeInd.push_back(21);
        cubeInd.push_back(21); cubeInd.push_back(23); cubeInd.push_back(22);
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void createCylinder(Assets::VerticeSet& cylinder, const float radius,  const float height)
{
    int32 nbDiv = 4;

    Core::List<Assets::Vertex> & cylinderVtx = cylinder.getVertices();
    Core::List<uint16> & cylinderInd = cylinder.getIndices();

    Vertex vtx0;
    Vertex vtx1;
    Vertex vtx2;

    float deltaAngle = 2*f_PI / (4*nbDiv);
    for (int32 ii=0; ii<4*nbDiv; ++ii)
    {
            vtx0.position = Core::Vector3f(0,  -height/2, 0);    
            vtx0.normale = Core::Vector3f(0,-1,0).getNormalizedSafe();
        cylinderVtx.push_back(vtx0);

            vtx1.position = Core::Vector3f(radius*cos(ii*deltaAngle)-radius*sin(ii*deltaAngle), -height/2, radius*cos(ii*deltaAngle)+radius*sin(ii*deltaAngle));                       
            vtx1.normale = (vtx1.position - vtx0.position).getNormalizedSafe();
        cylinderVtx.push_back(vtx1);

            vtx2.position = Core::Vector3f(radius*cos(ii*deltaAngle+deltaAngle)-radius*sin(ii*deltaAngle+deltaAngle), -height/2, radius*cos(ii*deltaAngle+deltaAngle)+radius*sin(ii*deltaAngle+deltaAngle));  
            vtx2.normale = (vtx2.position - vtx0.position).getNormalizedSafe();
        cylinderVtx.push_back(vtx2);

        cylinderInd.push_back(cylinderVtx.size()-3); cylinderInd.push_back(cylinderVtx.size()-2); cylinderInd.push_back(cylinderVtx.size()-1);
    }

    for (int32 ii=0; ii<4*nbDiv; ++ii)
    {
            vtx0.position = Core::Vector3f(0,  height/2, 0);    
            vtx0.normale = Core::Vector3f(0,1,0).getNormalizedSafe();
        cylinderVtx.push_back(vtx0);

            vtx1.position = Core::Vector3f(radius*cos(ii*deltaAngle)-radius*sin(ii*deltaAngle), height/2, radius*cos(ii*deltaAngle)+radius*sin(ii*deltaAngle));                       
            vtx1.normale = (vtx1.position - vtx0.position).getNormalizedSafe();
        cylinderVtx.push_back(vtx1);

            vtx2.position = Core::Vector3f(radius*cos(ii*deltaAngle+deltaAngle)-radius*sin(ii*deltaAngle+deltaAngle), height/2, radius*cos(ii*deltaAngle+deltaAngle)+radius*sin(ii*deltaAngle+deltaAngle));  
            vtx2.normale = (vtx2.position - vtx0.position).getNormalizedSafe();
        cylinderVtx.push_back(vtx2);

        cylinderInd.push_back(cylinderVtx.size()-1); cylinderInd.push_back(cylinderVtx.size()-2); cylinderInd.push_back(cylinderVtx.size()-3);
    }

    int32 tmp = 4*nbDiv*3;

    for (int32 ii=0; ii<4*nbDiv; ++ii)
    {
        cylinderInd.push_back(1+tmp+ii*3); cylinderInd.push_back(2+ii*3);  cylinderInd.push_back(1+ii*3); 
        cylinderInd.push_back(1+tmp+ii*3); cylinderInd.push_back(2+tmp+ii*3); cylinderInd.push_back(2+ii*3); 
    }
}  
//-----------------------------------------------------------------------------
LM_API_ASS void createCone(Assets::VerticeSet& cone, const float radius, const float height)
{
    int32 nbDiv = 4;

    Core::List<Assets::Vertex> & coneVtx = cone.getVertices();
    Core::List<uint16> & coneInd = cone.getIndices();

    Vertex vtx;
    Vertex vtx0;
    Vertex vtx1;
    Vertex vtx2;

    float deltaAngle = 2*f_PI / (4*nbDiv);
    for (int32 ii=0; ii<4*nbDiv; ++ii)
    {
            vtx0.position = Core::Vector3f(0,  -height/2, 0);    
            vtx0.normale = Core::Vector3f(0,-1,0).getNormalizedSafe();
        coneVtx.push_back(vtx0);

            vtx1.position = Core::Vector3f(radius*cos(ii*deltaAngle)-radius*sin(ii*deltaAngle), -height/2, radius*cos(ii*deltaAngle)+radius*sin(ii*deltaAngle));                       
            vtx1.normale = (vtx1.position - vtx0.position).getNormalizedSafe();
        coneVtx.push_back(vtx1);

            vtx2.position = Core::Vector3f(radius*cos(ii*deltaAngle+deltaAngle)-radius*sin(ii*deltaAngle+deltaAngle), -height/2, radius*cos(ii*deltaAngle+deltaAngle)+radius*sin(ii*deltaAngle+deltaAngle));  
            vtx2.normale = (vtx2.position - vtx0.position).getNormalizedSafe();
        coneVtx.push_back(vtx2);

        coneInd.push_back(coneVtx.size()-3); coneInd.push_back(coneVtx.size()-2); coneInd.push_back(coneVtx.size()-1);
    }


        vtx.position = Core::Vector3f(0,  height/2, 0);    
        vtx.normale = Core::Vector3f(0, 1, 0).getNormalizedSafe();
    coneVtx.push_back(vtx);

    int32 tmp = 4*nbDiv*3;

    for (int32 ii=0; ii<4*nbDiv; ++ii)
    {
        coneInd.push_back(coneVtx.size()-1); coneInd.push_back(2+ii*3); coneInd.push_back(1+ii*3);  
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void createTorus(Assets::VerticeSet& torus, const float innerRadius, const float outerRadius, const float factor)
{
    Core::List<Assets::Vertex> & torusVtx = torus.getVertices();
    Core::List<uint16> & torusInd = torus.getIndices();


    int32 nbSides = 8;
    int32 nbRings = 64;

    Vertex vtx0;
    Vertex vtx1;
    Vertex vtx2;
    Vertex vtx3;

    for (int32 iRing=0; iRing<nbRings; ++iRing)
    {
        float theta = factor*iRing*2*f_PI/nbRings;
        float theta1 = factor*(iRing+1)*2*f_PI/nbRings;

    /*    float theta = iRing*2*f_PI/nbRings;
        float theta1 = (iRing+1)*2*f_PI/nbRings;
*/
        for (int32 iSide=0; iSide<nbSides; ++iSide)
        {
            float phi = iSide*2*f_PI/nbSides;
            float phi1 = (iSide+1)*2*f_PI/nbSides;

            //position
            vtx0.position.x = Core::L_cos(theta)*(outerRadius + innerRadius*Core::L_cos(phi));
            vtx0.position.y = -Core::L_sin(theta)*(outerRadius + innerRadius*Core::L_cos(phi));
            vtx0.position.z = innerRadius*Core::L_sin(phi);

            vtx1.position.x = Core::L_cos(theta1)*(outerRadius + innerRadius*Core::L_cos(phi));
	        vtx1.position.y = -Core::L_sin(theta1)*(outerRadius + innerRadius*Core::L_cos(phi));
	        vtx1.position.z = innerRadius*Core::L_sin(phi);

            vtx2.position.x = Core::L_cos(theta1)*(outerRadius + innerRadius*Core::L_cos(phi1));
            vtx2.position.y = -Core::L_sin(theta1)*(outerRadius + innerRadius*Core::L_cos(phi1));
            vtx2.position.z = innerRadius*Core::L_sin(phi1);

            vtx3.position.x = Core::L_cos(theta)*(outerRadius + innerRadius*Core::L_cos(phi1));
            vtx3.position.y = -Core::L_sin(theta)*(outerRadius + innerRadius*Core::L_cos(phi1));
            vtx3.position.z = innerRadius*Core::L_sin(phi1);


            //normale
            vtx0.normale.x = Core::L_cos(theta)*(Core::L_cos(phi));
            vtx0.normale.y = -Core::L_sin(theta)*(Core::L_cos(phi));
            vtx0.normale.z = Core::L_sin(phi);

            vtx1.normale.x = Core::L_cos(theta1)*(Core::L_cos(phi));
            vtx1.normale.y = -Core::L_sin(theta1)*(Core::L_cos(phi));
            vtx1.normale.z = Core::L_sin(phi);

            vtx2.normale.x = Core::L_cos(theta1)*(Core::L_cos(phi1));
            vtx2.normale.y = -Core::L_sin(theta1)*(Core::L_cos(phi1));
            vtx2.normale.z = Core::L_sin(phi1);

            vtx3.normale.x = Core::L_cos(theta)*(Core::L_cos(phi1));
            vtx3.normale.y = -Core::L_sin(theta)*(Core::L_cos(phi1));
            vtx3.normale.z = Core::L_sin(phi1);


            torusVtx.push_back(vtx0);
            torusVtx.push_back(vtx1);
            torusVtx.push_back(vtx2);
            torusVtx.push_back(vtx3);

            torusInd.push_back(torusVtx.size()-2); torusInd.push_back(torusVtx.size()-3); torusInd.push_back(torusVtx.size()-4);
            torusInd.push_back(torusVtx.size()-2); torusInd.push_back(torusVtx.size()-4); torusInd.push_back(torusVtx.size()-1);
        }
    }
}
//-----------------------------------------------------------------------------
LM_API_ASS void createArrow(Assets::VerticeSet& arrow, const float arrowSize)
{
    const int32 nbDiv = 4;
    Core::List<Assets::Vertex> & arrowVtx = arrow.getVertices();
    Core::List<uint16> & arrowInd = arrow.getIndices();

    const float arrowCylinderSize = arrowSize * (1-0.22f);
    const float arrowConeSize = arrowSize * 0.22f;
    const float arrowCylinderRadius = arrowCylinderSize/25.f;
    const float arrowConeRadius = arrowCylinderRadius*1.9f;

  
    Assets::createCylinder(arrow, arrowCylinderRadius,  arrowCylinderSize);

    //Cone
    int32 nbVertexCylinder = arrowVtx.size();

    Assets::Vertex vtx;
    Assets::Vertex vtx0;
    Assets::Vertex vtx1;
    Assets::Vertex vtx2;

    float deltaAngle = 2*f_PI / (4*nbDiv);
    for (int32 ii=0; ii<4*nbDiv; ++ii)
    {
            vtx0.position = Core::Vector3f(0,  arrowCylinderSize/2, 0);    
            vtx0.normale = Core::Vector3f(0,-1,0).getNormalizedSafe();
        arrowVtx.push_back(vtx0);

            vtx1.position = Core::Vector3f(arrowConeRadius*cos(ii*deltaAngle)-arrowConeRadius*sin(ii*deltaAngle), arrowCylinderSize/2, arrowConeRadius*cos(ii*deltaAngle)+arrowConeRadius*sin(ii*deltaAngle));                       
            vtx1.normale = (vtx1.position - vtx0.position).getNormalizedSafe();
        arrowVtx.push_back(vtx1);

            vtx2.position = Core::Vector3f(arrowConeRadius*cos(ii*deltaAngle+deltaAngle)-arrowConeRadius*sin(ii*deltaAngle+deltaAngle), arrowCylinderSize/2, arrowConeRadius*cos(ii*deltaAngle+deltaAngle)+arrowConeRadius*sin(ii*deltaAngle+deltaAngle));  
            vtx2.normale = (vtx2.position - vtx0.position).getNormalizedSafe();
        arrowVtx.push_back(vtx2);

        arrowInd.push_back(arrowVtx.size()-3);
        arrowInd.push_back(arrowVtx.size()-2);
        arrowInd.push_back(arrowVtx.size()-1);
    }


        vtx.position = Core::Vector3f(0,  arrowCylinderSize+arrowConeSize, 0);    
        vtx.normale = Core::Vector3f(0, 1, 0).getNormalizedSafe();
    arrowVtx.push_back(vtx);

    int32 tmp = 4*nbDiv*3;

    for (int32 ii=0; ii<4*nbDiv; ++ii)
    {
        arrowInd.push_back(arrowVtx.size()-1);
        arrowInd.push_back(nbVertexCylinder+2+ii*3);
        arrowInd.push_back(nbVertexCylinder+1+ii*3);  
    }
}
//-----------------------------------------------------------------------------

namespace Detail
{
//-----------------------------------------------------------------------------
//génère un octaèdre  : polyèdre contenant 6 sommets et 8 faces
void createOctahedron(Assets::VerticeSet& octahedron)
{
    Core::List<Assets::Vertex> & octahedronVtx = octahedron.getVertices();
    Core::List<uint16> & octahedronInd = octahedron.getIndices();

    octahedronVtx.resize(24);
    octahedronInd.resize(8*3);

    float xVal[] = { 1.0f, 0.0f, -1.0f, 0.0f, 1.0f };
    float yVal[] = { 0.0f, 1.0f, 0.0f, -1.0f, 0.0f };

    //vertex
    for(int32 ii=0; ii<12; ii+=3)
    {
        octahedronVtx[ii+0].position.set(xVal[ii/3],   yVal[ii/3],   0);
        octahedronVtx[ii+1].position.set(xVal[ii/3+1], yVal[ii/3+1], 0);
        octahedronVtx[ii+2].position.set(0,            0,            1);

        octahedronVtx[ii+0].normale = octahedronVtx[ii+0].position;
        octahedronVtx[ii+1].normale = octahedronVtx[ii+1].position;
        octahedronVtx[ii+2].normale = octahedronVtx[ii+2].position;
    }
    for(int32 ii=12; ii<24; ii+=3)
    {
        octahedronVtx[ii+0].position.set(xVal[ii/3-4+1], yVal[ii/3-4+1], 0);
        octahedronVtx[ii+1].position.set(xVal[ii/3-4],   yVal[ii/3-4],   0);
        octahedronVtx[ii+2].position.set(0,              0,             -1);

        octahedronVtx[ii+0].normale = octahedronVtx[ii+0].position;
        octahedronVtx[ii+1].normale = octahedronVtx[ii+1].position;
        octahedronVtx[ii+2].normale = octahedronVtx[ii+2].position;
    }

    //indice
    for(int32 ii=0; ii<24; ++ii)
    {
        octahedronInd[ii] = ii;
    }
}

//-----------------------------------------------------------------------------
//génère un icosaèdre : un polyèdre contenant 12 sommets, et 20 faces
void createIcosahedron(Assets::VerticeSet& icosahedron)
{
    const float phiaa  = 26.56505f;       // l'angle permettant de determiner la latitude des sommets du tour
    const float radius = 1.f;             // le rayon de la sphere dans laquel le polyhedre est inscrit
    const float phia = f_PI*phiaa/180.f;
    const float theb = f_PI*36.f/180.f;   // offset de 36 degre (angle pour atteindre le sommet de latitude inférieur)
    const float the72 = f_PI*72.f/180.f;  // angle de 72 degrees (angle entre deux sommet d'une même latitude)
    float the = 0.f;                //variable temporaire
    float theSud;

    Core::List<Assets::Vertex> & icosahedronVtx = icosahedron.getVertices();
    Core::List<uint16> & icosahedronInd = icosahedron.getIndices();

    icosahedronVtx.resize(20*3);
    icosahedronInd.resize(20*3);

    //geometrie
    //pole Nord
    the = 0.f;
    int32 ind = 0;
    for (int32 ii= ind; ii<5*3; ii+=3)
    {
        icosahedronVtx[ii].position.set(0, 1, 0);
        icosahedronVtx[ii+1].position.set(radius*cos(the+the72)*cos(phia),
                                          radius*sin(phia),
                                          radius*sin(the+the72)*cos(phia));
        icosahedronVtx[ii+2].position.set(radius*cos(the)*cos(phia),
                                          radius*sin(phia),
                                          radius*sin(the)*cos(phia));
        the = the+the72;
    }
    ind = 5*3;

    //body
    the = 0.f;
    theSud = theb;
    for (int32 ii=ind; ii<ind+5*3*2; ii+=6)
    {
        icosahedronVtx[ii].position.set(radius*cos(the+the72)*cos(phia),
                                        radius*sin(phia),
                                        radius*sin(the+the72)*cos(phia));
        icosahedronVtx[ii+2].position.set(radius*cos(the)*cos(phia),
                                          radius*sin(phia),
                                          radius*sin(the)*cos(phia));
        icosahedronVtx[ii+1].position.set(radius*cos(theSud)*cos(-phia),
                                          radius*sin(-phia),
                                          radius*sin(theSud)*cos(-phia));

        icosahedronVtx[ii+3].position.set(radius*cos(theSud+the72)*cos(-phia),
                                          radius*sin(-phia),
                                          radius*sin(theSud+the72)*cos(-phia));
        icosahedronVtx[ii+4].position.set(radius*cos(theSud)*cos(-phia),
                                          radius*sin(-phia),
                                          radius*sin(theSud)*cos(-phia));
        icosahedronVtx[ii+5].position.set(radius*cos(the+the72)*cos(phia),
                                          radius*sin(phia),
                                          radius*sin(the+the72)*cos(phia));



        the = the+the72;
        theSud = theSud+the72;
    }
    ind +=5*3*2;

    //pole sud
    theSud = theb;
    for (int32 ii=ind; ii<ind+5*3; ii+=3)
    {
        icosahedronVtx[ii].position.set(0, -1, 0);
        icosahedronVtx[ii+2].position.set(radius*cos(theSud+the72)*cos(-phia),
                                          radius*sin(-phia),
                                          radius*sin(theSud+the72)*cos(-phia));
        icosahedronVtx[ii+1].position.set(radius*cos(theSud)*cos(-phia),
                                          radius*sin(-phia),
                                          radius*sin(theSud)*cos(-phia));

        theSud = theSud+the72;

    }
    ind += 5*3;

    //indice
    for(int32 ii=0; ii<icosahedronInd.size(); ++ii)
    {
        icosahedronInd[ii] = ii;
    }

    //normal
    for (int32 ii=0; ii<icosahedronVtx.size(); ii++)
    {
        icosahedronVtx[ii].normale = icosahedronVtx[ii].position;
    }
}

//-----------------------------------------------------------------------------
} // namespace Detail

}  // namespace Assets
