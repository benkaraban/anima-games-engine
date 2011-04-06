#ifndef CORE_ROTATION_INL_
#define CORE_ROTATION_INL_

namespace Core
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MTINLINE Rotationf::Rotationf(const Vector3f &rotationCenter, const Vector3f &rotationVector, const float rotationAngle):
    center(rotationCenter),			
	vec(rotationVector),	
	angle(-rotationAngle),
    isValid(false)
{
}
//-----------------------------------------------------------------------------
MTINLINE void Rotationf::computeAxis() const
{
    float wx, wy, wz;
	float xx, yy, yz;
	float xy, xz, zz;
	float x2, y2, z2;
	float a, c, s, x, y, z;

    a = angle * 0.5f;
	L_sincos( a, &s, &c );

	x = vec[0] * s;
	y = vec[1] * s;
	z = vec[2] * s;

	x2 = x + x;
	y2 = y + y;
	z2 = z + z;

	xx = x * x2;
	xy = x * y2;
	xz = x * z2;

	yy = y * y2;
	yz = y * z2;
	zz = z * z2;

	wx = c * x2;
	wy = c * y2;
	wz = c * z2;

	axis[ 0 ][ 0 ] = 1.0f - ( yy + zz );
	axis[ 0 ][ 1 ] = xy - wz;
	axis[ 0 ][ 2 ] = xz + wy;

	axis[ 1 ][ 0 ] = xy + wz;
	axis[ 1 ][ 1 ] = 1.0f - ( xx + zz );
	axis[ 1 ][ 2 ] = yz - wx;

	axis[ 2 ][ 0 ] = xz - wy;
	axis[ 2 ][ 1 ] = yz + wx;
	axis[ 2 ][ 2 ] = 1.0f - ( xx + yy );

    axis.transposeSelf();
	isValid = true;
}
//-----------------------------------------------------------------------------
MTINLINE const Matrix3f& Rotationf::getAxisMatrix() const
{
    if (isValid == false) computeAxis();
    return axis;
}
//-----------------------------------------------------------------------------
MTINLINE Vector3f Rotationf::rotatePoint(const Vector3f& pt) const
{
    if (isValid==false) computeAxis();

	return (axis * (pt - center)  + center);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace Core

#endif /*CORE_ROTATION_INL_*/