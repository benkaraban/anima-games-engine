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
#ifndef CORE_TRANSFORM_H_
#define CORE_TRANSFORM_H_

#include <Core/Math/MathTools.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Rotation.h>

namespace Core
{

//-----------------------------------------------------------------------------
struct LM_API_COR Transformf
{
    MTINLINE Transformf() {}
    MTINLINE explicit Transformf(const float TX,  const float TY,  const float TZ, 
                                  const float RX,  const float RY,  const float RZ, const float RW,
                                  const float SX,  const float SY,  const float SZ,
                                  const float SOX, const float SOY, const float SOZ, const float SOW);
    MTINLINE explicit Transformf(const Vector3f& t, const Quaternionf& r, const Vector3f& s = Vector3f(ONE),
                              const Quaternionf& so = Quaternionf(IDENTITY));
    MTINLINE explicit Transformf(Identity_t);
    MTINLINE explicit Transformf(const Matrix4f& mat);
    MTINLINE explicit Transformf(const Transformf& from, const Transformf& to, const float amount);

    MTINLINE void setIdentity();

    MTINLINE void invertSelf();
    MTINLINE Transformf getInverted() const;

    MTINLINE void setLerp(const Transformf& from, const Transformf& to, const float amount);

    MTINLINE bool operator==(const Transformf& other) const;
    MTINLINE bool operator!=(const Transformf& other) const;
    MTINLINE bool isEquivalent(const Transformf& other, const float epsilon = f_COMMON_EPSILON) const;

    MTINLINE Vector3f apply(const Vector3f& vec) const;

    MTINLINE    void    moveToInGlobalAxis(const Vector3f& position);
    MTINLINE    void    translateInGlobalAxis(const Vector3f& offset);
    MTINLINE    void    translateInTransformAxis(const Vector3f& offset);

    MTINLINE    void    rotateInGlobalAxis(const Rotationf& rotation);

    Vector3f    _translation;
    Quaternionf _rotation;
    Vector3f    _scale;
    Quaternionf _scaleOrientation;
};
//-----------------------------------------------------------------------------
/*
void randomize(Transformf& transf);
void randomize(Transformf& transf, const float min, const float max);
*/
//-----------------------------------------------------------------------------
String toString(const Transformf& transf);
String toString(const Transformf& transf, int32 precision);
//-----------------------------------------------------------------------------
} //namespace Core

#ifndef _DEBUG
#include <Core/Math/Transform.inl>
#endif

#endif /*Transform_H_*/
