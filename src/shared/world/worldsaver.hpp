// World saver
// Saves them worlds

// Worlds are stored in ONE file, not in multiple like minecraft
// It may come to the point where worlds are one file per dimension
// And the format is simple:
/*
	Header - 4 bytes that spell "MEEG"
	Version - 1 byte 
	World name - up to 32 bytes

	Block Palette size - 4 bytes
	Block Palette, defines what id means what block (this is ignored for now, but still written)

	Chunk count - 4 bytes
	Chunk data - Array of chunks, each chunk is written as:
		Chunk position - 4 bytes for X, Y, Z
		Chunk size - 4 bytes for X, Y, Z
		Chunk data - Chunk size X*Y*Z bytes
	
	Time of day - 4 bytes
*/

#define WORLDHEADER "MEEG"
#define WORLDVERSION 0x01

#pragma once

#include "../filesystem.hpp"

class CWorld; // Forward Decl.

namespace worldIO {
	bool saveWorld( CWorld *world, const char *filename );
	bool loadWorld( CWorld *world, const char *filename );
}