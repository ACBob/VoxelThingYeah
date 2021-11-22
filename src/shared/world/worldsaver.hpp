// World saver
// Saves them worlds

// Worlds are stored in ONE file, not in multiple like minecraft
// It may come to the point where worlds are one file per dimension
// And the format is simple:
/*
	Header - 4 bytes that spell "MEEG"
	Version - 1 byte 
	World name - up to 32 bytes

	Block Palette size - 1 byte
	Block Palette - Palette size pairs of (id, mapped id)
	  id - 2 bytes
	  mapped id - 2 bytes

	Chunk count - 4 bytes
	Chunk data - Array of chunks, each chunk is written as:
		Chunk position - 4 bytes for X, Y, Z
		Chunk size - 4 bytes for X, Y, Z
		Chunk data - Chunk size X*Y*Z :
		 Block ID - 2 bytes
		 Block Meta - 2 bytes
	
	Time of day - 2 bytes
*/

#define WORLDHEADER "MEEG"
#define WORLDVERSION 0x1

#pragma once

#include "../filesystem.hpp"

class CWorld; // Forward Decl.

namespace worldIO {
	bool saveWorld( CWorld *world, const char *filename );
	bool loadWorld( CWorld *world, const char *filename );
}