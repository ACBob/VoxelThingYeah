// CColours
// Holds stuff like biome and dyes

#include "types.hpp"

#pragma once

// 16 colours for base dyes
#define DYE_COLOURS 16
static const uint16_t DyePalette[DYE_COLOURS]{
	0x000F, // 0 Black
	0x555F, // 1 Dark Grey
	0xAAAF, // 2 Grey
	0xFFFF, // 3 White
	0x00AF, // 4 Blue
	0x55FF, // 5 Light Blue
	0x0A0F, // 6 Green
	0x5F5F, // 7 Lime Green
	0x0AAF, // 8 Cyan
	0x5FFF, // 9 Turqoise [Translate to Aqua Marine in the danish localisation >:)]
	0xA00F, // 10 Red
	0xF55F, // 11 Pink
	0xA0AF, // 12 Purple
	0xF5FF, // 13 Magenta
	0xA50F, // 14 Brown
	0xFF5F	// 15 Yellow
};