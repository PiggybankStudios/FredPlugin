
#include "std_types.h"
#include "std_macros.h"
#include "std_functions.h"
#include "base_types.h"
#include "fred_types.h"
#include "fred_api.h"
#include "fred_helpers.h"

#include "fred_example_plugins.c"

/*
// +--------------------------------------------------------------+
// |                     Original Plugin Code                     |
// +--------------------------------------------------------------+
// Basic Types
typedef __SIZE_TYPE__    size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
typedef __WCHAR_TYPE__   wchar_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __PTRDIFF_TYPE__ intptr_t;
typedef __SIZE_TYPE__    uintptr_t;

typedef signed char        i8;
typedef unsigned char      u8;
typedef short              i16;
typedef unsigned short     u16;
typedef int                i32;
typedef unsigned           u32;
typedef long long          i64;
typedef unsigned long long u64;

#if __STDC_VERSION__ >= 201112L
typedef union { long long __ll; long double __ld; } max_align_t;
#endif

#ifndef nullptr
#define nullptr ((void*)0)
#endif

#undef offsetof
#define offsetof(type, field) __builtin_offsetof(type, field)

typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_arg __builtin_va_arg
#define va_copy __builtin_va_copy
#define va_end __builtin_va_end

// fix a buggy dependency on GCC in libio.h
typedef va_list __gnuc_va_list;
#define _VA_LIST_DEFINED

// Basic CRT protos.
void* memcpy(void* _Dst, const void* _Src, size_t _Size);

typedef enum EditorCommands
{
	ED_SaveRequestSave, // Saves the current buffer if there are changes.
	ED_SaveRequest_END = 0xF,
	ED_CloseRequestClose, // Closes the current editor.
	ED_CloseRequest_END = 0x1F,
	ED_RequestClipboardCopy, // Copies selection to clipboard.  If no selection it will copy the content line.
	ED_RequestClipboardCut, // Copies with the same semantics as copy to clipboard, but also removes the content.
	ED_RequestClipboardPaste, // Inserts the content of the system clipboard into the current buffer.
	ED_RequestClipboardPasteBeforeCursor, // Inserts the content of the system clipboard into the buffer.  If it is a line copy, the line is pasted before the cursor line.
	ED_RequestClipboard_END = 0x2F,
	ED_FindRequestFind, // Opens the 'Find' widget.
	ED_FindRequestFindWithSeed, // Opens the 'Find' widget but seeds the search with the current selection or word under cursor.
	ED_FindRequest_END = 0x3F,
	ED_SelectionClearSelections, // Clears all selections.
	ED_Selection_END = 0x4F,
	ED_NavPageUp, // Moves cursor up by one page.
	ED_NavPageDown, // Moves cursor down by one page.
	ED_NavLeft, // Moves cursor to the left.  If at the beginning of the line, cursor will advance to end of line above.
	ED_NavRight, // Moves cursor to the right.  If at the end of the line, cursor will advance to the beginning of the line below.
	ED_NavRightNoNLAdvance, // Moves cursor to the right.
	ED_NavLineDown, // Moves cursor down a line maintaining column.
	ED_NavLineUp, // Moves cursor up a line maintaining column.
	ED_NavCursorTopScreen, // Moves cursor to top of viewport.
	ED_NavCursorBottomScreen, // Moves cursor to bottom of viewport.
	ED_NavCursorCenterScreen, // Moves cursor to middle of viewport.
	ED_NavBeginningOfLine, // Moves cursor to beginning of the line.
	ED_NavFirstNonemptyOfLine, // Moves cursor to first non-whitespace character of line.
	ED_NavEndOfLine, // Moves cursor to end of the line.
	ED_NavContentEnd, // Moves cursor to the end of the content.
	ED_NavContentBeginning, // Moves cursor to beginning of content.
	ED_NavMatchingEncloser, // Snaps cursor to nearest enclosing symbol.  If at an enclosing symbol, cursor will move to opposing enclosing symbol.
	ED_NavWordRight, // Moves cursor one word to the right.
	ED_NavChunkRight, // Moves cursor one word chunk to the right.
	ED_NavWordLeft, // Moves cursor one word to the left.
	ED_NavChunkLeft, // Moves cursor one word chunk to the left.
	ED_NavRequestGotoLine, // Opens go to line widget.
	ED_Nav_END = 0x4FF,
	ED_MCDupCursorDown, // Creates a new multi-cursor one line below.
	ED_MCDupCursorUp, // Creates a new multi-cursor one line above.
	ED_MCInsertGroup, // Performs a unique insert per-cursor.
	ED_MCDropCursors, // Deletes all multi-cursors.
	ED_MC_END = 0x50F,
	ED_InsInsert, // Inserts a buffer at the cursor.
	ED_InsReplace, // Inserts a buffer at cursor, overwriting characters overlapping the buffer.  Does not advance cursor.
	ED_InsOverwriteInsertBuf, // Inserts a buffer at cursor, overwriting characters overlapping the buffer.
	ED_InsOpenLineBelow, // Opens a new content line below cursor.
	ED_InsOpenLineAbove, // Opens a new content line above cursor.
	ED_Ins_END = 0x90F,
	ED_SpecJoinLineBelow, // Joins the content line below with the current line at cursor.
	ED_Spec_END = 0xA0F,
	ED_DelDeleteLine, // Deletes the line of content at cursor.
	ED_DelDeleteChar, // Deletes a single character at the cursor.  If a selection is active, the selection is deleted.
	ED_DelDeleteWord, // Deletes a word at the cursor.  If a selection is active, the selection is deleted.
	ED_DelDeleteChunk, // Deletes a word chunk at cursor.  If a selection is active, the selection is deleted.
	ED_DelBackspaceWord, // Deletes a word before the cursor.  If a selection is active, the selection is deleted.
	ED_DelBackspaceChunk, // Deletes a word chunk before the cursor.  If a selection is active, the selection is deleted.
	ED_Del_END = 0xF0F,
	ED_URTryUndo, // Attemps an undo operation.
	ED_URTryRedo, // Attemps a redo operation.
	ED_UR_END = 0xF1F,
} EditorCommands;

typedef enum EditorCommandFlags
{
	ED_FLG_None = 0, // No options.
	ED_FLG_UpdateSelection = 1 << 0, // If the cursor moves, add to the selection.
	ED_FLG_ResetCamera = 1 << 1, // Make the camera follow the cursor after command.
} EditorCommandFlags;

// --- fred plugin API ---
// --- Infra structures ---
// Core string struct.
typedef struct String8
{
	char* str; // String pointer (null termination not necessary).
	u64 size; // Size of string (not including any null terminator).
} String8;

// Collection of strings.
typedef struct String8Array
{
	String8* strs; // Pointer to strings.
	u64 size; // Size of array.
} String8Array;

// --- Context structures ---
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

// --- Command structures ---
// Structure for queuing editor commands.
typedef struct EditorCmd
{
	u32 cmd; // Command.
	u32 flags; // Flags applied to command.
	String8 buf; // Buffer for command.
	String8Array buffers; // Buffer collection for bulk operations.
} EditorCmd;

// --- Arenas ---
// Fetches a scratch arena that does not conflict with 'conflict'.
void* arena_pull_scratch(PluginManager* mgr, void* conflict);
// Bumps arena pointer, allocating space for the caller.
void* arena_push(void* a, u64 size, u64 align, u32 zero);
// Gets the arena position.
u64 arena_pos(void* a);
// Pops arena to specific position.
void arena_pop_to(void* a, u64 pos);

// --- Message feed ---
// Emit an info message.
void feed_queue_info(String8*);
// Emit an warning message.
void feed_queue_warning(String8*);
// Emit an error message.
void feed_queue_error(String8*);

// --- Editor ---
// Pushes a new editor command.
void ed_push_command(EditorCtx*, EditorCmd*);
// Populates each cursor selection (empty or not) to the 'result' array.
void ed_cursor_selections(void* a, EditorCtx* ctx, String8Array* result);

// --- Utility ---
// Formats a string printf-style and allocates result to specified arena.  There is a unique specifier '%S' for String8.
void str8_fmt_internal(void* a, String8* result, const char* fmt, va_list vlst);

// --- fred API version info ---
// The version of this fred API.  If there is a mismatch, you should regenerate the API.
const u32 api_version = 0;

// API definition hook.
typedef void(*HotkeyPluginEditorFn)(EditorCtx*);

typedef struct HotkeyPluginEditorHook
{
	const char* name;
	const char* description;
	HotkeyPluginEditorFn fn;
} HotkeyPluginEditorHook;

// This is the 'ED' group.

#define DEF_PLUGIN_EDITOR_HOOK(ui_name, desc, fn_name)                                                                   \
	void ED_ ## fn_name ## _impl_fn(EditorCtx*);                                                                         \
	HotkeyPluginEditorHook ED_ ## fn_name = { .name = ui_name, .description = desc, .fn = &ED_ ## fn_name ## _impl_fn }; \
	void ED_ ## fn_name ## _impl_fn(EditorCtx* ctx)

// Example plugins.
#if 0
DEF_PLUGIN_EDITOR_HOOK("Says hello", "Says hello in each feed.", say_hello)
{
	String8 str = str8_lit("Hello, world!");
	feed_queue_warning(&str);
	feed_queue_error(&str);
	Temp scratch = scratch_begin(nullptr);
	String8 msg = str8_fmt(scratch.arena, "Hello! This is an int '%d'. This is a char '%c'. This is a float '%f'. This is a string '%S'", 42, 'C', 3.1415, str);
	feed_queue_info(&msg);
	scratch_end(scratch);
}

DEF_PLUGIN_EDITOR_HOOK("Message demo", "Demos a formatted message to each feed.", message_demo)
{
	Temp scratch = scratch_begin(nullptr);
	String8 str = str8_lit("String");
	String8 msg = str8_fmt(scratch.arena, "Hello! This is an int '%d'. This is a char '%c'. This is a float '%f'. This is a string '%S'", 42, 'C', 3.1415, str);
	feed_queue_info(&msg);
	feed_queue_warning(&msg);
	feed_queue_error(&msg);
	scratch_end(scratch);
}

DEF_PLUGIN_EDITOR_HOOK("Replace word with FRED", "Replaces the next word with 'FRED'", replace_w_fred)
{
	EditorCmd cmd = {0};
	cmd.cmd = ED_NavWordRight;
	cmd.flags = ED_FLG_UpdateSelection;
	ed_push_command(ctx, &cmd);
	cmd.cmd = ED_InsInsert;
	cmd.flags = 0;
	cmd.buf = str8_lit("FRED");
	ed_push_command(ctx, &cmd);
}

DEF_PLUGIN_EDITOR_HOOK("Show selections", "Emits a message for each cursor selection", show_selections)
{
	Temp scratch = scratch_begin(nullptr);
	String8Array strings;
	ed_cursor_selections(scratch.arena, ctx, &strings);
	String8 msg = {0};
	for EachIndex(i, strings.size)
	{
		String8 str = strings.strs[i];
		if (str.size != 0)
		{
			msg = str8_fmt(scratch.arena, "[%d] = '%S'", i, str);
		}
		else
		{
			msg = str8_fmt(scratch.arena, "[%d] = empty", i);
		}
		feed_queue_info(&msg);
	}
	scratch_end(scratch);
}

DEF_PLUGIN_EDITOR_HOOK("Change selection to snake case", "For each selection if it is PascalCase or camelCase, change it to snake_case", snake_case)
{
	Temp scratch = scratch_begin(nullptr);
	EditorCmd cmd = {0};
	String8Array strings;
	ed_cursor_selections(scratch.arena, ctx, &strings);
	// First, let's allocate our result array which will be inserted as a command.  The strings here will have lengths +1 for each capital letter.
	String8Array ins_buf = {0};
	ins_buf.size = strings.size;
	ins_buf.strs = push_array(scratch.arena, String8, strings.size);
	for EachIndex(i, strings.size)
	{
		String8 str = strings.strs[i];
		for EachIndex(j, str.size)
		{
			// Skip the first index because we only care about internal characters.
			if (j != 0 && str.str[j] >= 'A' && str.str[j] <= 'Z')
			{
				ins_buf.strs[i].size += 2;
			}
			else
			{
				ins_buf.strs[i].size += 1;
			}
		}
	}
	// Now we can compute the result.
	const u32 delta = 'a' - 'A';
	char ins_char = 0;
	for EachIndex(i, strings.size)
	{
		String8 str = strings.strs[i];
		ins_buf.strs[i].str = push_array_no_zero(scratch.arena, char, ins_buf.strs[i].size);
		char* r_buf = ins_buf.strs[i].str;
		for EachIndex(j, str.size)
		{
			ins_char = str.str[j];
			if (j != 0 && str.str[j] >= 'A' && str.str[j] <= 'Z')
			{
				*r_buf++ = '_';
				ins_char = str.str[j] + delta;
			}
			else
			{
					if (j == 0 && str.str[j] >= 'A' && str.str[j] <= 'Z')
					{
						ins_char = str.str[j] + delta;
					}
					else
					{
						ins_char = str.str[j];
					}
			}
			*r_buf++ = ins_char;
		}
	}
	// Now we can compose the command.
	cmd.cmd = ED_MCInsertGroup;
	cmd.buffers = ins_buf;
	ed_push_command(ctx, &cmd);
	scratch_end(scratch);
}
#endif

#include "my_helpers.h"
#include "my_commands.c"
*/
