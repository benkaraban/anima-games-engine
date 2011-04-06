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
#ifndef RENDERER_IRENDERVIEW_H_
#define RENDERER_IRENDERVIEW_H_

#include <Core/Standard.h>

namespace Renderer
{
struct LM_API_SM2 Screenshots
{
    Ptr<Core::Bitmap> pColor;
    Ptr<Core::Bitmap> pReflec;
    Ptr<Core::Bitmap> pRefrac;
    Ptr<Core::Bitmap> pGlow0;
    Ptr<Core::Bitmap> pGlow1;
    Ptr<Core::Bitmap> pGlow2;
    Ptr<Core::Bitmap> pGlowHalf;
};

class LM_API_RDR IRenderView
{
public:
    virtual ~IRenderView() {};

    virtual void present() = 0;

    virtual void resize(int32 width, int32 height) = 0;
    virtual int32 getWidth() const = 0;
    virtual int32 getHeight() const = 0;
    virtual Ptr<Core::Bitmap> getScreenShot() const = 0;
    virtual void getDebugScreenShots(Screenshots & shots) const = 0;
};
}

#endif
