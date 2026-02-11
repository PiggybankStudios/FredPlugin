
typedef String8 Str8;
typedef void* Arena;

static EditorCtx* CtxGlobal = NULL;

size_t strlen(const char* pntr)
{
	size_t result = 0;
	while (pntr[result] != '\0') { result++; }
	return result;
}

#define MakeStr8(length, pntr)                  str8((pntr), (length))
#define MakeStr8Nt(nullTermStr)                 str8((char*)(nullTermStr), strlen(nullTermStr))
#define StrLit(lit)                             str8((char*)(lit), sizeof(lit) - 1)
#define ScratchBegin(name)                      Temp name_##Temp = arena_temp_begin(arena_pull_scratch(CtxGlobal->mgr, NULL)); Arena name = name_##Temp.arena
#define ScratchEnd(name)                        arena_temp_end(name_##Temp)
#define PrintInArena(arenaPntr, formatStr, ...) str8_fmt((arenaPntr), formatStr, ##__VA_ARGS__)

void Notify_I(const char* message)
{
	Str8 str = MakeStr8Nt(message);
	feed_queue_info(&str);
}
void Notify_W(const char* message)
{
	Str8 str = MakeStr8Nt(message);
	feed_queue_warning(&str);
}
void Notify_E(const char* message)
{
	Str8 str = MakeStr8Nt(message);
	feed_queue_error(&str);
}

void NotifyPrint_I(const char* formatStr, ...)
{
	ScratchBegin(scratch);
	Str8 message;
	va_list args;
	va_start(args, formatStr);
	str8_fmt_internal(scratch, &message, formatStr, args);
	//TODO: Check printResult
	va_end(args);
	feed_queue_info(&message);
	ScratchEnd(scratch);
}
void NotifyPrint_W(const char* formatStr, ...)
{
	ScratchBegin(scratch);
	Str8 message;
	va_list args;
	va_start(args, formatStr);
	str8_fmt_internal(scratch, &message, formatStr, args);
	//TODO: Check printResult
	va_end(args);
	feed_queue_warning(&message);
	ScratchEnd(scratch);
}
void NotifyPrint_E(const char* formatStr, ...)
{
	ScratchBegin(scratch);
	Str8 message;
	va_list args;
	va_start(args, formatStr);
	str8_fmt_internal(scratch, &message, formatStr, args);
	//TODO: Check printResult
	va_end(args);
	feed_queue_error(&message);
	ScratchEnd(scratch);
}
