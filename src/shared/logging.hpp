#pragma once

#include <stdio.h>
#include <time.h>

#ifdef _WIN32
	#include <windows.h>
#endif

/* Default level */
#ifndef LOG_LEVEL
	#define LOG_LEVEL DEBUG
#endif

/* CColour customization */
// Because windows is from the stone age and doesn't understand the modern day...
#ifndef _WIN32
	#define DEBUG_COLOUR ""
	#define NOCOLOUR "\x1B[0m"
	#define INFO_COLOUR "\x1B[36m"
	#define NOTICE_COLOUR "\x1B[32;1m"
	#define WARNING_COLOUR "\x1B[33m"
	#define ERROR_COLOUR "\x1B[31m"
	#define CRITICAL_COLOUR "\x1B[41;1m"

	#define LOG_DISPLAYCOLOUR( col ) printf( "%s", col )
	#define LOG_RESETCOLOUR printf( "%s", RESET_COLOUR )
#else
	#define DEBUG_COLOUR 0x0F
	#define NOCOLOUR 0x0F
	#define INFO_COLOUR 0x0B
	#define NOTICE_COLOUR 0x06
	#define WARNING_COLOUR 0x0E
	#define ERROR_COLOUR 0x0C
	#define CRITICAL_COLOUR 0x47

	#define LOG_DISPLAYCOLOUR( col ) SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), col );
	#define LOG_RESETCOLOUR SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), RESET_COLOUR );
#endif

/* Do not change this. */
#define RESET_COLOUR NOCOLOUR

/* Formatting prefs. */
#define MSG_ENDING "\n"
#define TIME_FORMAT "%H:%M:%S"
#define BORDER "-"

/* Enabler flags */
/* Debug shows a lot more */
#if LOG_LEVEL == DEBUG
	#define DISPLAY_LEVEL 1
	#define DISPLAY_FUNC 1
	#define DISPLAY_FILE 1
	#define DISPLAY_LINE 1
	#define DISPLAY_BORDER 1
	#define DISPLAY_MESSAGE 1
	#define DISPLAY_ENDING 1
	#define DISPLAY_RESET 1
#else
	#define DISPLAY_LEVEL 1
	#define DISPLAY_FUNC 0
	#define DISPLAY_FILE 0
	#define DISPLAY_LINE 0
	#define DISPLAY_BORDER 1
	#define DISPLAY_MESSAGE 1
	#define DISPLAY_ENDING 1
	#define DISPLAY_RESET 1
#endif

/* Log to screen */
#define emit_log( colour, level, file, func, line, ... )                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
                                                                                                                       \
		/* notate the time */                                                                                          \
		time_t raw_time = time( NULL );                                                                                \
		char time_buffer[80];                                                                                          \
		strftime( time_buffer, 80, TIME_FORMAT, localtime( &raw_time ) );                                              \
                                                                                                                       \
		/* enable colour */                                                                                            \
		LOG_DISPLAYCOLOUR( colour );                                                                                   \
                                                                                                                       \
		/* display the time */                                                                                         \
		printf( "%s ", time_buffer );                                                                                  \
                                                                                                                       \
		/* display the level */                                                                                        \
		printf( "%10s%s", DISPLAY_LEVEL ? level : "", DISPLAY_LEVEL ? " " : "" );                                      \
                                                                                                                       \
		/* display the function doing the logging */                                                                   \
		printf( "%s%s", DISPLAY_FUNC ? func : "", DISPLAY_FUNC ? " " : "" );                                           \
                                                                                                                       \
		/* display the file and/or the line number */                                                                  \
		printf( "%s%s%s%.d%s%s", DISPLAY_FUNC && ( DISPLAY_FILE || DISPLAY_LINE ) ? "(" : "",                          \
				DISPLAY_FILE ? file : "", DISPLAY_FILE && DISPLAY_LINE ? ":" : "", DISPLAY_LINE ? line : 0,            \
				DISPLAY_FUNC && ( DISPLAY_FILE || DISPLAY_LINE ) ? ") " : "",                                          \
				!DISPLAY_FUNC && ( DISPLAY_FILE || DISPLAY_LINE ) ? " " : "" );                                        \
                                                                                                                       \
		/* display message border */                                                                                   \
		printf( "%s%s", DISPLAY_BORDER ? BORDER : "", DISPLAY_BORDER ? " " : "" );                                     \
                                                                                                                       \
		/* display the callee's message */                                                                             \
		if ( DISPLAY_MESSAGE )                                                                                         \
			printf( __VA_ARGS__ );                                                                                     \
                                                                                                                       \
		/* add the message ending (usually '\n') */                                                                    \
		printf( "%s", DISPLAY_ENDING ? MSG_ENDING : "" );                                                              \
                                                                                                                       \
		/* reset the colour */                                                                                         \
		LOG_RESETCOLOUR;                                                                                               \
                                                                                                                       \
	} while ( 0 )

/* Level enum */
#define DEBUG 0
#define INFO 1
#define NOTICE 2
#define WARNING 3
#define ERROR 4
#define CRITICAL 5
#define SILENT 6

/* DEBUG LOG */
#if LOG_LEVEL == DEBUG
	#define con_debug( ... ) emit_log( DEBUG_COLOUR, "[DEBUG]", __FILE__, __func__, __LINE__, __VA_ARGS__ )
#else
	#define con_debug( ... )
#endif

/* INFO LOG */
#if LOG_LEVEL == DEBUG || LOG_LEVEL == INFO
	#define con_info( ... ) emit_log( INFO_COLOUR, "[INFO]", __FILE__, __func__, __LINE__, __VA_ARGS__ )
#else
	#define con_info( ... )
#endif

/* NOTICE LOG */
#if LOG_LEVEL == DEBUG || LOG_LEVEL == INFO || LOG_LEVEL == NOTICE
	#define con_notice( ... ) emit_log( NOTICE_COLOUR, "[NOTICE]", __FILE__, __func__, __LINE__, __VA_ARGS__ )
#else
	#define con_notice( ... )
#endif

/* WARNING LOG */
#if LOG_LEVEL == DEBUG || LOG_LEVEL == INFO || LOG_LEVEL == NOTICE || LOG_LEVEL == WARNING
	#define con_warning( ... ) emit_log( WARNING_COLOUR, "[WARNING]", __FILE__, __func__, __LINE__, __VA_ARGS__ )
#else
	#define con_warning( ... )
#endif

/* ERROR LOG */
#if LOG_LEVEL == DEBUG || LOG_LEVEL == INFO || LOG_LEVEL == NOTICE || LOG_LEVEL == WARNING || LOG_LEVEL == ERROR
	#define con_error( ... ) emit_log( ERROR_COLOUR, "[ERROR]", __FILE__, __func__, __LINE__, __VA_ARGS__ )
#else
	#define con_error( ... )
#endif

/* CRITICAL LOG */
#if LOG_LEVEL == DEBUG || LOG_LEVEL == INFO || LOG_LEVEL == NOTICE || LOG_LEVEL == WARNING || LOG_LEVEL == ERROR ||    \
	LOG_LEVEL == CRITICAL
	#define con_critical( ... ) emit_log( CRITICAL_COLOUR, "[CRITICAL]", __FILE__, __func__, __LINE__, __VA_ARGS__ )
#else
	#define con_critical( ... )
#endif
