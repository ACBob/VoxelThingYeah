// Header-only assorted functions

#pragma once

#include <cstring>

inline char *FlattenCharArray(char *array[], int offset, int len, char sep=' ')
{
	int l = 0;
	for (int i=0; i < len; i++)
	{
		l += strlen(array[i + offset]);
	}
	l++;
	
	char *buf = new char[l + len];
	buf[l + len - 1] = '\0';
	for (int i = 0, j = 0; i < len; i++)
	{
		memcpy(buf + j + i, array[i + offset], strlen(array[i + offset]));
		buf[j + strlen(array[i + offset]) + i] = sep;
		j += strlen(array[i + offset]);
	}

	return buf;
}

#define DeclBaseClass(baseClass) \
	typedef baseClass BaseClass