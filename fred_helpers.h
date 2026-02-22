/*
File:   fred_helpers.h
Author: Taylor Robbins
Date:   02\21\2026
Description:
	** Contains functions and macros that were provided by the fred template
	** but aren't actually external calls (thus we can change or rename them as much as we want)
*/

#ifndef _FRED_HELPERS_H
#define _FRED_HELPERS_H

//This variable should get filled out at the beginning (and cleared at the end) of each plugin hook function so that macros like ScratchBegin don't need the context as a parameter
EditorCtx* CtxGlobal = nullptr;
#define WithContext(context) DeferBlockWithStart(CtxGlobal = (context), CtxGlobal = nullptr)

#define AllocArrayNoZero(type, arenaPntr, numElements) (type*)arena_push((arenaPntr), sizeof(type)*(numElements), Max2(8, ALIGNMENT_OF(type)), 0)
#define AllocArray(type, arenaPntr, numElements)       (type*)arena_push((arenaPntr), sizeof(type)*(numElements), Max2(8, ALIGNMENT_OF(type)), 1)
#define AllocTypeNoZero(type, arenaPntr)               (type*)arena_push((arenaPntr), sizeof(type),                       ALIGNMENT_OF(type),  0)
#define AllocType(type, arenaPntr)                     (type*)arena_push((arenaPntr), sizeof(type),                       ALIGNMENT_OF(type),  1)

#define ScratchBegin(name)             Arena* name = arena_pull_scratch(CtxGlobal->mgr, nullptr); u64 name_##Mark = arena_pos(name)
#define ScratchBegin1(name, conflict)  Arena* name = arena_pull_scratch(CtxGlobal->mgr, (conflict)); u64 name_##Mark = arena_pos(name)
#define ScratchEnd(name)               arena_pop_to(name, name_##Mark)

Str8 PrintInArena(Arena* arena, const char* formatStr, ...)
{
	Str8 result = ZEROED;
	va_list args;
	va_start(args, fmt);
	str8_fmt_internal(arena, &result, formatStr, args);
	va_end(args);
	return result;
}

void NotifyAt(DbgLevel level, Str8 message)
{
	switch (level)
	{
		case DbgLevel_Info:    feed_queue_info_internal(&message);    break;
		case DbgLevel_Warning: feed_queue_warning_internal(&message); break;
		case DbgLevel_Error:   feed_queue_error_internal(&message);   break;
	}
}
void NotifyPrintAt(DbgLevel level, const char* formatStr, ...)
{
	ScratchBegin(scratch);
	Str8 message;
	va_list args;
	va_start(args, formatStr);
	str8_fmt_internal(scratch, &message, formatStr, args);
	va_end(args);
	NotifyAt(level, message);
	ScratchEnd(scratch);
}

#define Notify_I(messageStrLit)       NotifyAt(DbgLevel_Info,    StrLit(messageStrLit))
#define Notify_W(messageStrLit)       NotifyAt(DbgLevel_Warning, StrLit(messageStrLit))
#define Notify_E(messageStrLit)       NotifyAt(DbgLevel_Error,   StrLit(messageStrLit))
#define NotifyPrint_I(formatStr, ...) NotifyPrintAt(DbgLevel_Info,    formatStr, ##__VA_ARGS__)
#define NotifyPrint_W(formatStr, ...) NotifyPrintAt(DbgLevel_Warning, formatStr, ##__VA_ARGS__)
#define NotifyPrint_E(formatStr, ...) NotifyPrintAt(DbgLevel_Error,   formatStr, ##__VA_ARGS__)


Str8 AllocStr8(Arena* arena, Str8 str)
{
	Str8 cpy = ZEROED;
	cpy.size = str.size;
	cpy.str = AllocArrayNoZero(char, arena, str.size);
	memcpy(cpy.str, str.str, str.size);
	return cpy;
}

i32 str8_compare(Str8 a, Str8 b) { return str8_compare_internal(&a, &b); }
// String conversions.
// Returns 1 if the input string is an integer of the desired radix.
u32 str8_is_integer(Str8 str, u32 radix) { return str8_is_integer_internal(&str, radix); }
u64 u64_from_str8(Str8 str, u32 radix) { return u64_from_str8_internal(&str, radix); }
// Returns 0 if the conversion failed.
u32 try_f64_from_str8(Str8 str, double* result) { return try_f64_from_str8_internal(&str, result); }

// +--------------------------------------------------------------+
// |                      Plguin Hook Macro                       |
// +--------------------------------------------------------------+
// This is the 'ED' group.
#define DEF_PLUGIN_EDITOR_HOOK(ui_name, desc, fn_name)                                                                 \
	void ED_ ## fn_name ## _impl_fn(EditorCtx*);                                                                         \
	HotkeyPluginEditorHook ED_ ## fn_name = { .name = ui_name, .description = desc, .fn = &ED_ ## fn_name ## _impl_fn }; \
	void ED_ ## fn_name ## _impl_fn(EditorCtx* ctx)

#endif //  _FRED_HELPERS_H
