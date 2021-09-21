// Header-only assorted functions

#pragma once

#include <cstring>

#include "vector.hpp"

#include <math.h>

inline char *FlattenCharArray( char *array[], int offset, int len, char sep = ' ' )
{
	int l = 0;
	for ( int i = 0; i < len; i++ )
	{
		l += strlen( array[i + offset] );
	}
	l++;

	char *buf		 = new char[l + len];
	buf[l + len - 1] = '\0';
	for ( int i = 0, j = 0; i < len; i++ )
	{
		memcpy( buf + j + i, array[i + offset], strlen( array[i + offset] ) );
		buf[j + strlen( array[i + offset] ) + i] = sep;
		j += strlen( array[i + offset] );
	}

	return buf;
}

#define DeclBaseClass( baseClass ) typedef baseClass BaseClass

inline void i1Dto3D( int i, int sizeX, int sizeY, int &x, int &y, int &z )
{
	// z = round(i / (CHUNKSIZE_X * CHUNKSIZE_Y));
	// y = round((i - z * CHUNKSIZE_X * CHUNKSIZE_Y) / CHUNKSIZE_X);
	// x = i - CHUNKSIZE_X * (y + CHUNKSIZE_Y * z)

	z = round( i / ( sizeX * sizeY ) );
	y = round( ( i - z * sizeX * sizeY ) / sizeX );
	x = i - sizeX * ( y + sizeY * z );
}

inline float lerp(float a, float b, float t)
{
	return (1.0f - t) * a + t*b;
}