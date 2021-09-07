#include "vector.hpp"
#include <math.h>

CVector::CVector( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

// Methods
CVector CVector::Rotate( int axis, float degrees ) const
{
	CVector g = CVector( x, y, z );

	degrees *= DEG2RAD;

	switch ( axis )
	{
		case 1: // X
			g.y = y * cos( degrees ) - z * sin( degrees );
			g.z = y * sin( degrees ) + z * cos( degrees );
			break;
		case 2: // Y
			g.x = x * cos( degrees ) - z * sin( degrees );
			g.z = x * sin( degrees ) + z * cos( degrees );
			break;
		case 3: // Z
			g.x = x * cos( degrees ) - y * sin( degrees );
			g.y = x * sin( degrees ) + y * cos( degrees );
			break;
	}

	return g;
}
CVector CVector::Rotate( CVector oVec ) const
{
	CVector g( x, y, z );
	for ( int i = 1; i < 3; i++ )
	{
		g = g.Rotate( i, oVec[i] );
	}
	return g;
}
float CVector::Magnitude() const { return sqrt( ( x * x ) + ( y * y ) + ( z * z ) ); }
CVector CVector::Normal() const
{
	CVector g = CVector( x, y, z );

	// Magnitude
	float mag = Magnitude();

	g.x = x / mag;
	g.y = y / mag;
	g.z = z / mag;

	return g;
}
CVector CVector::Floor() const
{
	CVector g = CVector( floor( x ), floor( y ), floor( z ) );
	g.x		 = g.x == INFINITY ? 0 : g.x;
	g.y		 = g.y == INFINITY ? 0 : g.y;
	g.z		 = g.z == INFINITY ? 0 : g.z;
	return g;
}
CVector CVector::Ceil() const
{
	CVector g = CVector( ceil( x ), ceil( y ), ceil( z ) );
	g.x		 = g.x == INFINITY ? 0 : g.x;
	g.y		 = g.y == INFINITY ? 0 : g.y;
	g.z		 = g.z == INFINITY ? 0 : g.z;
	return g;
}

// Operators
// Math, with other vectors
CVector CVector::operator+( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x += oVec.x;
	g.y += oVec.y;
	g.z += oVec.z;
	return g;
}
CVector CVector::operator-( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x -= oVec.x;
	g.y -= oVec.y;
	g.z -= oVec.z;
	return g;
}
CVector CVector::operator*( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x *= oVec.x;
	g.y *= oVec.y;
	g.z *= oVec.z;
	return g;
}
CVector CVector::operator/( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x /= oVec.x;
	g.y /= oVec.y;
	g.z /= oVec.z;
	return g;
}
// Math, with single ints/floats
CVector CVector::operator+( float i ) const { return operator+( CVector( i, i, i ) ); }
CVector CVector::operator-( float i ) const { return operator-( CVector( i, i, i ) ); }
CVector CVector::operator*( float i ) const { return operator*( CVector( i, i, i ) ); }
CVector CVector::operator/( float i ) const { return operator/( CVector( i, i, i ) ); }
// Access
float CVector::operator[]( int index )
{
	switch ( index )
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
bool CVector::operator==( CVector oVec ) const { return oVec.x == x && oVec.y == y && oVec.z == z; }
bool CVector::operator!=( CVector oVec ) const { return !operator==( oVec ); }
bool CVector::operator>( CVector oVec ) const { return Magnitude() > oVec.Magnitude(); }
bool CVector::operator<( CVector oVec ) const { return !operator>( oVec ) && !operator==( oVec ); }
bool CVector::operator>=( CVector oVec ) const { return operator==( oVec ) || operator>( oVec ); }
bool CVector::operator<=( CVector oVec ) const { return operator==( oVec ) || operator<( oVec ); }