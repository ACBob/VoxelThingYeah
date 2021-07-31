#include "vector.h"
#include <math.h>

Vector::Vector( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

#define DEG2RAD (M_PI/180.0)

// Methods
Vector Vector::Rotate(int axis, float degrees)
{
	Vector g = Vector(x,y,z);

	degrees *= DEG2RAD;

	switch(axis)
	{
		case 1:
			g.y = y * cos(degrees) - z * sin(degrees);
			g.z = y * sin(degrees) + z * cos(degrees);
		break;
		case 2:
			g.x = x * cos(degrees) - z * sin(degrees);
			g.z = x * sin(degrees) + z * cos(degrees);
		break;
		case 3:
			g.x = x * cos(degrees) - y * sin(degrees);
			g.y = x * sin(degrees) + y * cos(degrees);
		break;
	}
	
	return g;
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