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
#include "ViewVolume.h"

#include <Core/Logger.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
PerspectiveViewVolume::PerspectiveViewVolume(const PerspectiveCamera& cam)
{
    cam.getWorldMatrix(_matWorld);

    _projPoint   = _matWorld.getTranslation();

    _near        = cam.getNearPlane();
    _nearHeight = cam.getNearPlane() * float(tan( cam.getFOV() / 2.f ));
    _aspectRatio = cam.getAspectRatio();
}
//-----------------------------------------------------------------------------
PerspectiveViewVolume::~PerspectiveViewVolume()
{
}
//-----------------------------------------------------------------------------
Core::Rayf PerspectiveViewVolume::makeRayFrom2DPoint(const Core::Vector2f& pt2D) const
{
    float yCentered = ((1 - pt2D.y) - 0.5f)*2;
    float xCentered = (pt2D.x - 0.5f)*2;
    
    Core::Vector3f ptNear_cam (xCentered * _nearHeight * _aspectRatio, yCentered * _nearHeight, _near);
    Core::Vector3f ptNear_world = _matWorld.apply(ptNear_cam);
    
    return Core::Rayf (Core::Ray_OriginDirection, ptNear_world, ptNear_world-_projPoint);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
OrthoViewVolume::OrthoViewVolume(const OrthoCamera& cam)
{
    cam.getWorldMatrix(_matWorld);

    _top    = cam.getTopPlane();
    _down   = cam.getDownPlane();
    _left   = cam.getLeftPlane();
    _right  = cam.getRightPlane();
    _near   = cam.getNearPlane();
    _far    = cam.getFarPlane();
}
//-----------------------------------------------------------------------------
OrthoViewVolume::~OrthoViewVolume()
{

}
//-----------------------------------------------------------------------------
Core::Rayf OrthoViewVolume::makeRayFrom2DPoint(const Core::Vector2f& pt2D) const
{
    float yCentered = ((1 - pt2D.y) - 0.5f)*2;
    float xCentered = (pt2D.x - 0.5f)*2;

    Core::Vector3f ptNear_cam;
        ptNear_cam.x = xCentered * (_right - _left) / 2.f;
        ptNear_cam.y = yCentered * (_top - _down) / 2.f;
        ptNear_cam.z = _near;

    Core::Vector3f ptNear_world = _matWorld.apply(ptNear_cam);

    return Core::Rayf (Core::Ray_OriginDirection, ptNear_world, _matWorld.getZVector());
}
//-----------------------------------------------------------------------------
} // namespace Core
