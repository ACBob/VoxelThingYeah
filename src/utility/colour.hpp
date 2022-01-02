#include "types.hpp"
#include "vector.hpp"

#pragma once

class CColour
{
  public:
	uchar_t r, g, b, a;

	// All comments show how you would create the CColour of Baob
	// (0xb180e2ff)

	// Default constructor.
	// Creates a black colour. (of full opacity)
	CColour();
	// R/G/B/A in 0-255 range
	// i.e 177, 128, 226, 255
	CColour( uchar_t r, uchar_t g, uchar_t b, uchar_t a = 255 );
	// CColour in 0xRGBA format
	// i.e 0xb8ef
	CColour( uint16_t CColour );
	// CColour in 0xRRGGBBAA format
	// i.e 0xb180e2ff
	CColour( uint32_t CColour );

	CColour( const CColour &other );

	CColour( const Vector3c &colour );
	CColour( const Vector4c &colour );

	CColour( const Vector3f &colour );
	CColour( const Vector4f &colour );

	// Returns the CColour in 0xRRGGBBAA format
	uint32_t GetColour32b() const;
	// Returns the CColour in 0xRGBA format
	uint16_t GetColour16b() const;

	CColour operator/( CColour ) const;
	CColour operator*( CColour ) const;
	CColour operator+( CColour ) const;
	CColour operator-( CColour ) const;

	CColour operator/( int ) const;
	CColour operator*( int ) const;
	CColour operator+( int ) const;
	CColour operator-( int ) const;

	CColour &operator/=( CColour );
	CColour &operator*=( CColour );
	CColour &operator+=( CColour );
	CColour &operator-=( CColour );

	CColour &operator/=( int );
	CColour &operator*=( int );
	CColour &operator+=( int );
	CColour &operator-=( int );

	bool operator==( CColour ) const;
	bool operator!=( CColour ) const;

	operator uint16_t() const;
	operator uint32_t() const;
	operator Vector3f() const;
	operator Vector4f() const;
	operator Vector3c() const;
	operator Vector4c() const;
};

CColour RandomCColour( uchar_t rmax, uchar_t rmin, uchar_t gmax, uchar_t gmin, uchar_t bmax, uchar_t bmin,
					   uchar_t amax = 255, uchar_t amin = 255 );