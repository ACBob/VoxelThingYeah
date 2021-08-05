#pragma once

#include <glm/glm.hpp>

#define VEC_UP Vector(0.0f, 1.0f, 0.0f)

class Vector
{
	public:
	float x, y, z = 0.0f;

	// Default constructor, just keeps everything 0,0,0
	Vector( float x = 0, float y = 0, float z = 0 );

	// Methods
	Vector Rotate(int axis, float degrees); // Returns this vector rotated around axis (1=x,3=z) by degrees
	float Magnitude(); // Returns the length (or magnitude) of this vector
	Vector Normal(); // Returns the normal

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
	bool operator> ( Vector oVec );
	bool operator< ( Vector oVec );
	bool operator>=( Vector oVec );
	bool operator<=( Vector oVec );

	operator glm::vec3() const;
	Vector(glm::vec3 glmVec);
};