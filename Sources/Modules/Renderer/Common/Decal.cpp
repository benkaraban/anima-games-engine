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
#include <Assets/Vertex.h>
#include <Renderer/Common/Decal.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
Decal::Decal(const Ptr<ITextureMap> & pTexture, EDecalMode mode) :
    _matrix(Core::IDENTITY), _width(1.0), _height(1.0), _timeKey(0.0f), _pTexture(pTexture),
    _mode(mode),
    _normal(DECAL_NORMAL_Z),
    _color(1.0, 1.0, 1.0, 1.0),
    _glow(0.0, 0.0, 0.0, 0.0),
    _isAlive(true),
    _isVisible(true),
    _spriteU(1),
    _spriteV(1),
    _zBias(0.0f),
    _fogFactor(1.0f)
{
    updateBoundingBox();
};
//-----------------------------------------------------------------------------
void Decal::getVertices(DecalVertex pVertices [], bool glowColor) const
{
    DecalVertex vert;
    vert.position = Core::Vector3f(0.0, 0.0, 0.0);
    vert.zbias = _zBias;
    
    if(glowColor)
        Assets::packColor(_glow, vert.color);
    else
        Assets::packColor(_color, vert.color);

    pVertices[0] = pVertices[1] = pVertices[2] = pVertices[3] = vert;

    float x1 = -0.5f * _width;
    float x2 =  0.5f * _width;
    float y1 = -0.5f * _height;
    float y2 =  0.5f * _height;

    if(_normal == DECAL_NORMAL_Y)
    {
        pVertices[0].position._x = x1; pVertices[0].position._z = y1;
        pVertices[1].position._x = x2; pVertices[1].position._z = y1;
        pVertices[2].position._x = x2; pVertices[2].position._z = y2;
        pVertices[3].position._x = x1; pVertices[3].position._z = y2;
    }
    else
    {
        pVertices[0].position._x = x1; pVertices[0].position._y = y1;
        pVertices[1].position._x = x2; pVertices[1].position._y = y1;
        pVertices[2].position._x = x2; pVertices[2].position._y = y2;
        pVertices[3].position._x = x1; pVertices[3].position._y = y2;
    }

    pVertices[0].texcoords[0] = 0.0; pVertices[0].texcoords[1] = 1.0;
    pVertices[1].texcoords[0] = 1.0; pVertices[1].texcoords[1] = 1.0;
    pVertices[2].texcoords[0] = 1.0; pVertices[2].texcoords[1] = 0.0;
    pVertices[3].texcoords[0] = 0.0; pVertices[3].texcoords[1] = 0.0;

    pVertices[0].texcoords[2] = _timeKey;
    pVertices[1].texcoords[2] = _timeKey;
    pVertices[2].texcoords[2] = _timeKey;
    pVertices[3].texcoords[2] = _timeKey;
}
//-----------------------------------------------------------------------------
void Decal::setSize(float width, float height)
{
    _width = width;
    _height = height;
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
float Decal::getSqDist(const Core::Vector3f & position) const
{
    return _position.getDistanceSquared(position);
}
//-----------------------------------------------------------------------------
void Decal::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _matrix = matrix;
    _position = matrix.getTVector();
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void Decal::updateBoundingBox()
{
    if(_normal == DECAL_NORMAL_Y)
    {
        Core::Vector3f v1(-0.5f * _width, 0.0f, -0.5f * _height);
        Core::Vector3f v2( 0.5f * _width, 0.0f,  0.5f * _height);
        Core::AABoxf box(v1, v2);
        _box.set(box, _matrix);
    }
    else
    {
        Core::Vector3f v1(-0.5f * _width, -0.5f * _height, 0.0f);
        Core::Vector3f v2( 0.5f * _width,  0.5f * _height, 0.0f);
        Core::AABoxf box(v1, v2);
        _box.set(box, _matrix);
    }
}
//-----------------------------------------------------------------------------
void Decal::setSpriteLayout(int32 uCount, int32 vCount)
{
    _spriteU = uCount;
    _spriteV = vCount;
}
//-----------------------------------------------------------------------------
bool Decal::isAnimated() const 
{ 
    return (_pTexture != null && _pTexture->isAnimatedTexture()) ||
        _spriteU > 1 ||
        _spriteV > 1;
}
//-----------------------------------------------------------------------------
}
