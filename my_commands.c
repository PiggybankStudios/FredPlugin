
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

// +==============================+
// |        toggle_header         |
// +==============================+
Str8 GetHeaderLinesForTitle(Arena* arena, Str8 title, Str8 lineCommentSyntax, u64 width, char cornerChar, char topChar, char bottomChar, char leftChar, char rightChar)
{
	Str8 result = Str8_Empty;
	ScratchBegin1(scratch, arena);
	
	u64 actualWidth = Max2(width, title.size + 4);
	Str8 lineIndentation = Str8_Empty;
	for (u64 charIndex = 0; charIndex < title.size; charIndex++)
	{
		if (title.str[charIndex] == ' ' || title.str[charIndex] == '\t')
		{
			lineIndentation = StrSlice(title, 0, charIndex);
		}
		else { break; }
	}
	Str8 line1 = Str8_Empty;
	Str8 line2 = Str8_Empty;
	Str8 line3 = Str8_Empty;
	line1.size = lineCommentSyntax.size + actualWidth;
	line2.size = lineIndentation.size + lineCommentSyntax.size + actualWidth;
	line3.size = lineIndentation.size + lineCommentSyntax.size + actualWidth;
	line1.str = AllocArray(char, scratch, line1.size);
	line2.str = AllocArray(char, scratch, line2.size);
	line3.str = AllocArray(char, scratch, line3.size);
	
	u64 textOffset = actualWidth/2 - title.size/2;
	
	if (lineIndentation.size > 0)
	{
		memcpy(&line2.str[0], lineIndentation.str, lineIndentation.size);
		memcpy(&line3.str[0], lineIndentation.str, lineIndentation.size);
	}
	if (lineCommentSyntax.size > 0)
	{
		memcpy(&line1.str[lineIndentation.size], lineCommentSyntax.str, lineCommentSyntax.size);
		memcpy(&line2.str[lineIndentation.size], lineCommentSyntax.str, lineCommentSyntax.size);
		memcpy(&line3.str[lineIndentation.size], lineCommentSyntax.str, lineCommentSyntax.size);
	}
	for (u64 charIndex = 0; charIndex < actualWidth; charIndex++)
	{
		char* lineChar1 = &line1.str[lineIndentation.size + lineCommentSyntax.size + charIndex];
		char* lineChar2 = &line2.str[lineIndentation.size + lineCommentSyntax.size + charIndex];
		char* lineChar3 = &line3.str[lineIndentation.size + lineCommentSyntax.size + charIndex];
		
		if (charIndex == 0 || charIndex == actualWidth-1) { *lineChar1 = cornerChar; *lineChar3 = cornerChar; }
		else { *lineChar1 = topChar; *lineChar3 = bottomChar; }
		
		if (charIndex == 0) { *lineChar2 = leftChar; }
		else if (charIndex == actualWidth-1) { *lineChar2 = rightChar; }
		else if (charIndex < textOffset || charIndex >= textOffset + title.size) { *lineChar2 = ' '; }
		else { *lineChar2 = title.str[charIndex - textOffset]; }
	}
	
	 //TODO: We should probably do platfom correct line endings (or maybe ask Fred what style line endings the file is)
	result.size = line1.size + 1 + line2.size + 1 + line3.size;
	result.str = AllocArray(char, arena, result.size);
	memcpy(&result.str[0], line1.str, line1.size);
	result.str[line1.size] = '\n';
	memcpy(&result.str[line1.size + 1], line2.str, line2.size);
	result.str[line1.size + 1 + line2.size] = '\n';
	memcpy(&result.str[line1.size + 1 + line2.size + 1], line3.str, line3.size);
	
	ScratchEnd(scratch);
	return result;
}
bool IsValidTitleChar(char c)
{
	if (c >= 'A' && c <= 'Z') { return true; }
	if (c >= 'a' && c <= 'z') { return true; }
	if (c >= '0' && c <= '9') { return true; }
	if (c == ' ') { return true; }
	return false;
}
bool IsLinePartOfHeader(Str8 line, Str8 lineCommentSyntax, Str8* titleOut)
{
	SetOptionalOutPntr(titleOut, Str8_Empty);
	bool foundCommentSyntax = false;
	bool foundFirstCornerOrSide = false;
	bool foundTopOrBottomCorner = false;
	bool foundSecondCornerOrSide = false;
	u64 charIndex = 0;
	
	while (charIndex < line.size && (line.str[charIndex] == ' ' || line.str[charIndex] == '\t')) { charIndex++; }
	
	if (line.size - charIndex < lineCommentSyntax.size) { return false; }
	if (memcmp(&line.str[charIndex], lineCommentSyntax.str, lineCommentSyntax.size) != 0) { return false; }
	charIndex += lineCommentSyntax.size;
	
	// Does it look like a center line of a header? (with '|' and spaces on either side)
	if (line.size - charIndex >= 5 &&
		line.str[charIndex] == '|' && line.str[charIndex+1] == ' ' &&
		line.str[line.size-2] == ' ' && line.str[line.size-1] == '|')
	{
		charIndex += 2;
		
		while (charIndex < line.size && line.str[charIndex] == ' ') { charIndex++; }
		if (charIndex >= line.size-2) { return false; }
		
		if (titleOut != nullptr) { titleOut->str = &line.str[charIndex]; }
		while (charIndex < line.size && IsValidTitleChar(line.str[charIndex])) { charIndex++; if (titleOut != nullptr) { titleOut->size++; } }
		if (charIndex < line.size-2) { return false; }
		
		if (titleOut != nullptr)
		{
			// We probably slurped up the whitespace on the right-side of the header. Let's chop that off again
			while (titleOut->size > 0 && titleOut->str[titleOut->size-1] == ' ') { titleOut->size--; }
		}
		return true;
	}
	// Does it look like a top/bottom line of a header? Where it has corner chars '+' and has a single repeated char in-between?
	else if (line.size - charIndex >= 3 &&
		line.str[charIndex] == '+' && line.str[line.size-1] == '+')
	{
		charIndex++;
		char topChar = line.str[charIndex];
		while (charIndex < line.size-1 && line.str[charIndex] == topChar) { charIndex++; }
		if (charIndex < line.size-1) { return false; } //not all the chars were the same topChar
		return true;
	}
	else { return false; }
}
void ToggleHeader(Str8 lineCommentSyntax, u64 width, char cornerChar, char topChar, char bottomChar, char leftChar, char rightChar)
{
	ScratchBegin(scratch);
	EditorCursorArray cursors = ZEROED;
	ed_cursor_ranges(scratch, Ctx, &cursors);
	if (cursors.size == 0) { Notify_W("No cursors"); return; }
	
	EditorBatchReplace batchReplace = ZEROED;
	batchReplace.size = cursors.size;
	batchReplace.array = AllocArray(EditorReplaceData, scratch, batchReplace.size);
	u64 replaceIndex = 0;
	
	for (u64 cIndex = 0; cIndex < cursors.size; cIndex++)
	{
		EditorCursorRange cursor = cursors.array[cIndex];
		Str8 cursorContents = ZEROED;
		ed_string_at_range(scratch, Ctx, &cursor.sel, &cursorContents);
		u64 cursorLine = ed_line_at_offset(Ctx, Min2(cursor.sel.first_off, cursor.sel.last_off));
		
		bool isCursorOnHeader = false;
		Str8 headerText = Str8_Empty;
		u64 headerTopLine = 0;
		
		do //breakable block TODO: Make a macro for this?
		{
			EditorOffsetRange lineRange = ZEROED;
			ed_byte_range_at_line(Ctx, cursorLine + 0, &lineRange);
			Str8 lineStr = Str8_Empty;
			ed_string_at_range(scratch, Ctx, &lineRange, &lineStr);
			if (IsLinePartOfHeader(lineStr, lineCommentSyntax, &headerText))
			{
				// If this was a top or bottom line we gotta look up/down 2 lines and see if the entire header is there
				if (headerText.size == 0)
				{
					if (cursorLine >= 2)
					{
						EditorOffsetRange firstLineRange = ZEROED;
						ed_byte_range_at_line(Ctx, cursorLine - 2, &firstLineRange);
						Str8 firstLineStr = Str8_Empty;
						ed_string_at_range(scratch, Ctx, &firstLineRange, &firstLineStr);
						EditorOffsetRange middleLineRange = ZEROED;
						ed_byte_range_at_line(Ctx, cursorLine - 1, &middleLineRange);
						Str8 middleLineStr = Str8_Empty;
						ed_string_at_range(scratch, Ctx, &middleLineRange, &middleLineStr);
						
						if (IsLinePartOfHeader(middleLineStr, lineCommentSyntax, &headerText) && headerText.size > 0)
						{
							Str8 topLineHeaderText = Str8_Empty;
							if (IsLinePartOfHeader(firstLineStr, lineCommentSyntax, &topLineHeaderText) && topLineHeaderText.size == 0)
							{
								isCursorOnHeader = true;
								headerTopLine = cursorLine-2;
							}
							else { Notify_W("Line-2 is not a valid header top"); break; }
						}
						else { Notify_W("Line-1 is not a valid header center"); break; }
					}
					else if (cursorLine <= ed_line_count(Ctx) - 2)
					{
						EditorOffsetRange middleLineRange = ZEROED;
						ed_byte_range_at_line(Ctx, cursorLine + 1, &middleLineRange);
						Str8 middleLineStr = Str8_Empty;
						ed_string_at_range(scratch, Ctx, &middleLineRange, &middleLineStr);
						EditorOffsetRange bottomLineRange = ZEROED;
						ed_byte_range_at_line(Ctx, cursorLine + 2, &bottomLineRange);
						Str8 bottomLineStr = Str8_Empty;
						ed_string_at_range(scratch, Ctx, &bottomLineRange, &bottomLineStr);
						
						if (IsLinePartOfHeader(middleLineStr, lineCommentSyntax, &headerText) && headerText.size > 0)
						{
							Str8 bottomLineHeaderText = Str8_Empty;
							if (IsLinePartOfHeader(bottomLineStr, lineCommentSyntax, &bottomLineHeaderText) && bottomLineHeaderText.size == 0)
							{
								isCursorOnHeader = true;
								headerTopLine = cursorLine;
							}
							else { Notify_W("Line+1 is not a valid header center"); break; }
						}
						else { Notify_W("Line+2 is not a valid header bottom"); break; }
					}
					else { Notify_W("Not enough surrounding lines"); break; }
				}
				// If this is the middle line then we just need to check that the line above and below are header lines
				else if (cursorLine > 0)
				{
					//TODO: Check lines above and below to see if they are part of the header
					// isCursorOnHeader = true;
					// headerTopLine = cursorLine-1;
					Notify_W("Unimplemented");
				}
			}
		} while(false);
		
		
		// Replace the entirety of the 3-lines with just the headerText
		if (isCursorOnHeader)
		{
			EditorOffsetRange topLineRange = ZEROED;
			ed_byte_range_at_line(Ctx, headerTopLine, &topLineRange);
			EditorOffsetRange bottomLineRange = ZEROED;
			ed_byte_range_at_line(Ctx, headerTopLine+2, &bottomLineRange);
			EditorOffsetRange headerRange = {
				.first_off=Min2(topLineRange.first_off, topLineRange.last_off),
				.last_off=Max2(bottomLineRange.first_off, bottomLineRange.last_off),
			};
			
			NotifyPrint_I("Collapsing header on lines %llu-%llu to \"%S\"", headerTopLine, headerTopLine+2, headerText);
			#if 1
			batchReplace.array[replaceIndex].range = headerRange;
			batchReplace.array[replaceIndex].buf = headerText;
			replaceIndex++;
			#else
			EditorCmd cmd = ZEROED;
			//Select all 3 lines
			cmd = (EditorCmd)ZEROED;
			cmd.cmd = ED_NavMoveCursorTo;
			cmd.flags = 0;
			cmd.byte_offsets.size = 1;
			cmd.byte_offsets.array = &headerRange.last_off;
			ed_push_command(Ctx, &cmd);
			cmd = (EditorCmd)ZEROED;
			cmd.cmd = ED_NavMoveCursorTo;
			cmd.flags = ED_FLG_UpdateSelection;
			cmd.byte_offsets.size = 1;
			cmd.byte_offsets.array = &headerRange.last_off;
			ed_push_command(Ctx, &cmd);
			
			cmd = (EditorCmd)ZEROED;
			cmd.cmd = ED_InsInsert;
			cmd.flags = 0;
			cmd.buf = headerText;
			ed_push_command(Ctx, &cmd);
			#endif
		}
		else if (cursorContents.size > 0)
		{
			// Create the header
			Str8 headerLines = GetHeaderLinesForTitle(scratch, cursorContents, lineCommentSyntax, width, cornerChar, topChar, bottomChar, leftChar, rightChar);
			
			#if 1
			batchReplace.array[replaceIndex].range = cursor.sel;
			batchReplace.array[replaceIndex].buf = headerLines;
			replaceIndex++;
			#else
			EditorCmd cmd = ZEROED;
			cmd.cmd = ED_InsInsert;
			cmd.flags = 0;
			cmd.buf = headerLines;
			ed_push_command(Ctx, &cmd);
			#endif
		}
		else
		{
			// Strip the header
			//TODO: Implement me!
		}
	}
	
	if (replaceIndex > 0)
	{
		batchReplace.size = replaceIndex;
		EditorBatchEdit batchEdit = ZEROED;
		ed_edit_batch_begin(Ctx, &batchEdit);
		ed_edit_batch_replace(&batchEdit, &batchReplace);
		ed_edit_batch_end(Ctx, &batchEdit);
	}
	else { Notify_W("No selections are on a header line or selecting text to convert to header"); }
	
	ScratchEnd(scratch);
}
DEF_PLUGIN_EDITOR_HOOK("Toggle header (small)", "Wraps the current selection in a simple 3-line header syntax (inside a comment). If the cursor is on of those three lines of an existing header it strips the syntax out instead", toggle_header) //Ctrl+T
{
	WithContext(ctx) { ToggleHeader(StrLit("// "), 32, '+', '=', '=', '|', '|'); }
}
DEF_PLUGIN_EDITOR_HOOK("Toggle header (large)", "Wraps the current selection in a simple 3-line header syntax (inside a comment). If the cursor is on of those three lines of an existing header it strips the syntax out instead", toggle_header_large) //Ctrl+T
{
	WithContext(ctx) { ToggleHeader(StrLit("// "), 64, '+', '-', '-', '|', '|'); }
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
