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
#ifndef VIEW_VOLUME_H_
#define VIEW_VOLUME_H_

#include <Core/Standard.h>
#include <Renderer/IViewVolume.h>
#include <Renderer/Common/Camera.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
class LM_API_RDR PerspectiveViewVolume : public IViewVolume
{
public:
    PerspectiveViewVolume(const PerspectiveCamera& cam);

    virtual ~PerspectiveViewVolume();

    virtual Core::Rayf makeRayFrom2DPoint(const Core::Vector2f& pt2D) const;
    
private:
    Core::Vector3f _projPoint;  //le centre de la camera dans le repere monde
    Core::Matrix4f _matWorld;   //la matrice monde de la camera

    float _near;
    float _nearHeight;
    float _aspectRatio;
};
//-----------------------------------------------------------------------------
class LM_API_RDR OrthoViewVolume : public IViewVolume
{
public:
    OrthoViewVolume(const OrthoCamera& pCam);

    virtual ~OrthoViewVolume();

    virtual Core::Rayf makeRayFrom2DPoint(const Core::Vector2f& pt2D) const;
    
private:
    Core::Matrix4f _matWorld;   //la matrice monde de la camera
    float _top;
    float _down;
    float _left;
    float _right;
    float _near;
    float _far;
};
//-----------------------------------------------------------------------------
} //namespace Renderer
#endif VIEW_VOLUME_H_