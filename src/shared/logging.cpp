#include "logging.h"

#include <stdarg.h>
#include <cstdio>

namespace log
{
	int GetColor(TEXTCOLOR color)
	{
		switch (color)
		{
			case RED:
				return 31;
			break;
			case GREEN:
				return 32;
			break;
			case YELLOW:
				return 33;
			break;
			case BLUE:
				return 34;
			break;
			case MAGENTA:
				return 35;
			break;
			
			default:
			NONE:
				return 37;
			break;
		}
	}
	void log_raw(TEXTCOLOR color, const char* fmt, va_list vlist)
	{
		char buffer[512];
		vsnprintf(buffer, 512, fmt, vlist);
		printf("\x1B" "[%dm%s\n" "\x1B[0m", GetColor(color), buffer);
	}

	// Logs based on color, always logs
	void log(TEXTCOLOR color, const char* fmt, ...)
	{
		va_list vlist;
		va_start(vlist, fmt);
		log_raw(color, fmt, vlist);
		va_end(vlist);
	}
	// Logs based on level, logs if the level is correct
	void log(LEVEL lvl, const char* fmt, ...)
	{
		// if (lvl > logLevel)
		// 	return; // Don't log above the log level
		
		va_list vlist;

		TEXTCOLOR c = NONE;
		switch (lvl)
		{
			case INFO:
			default:
			break;

			case WARN:
				c = YELLOW;
			break;
			case ERRR:
				c = RED;
			break;
			case DBUG:
				c = MAGENTA;
			break;
		}

		va_start(vlist, fmt);
		log(c, fmt, vlist);
		va_end(vlist);
	}
}