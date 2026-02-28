/*
File:   fred_api.h
Author: Taylor Robbins
Date:   02\21\2026
Description:
	** Contains the "builtin" functions that Fred implements for us
	** as well as the EditorCommands enum and types that are only
	** used for "builtin" functions
*/

#ifndef _FRED_API_H
#define _FRED_API_H

// --- fred API version info ---
// The version of this fred API.  If there is a mismatch, you should regenerate the API.
const u32 api_version = 1;

// +==============================+
// |        EditorCommands        |
// +==============================+
typedef enum EditorCommands
{
	ED_SaveRequestSave, // (.cmd) Saves the current buffer if there are changes.
	ED_SaveRequest_END = 0xF,
	ED_CloseRequestClose, // (.cmd) Closes the current editor.
	ED_CloseRequest_END = 0x1F,
	ED_RequestClipboardCopy, // (.cmd) Copies selection to clipboard.  If no selection it will copy the content line.
	ED_RequestClipboardCut, // (.cmd) Copies with the same semantics as copy to clipboard, but also removes the content.
	ED_RequestClipboardPaste, // (.cmd) Inserts the content of the system clipboard into the current buffer.
	ED_RequestClipboardPasteBeforeCursor, // (.cmd) Inserts the content of the system clipboard into the buffer.  If it is a line copy, the line is pasted before the cursor line.
	ED_RequestClipboard_END = 0x2F,
	ED_FindRequestFind, // (.cmd) Opens the 'Find' widget.
	ED_FindRequestFindWithSeed, // (.cmd) Opens the 'Find' widget but seeds the search with the current selection or word under cursor.
	ED_FindRequest_END = 0x3F,
	ED_SelectionClearSelections, // (.cmd) Clears all selections.
	ED_Selection_END = 0x4F,
	ED_NavPageUp, // (.cmd,.flags) Moves cursor up by one page.
	ED_NavPageDown, // (.cmd,.flags) Moves cursor down by one page.
	ED_NavLeft, // (.cmd,.flags) Moves cursor to the left.  If at the beginning of the line, cursor will advance to end of line above.
	ED_NavRight, // (.cmd,.flags) Moves cursor to the right.  If at the end of the line, cursor will advance to the beginning of the line below.
	ED_NavRightNoNLAdvance, // (.cmd,.flags) Moves cursor to the right.
	ED_NavLineDown, // (.cmd,.flags) Moves cursor down a line maintaining column.
	ED_NavLineUp, // (.cmd,.flags) Moves cursor up a line maintaining column.
	ED_NavCursorTopScreen, // (.cmd,.flags) Moves cursor to top of viewport.
	ED_NavCursorBottomScreen, // (.cmd,.flags) Moves cursor to bottom of viewport.
	ED_NavCursorCenterScreen, // (.cmd,.flags) Moves cursor to middle of viewport.
	ED_NavBeginningOfLine, // (.cmd,.flags) Moves cursor to beginning of the line.
	ED_NavFirstNonemptyOfLine, // (.cmd,.flags) Moves cursor to first non-whitespace character of line.
	ED_NavEndOfLine, // (.cmd,.flags) Moves cursor to end of the line.
	ED_NavContentEnd, // (.cmd,.flags) Moves cursor to the end of the content.
	ED_NavContentBeginning, // (.cmd,.flags) Moves cursor to beginning of content.
	ED_NavMatchingEncloser, // (.cmd,.flags) Snaps cursor to nearest enclosing symbol.  If at an enclosing symbol, cursor will move to opposing enclosing symbol.
	ED_NavWordRight, // (.cmd,.flags) Moves cursor one word to the right.
	ED_NavChunkRight, // (.cmd,.flags) Moves cursor one word chunk to the right.
	ED_NavWordLeft, // (.cmd,.flags) Moves cursor one word to the left.
	ED_NavChunkLeft, // (.cmd,.flags) Moves cursor one word chunk to the left.
	ED_NavRequestGotoLine, // (.cmd,.flags) Opens go to line widget.
	ED_NavCenterCameraCursor, // (.cmd,.flags) Moves the camera view such that the primary cursor is in mid screen.
	ED_NavEmptyBlockUp, // (.cmd,.flags) Moves cursor to nearest empty line above.
	ED_NavEmptyBlockDown, // (.cmd,.flags) Moves cursor to nearest empty line below.
	ED_NavCursorHistoryBack, // (.cmd,.flags) Moves cursor to its previous position.
	ED_NavCursorHistoryForward, // (.cmd,.flags) Moves cursor to its next future position.
	ED_NavMoveCursorTo, // (.cmd,.flags,.byte_offsets) Moves cursor to a specific byte position in the buffer.
	ED_Nav_END = 0x4FF,
	ED_MCDupCursorDown, // (.cmd) Creates a new multi-cursor one line below.
	ED_MCDupCursorUp, // (.cmd) Creates a new multi-cursor one line above.
	ED_MCInsertGroup, // (.cmd,.buffers) Performs a unique insert per-cursor.
	ED_MCDropCursors, // (.cmd) Deletes all multi-cursors.
	ED_MCSelectionToCursor, // (.cmd) Creates a selection at the current word.  If selection exists, creates a new multi-cursor at the next identical selection.
	ED_MCCreateCursors, // (.cmd,.byte_offsets) Creates multi-cursors at the specified byte positions.
	ED_MC_END = 0x50F,
	ED_InsInsert, // (.cmd,.buf) Inserts a buffer at the cursor.
	ED_InsReplace, // (.cmd,.buf) Inserts a buffer at cursor, overwriting characters overlapping the buffer.  Does not advance cursor.
	ED_InsOverwriteInsertBuf, // (.cmd,.buf) Inserts a buffer at cursor, overwriting characters overlapping the buffer.
	ED_InsOpenLineBelow, // (.cmd) Opens a new content line below cursor.
	ED_InsOpenLineAbove, // (.cmd) Opens a new content line above cursor.
	ED_InsTab, // (.cmd) Inserts a tap (may be expanded to spaces).
	ED_Ins_END = 0x90F,
	ED_SpecJoinLineBelow, // (.cmd) Joins the content line below with the current line at cursor.
	ED_SpecTrimLineEndings, // (.cmd) Trims trailing spaces at all lines.
	ED_SpecTab, // (.cmd) Tabs a line.
	ED_SpecUntab, // (.cmd) Untabs a line.
	ED_Spec_END = 0xA0F,
	ED_DelDeleteLine, // (.cmd) Deletes the line of content at cursor.
	ED_DelDeleteChar, // (.cmd) Deletes a single character at the cursor.  If a selection is active, the selection is deleted.
	ED_DelDeleteWord, // (.cmd) Deletes a word at the cursor.  If a selection is active, the selection is deleted.
	ED_DelDeleteChunk, // (.cmd) Deletes a word chunk at cursor.  If a selection is active, the selection is deleted.
	ED_DelBackspaceWord, // (.cmd) Deletes a word before the cursor.  If a selection is active, the selection is deleted.
	ED_DelBackspaceChunk, // (.cmd) Deletes a word chunk before the cursor.  If a selection is active, the selection is deleted.
	ED_Del_END = 0xF0F,
	ED_URTryUndo, // (.cmd) Attemps an undo operation.
	ED_URTryRedo, // (.cmd) Attemps a redo operation.
	ED_UR_END = 0xF1F,
} EditorCommands;

typedef enum EditorCommandFlags
{
	ED_FLG_None = 0, // No options.
	ED_FLG_UpdateSelection = (1 << 0), // If the cursor moves, add to the selection.
	ED_FLG_ResetCamera = (1 << 1), // Make the camera follow the cursor after command.
} EditorCommandFlags;

// +==============================+
// |        Command Types         |
// +==============================+
// Structure storing a collection of offsets into the buffer.
typedef struct EditorOffsetArray
{
	u64* array; // Pointer to offsets.
	u64 size; // Size of array.
} EditorOffsetArray;

// Structure storing a pair of buffer offset ranges.
typedef struct EditorOffsetRange
{
	u64 first_off; // Offset to the first byte.
	u64 last_off; // Last byte offset of the range (not inclusive).
} EditorOffsetRange;

// Structure storing cursor byte offsets.
typedef struct EditorCursorRange
{
	u64 cursor_off; // Byte offset of the cursor.
	EditorOffsetRange sel; // Will be non-empty if there is an active selection.
} EditorCursorRange;

// Structure storing a collection of cursor ranges.
typedef struct EditorCursorArray
{
	EditorCursorRange* array; // Pointer to cursor ranges.
	u64 size; // Size of array.
} EditorCursorArray;

// Structure storing a collection of search result ranges.
typedef struct EditorFindResults
{
	EditorOffsetRange* array; // Pointer to search result ranges.
	u64 size; // Size of array.
} EditorFindResults;

// +==============================+
// |     Editor batching API      |
// +==============================+
// Structure for working with batch edits.
typedef struct EditorBatchEdit
{
	void* pvt[2]; // Private data.
} EditorBatchEdit;

// Structure storing a collection of batch offset and string buffer.
typedef struct EditorInsertData
{
	u64 off; // Offset to the byte to insert at.
	Str8 buf; // Buffer to insert.
} EditorInsertData;

// Structure storing a collection of batch offset and string buffer.
typedef struct EditorReplaceData
{
	EditorOffsetRange range; // Range of bytes to remove and replace with 'buf'.
	Str8 buf; // Buffer to replace at 'range'.
} EditorReplaceData;

// Structure storing a collection of batch insertions.
typedef struct EditorBatchInsert
{
	EditorInsertData* array; // Pointer to insert operations.
	u64 size; // Size of array.
} EditorBatchInsert;

// Structure storing a collection of batch removals.
typedef struct EditorBatchRemove
{
	EditorOffsetRange* array; // Pointer to removal operations.
	u64 size; // Size of array.
} EditorBatchRemove;

// Structure storing a collection of batch replacements.
typedef struct EditorBatchReplace
{
	EditorReplaceData* array; // Pointer to replacement operations.
	u64 size; // Size of array.
} EditorBatchReplace;

// +==============================+
// |      Command structures      |
// +==============================+
// Structure for queuing editor commands.
typedef struct EditorCmd
{
	u32 cmd; // Command.
	u32 flags; // Flags applied to command.
	Str8 buf; // Buffer for command.
	Str8Array buffers; // Buffer collection for bulk operations.
	EditorOffsetArray byte_offsets; // Byte offsets into the buffer.
} EditorCmd;

// +==============================+
// |            Arena             |
// +==============================+
// Fetches a scratch arena that does not conflict with 'conflict'.
Arena* arena_pull_scratch(PluginManager* mgr, Arena* conflict);
// Bumps arena pointer, allocating space for the caller.
void* arena_push(Arena* a, u64 size, u64 align, u32 zero);
// Gets the arena position.
u64 arena_pos(Arena* a);
// Pops arena to specific position.
void arena_pop_to(Arena* a, u64 pos);

// +==============================+
// |         Message feed         |
// +==============================+
// Emit an info message.
void feed_queue_info_internal(Str8* strPntr);
// Emit an warning message.
void feed_queue_warning_internal(Str8* strPntr);
// Emit an error message.
void feed_queue_error_internal(Str8* strPntr);

// +==============================+
// |            Editor            |
// +==============================+
// Queries.
// Populates each cursor selection (empty or not) to the 'result' array.
void ed_cursor_selections(Arena* a, EditorCtx* ctx, Str8Array* result);
// Gets the cursors in the current editor.
void ed_cursor_ranges(Arena* a, EditorCtx* ctx, EditorCursorArray* result);
// Gets a string at the specified byte range.
void ed_string_at_range(Arena* a, EditorCtx* ctx, EditorOffsetRange* rng, Str8* result);
// Gets the line at a given byte offset in the buffer.
u64 ed_line_at_offset(EditorCtx* ctx, u64 off);
// Gets the byte range for a specific line.  Note: Lines are 1-indexed.
void ed_byte_range_at_line(EditorCtx* ctx, u64 line, EditorOffsetRange* result);
// Gets the byte range for a specific line including the newline character.  Note: Lines are 1-indexed.
void ed_byte_range_at_line_with_newline(EditorCtx* ctx, u64 line, EditorOffsetRange* result);
// Gets the number of lines for the buffer.
u64 ed_line_count(EditorCtx* ctx);
// Gets the total content length for the buffer.
u64 ed_content_byte_count(EditorCtx* ctx);
// Performs an immediate incremental search for 'str' over the current buffer.
void ed_find_all(Arena* a, EditorCtx* ctx, Str8* str, u32 ignore_case, EditorFindResults* result);
// Batching.
// Starts a new batch edit.  WARNING: You must not queue new commands until the 'ed_edit_batch_end' is called.
void ed_edit_batch_begin(EditorCtx* ctx, EditorBatchEdit* batch);
// Completes the batch and recomputes internal editor state.
void ed_edit_batch_end(EditorCtx* ctx, EditorBatchEdit* batch);
// Performs a batch insert.  After calling this, you must recompute content length for subsequent batch edits.
void ed_edit_batch_insert(EditorBatchEdit* batch, EditorBatchInsert* ops);
// Performs a batch remove.  After calling this, you must recompute content length for subsequent batch edits.
void ed_edit_batch_remove(EditorBatchEdit* batch, EditorBatchRemove* ops);
// Performs a batch range replacement.  After calling this, you must recompute content length for subsequent batch edits.
void ed_edit_batch_replace(EditorBatchEdit* batch, EditorBatchReplace* ops);
// Commands.
// Pushes a new editor command.
void ed_push_command(EditorCtx* ctx, EditorCmd* cmd);

// +==============================+
// |           Utility            |
// +==============================+
// Formats a string printf-style and allocates result to specified arena.  There is a unique specifier '%S' for Str8.
void str8_fmt_internal(Arena* a, Str8* result, const char* fmt, va_list vlst);
// Performs a lexicographic comparison on input strings.
i32 str8_compare_internal(Str8* a, Str8* b);
// Returns 1 if the input string is an integral value of the specified radix.
u32 str8_is_integer_internal(Str8* str, u32 radix);
// Converts the input string to an integral value of the specified radix.
u64 u64_from_str8_internal(Str8* str, u32 radix);
// Tries to convert the intput string to a floating-point value.  Returns 0 if the conversion failed.
u32 try_f64_from_str8_internal(Str8* str, double* result);

#endif //  _FRED_API_H
