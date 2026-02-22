/*
File:   fred_helpers.h
Author: Taylor Robbins
Date:   02\21\2026
Description:
	** Contains functions and macros that were provided by the fred template but aren't actually external calls (thus we can change or rename them as much as we want)
*/

#ifndef _FRED_HELPERS_H
#define _FRED_HELPERS_H

Temp arena_temp_begin(void* arena)
{
	u64 pos =  arena_pos(arena);
	Temp t = { .arena = arena, .pos = pos };
	return t;
}

void arena_temp_end(Temp t)
{
	arena_pop_to(t.arena, t.pos);
}

// Helpers for walking.
#define EachIndex(it, count) (u64 it = 0; it < (count); it += 1)
#define EachLine(it, count) (u64 it = 1; it <= (count); it += 1)

#define FRED_Min(A, B) (((A)<(B))?(A):(B))
#define FRED_Max(A,B) (((A)>(B))?(A):(B))
#define FRED_AlignOf(T) __alignof(T)
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align), (0))
#define push_array_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align), (1))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, FRED_Max(8, FRED_AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, FRED_Max(8, FRED_AlignOf(T)))

// Scratch arena creation.  You pass in a 'conflict' arena when you don't want one scratch arena to overwrite another when nested functions are involved.
#define scratch_begin(conflict) arena_temp_begin(arena_pull_scratch(ctx->mgr, (conflict)))
#define scratch_end(scratch) arena_temp_end(scratch)

// Feed.
void feed_queue_info(String8 msg) { feed_queue_info_internal(&msg); }
void feed_queue_warning(String8 msg) { feed_queue_warning_internal(&msg); }
void feed_queue_error(String8 msg) { feed_queue_error_internal(&msg); }

String8 str8(char* s, u64 size)
{
	String8 r = { .str = s, .size = size };
	return r;
}

#define str8_lit(S) str8((char*)(S), sizeof(S) - 1)

String8 str8_substr_impl(String8 str, String8Slice slice)
{
	slice.off = FRED_Min(str.size, slice.off);
	slice.len = FRED_Min(str.size - slice.off, slice.len);
	str.str += slice.off;
	str.size = slice.len;
	return str;
}

#define str8_substr(str, ...) str8_substr_impl(str, (String8Slice){ .off = 0, .len = (u64)-1, __VA_ARGS__ })

String8 str8_fmt(void* a, const char* fmt, ...)
{
	String8 result = {0};
	va_list va;
	va_start(va, fmt);
	str8_fmt_internal(a, &result, fmt, va);
	va_end(va);
	return result;
}

i32 str8_compare(String8 a, String8 b)
{
	return str8_compare_internal(&a, &b);
}

String8 str8_copy(void* a, String8 str)
{
	String8 cpy = {0};
	cpy.size = str.size;
	cpy.str = push_array_no_zero(a, char, str.size);
	memcpy(cpy.str, str.str, str.size);
	return cpy;
}

// String conversions.
// Returns 1 if the input string is an integer of the desired radix.
u32 str8_is_integer(String8 str, u32 radix)
{
	return str8_is_integer_internal(&str, radix);
}

u64 u64_from_str8(String8 str, u32 radix)
{
	return u64_from_str8_internal(&str, radix);
}

// Returns 0 if the conversion failed.
u32 try_f64_from_str8(String8 str, double* result)
{
	return try_f64_from_str8_internal(&str, result);
}

// +--------------------------------------------------------------+
// |                      Plguin Hook Macro                       |
// +--------------------------------------------------------------+
// This is the 'ED' group.
#define DEF_PLUGIN_EDITOR_HOOK(ui_name, desc, fn_name)                                                                 \
	void ED_ ## fn_name ## _impl_fn(EditorCtx*);                                                                         \
	HotkeyPluginEditorHook ED_ ## fn_name = { .name = ui_name, .description = desc, .fn = &ED_ ## fn_name ## _impl_fn }; \
	void ED_ ## fn_name ## _impl_fn(EditorCtx* ctx)

#endif //  _FRED_HELPERS_H
