#ifdef __linux__

	#include <inttypes.h>
	#include <stdint.h>
	#include <uchar.h>

#else

	#include <inttypes.h>
	#include <stdint.h>
	#include <uchar.h>

using uint = unsigned int;

#endif

using uchar_t = unsigned char;