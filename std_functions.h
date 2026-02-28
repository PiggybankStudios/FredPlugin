/*
File:   std_functions.h
Author: Taylor Robbins
Date:   02\21\2026
*/

#ifndef _STD_FUNCTIONS_H
#define _STD_FUNCTIONS_H

// Basic CRT protos.
int memcmp(const void* _Buf1, const void* _Buf2, size_t _Size);
void* memcpy(void* _Dst, const void* _Src, size_t _Size);
void* memset(void* _Dst, int _Val, size_t _Size);

//TODO: Can we get this from the c standard library?
int memcmp(const void * ptr1, const void * ptr2, size_t num)
{
	for (size_t bIndex = 0; bIndex < num; bIndex++)
	{
		if (((unsigned char*)ptr1)[bIndex] > ((unsigned char*)ptr2)[bIndex]) { return 1; }
		if (((unsigned char*)ptr1)[bIndex] < ((unsigned char*)ptr2)[bIndex]) { return -1; }
	}
	return 0;
}

//TODO: Can we get this from the c standard library?
size_t strlen(const char* pntr)
{
	size_t result = 0;
	while (pntr[result] != '\0') { result++; }
	return result;
}

#endif //  _STD_FUNCTIONS_H
