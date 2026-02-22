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

#undef STRUCT_VAR_OFFSET
#define STRUCT_VAR_OFFSET(type, field) __builtin_offsetof(type, field)
#define ALIGNMENT_OF(type) __alignof(type)

//TODO: Do we need to be worried about compiling in C++ mode?
#define ZEROED {0}

#define Min2(A, B)  (((A) < (B)) ? (A) : (B))
#define Max2(A, B)  (((A) > (B)) ? (A) : (B))

// Helpers for walking.
#define EachIndex(it, count) (u64 it = 0; it < (count); it += 1)
#define EachLine(it, count)  (u64 it = 1; it <= (count); it += 1)

//Use a for loop to execute code at the end of a block (warning: if a break is hit inside the block then the endCode will NOT run!)
#define DeferBlockEx(uniqueName, endCode)                                 for (int uniqueName = 0; uniqueName == 0; (uniqueName = 1, (endCode)))
#define DeferBlock(endCode)                                               DeferBlockEx(DeferBlockIter, (endCode))
//startCode runs at beginning of block
#define DeferBlockWithStartEx(uniqueName, startCode, endCode)             for (int uniqueName = ((startCode), 0); uniqueName == 0; (uniqueName = 1, (endCode)))
#define DeferBlockWithStart(startCode, endCode)                           DeferBlockWithStartEx(DeferBlockIter, (startCode), (endCode))
//startCode returns bool to determine if block should run, endCode always runs
#define DeferIfBlockEx(uniqueName, startCodeAndCondition, endCode)        for (int uniqueName = 2 * !(startCodeAndCondition); (uniqueName == 2) ? ((endCode), false) : (uniqueName == 0); (uniqueName = 1, (endCode)))
#define DeferIfBlock(startCodeAndCondition, endCode)                      DeferIfBlockEx(DeferBlockIter, (startCodeAndCondition), (endCode))
//startCode returns bool to determine block should run, endCode only runs if startCode returns true
#define DeferIfBlockCondEndEx(uniqueName, startCodeAndCondition, endCode) for (int uniqueName = 1 * !(startCodeAndCondition); uniqueName == 0; (uniqueName = 1, (endCode)))
#define DeferIfBlockCondEnd(startCodeAndCondition, endCode)               DeferIfBlockCondEndEx(DeferBlockIter, (startCodeAndCondition), (endCode))

#define SetOptionalOutPntr(outputPntr, value) if ((outputPntr) != nullptr) { *(outputPntr) = (value); }

#endif //  _STD_MACROS_H
