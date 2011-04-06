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
#ifndef CORE_ROTATION_H_
#define CORE_ROTATION_H_

#include <Core/Math/MathTools.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Matrix.h>
namespace Core
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct LM_API_COR Rotationf
{
    MTINLINE explicit Rotationf(const Vector3f &rotationCenter, const Vector3f &rotationVector, const float rotationAngle);

    Vector3f				center;			// origine de la rotation
	Vector3f				vec;			// la rotation a lieu autour de ce vecteur normalise 
	float				    angle;			// angle de la rotation (radian)
    
    

    const Matrix3f&                getAxisMatrix() const;
    Vector3f                rotatePoint(const Vector3f& pt) const;

private:
    mutable bool                    isValid;
    void computeAxis() const;
    mutable Matrix3f		        axis;			// axe de la roation
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


} //namespace Core

#ifndef _DEBUG
#include <Core/Math/Rotation.inl>
#endif


#endif


