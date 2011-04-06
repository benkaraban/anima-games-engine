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
#ifndef ABSTRACT_IFREEFORM_H_
#define ABSTRACT_IFREEFORM_H_

#include <Assets/VerticeSet.h>
#include <Core/Standard.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/GeometryComputations.h>
#include <Renderer/ITexture.h>

namespace Renderer
{
struct FreeFormVertex
{
    Core::Vector3f position;      // 12 - 12

    union
    {
        byte    color[4];         //  4 - 16
        uint32  colors32;
    };

    float  texcoords[3];          // 12 - 24

    union
    {
        byte    glow[4];          //  4 - 32
        uint32  glow32;
    };
};

LM_STATIC_ASSERT(sizeof(FreeFormVertex) == 32);

struct FreeFormData
{
    Core::List<FreeFormVertex>   vertices;
    Core::List<uint16>           indices;
};

LM_ENUM_4(EFreeFormMode,
        FREE_FORM_LERP,
        FREE_FORM_ADD,
        FREE_FORM_REFRAC,
        FREE_FORM_LIGHT_MESH);

class LM_API_RDR IFreeForm
{
public:
    virtual ~IFreeForm() {};

    virtual void setWorldMatrix(const Core::Matrix4f & matrix) = 0;
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const = 0;

    virtual void setMode(EFreeFormMode mode) = 0;
    virtual EFreeFormMode getMode() const = 0;

    virtual void setGlowFlag(bool glow) = 0;
    virtual bool getGlowFlag() const = 0;

    virtual void setMax(int32 maxVert, int32 maxIndices) = 0;
    virtual void getMax(int32 & maxVert, int32 & maxIndices) = 0;

    virtual void setPrecomputedAABox(const Core::AABoxf & box) =0;
    virtual const Core::AABoxf & getPrecomputedAABox() const = 0;

    virtual void setWorldSpaceCoords(bool flag) = 0;
    virtual bool isWorldSpaceCoords() const = 0;

    virtual void unsetPrecomputedAABox() = 0;
    virtual bool usePrecomputedAABox() const = 0;

    virtual FreeFormData & beginUpdate() = 0;
    virtual void endUpdate() = 0;

    virtual void setTexture(const Ptr<ITextureMap> & color) = 0;
    virtual Ptr<ITextureMap> getTexture() const = 0;

    virtual const Core::AABoxf & getBoundingBox() const = 0;

    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;
};
}

#endif /*ABSTRACT_IDECAL_H_*/
