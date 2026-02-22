/*
File:   std_macros.h
Author: Taylor Robbins
Date:   02\21\2026
*/

#ifndef _STD_MACROS_H
#define _STD_MACROS_H

#ifndef nullptr
#define nullptr ((void*)0)
#endif

#undef offsetof
#define offsetof(type, field) __builtin_offsetof(type, field)

#endif //  _STD_MACROS_H
