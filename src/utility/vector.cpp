#include "vector.hpp"
#include "assorted.hpp"
#include <math.h>

CVector::CVector( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

// Methods
CVector CVector::Rotate( int axis, float degrees ) const
{
	// TODO: w

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
	// TODO: w

	CVector g( x, y, z );
	for ( int i = 1; i < 3; i++ )
	{
		g = g.Rotate( i, oVec[i] );
	}
	return g;
}
float CVector::Magnitude() const { return sqrt( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ); }
CVector CVector::Normal() const
{
	CVector g = CVector( x, y, z );

	// Magnitude
	float mag = Magnitude();

	g.x = x / mag;
	g.y = y / mag;
	g.z = z / mag;
	g.w = w / mag;

	return g;
}
CVector CVector::Floor() const
{
	CVector g = CVector( floor( x ), floor( y ), floor( z ) );
	g.x		  = g.x == INFINITY ? 0 : g.x;
	g.y		  = g.y == INFINITY ? 0 : g.y;
	g.z		  = g.z == INFINITY ? 0 : g.z;
	g.w		  = g.w == INFINITY ? 0 : g.w;
	return g;
}
CVector CVector::Ceil() const
{
	CVector g = CVector( ceil( x ), ceil( y ), ceil( z ) );
	g.x		  = g.x == INFINITY ? 0 : g.x;
	g.y		  = g.y == INFINITY ? 0 : g.y;
	g.z		  = g.z == INFINITY ? 0 : g.z;
	g.w		  = g.w == INFINITY ? 0 : g.w;
	return g;
}
CVector CVector::Lerp( CVector oVec, float fPc ) const
{
	CVector g = CVector( x, y, z );
	g.x		  = lerp( x, oVec.x, fPc );
	g.y		  = lerp( y, oVec.y, fPc );
	g.z		  = lerp( z, oVec.z, fPc );
	g.w		  = lerp( w, oVec.w, fPc );

	return g;
}
CVector CVector::Inverse() const
{
	CVector g = CVector(-x, -y, -z, -w);
	return g;
}
CVector CVector::LookingAt(CVector oVec, CVector up) const
{
	CVector a = {x,0,z};
	CVector b = {x,y,0};
	CVector c = {oVec.x,0,oVec.z};
	CVector d = {oVec.x,oVec.y,0};

	float d1 = acosf(a.Dot(c) / (a.Magnitude() * c.Magnitude())) * RAD2DEG;
	float d2 = acosf(b.Dot(d) / (b.Magnitude() * d.Magnitude())) * RAD2DEG;

	return {d2,d1,0};
}
float CVector::Dot(CVector oVec) const
{
	return x * oVec.x + y * oVec.y + z * oVec.z + w * oVec.w;
} 

// Operators
// Math, with other vectors
CVector CVector::operator+( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x += oVec.x;
	g.y += oVec.y;
	g.z += oVec.z;
	g.w += oVec.w;
	return g;
}
CVector CVector::operator-( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x -= oVec.x;
	g.y -= oVec.y;
	g.z -= oVec.z;
	g.w -= oVec.w;
	return g;
}
CVector CVector::operator*( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x *= oVec.x;
	g.y *= oVec.y;
	g.z *= oVec.z;
	g.w *= oVec.w;
	return g;
}
CVector CVector::operator/( CVector oVec ) const
{
	CVector g( x, y, z );
	g.x /= oVec.x;
	g.y /= oVec.y;
	g.z /= oVec.z;
	g.w /= oVec.w;
	return g;
}
// Math, with single ints/floats
CVector CVector::operator+( float i ) const { return operator+( CVector( i, i, i, i ) ); }
CVector CVector::operator-( float i ) const { return operator-( CVector( i, i, i, i ) ); }
CVector CVector::operator*( float i ) const { return operator*( CVector( i, i, i, i ) ); }
CVector CVector::operator/( float i ) const { return operator/( CVector( i, i, i, i ) ); }
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
		case 4:
			return w;
			break;
	}
}
// Comparison
bool CVector::operator==( CVector oVec ) const { return oVec.x == x && oVec.y == y && oVec.z == z && oVec.w == w; }
bool CVector::operator!=( CVector oVec ) const { return oVec.x != x || oVec.y != y || oVec.z != z || oVec.w != w; }
bool CVector::operator>( CVector oVec ) const { return Magnitude() > oVec.Magnitude(); }
bool CVector::operator<( CVector oVec ) const { return oVec > CVector{ x, y, z, w }; }
bool CVector::operator>=( CVector oVec ) const { return operator==( oVec ) || operator>( oVec ); }
bool CVector::operator<=( CVector oVec ) const { return operator==( oVec ) || operator<( oVec ); }