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
#include <Renderer/Common/Spline.h>

namespace Renderer
{
//------------------------------------------------------------------------------
Spline::Spline() : 
    _worldMatrix(Core::IDENTITY),
    _color(1.0f, 1.0f, 1.0f, 1.0f),
    _resolution(100),
    _isAlive(true), 
    _isVisible(true)
{
    updatePoints();
}
//------------------------------------------------------------------------------
void Spline::setResolution(int32 ptsCount)
{ 
    _resolution = std::min(1000, std::max(2, ptsCount));
    updatePoints();
}
//------------------------------------------------------------------------------
void Spline::setSpline(const Core::CRSpline & spline)
{
    _spline = spline;
    updatePoints();
}
//------------------------------------------------------------------------------
const Core::Matrix4f & Spline::getWorldMatrix() const
{
    return _worldMatrix;
}
//------------------------------------------------------------------------------
void Spline::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    updatePoints();
}
//------------------------------------------------------------------------------
void Spline::updatePoints()
{
    _box.clear();
    _points.resize(_resolution);

    if(_spline.getPoints().size() >= 4)
    {
        for(int32 ii=0; ii < _points.size(); ii++)
        {
            float u = float(ii) / float(_resolution - 1);
            _points[ii] = _worldMatrix.apply(_spline.eval(u));
            _box.addPoint(_points[ii]);
        }
    }
    else
    {
        for(int32 ii=0; ii < _points.size(); ii++)
        {
            _points[ii] = _worldMatrix.apply(Core::Vector3f(0.0f, 0.0f, 0.0f));
            _box.addPoint(_points[ii]);
        }
    }
}
//------------------------------------------------------------------------------
}
