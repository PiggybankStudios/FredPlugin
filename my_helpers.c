/*
File:   my_helpers.c
Author: Taylor Robbins
Date:   02\27\2026
Description: 
	** Helper functions that I've made on top of what Fred provides to perform common tasks
*/

EditorCursorArray Fred_GetCursors(Arena* arena)
{
	EditorCursorArray cursors = ZEROED;
	ed_cursor_ranges(arena, Ctx, &cursors);
	return cursors;
}
