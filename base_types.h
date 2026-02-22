/*
File:   base_types.h
Author: Taylor Robbins
Date:   02\21\2026
Description:
	** Contains basic typedefs that aren't really "standard" but match our own commonly used typed in PigCore
*/

#ifndef _BASE_TYPES_H
#define _BASE_TYPES_H

// --- fred plugin API ---
// --- Infra structures ---
// Core string struct.
typedef struct String8
{
	// String pointer (null termination not necessary).
	char* str;
	// Size of string (not including any null terminator).
	u64 size;
} String8;

// Collection of strings.
typedef struct String8Array
{
	// Pointer to strings.
	String8* strs;
	// Size of array.
	u64 size;
} String8Array;

typedef struct String8Slice
{
	u64 off;
	u64 len;
} String8Slice;

#endif //  _BASE_TYPES_H
