
// +==============================+
// |            taylor            |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Run 'Taylor' Command", "A test command for experimenting with plugin code in Fred", taylor) //Ctrl+Tilde
{
	WithContext(ctx)
	{
		ScratchBegin(scratch);
		NotifyPrint_W("Context pntr: %p", ctx);
		
		EditorCursorArray cursors = ZEROED;
		ed_cursor_ranges(scratch, Ctx, &cursors);
		for (u64 cIndex = 0; cIndex < cursors.size; cIndex++)
		{
			Str8 cursorContents = ZEROED;
			ed_string_at_range(scratch, Ctx, &cursors.array[cIndex].sel, &cursorContents);
			NotifyPrint_W("Cursor[%llu]: \"%S\"", cIndex, cursorContents);
		}
		
		ScratchEnd(scratch);
	}
}

// +==============================+
// |          kill_line           |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Kill Line(s)", "Removes the entire line that the cursor is on (works with multi-cursor)", kill_line) //Ctrl+R
{
	WithContext(ctx)
	{
		EditorCmd command = ZEROED;
		command.cmd = ED_DelDeleteLine;
		ed_push_command(ctx, &command);
	}
}

// +==============================+
// |       insert_nums_0/1        |
// +==============================+
void InsertNums(i64 start, i64 increment)
{
	ScratchBegin(scratch);
	
	EditorCursorArray cursors = ZEROED;
	ed_cursor_ranges(scratch, Ctx, &cursors);
	if (cursors.size == 0) { return; }
	
	EditorBatchReplace batchReplace = ZEROED;
	batchReplace.size = cursors.size;
	batchReplace.array = AllocArray(EditorReplaceData, scratch, batchReplace.size);
	
	u64 currentNumber = start;
	for (u64 cIndex = 0; cIndex < cursors.size; cIndex++)
	{
		EditorCursorRange cursor = cursors.array[cIndex];
		
		// EditorOffsetRange cursorOffsetRange = { .first_off = cursor.sel.first_off, .last_off = cursor.sel.last_off };
		// Str8 cursorContents = ZEROED;
		// ed_string_at_range(scratch, Ctx, &cursorOffsetRange, &cursorContents);
		// NotifyPrint_W("Cursor[%llu]: \"%S\"", cIndex, cursorContents);
		
		batchReplace.array[cIndex].range = cursor.sel; // (EditorOffsetRange){ .first_off = cursor.sel.first_off, .last_off = cursor.sel.last_off };
		batchReplace.array[cIndex].buf = PrintInArena(scratch, "%llu", currentNumber);
		currentNumber += increment;
	}
	
	EditorBatchEdit batchEdit = ZEROED;
	ed_edit_batch_begin(Ctx, &batchEdit);
	ed_edit_batch_replace(&batchEdit, &batchReplace);
	ed_edit_batch_end(Ctx, &batchEdit);
	
	ScratchEnd(scratch);
}
DEF_PLUGIN_EDITOR_HOOK("Insert numbers (from 0)", "Inserts ascending numbers (starting at 0) for each multi-cursor", insert_nums_0) //Ctrl+Alt+N
{
	WithContext(ctx) { InsertNums(0, 1); }
}
DEF_PLUGIN_EDITOR_HOOK("Insert numbers (from 1)", "Inserts ascending numbers (starting at 1) for each multi-cursor", insert_nums_1) //Ctrl+Alt+Shift+N
{
	WithContext(ctx) { InsertNums(1, 1); }
}

// +==============================+
// |        align_cursors         |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Align cursors", "Inserts spaces at each cursor until all cursors are aligned vertically", align_cursors) //Alt+A
{
	WithContext(ctx)
	{
		ScratchBegin(scratch);
		EditorCursorArray cursors = ZEROED;
		ed_cursor_ranges(scratch, Ctx, &cursors);
		if (cursors.size <= 1) { Ctx = nullptr; return; }
		
		u64 maxColumn = 0;
		for (u64 cIndex = 0; cIndex < cursors.size; cIndex++)
		{
			EditorCursorRange cursor = cursors.array[cIndex];
			u64 minOffset = Min2(cursor.sel.first_off, cursor.sel.last_off);
			u64 cursorLine = ed_line_at_offset(Ctx, minOffset);
			EditorOffsetRange lineRange = ZEROED;
			ed_byte_range_at_line(Ctx, cursorLine, &lineRange);
			//TODO: Eventually we should consider tab characters and the current tab width in order to calculate a more accurate cursorColumn value
			u64 cursorColumn = minOffset - Min2(lineRange.first_off, lineRange.last_off);
			// NotifyPrint_I("Cursor[%llu] is on line %llu column %llu", cIndex, cursorLine, cursorColumn);
			maxColumn = Max2(maxColumn, cursorColumn);
		}
		
		EditorBatchReplace batchReplace = ZEROED;
		batchReplace.size = cursors.size;
		batchReplace.array = AllocArray(EditorReplaceData, scratch, batchReplace.size);
		
		for (u64 cIndex = 0; cIndex < cursors.size; cIndex++)
		{
			EditorCursorRange cursor = cursors.array[cIndex];
			u64 minOffset = Min2(cursor.sel.first_off, cursor.sel.last_off);
			u64 cursorLine = ed_line_at_offset(Ctx, minOffset);
			EditorOffsetRange lineRange = ZEROED;
			ed_byte_range_at_line(Ctx, cursorLine, &lineRange);
			//TODO: Eventually we should consider tab characters and the current tab width in order to calculate a more accurate cursorColumn value
			u64 cursorColumn = minOffset - Min2(lineRange.first_off, lineRange.last_off);
			u64 numSpacesToInsert = (maxColumn - cursorColumn);
			
			Str8 cursorContents = ZEROED;
			ed_string_at_range(scratch, Ctx, &cursor.sel, &cursorContents);
			Str8 replaceStr = ZEROED;
			replaceStr.size = numSpacesToInsert;// + cursorContents.size;
			replaceStr.str = AllocArray(char, scratch, replaceStr.size);
			for (u64 sIndex = 0; sIndex < numSpacesToInsert; sIndex++) { replaceStr.str[sIndex] = ' '; }
			// memcpy(&replaceStr.str[numSpacesToInsert], cursorContents.str, cursorContents.size);
			
			// NotifyPrint_I("Cursor[%llu] inserting %llu spaces \"%S\"", cIndex, numSpacesToInsert, replaceStr);
			batchReplace.array[cIndex].buf = replaceStr;
			batchReplace.array[cIndex].range = (EditorOffsetRange){ .first_off=minOffset, .last_off=minOffset }; // cursor.sel;
		}
		
		EditorBatchEdit batchEdit = ZEROED;
		ed_edit_batch_begin(Ctx, &batchEdit);
		ed_edit_batch_replace(&batchEdit, &batchReplace);
		ed_edit_batch_end(Ctx, &batchEdit);
		
		ScratchEnd(scratch);
	}
}

//TODO: toggle_header/toggle_header_large
//TODO: toggle_comment
//TODO: add_next_no_wrapping
//TODO: jump_to_next
//TODO: move_left_subword/move_left_subword
//TODO: duplicate_selection
//TODO: jump_to_header_prev/jump_to_header_next
//TODO: open_parens_wrap_selection
//TODO: check_off_todo_line

// +==============================+
// |   Not Possible Right Now?    |
// +==============================+
//TODO: rename_file
//TODO: save_as_fixed
//TODO: goto_definition
