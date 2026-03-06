/*
File:   str8_api.h
Author: Taylor Robbins
Date:   03\06\2026
*/

#ifndef _STR_API_8_H
#define _STR_API_8_H

Str8 AllocStr8(Arena* arena, Str8 str)
{
	Str8 cpy = ZEROED;
	cpy.size = str.size;
	cpy.str = AllocArrayNoZero(char, arena, str.size);
	memcpy(cpy.str, str.str, str.size);
	return cpy;
}

Str8 JoinStringsInArena(Arena* arena, Str8 left, Str8 right)
{
	Str8 result = Str8_Empty;
	result.size = left.size + right.size;
	if (result.size == 0) { return result; }
	result.str = AllocArray(char, arena, result.size);
	if (left.size > 0) { memcpy(&result.str[0], left.str, left.size); }
	if (right.size > 0) { memcpy(&result.str[left.size], right.str, right.size); }
	return result;
}

Str8 JoinStringsInArena3(Arena* arena, Str8 left, Str8 middle, Str8 right)
{
	Str8 result = Str8_Empty;
	result.size = left.size + middle.size + right.size;
	if (result.size == 0) { return result; }
	result.str = AllocArray(char, arena, result.size);
	if (left.size > 0) { memcpy(&result.str[0], left.str, left.size); }
	if (middle.size > 0) { memcpy(&result.str[left.size], middle.str, middle.size); }
	if (right.size > 0) { memcpy(&result.str[left.size + middle.size], right.str, right.size); }
	return result;
}

Str8 JoinStringsInArenaWithChar(Arena* arena, Str8 left, char sepChar, Str8 right)
{
	return JoinStringsInArena3(arena, left, MakeStr8(1, &sepChar), right);
}

#endif //  _STR_API_8_H
