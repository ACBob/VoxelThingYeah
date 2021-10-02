#pragma once

#include "types.hpp"

class CVector
{
  public:
	float x, y, z, w = 0.0f;

	// Default constructor, just keeps everything 0,0,0
	CVector( float x = 0, float y = 0, float z = 0, float w = 0 );

	// Methods
	CVector Rotate( int axis, float degrees ) const; // Returns this vector rotated around axis (1=x,3=z) by degrees
	CVector Rotate( CVector oVec ) const;			 // Returns this vector rotated by degrees x,y,z
	float Magnitude() const;						 // Returns the length (or magnitude) of this vector
	CVector Normal() const;							 // Returns the normal
	CVector Floor() const;							 // returns a floored vector
	CVector Ceil() const;							 // returns a ceiled vector
	CVector Lerp( CVector oVec, float perc ) const;	 // lerps
	CVector Inverse() const; // Gets the inverse vector
	CVector LookingAt(CVector oVec, CVector up = {0,1,0}) const; // Returns a vector of degrees for a rotation that would point at oVec from here
	float Dot(CVector oVec) const;

	// Operators
	// Math, with other vectors
	CVector operator+( CVector oVec ) const;
	CVector operator-( CVector oVec ) const;
	CVector operator*( CVector oVec ) const;
	CVector operator/( CVector oVec ) const;
	// Math, with single ints/floats
	CVector operator+( float i ) const;
	CVector operator-( float i ) const;
	CVector operator*( float i ) const;
	CVector operator/( float i ) const;
	// Access
	float operator[]( int index );
	// Comparison
	bool operator==( CVector oVec ) const;
	bool operator!=( CVector oVec ) const;
	bool operator>( CVector oVec ) const;
	bool operator<( CVector oVec ) const;
	bool operator>=( CVector oVec ) const;
	bool operator<=( CVector oVec ) const;
};

const CVector VEC_UP( 0.0f, 1.0f, 0.0f );
const CVector VEC_RIGHT( 1.0f, 0.0f, 0.0f );

// Trollface
typedef CVector Color;
typedef Color Colour;

#define DEG2RAD ( M_PI / 180.0 )
#define RAD2DEG ( 180.0 / M_PI )