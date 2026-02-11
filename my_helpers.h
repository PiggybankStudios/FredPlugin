
typedef String8 Str8;
typedef void* Arena;

#define nullptr     ((void*)0)
#define ZEROED      {0}
#define Max(A,B)    (((A)>(B))?(A):(B))
#define AlignOf(T)  __alignof(T)

static EditorCtx* CtxGlobal = nullptr;

//TODO: Can we get this from the c standard library?
size_t strlen(const char* pntr)
{
	size_t result = 0;
	while (pntr[result] != '\0') { result++; }
	return result;
}

Str8 MakeStr8(u64 length, const void* pntr)
{
	Str8 result = ZEROED;
	result.size = length;
	result.str = (char*)pntr;
	return result;
}
#define MakeStr8Nt(nullTermStr)  MakeStr8(strlen(nullTermStr), (char*)(nullTermStr))
#define StrLit(lit)              MakeStr8(sizeof(lit) - 1,     (char*)(lit))

#define AllocArrayNoZero(type, arenaPntr, numElements) (type*)arena_push((arenaPntr), sizeof(type)*(numElements), Max(8, AlignOf(type)), 0)
#define AllocArray(type, arenaPntr, numElements)       (type*)arena_push((arenaPntr), sizeof(type)*(numElements), Max(8, AlignOf(type)), 1)
#define AllocTypeNoZero(type, arenaPntr)               (type*)arena_push((arenaPntr), sizeof(type),               AlignOf(type),         0)
#define AllocType(type, arenaPntr)                     (type*)arena_push((arenaPntr), sizeof(type),               AlignOf(type),         1)

#define ScratchBegin(name)             Arena name = arena_pull_scratch(CtxGlobal->mgr, nullptr); u64 name_##Mark = arena_pos(name)
#define ScratchBegin1(name, conflict)  Arena name = arena_pull_scratch(CtxGlobal->mgr, (conflict)); u64 name_##Mark = arena_pos(name)
#define ScratchEnd(name)               arena_pop_to(name, name_##Mark)

Str8 AllocStr8(Arena arena, Str8 str)
{
	Str8 result = ZEROED;
	result.size = str.size;
	result.str = AllocArrayNoZero(char, arena, str.size);
	memcpy(result.str, str.str, str.size);
	return result;
}

Str8 PrintInArena(Arena arena, const char* formatStr, ...)
{
	Str8 result = ZEROED;
	va_list args;
	va_start(args, formatStr);
	str8_fmt_internal(arena, &result, formatStr, args);
	va_end(args);
	return result;
}

typedef enum DbgLevel
{
	DbgLevel_None = 0,
	DbgLevel_Info,
	DbgLevel_Warning,
	DbgLevel_Error,
	DbgLevel_Count,
} DbgLevel;

void NotifyAt(DbgLevel level, Str8 message)
{
	switch (level)
	{
		case DbgLevel_Info:    feed_queue_info(&message);    break;
		case DbgLevel_Warning: feed_queue_warning(&message); break;
		case DbgLevel_Error:   feed_queue_error(&message);   break;
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

#define Notify_I(messageStrLit) NotifyAt(DbgLevel_Info,    StrLit(messageStrLit))
#define Notify_W(messageStrLit) NotifyAt(DbgLevel_Warning, StrLit(messageStrLit))
#define Notify_E(messageStrLit) NotifyAt(DbgLevel_Error,   StrLit(messageStrLit))
#define NotifyPrint_I(formatStr, ...) NotifyPrintAt(DbgLevel_Info,    formatStr, ##__VA_ARGS__)
#define NotifyPrint_W(formatStr, ...) NotifyPrintAt(DbgLevel_Warning, formatStr, ##__VA_ARGS__)
#define NotifyPrint_E(formatStr, ...) NotifyPrintAt(DbgLevel_Error,   formatStr, ##__VA_ARGS__)
