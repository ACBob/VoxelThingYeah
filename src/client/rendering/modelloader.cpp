#include "filesystem.hpp"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include "modelloader.hpp"

void LoadModel(Model &m, const char *fp)
{
	bool succeed;
	int64_t l;
	const unsigned char *file = fileSystem::LoadFile(fp, l, succeed);

	if (!succeed)
		return;
	
	con_info("out: %s", file);
}