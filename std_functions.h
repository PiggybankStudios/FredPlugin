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

//Not provided by Fred but very easy to implement
size_t strlen(const char* pntr)
{
	size_t result = 0;
	while (pntr[result] != '\0') { result++; }
	return result;
}

#endif //  _STD_FUNCTIONS_H
