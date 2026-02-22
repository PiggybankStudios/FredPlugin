/*
File:   fred_types.h
Author: Taylor Robbins
Date:   02\21\2026
Description:
	** Contains type that are used by the fred API
*/

#ifndef _FRED_TYPES_H
#define _FRED_TYPES_H

// Opaque: Pointer to an allocating arena.
typedef struct Arena Arena;
// Opaque: Pointer to plugin manager instance.
typedef struct PluginManager PluginManager;
// Opaque: Pointer to editor instance.
typedef struct Editor Editor;
// Context structure for editor interactions.
typedef struct EditorCtx
{
	PluginManager* mgr; // Plugin manager.
	Editor* editor; // Calling editor.
} EditorCtx;

typedef struct ArenaAndMark
{
	Arena* arena;
	u64 pos;
} ArenaAndMark;

typedef enum DbgLevel
{
	DbgLevel_None = 0,
	DbgLevel_Info,
	DbgLevel_Warning,
	DbgLevel_Error,
	DbgLevel_Count,
} DbgLevel;

const char* GetDbgLevelStr(DbgLevel enumValue)
{
	switch (enumValue)
	{
		case DbgLevel_None:    return "None";
		case DbgLevel_Info:    return "Info";
		case DbgLevel_Warning: return "Warning";
		case DbgLevel_Error:   return "Error";
		default: return "Unknown";
	}
}

// +==============================+
// |     API definition hook.     |
// +==============================+
typedef void(*HotkeyPluginEditorFn)(EditorCtx*);

typedef struct HotkeyPluginEditorHook
{
	const char* name;
	const char* description;
	HotkeyPluginEditorFn fn;
} HotkeyPluginEditorHook;

// +==============================+
// |         String Type          |
// +==============================+
// Core string struct.
typedef struct Str8
{
	char* str; // String pointer (null termination not necessary).
	u64 size; // Size of string (not including any null terminator).
} Str8;

// Collection of strings.
typedef struct Str8Array
{
	Str8* strs; // Pointer to strings.
	u64 size; // Size of array.
} Str8Array;

// +--------------------------------------------------------------+
// |                   Helper Functions/Macros                    |
// +--------------------------------------------------------------+
Str8 MakeStr8(u64 length, void* pntr)
{
	Str8 r = { .str = pntr, .size = length };
	return r;
}
#define StrLit(stringLiteral) MakeStr8(sizeof(stringLiteral)-1, (char*)(stringLiteral))
#define MakeStr8Nt(nullTermStr)  MakeStr8(strlen(nullTermStr), (char*)(nullTermStr))
#define Str8_Empty MakeStr8(0, nullptr)

typedef struct Str8SliceParams
{
	u64 off;
	u64 len;
} Str8SliceParams;
Str8 _StrSlice(Str8 str, Str8SliceParams params)
{
	params.off = Min2(str.size, params.off);
	params.len = Min2(str.size - params.off, params.len);
	str.str += params.off;
	str.size = params.len;
	return str;
}
#define StrSliceEx(str, ...)                _StrSlice((str), (Str8SliceParams){ .off = 0, .len = (u64)-1, __VA_ARGS__ })
#define StrSlice(str, startIndex, endIndex) _StrSlice((str), (Str8SliceParams){ .off = (startIndex), .len = (endIndex) })
#define StrSliceFrom(str, startIndex)       _StrSlice((str), (Str8SliceParams){ .off = (startIndex), .len = (u64)-1 })

#endif //  _FRED_TYPES_H
