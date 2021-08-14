#pragma once

namespace log
{
	enum TEXTCOLOR
	{
		NONE, // WHITE
		RED,
		GREEN,
		BLUE,
		YELLOW,
		MAGENTA
	};

	enum LEVEL
	{
		INFO,
		WARN,
		ERRR,
		DBUG
	};

	// Logs based on color, always logs
	void log(TEXTCOLOR color, const char* fmt, ...);
	// Logs based on level, logs if the level is correct
	void log(LEVEL lvl, const char* fmt, ...);
}