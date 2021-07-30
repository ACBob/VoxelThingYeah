#pragma once

#include "bx/math.h"

class Vector
{
	public:
	float x, y, z = 0.0f;

	// Default constructor, just keeps everything 0,0,0
	Vector( float x = 0, float y = 0, float z = 0 );

	// Operators
	// Math, with other vectors
	Vector operator+( Vector oVec );
	Vector operator-( Vector oVec );
	Vector operator*( Vector oVec );
	Vector operator/( Vector oVec );
	// Math, with single ints/floats
	Vector operator+( float i );
	Vector operator-( float i );
	Vector operator*( float i );
	Vector operator/( float i );
	// Access
	float operator[](int index);
	// Comparison
	bool operator==( Vector oVec );
	bool operator!=( Vector oVec );

	// Conversion
	operator bx::Vec3() const;
};