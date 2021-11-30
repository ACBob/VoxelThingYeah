// Util Interface

#ifdef BUILD_UTIL
	#ifdef _WIN32
		#define DLLEXPORT __declspec(dllexport)
	#else
		#define DLLEXPORT
	#endif
	#else
	#ifdef _WIN32
		#define DLLEXPORT __declspec(dllimport)
	#else
		#define DLLEXPORT
	#endif
#endif // BUILD_UTIL

#include "vector.hpp"
#include "fastnoise.h"