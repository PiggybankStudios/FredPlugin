/*
File:   std_functions.h
Author: Taylor Robbins
Date:   02\21\2026
*/

#ifndef _STD_FUNCTIONS_H
#define _STD_FUNCTIONS_H

// Basic CRT protos.
void* memcpy(void* _Dst, const void* _Src, size_t _Size);

//TODO: Can we get this from the c standard library?
size_t strlen(const char* pntr)
{
	size_t result = 0;
	while (pntr[result] != '\0') { result++; }
	return result;
}

#endif //  _STD_FUNCTIONS_H
