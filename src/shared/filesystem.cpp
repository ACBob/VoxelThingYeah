#include "filesystem.hpp"

#include <cstdio>

#include "physfs.h"

#define LOG_LEVEL DEBUG
#include "seethe.h"

#include "types.hpp"

#include <cstring>

#define PRINTPHYSFSERR \
		con_error("PhysFS Error: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()))

namespace fileSystem
{

	bool Init(const char* exePath)
	{
		// Initialise PhysFS first and foremost
		if (PHYSFS_init(exePath) == 0)
		{
			PRINTPHYSFSERR;
			return false;
		}

		return true;
	}

	void UnInit()
	{
		PHYSFS_deinit();
	}

	const unsigned char* LoadFile(const char* virtualPath, int64_t& len, bool& success)
	{
		PHYSFS_File *f = PHYSFS_openRead(virtualPath);

		if (!f)
		{
			PRINTPHYSFSERR;
			PHYSFS_close(f);
			len = 0;
			success = false;
			return nullptr;
		}

		int64_t fileLen = PHYSFS_fileLength(f);
		unsigned char* buf = new unsigned char[fileLen + 1];
		success = true;

		if (PHYSFS_readBytes(f, buf, fileLen) < fileLen)
		{
			PRINTPHYSFSERR;
			PHYSFS_close(f);
			len = 0;
			success = false;
			return nullptr;
		}

		buf[fileLen] = '\0';
		len = fileLen;

		return buf;
	}

	bool Mount(const char* realPath, const char* virtualPath, bool prepend)
	{
		if (PHYSFS_mount(realPath, virtualPath, prepend ? 0 : 1) == 0)
		{
			PRINTPHYSFSERR;
			return false;
		}

		return true;
	}
}