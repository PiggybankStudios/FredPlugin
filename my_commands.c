
DEF_PLUGIN_EDITOR_HOOK("Run 'Taylor' Command", "A test command for experimenting with plugin code in Fred", taylor)
{
	WithContext(ctx)
	{
		NotifyPrint_W("Context pntr: %p", ctx);
	}
}

DEF_PLUGIN_EDITOR_HOOK("Kill Line(s)", "Removes the entire line that the cursor is on (works with multi-cursor)", kill_line)
{
	WithContext(ctx)
	{
		EditorCmd command = ZEROED;
		command.cmd = ED_DelDeleteLine;
		ed_push_command(ctx, &command);
	}
}

void InsertNums(i64 start, i64 increment)
{
	ScratchBegin(scratch);
	
	EditorCursorArray cursors = ZEROED;
	ed_cursor_ranges(scratch, CtxGlobal, &cursors);
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
		// ed_string_at_range(scratch, CtxGlobal, &cursorOffsetRange, &cursorContents);
		// NotifyPrint_W("Cursor[%llu]: \"%S\"", cIndex, cursorContents);
		
		batchReplace.array[cIndex].range = (EditorOffsetRange){ .first_off = cursor.sel.first_off, .last_off = cursor.sel.last_off };
		batchReplace.array[cIndex].buf = PrintInArena(scratch, "%llu", currentNumber);
		currentNumber += increment;
	}
	
	EditorBatchEdit batchEdit = ZEROED;
	ed_edit_batch_begin(CtxGlobal, &batchEdit);
	ed_edit_batch_replace(&batchEdit, &batchReplace);
	ed_edit_batch_end(CtxGlobal, &batchEdit);
	
	ScratchEnd(scratch);
}

DEF_PLUGIN_EDITOR_HOOK("Insert Numbers (from 0)", "Inserts ascending numbers (starting at 0) for each multi-cursor", insert_nums_0)
{
	WithContext(ctx) { InsertNums(0, 1); }
}
DEF_PLUGIN_EDITOR_HOOK("Insert Numbers (from 1)", "Inserts ascending numbers (starting at 1) for each multi-cursor", insert_nums_1)
{
	WithContext(ctx) { InsertNums(1, 1); }
}
