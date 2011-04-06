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
#ifndef RENDERER_SPLINE_H_
#define RENDERER_SPLINE_H_

#include <Core/Object.h>
#include <Renderer/ISpline.h>

namespace Renderer
{
class LM_API_RDR Spline : public Core::Object, public ISpline
{
public:
    Spline();

    const Core::List<Core::Vector3f> & getPoints() const { return _points; };

    virtual void setResolution(int32 ptsCount);
    virtual int32 getResolution() const { return _resolution; };

    virtual void setSpline(const Core::CRSpline & spline);
    virtual const Core::CRSpline & getSpline() const { return _spline; };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual const Core::Matrix4f & getWorldMatrix() const;
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);

    virtual void setColor(const Core::Vector4f & color) { _color = color; };
    virtual const Core::Vector4f & getColor() const { return _color; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

protected:
    void updatePoints();

    Core::CRSpline      _spline;
    Core::List<Core::Vector3f> _points;
    Core::Matrix4f      _worldMatrix;
    Core::Vector4f      _color;
    Core::AABoxf        _box;
    int32               _resolution;
    bool                _isAlive;
    bool                _isVisible;
};
}

#endif
