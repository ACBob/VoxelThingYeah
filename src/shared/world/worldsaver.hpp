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
	Block Palette - 2 * Palette size bytes,
	  id - 1 byte
	  mapped id - 1 byte

	Chunk count - 4 bytes
	Chunk data - Array of chunks, each chunk is written as:
		Chunk position - 4 bytes for X, Y, Z
		Chunk size - 4 bytes for X, Y, Z
		Chunk data - Chunk size X*Y*Z :
		 Block ID - 1 byte
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