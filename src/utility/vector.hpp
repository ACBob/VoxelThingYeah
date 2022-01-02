// -- VoxelThingYeah's vector class
#pragma once

#include <math.h>

#define DEG2RAD ( M_PI / 180.0 )
#define RAD2DEG ( 180.0 / M_PI )

// Forward declarations
template <typename T> class Vector3;
template <typename T> class Vector4;

#define DECL_INPLACE_OPERATOR(classname, op) \
	const classname<T> &operator op (const classname<T> &v); \
	const classname<T> &operator op (T v);

#define DECL_OPERATOR(classname, op) \
	const classname<T> operator op (const classname<T> &v) const; \
	const classname<T> operator op (T v) const;

#define DECL_COMPARE_OPERATOR(classname, op) \
	bool operator op (const classname<T> &v) const; \
	bool operator op (T v) const;

// Forward Decl.
template <typename T> class Vector3;
template <typename T> class Vector4;

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
using Vector3c = Vector3<char>;
using Vector3uc = Vector3<unsigned char>;
using Vector3d = Vector3<double>;

using Vector4f = Vector4<float>;
using Vector4i = Vector4<int>;
using Vector4c = Vector4<char>;
using Vector4uc = Vector4<unsigned char>;
using Vector4d = Vector4<double>;

// Generic vector class of 3 elements
// Defines the math operations
// Woah mamma, that's very verbose!
// Thanks C++ for making me do this
template <typename T> class Vector3
{
  public:
	T x, y, z;

	Vector3();
	Vector3( T i );
	Vector3( T x, T y );
	Vector3( T x, T y, T z );
	Vector3( const Vector3<T> &v );
	
	const Vector3<T> &operator=(const Vector3<T> &v);
	const Vector3<T> &operator=(T v);

	DECL_INPLACE_OPERATOR(Vector3, +=);
	DECL_INPLACE_OPERATOR(Vector3, -=);
	DECL_INPLACE_OPERATOR(Vector3, *=);
	DECL_INPLACE_OPERATOR(Vector3, /=);
	
	DECL_OPERATOR(Vector3, +);
	DECL_OPERATOR(Vector3, -);
	DECL_OPERATOR(Vector3, *);
	DECL_OPERATOR(Vector3, /);

	T Magnitude() const; // Magnitude of the vector
	T MagnitudeSq() const; // Magnitude squared

	T Dot( const Vector3<T> &v ) const; // Dot product

	Vector3<T> Cross( const Vector3<T> &v ) const; // Cross product

	Vector3<T> Normal() const; // Returns the normalized vector
	const Vector3<T> &Normalize(); // Performs the normalization in-place

	Vector3<T> Floor() const; // Returns a vector with each element rounded down to the nearest integer
	Vector3<T> Ceil() const; // Returns a vector with each element rounded up to the nearest integer

	Vector3<T> Lerp( const Vector3<T> &v, T t ) const; // Returns a vector that is the linear interpolation between this vector and v
	Vector3<T> Slerp( const Vector3<T> &v, T t ) const; // Returns a vector that is the spherical interpolation between this vector and v

	const Vector3<T> &LerpInPlace( const Vector3<T> &v, T t ); // Performs the linear interpolation in-place
	const Vector3<T> &SlerpInPlace( const Vector3<T> &v, T t ); // Performs the spherical interpolation in-place

	Vector3<T> Rotate( const Vector3<T> &v, T angle ) const; // Returns a rotated by angle around v, in radians
	const Vector3<T> &RotateInPlace( const Vector3<T> &v, T angle ); // Rotates in-place by angle around v, in radians

	T Distance( const Vector3<T> &v ) const;

	Vector3<T> LookingAt( const Vector3<T> &v, const Vector3<T> &up ) const; // Returns a vector that is looking at v from this vector
	const Vector3<T> &LookAt( const Vector3<T> &v, const Vector3<T> &up ); // Performs the look-at in-place

	// Math hurts my head
	// Copied the project/unproject from the glu library (roughly)
	// Substituted a Matrix4x4 with a Vector4<Vector4<T>>

	// Takes a point in screen space and returns a point in world space
	Vector3<T> UnProject( const Vector3<T> &v, const Vector4<Vector4<T>> &m, const Vector4<T> &viewport ) const
	{
		Vector4<T> v2 = Vector4<T>( v.x, v.y, v.z, 1.0f );
		v2			  = m * v2;
		v2 /= v2.w;
		v2 = v2 * viewport;
		return Vector3<T>( v2.x, v2.y, v2.z );
	}

	// Takes a point in world space and returns a point in screen space
	// TODO: is this correct?
	Vector3<T> Project( const Vector3<T> &v, const Vector4<Vector4<T>> &m, const Vector4<T> &viewport ) const
	{
		Vector4<T> v2 = Vector4<T>( v.x, v.y, v.z, 1.0f );
		v2			  = m * v2;
		v2 /= v2.w;
		v2 = v2 * viewport;
		return Vector3<T>( v2.x, v2.y, v2.z );
	}

	// Comparison
	DECL_COMPARE_OPERATOR(Vector3, ==);
	DECL_COMPARE_OPERATOR(Vector3, !=);
	DECL_COMPARE_OPERATOR(Vector3, <);
	DECL_COMPARE_OPERATOR(Vector3, >);
	DECL_COMPARE_OPERATOR(Vector3, <=);
	DECL_COMPARE_OPERATOR(Vector3, >=);

	// Conversion to arbitrary types
	template <typename U> operator Vector3<U>() const;

	// Conversion to Vector4
	operator Vector4<T>() const;
};

// Generic vector class of 4 elements
// (Identical to Vector3, with the addition of a w component)
template <typename T> class Vector4
{
  public:
	T x, y, z, w;

	Vector4();
	Vector4( T i );
	Vector4( T x, T y );
	Vector4( T x, T y, T z );
	Vector4( T x, T y, T z, T w );
	Vector4( const Vector4<T> &v );

	const Vector4<T> &operator=(const Vector4<T> &v);
	const Vector4<T> &operator=(T v);

	DECL_INPLACE_OPERATOR(Vector4, +=);
	DECL_INPLACE_OPERATOR(Vector4, -=);
	DECL_INPLACE_OPERATOR(Vector4, *=);
	DECL_INPLACE_OPERATOR(Vector4, /=);

	DECL_OPERATOR(Vector4, +);
	DECL_OPERATOR(Vector4, -);
	DECL_OPERATOR(Vector4, *);
	DECL_OPERATOR(Vector4, /);

	T Magnitude() const; // Magnitude of the vector
	T MagnitudeSq() const; // Magnitude squared

	T Dot( const Vector4<T> &v ) const; // Dot product

	Vector4<T> Cross( const Vector4<T> &v ) const; // Cross product

	Vector4<T> Normal() const; // Returns the normalized vector
	const Vector4<T> &Normalize(); // Performs the normalization in-place

	Vector4<T> Floor() const; // Returns a vector with each element rounded down to the nearest integer
	Vector4<T> Ceil() const; // Returns a vector with each element rounded up to the nearest integer

	Vector4<T> Lerp( const Vector4<T> &v, T t ) const; // Returns a vector that is the linear interpolation between this vector and v
	Vector4<T> Slerp( const Vector4<T> &v, T t ) const; // Returns a vector that is the spherical interpolation between this vector and v

	const Vector4<T> &LerpInPlace( const Vector4<T> &v, T t ); // Performs the linear interpolation in-place
	const Vector4<T> &SlerpInPlace( const Vector4<T> &v, T t ); // Performs the spherical interpolation in-place

	Vector4<T> Rotate( const Vector4<T> &v, T angle ) const; // Returns a rotated by angle around v, in radians
	const Vector4<T> &RotateInPlace( const Vector4<T> &v, T angle ); // Rotates in-place by angle around v, in radians

	T Distance( const Vector4<T> &v ) const;

	Vector4<T> LookingAt( const Vector4<T> &v, const Vector4<T> &up ) const; // Returns a vector that is looking at v from this vector
	const Vector4<T> &LookAt( const Vector4<T> &v, const Vector4<T> &up ); // Performs the look-at in-place

	// Sans the projection functions

	// Comparison
	DECL_COMPARE_OPERATOR(Vector4, ==);
	DECL_COMPARE_OPERATOR(Vector4, !=);
	DECL_COMPARE_OPERATOR(Vector4, <);
	DECL_COMPARE_OPERATOR(Vector4, >);
	DECL_COMPARE_OPERATOR(Vector4, <=);
	DECL_COMPARE_OPERATOR(Vector4, >=);

	// Conversion to arbitrary types
	template <typename U> operator Vector4<U>() const;

	// Conversion to Vector3
	operator Vector3<T>() const;
};