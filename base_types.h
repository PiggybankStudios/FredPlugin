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

#endif //  _FRED_TYPES_H
