/*
File:   fred_api.h
Author: Taylor Robbins
Date:   02\21\2026
*/

#ifndef _FRED_API_H
#define _FRED_API_H

// --- fred API version info ---
// The version of this fred API.  If there is a mismatch, you should regenerate the API.
const u32 api_version = 1;

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
void feed_queue_info_internal(String8*);
// Emit an warning message.
void feed_queue_warning_internal(String8*);
// Emit an error message.
void feed_queue_error_internal(String8*);

// +==============================+
// |            Editor            |
// +==============================+
// Queries.
// Populates each cursor selection (empty or not) to the 'result' array.
void ed_cursor_selections(Arena* a, EditorCtx* ctx, String8Array* result);
// Gets the cursors in the current editor.
void ed_cursor_ranges(Arena* a, EditorCtx* ctx, EditorCursorArray* result);
// Gets a string at the specified byte range.
void ed_string_at_range(Arena* a, EditorCtx* ctx, EditorOffsetRange* rng, String8* result);
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
void ed_find_all(Arena* a, EditorCtx* ctx, String8* str, u32 ignore_case, EditorFindResults* result);
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
void ed_push_command(EditorCtx*, EditorCmd*);

// +==============================+
// |           Utility            |
// +==============================+
// Formats a string printf-style and allocates result to specified arena.  There is a unique specifier '%S' for String8.
void str8_fmt_internal(Arena* a, String8* result, const char* fmt, va_list vlst);
// Performs a lexicographic comparison on input strings.
i32 str8_compare_internal(String8* a, String8* b);
// Returns 1 if the input string is an integral value of the specified radix.
u32 str8_is_integer_internal(String8* str, u32 radix);
// Converts the input string to an integral value of the specified radix.
u64 u64_from_str8_internal(String8* str, u32 radix);
// Tries to convert the intput string to a floating-point value.  Returns 0 if the conversion failed.
u32 try_f64_from_str8_internal(String8* str, double* result);

#endif //  _FRED_API_H
