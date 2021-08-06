#include "vector.h"
#include <math.h>

Vector::Vector( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

// Vector::Vector( const Vector& origin )
// {
// 	this->x = origin.x;
// 	this->y = origin.y;
// 	this->z = origin.z;
// }

#define DEG2RAD (M_PI/180.0)

// Methods
Vector Vector::Rotate(int axis, float degrees)
{
	Vector g = Vector(x,y,z);

	degrees *= DEG2RAD;

	switch(axis)
	{
		case 1: // X
			g.y = y * cos(degrees) - z * sin(degrees);
			g.z = y * sin(degrees) + z * cos(degrees);
		break;
		case 2: // Y
			g.x = x * cos(degrees) - z * sin(degrees);
			g.z = x * sin(degrees) + z * cos(degrees);
		break;
		case 3: // Z
			g.x = x * cos(degrees) - y * sin(degrees);
			g.y = x * sin(degrees) + y * cos(degrees);
		break;
	}
	
	return g;
}
float Vector::Magnitude()
{
	return sqrt((x*x) + (y*y) + (z*z));
}
Vector Vector::Normal()
{
	Vector g = Vector(x,y,z);

	// Magnitude
	float mag = Magnitude();
	
	g.x = x / mag;
	g.y = y / mag;
	g.z = z / mag;

	return g;
}

// Operators
// Math, with other vectors
Vector Vector::operator+( const Vector oVec ) const
{
	Vector g(oVec);
	g.x += x, g.y += y, g.z += z;
	return oVec;
}
Vector Vector::operator-( const Vector oVec ) const
{
	Vector g(oVec);
	g.x -= x, g.y -= y, g.z -= z;
	return oVec;
}
Vector Vector::operator*( const Vector oVec ) const
{
	Vector g(oVec);
	g.x *= x, g.y *= y, g.z *= z;
	return oVec;
}
Vector Vector::operator/( const Vector oVec ) const
{
	Vector g(oVec);
	g.x /= x, g.y /= y, g.z /= z;
	return oVec;
}
// Math, with single ints/floats
Vector Vector::operator+( const float i ) const
{
	return operator+( Vector( i, i, i ) );
}
Vector Vector::operator-( const float i ) const
{
	return operator-( Vector( i, i, i ) );
}
Vector Vector::operator*( const float i ) const
{
	return operator*( Vector( i, i, i ) );
}
Vector Vector::operator/( const float i ) const
{
	return operator/( Vector( i, i, i ) );
}
// Access
float Vector::operator[](const int index) const
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
bool Vector::operator==( const Vector oVec ) const
{
	return oVec.x == x && oVec.y == y && oVec.z == z;
}
bool Vector::operator!=( const Vector oVec ) const
{
	return !operator==( oVec );
}
bool Vector::operator>( const Vector oVec ) const
{
	return (x > oVec.x && y > oVec.y && z > oVec.z);
}
bool Vector::operator<( const Vector oVec ) const
{
	return !operator>( oVec );
}
bool Vector::operator>=( const Vector oVec ) const
{
	return operator==( oVec ) && operator>( oVec );
}
bool Vector::operator<=( const Vector oVec ) const
{
	return operator==( oVec ) && operator<( oVec );
}

Vector::operator glm::vec3() const
{
	return glm::vec3(x, y, z);
}
Vector::Vector(glm::vec3 glmVec)
{
	x = glmVec.x;
	y = glmVec.y;
	z = glmVec.z;
}