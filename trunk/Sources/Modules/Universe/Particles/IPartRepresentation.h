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
#ifndef UNIVERSE_IPARTREPRESENTATION_H_
#define UNIVERSE_IPARTREPRESENTATION_H_

#include <Core/Math/Matrix.h>
#include <Core/Math/GeometryComputations.h>
#include <Universe/Particles/Particle.h>

namespace Universe
{
LM_ENUM_2(EPartRepresentation, PR_BILLBOARD, PR_GROUP);

class LM_API_UNI IPartRepresentation
{
public:
    virtual ~IPartRepresentation() {};

    virtual EPartRepresentation getType() const = 0;

    virtual void setMaxSize(int32 maxSize) = 0;
    virtual int32 getMaxSize() const = 0;

    virtual void setPartScale(float s) = 0;
    virtual float getPartScale() const = 0;

    virtual void setColorFactor(float f) = 0;
    virtual float getColorFactor() const = 0;

    virtual void setGlowFactor(float f) = 0;
    virtual float getGlowFactor() const = 0;

    virtual void setZBias(float z) = 0;
    virtual float getZBias() const = 0;

    virtual void setFogFactor(float f) = 0;
    virtual float getFogFactor() const = 0;

    virtual void update(const Core::List<Particle> & particles) = 0;
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) = 0;
    virtual void setVisible(bool flag) = 0;
    virtual void kill() = 0;
    virtual Core::AABoxf getBoundingBox() const = 0;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode) = 0;
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode) = 0;
};
}

#endif /* IPARTREPRESENTATION_H_ */
