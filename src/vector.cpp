#include "vector.h"

Vector::Vector( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

// Operators
// Math, with other vectors
Vector Vector::operator+( Vector oVec )
{
	oVec.x += x, oVec.y += y, oVec.z += z;
	return oVec;
}
Vector Vector::operator-( Vector oVec )
{
	oVec.x -= x, oVec.y -= y, oVec.z -= z;
	return oVec;
}
Vector Vector::operator*( Vector oVec )
{
	oVec.x *= x, oVec.y *= y, oVec.z *= z;
	return oVec;
}
Vector Vector::operator/( Vector oVec )
{
	oVec.x /= x, oVec.y /= y, oVec.z /= z;
	return oVec;
}
// Math, with single ints/floats
Vector Vector::operator+( float i )
{
	return operator+( Vector( i, i, i ) );
}
Vector Vector::operator-( float i )
{
	return operator-( Vector( i, i, i ) );
}
Vector Vector::operator*( float i )
{
	return operator*( Vector( i, i, i ) );
}
Vector Vector::operator/( float i )
{
	return operator/( Vector( i, i, i ) );
}
// Access
float Vector::operator[](int index)
{
	switch (index)
	{
		case 1:
			return x;
		break;
		case 2:
			return y;
		break;
		case 3:
			return z;
		break;
	}
}
// Comparison
bool Vector::operator==( Vector oVec )
{
	return oVec.x == x && oVec.y == y && oVec.z == z;
}
bool Vector::operator!=( Vector oVec )
{
	return !operator==( oVec );
}
// Conversion
Vector::operator bx::Vec3() const
{
	return bx::Vec3(x,y,z);
}