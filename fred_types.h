/*
File:   fred_types.h
Author: Taylor Robbins
Date:   02\21\2026
Description:
	** Contains type that are used by the fred API
*/

#ifndef _FRED_TYPES_H
#define _FRED_TYPES_H

typedef struct Temp
{
	void* arena;
	u64 pos;
} Temp;

// +==============================+
// |        EditorCommands        |
// +==============================+
typedef enum EditorCommands
{
	// Saves the current buffer if there are changes.
	ED_SaveRequestSave,
	ED_SaveRequest_END = 0xF,
	// Closes the current editor.
	ED_CloseRequestClose,
	ED_CloseRequest_END = 0x1F,
	// Copies selection to clipboard.  If no selection it will copy the content line.
	ED_RequestClipboardCopy,
	// Copies with the same semantics as copy to clipboard, but also removes the content.
	ED_RequestClipboardCut,
	// Inserts the content of the system clipboard into the current buffer.
	ED_RequestClipboardPaste,
	// Inserts the content of the system clipboard into the buffer.  If it is a line copy, the line is pasted before the cursor line.
	ED_RequestClipboardPasteBeforeCursor,
	ED_RequestClipboard_END = 0x2F,
	// Opens the 'Find' widget.
	ED_FindRequestFind,
	// Opens the 'Find' widget but seeds the search with the current selection or word under cursor.
	ED_FindRequestFindWithSeed,
	ED_FindRequest_END = 0x3F,
	// Clears all selections.
	ED_SelectionClearSelections,
	ED_Selection_END = 0x4F,
	// Moves cursor up by one page.
	ED_NavPageUp,
	// Moves cursor down by one page.
	ED_NavPageDown,
	// Moves cursor to the left.  If at the beginning of the line, cursor will advance to end of line above.
	ED_NavLeft,
	// Moves cursor to the right.  If at the end of the line, cursor will advance to the beginning of the line below.
	ED_NavRight,
	// Moves cursor to the right.
	ED_NavRightNoNLAdvance,
	// Moves cursor down a line maintaining column.
	ED_NavLineDown,
	// Moves cursor up a line maintaining column.
	ED_NavLineUp,
	// Moves cursor to top of viewport.
	ED_NavCursorTopScreen,
	// Moves cursor to bottom of viewport.
	ED_NavCursorBottomScreen,
	// Moves cursor to middle of viewport.
	ED_NavCursorCenterScreen,
	// Moves cursor to beginning of the line.
	ED_NavBeginningOfLine,
	// Moves cursor to first non-whitespace character of line.
	ED_NavFirstNonemptyOfLine,
	// Moves cursor to end of the line.
	ED_NavEndOfLine,
	// Moves cursor to the end of the content.
	ED_NavContentEnd,
	// Moves cursor to beginning of content.
	ED_NavContentBeginning,
	// Snaps cursor to nearest enclosing symbol.  If at an enclosing symbol, cursor will move to opposing enclosing symbol.
	ED_NavMatchingEncloser,
	// Moves cursor one word to the right.
	ED_NavWordRight,
	// Moves cursor one word chunk to the right.
	ED_NavChunkRight,
	// Moves cursor one word to the left.
	ED_NavWordLeft,
	// Moves cursor one word chunk to the left.
	ED_NavChunkLeft,
	// Opens go to line widget.
	ED_NavRequestGotoLine,
	// Moves the camera view such that the primary cursor is in mid screen.
	ED_NavCenterCameraCursor,
	// Moves cursor to nearest empty line above.
	ED_NavEmptyBlockUp,
	// Moves cursor to nearest empty line below.
	ED_NavEmptyBlockDown,
	// Moves cursor to its previous position.
	ED_NavCursorHistoryBack,
	// Moves cursor to its next future position.
	ED_NavCursorHistoryForward,
	// Moves cursor to a specific byte position in the buffer.
	ED_NavMoveCursorTo,
	ED_Nav_END = 0x4FF,
	// Creates a new multi-cursor one line below.
	ED_MCDupCursorDown,
	// Creates a new multi-cursor one line above.
	ED_MCDupCursorUp,
	// Performs a unique insert per-cursor.
	ED_MCInsertGroup,
	// Deletes all multi-cursors.
	ED_MCDropCursors,
	// Creates a selection at the current word.  If selection exists, creates a new multi-cursor at the next identical selection.
	ED_MCSelectionToCursor,
	ED_MC_END = 0x50F,
	// Inserts a buffer at the cursor.
	ED_InsInsert,
	// Inserts a buffer at cursor, overwriting characters overlapping the buffer.  Does not advance cursor.
	ED_InsReplace,
	// Inserts a buffer at cursor, overwriting characters overlapping the buffer.
	ED_InsOverwriteInsertBuf,
	// Opens a new content line below cursor.
	ED_InsOpenLineBelow,
	// Opens a new content line above cursor.
	ED_InsOpenLineAbove,
	// Inserts a tap (may be expanded to spaces).
	ED_InsTab,
	ED_Ins_END = 0x90F,
	// Joins the content line below with the current line at cursor.
	ED_SpecJoinLineBelow,
	// Trims trailing spaces at all lines.
	ED_SpecTrimLineEndings,
	// Tabs a line.
	ED_SpecTab,
	// Untabs a line.
	ED_SpecUntab,
	ED_Spec_END = 0xA0F,
	// Deletes the line of content at cursor.
	ED_DelDeleteLine,
	// Deletes a single character at the cursor.  If a selection is active, the selection is deleted.
	ED_DelDeleteChar,
	// Deletes a word at the cursor.  If a selection is active, the selection is deleted.
	ED_DelDeleteWord,
	// Deletes a word chunk at cursor.  If a selection is active, the selection is deleted.
	ED_DelDeleteChunk,
	// Deletes a word before the cursor.  If a selection is active, the selection is deleted.
	ED_DelBackspaceWord,
	// Deletes a word chunk before the cursor.  If a selection is active, the selection is deleted.
	ED_DelBackspaceChunk,
	ED_Del_END = 0xF0F,
	// Attemps an undo operation.
	ED_URTryUndo,
	// Attemps a redo operation.
	ED_URTryRedo,
	ED_UR_END = 0xF1F,
} EditorCommands;

typedef enum EditorCommandFlags
{
	// No options.
	ED_FLG_None = 0,
	// If the cursor moves, add to the selection.
	ED_FLG_UpdateSelection = 1 << 0,
	// Make the camera follow the cursor after command.
	ED_FLG_ResetCamera = 1 << 1,
} EditorCommandFlags;

// +==============================+
// |        Command Types         |
// +==============================+
// Structure storing a collection of offsets into the buffer.
typedef struct EditorOffsetArray
{
	// Pointer to offsets.
	u64* array;
	// Size of array.
	u64 size;
} EditorOffsetArray;

// Structure storing a pair of buffer offset ranges.
typedef struct EditorOffsetRange
{
	// Offset to the first byte.
	u64 first_off;
	// Last byte offset of the range (not inclusive).
	u64 last_off;
} EditorOffsetRange;

// Structure storing cursor byte offsets.
typedef struct EditorCursorRange
{
	// Byte offset of the cursor.
	u64 cursor_off;
	// Will be non-empty if there is an active selection.
	EditorOffsetRange sel;
} EditorCursorRange;

// Structure storing a collection of cursor ranges.
typedef struct EditorCursorArray
{
	// Pointer to cursor ranges.
	EditorCursorRange* array;
	// Size of array.
	u64 size;
} EditorCursorArray;

// Structure storing a collection of search result ranges.
typedef struct EditorFindResults
{
	// Pointer to search result ranges.
	EditorOffsetRange* array;
	// Size of array.
	u64 size;
} EditorFindResults;

// +==============================+
// |     Editor batching API      |
// +==============================+
// Structure for working with batch edits.
typedef struct EditorBatchEdit
{
	// Private data.
	void* pvt[2];
} EditorBatchEdit;

// Structure storing a collection of batch offset and string buffer.
typedef struct EditorInsertData
{
	// Offset to the byte to insert at.
	u64 off;
	// Buffer to insert.
	String8 buf;
} EditorInsertData;

// Structure storing a collection of batch offset and string buffer.
typedef struct EditorReplaceData
{
	// Range of bytes to remove and replace with 'buf'.
	EditorOffsetRange range;
	// Buffer to replace at 'range'.
	String8 buf;
} EditorReplaceData;

// Structure storing a collection of batch insertions.
typedef struct EditorBatchInsert
{
	// Pointer to insert operations.
	EditorInsertData* array;
	// Size of array.
	u64 size;
} EditorBatchInsert;

// Structure storing a collection of batch removals.
typedef struct EditorBatchRemove
{
	// Pointer to removal operations.
	EditorOffsetRange* array;
	// Size of array.
	u64 size;
} EditorBatchRemove;

// Structure storing a collection of batch replacements.
typedef struct EditorBatchReplace
{
	// Pointer to replacement operations.
	EditorReplaceData* array;
	// Size of array.
	u64 size;
} EditorBatchReplace;

// +==============================+
// |      Context structures      |
// +==============================+
// Opaque: Pointer to an allocating arena.
typedef struct Arena Arena;
// Opaque: Pointer to plugin manager instance.
typedef struct PluginManager PluginManager;
// Opaque: Pointer to editor instance.
typedef struct Editor Editor;
// Context structure for editor interactions.
typedef struct EditorCtx
{
	// Plugin manager.
	PluginManager* mgr;
	// Calling editor.
	Editor* editor;
} EditorCtx;

// +==============================+
// |      Command structures      |
// +==============================+
// Structure for queuing editor commands.
typedef struct EditorCmd
{
	// Command.
	u32 cmd;
	// Flags applied to command.
	u32 flags;
	// Buffer for command.
	String8 buf;
	// Buffer collection for bulk operations.
	String8Array buffers;
	// Byte offsets into the buffer.
	EditorOffsetArray byte_offsets;
} EditorCmd;

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
