/*
File:   std_types.h
Author: Taylor Robbins
Date:   02\21\2026
Description:
	** Contains standard typedefs
*/

#ifndef _STD_TYPES_H
#define _STD_TYPES_H

typedef __SIZE_TYPE__    size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
typedef __WCHAR_TYPE__   wchar_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __PTRDIFF_TYPE__ intptr_t;
typedef __SIZE_TYPE__    uintptr_t;

typedef signed char        i8;
typedef unsigned char      ui8;
typedef short              i16;
typedef unsigned short     ui16;
typedef int                i32;
typedef unsigned           u32;
typedef long long          i64;
typedef unsigned long long u64;

#if __STDC_VERSION__ >= 201112L
typedef union { long long __ll; long double __ld; } max_align_t;
#endif

typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_arg __builtin_va_arg
#define va_copy __builtin_va_copy
#define va_end __builtin_va_end
// fix a buggy dependency on GCC in libio.h
typedef va_list __gnuc_va_list;
#define _VA_LIST_DEFINED

#endif //  _STD_TYPES_H
