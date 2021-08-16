// Header-only assorted functions

#pragma once

#include <cstring>

char *FlattenCharArray(char *array[], int len, char sep=' ')
{
	int l = 0;
	for (int i=0; i < len; i++)
	{
		l += strlen(array[i]);
	}
	l++;
	
	char *buf = new char[l + len];
	buf[l + len - 1] = '\0';
	for (int i = 0, j = 0; i < len; i++)
	{
		memcpy(buf + j + i, array[i], strlen(array[i]));
		buf[j + strlen(array[i]) + i] = sep;
		j += strlen(array[i]);
	}

	return buf;
}