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
#include <Renderer/Common/WaterPatch.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
WaterPatch::WaterPatch(float width, float height) :
    _diffuse(0.0f, 0.0f, 1.0f, 1.0f),
    _specular(1.0f, 1.0f, 1.0f, 1.0f),
    _position(0.0f, 0.0f),
    _shininess(1.0f),
    _reflScale(0.05f),
    _refrScale(0.01f),
    _fresnelMin(0.0f),
    _fresnelPower(5.0f),
    _opacity(0.5f),
    _width(width),
    _height(height),
    _waterLevel(0.0f),
    _isAlive(true),
    _isVisible(true),
    _isRefractive(true),
    _isReflective(true)
{
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void WaterPatch::getLayer(int32 id, WaterLayer & layer) const
{
    LM_ASSERT(id >= 0 && id < WATER_LAYER_COUNT);
    layer = _layers[id];
}
//-----------------------------------------------------------------------------
void WaterPatch::setLayer(int32 id, const WaterLayer & layer)
{
    LM_ASSERT(id >= 0 && id < WATER_LAYER_COUNT);
    _layers[id] = layer;
}
//-----------------------------------------------------------------------------
void WaterPatch::setSize(float width, float height)
{
    _width = width;
    _height = height;
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
const Core::Vector2f & WaterPatch::getPosition() const
{
    return _position;
}
//-----------------------------------------------------------------------------
void WaterPatch::setPosition(const Core::Vector2f & position)
{
    _position = position;
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void WaterPatch::setWaterLevel(float level)
{
    _waterLevel = level;
    updateBoundingBox();
};
//-----------------------------------------------------------------------------
void WaterPatch::updateBoundingBox()
{
    const float SIZE = 0.25;
    Core::Vector3f p1(-SIZE * _width + _position.x, _waterLevel, -SIZE * _width + _position.y);
    Core::Vector3f p2( SIZE * _width + _position.x, _waterLevel,  SIZE * _width + _position.y);
    _box = Core::AABoxf(p1, p2);
}
//-----------------------------------------------------------------------------
}
