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
#include <algorithm>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Orientation.h>
#include <Core/Math/Matrix.h>

#ifdef _DEBUG
#include <Core/Math/Orientation.inl>
#endif

namespace Core
{
    float L_atan2_safe(float val1, float val2)
    {
        if (L_abs(val1) < 0.005f || L_abs(val2) < 0.005f)
        {
            return 0.f;
        }
        
        return L_atan2(val1, val2);
    }
//-----------------------------------------------------------------------------
void Orientationf::setOrientations(const Quaternionf& q)
{
    Quaternionf quat = q;

    quat.normalizeSafe();


    float test = quat._v.x*quat._v.y + quat._v.z*quat._w;

	if (test > 0.499f)
    { // singularity at north pole
		_roll = 2 * L_atan2_safe(quat._v.x,quat._w);
		_yaw = f_PI/2;
		_pitch = 0;
		return;
	}
	if (test < -0.499f)
    { // singularity at south pole
		_roll = -2 * L_atan2_safe(quat._v.x,quat._w);
		_yaw = - f_PI/2;
		_pitch = 0;
		return;
	}

    float sqx = quat._v.x*quat._v.x;
    float sqy = quat._v.y*quat._v.y;
    float sqz = quat._v.z*quat._v.z;

    _roll  = L_atan2_safe(2*quat._v.y*quat._w-2*quat._v.x*quat._v.z , 1 - 2*sqy - 2*sqz);
	_yaw   = L_asin(2*test);
	_pitch = L_atan2_safe(2*quat._v.x*quat._w-2*quat._v.y*quat._v.z , 1 - 2*sqx - 2*sqz);
}

//-----------------------------------------------------------------------------
void Orientationf::setOrientations(const Matrix3f& mat)
{
    float sp = mat._20;

    // cap off our sin value so that we don't get any NANs
	if ( sp > 1.0f ) {
		sp = 1.0f;
	} else if ( sp < -1.0f ) {
		sp = -1.0f;
	}

    float theta = -asin( sp );
	float cp = cos( theta );

    if ( cp > 8192.0f * 1.192092896e-07f ) {
		_yaw	=  theta ;
		_roll		=  atan2( mat._10, mat._00 ) ;
		_pitch		=  atan2( mat._21, mat._22 ) ;
	} else {
		_yaw	=  theta ;
		_pitch		=   -atan2( mat._01, mat._11 ) ;
		_roll		= 0;
	}
}
//-----------------------------------------------------------------------------
String toString(const Orientationf& ang)
{
    String result;

    result<<L"[";

    result << L"Yaw : " << Core::toString(ang._yaw) << L", ";
    result << L"Pitch : " << Core::toString(ang._pitch) << L", ";
    result << L"Roll : " << Core::toString(ang._roll);

    result<<L"]";
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Orientationf& ang, int32 precision)
{
    String result;

    result<<L"[";

    result << L"Yaw : " << Core::toString(ang._yaw, precision) << L", ";
    result << L"Pitch : " << Core::toString(ang._pitch, precision) << L", ";
    result << L"Roll : " << Core::toString(ang._roll, precision);

    result<<L"]";
    return result;
}
//-----------------------------------------------------------------------------
} //namespace Core
