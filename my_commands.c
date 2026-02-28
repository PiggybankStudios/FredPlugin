
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
//TODO: This code is quite verbose and unweildy, we should probably make some helper functions that make it cleaner (most string manipulation)
Str8 ConvertTitleToHeader(Arena* arena, Str8 title, Str8 indentationStr, Str8 lineCommentSyntax, u64 width, char cornerChar, char topChar, char bottomChar, char leftChar, char rightChar)
{
	Str8 result = Str8_Empty;
	ScratchBegin1(scratch, arena);
	
	u64 actualWidth = Max2(width, title.size + 4);
	Str8 line1 = Str8_Empty;
	Str8 line2 = Str8_Empty;
	Str8 line3 = Str8_Empty;
	line1.size = lineCommentSyntax.size + actualWidth;
	line2.size = indentationStr.size + lineCommentSyntax.size + actualWidth;
	line3.size = indentationStr.size + lineCommentSyntax.size + actualWidth;
	line1.str = AllocArray(char, scratch, line1.size);
	line2.str = AllocArray(char, scratch, line2.size);
	line3.str = AllocArray(char, scratch, line3.size);
	
	u64 textOffset = actualWidth/2 - title.size/2;
	
	if (indentationStr.size > 0)
	{
		memcpy(&line2.str[0], indentationStr.str, indentationStr.size);
		memcpy(&line3.str[0], indentationStr.str, indentationStr.size);
	}
	if (lineCommentSyntax.size > 0)
	{
		memcpy(&line1.str[0], lineCommentSyntax.str, lineCommentSyntax.size);
		memcpy(&line2.str[indentationStr.size], lineCommentSyntax.str, lineCommentSyntax.size);
		memcpy(&line3.str[indentationStr.size], lineCommentSyntax.str, lineCommentSyntax.size);
	}
	for (u64 charIndex = 0; charIndex < actualWidth; charIndex++)
	{
		char* lineChar1 = &line1.str[lineCommentSyntax.size + charIndex];
		char* lineChar2 = &line2.str[indentationStr.size + lineCommentSyntax.size + charIndex];
		char* lineChar3 = &line3.str[indentationStr.size + lineCommentSyntax.size + charIndex];
		
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
	if (c == ',' || c == '\'' || c == '/' || c == '\\' || c == '+' ||
	    c == '(' || c == ')' || c == '-' || c == '_' || c == '|')
	{
		return true;
	}
	return false;
}
bool IsStrPartOfHeader(Str8 lineStr, Str8 lineCommentSyntax, Str8* titleOut, Str8* indentationOut)
{
	SetOptionalOutPntr(titleOut, Str8_Empty);
	bool foundCommentSyntax = false;
	bool foundFirstCornerOrSide = false;
	bool foundTopOrBottomCorner = false;
	bool foundSecondCornerOrSide = false;
	u64 charIndex = 0;
	
	//Eat indentation
	Str8 indentationStr = StrSlice(lineStr, 0, 0);
	while (charIndex < lineStr.size && (lineStr.str[charIndex] == ' ' || lineStr.str[charIndex] == '\t')) { charIndex++; indentationStr.size++; }
	SetOptionalOutPntr(indentationOut, indentationStr);
	
	if (lineStr.size - charIndex < lineCommentSyntax.size) { return false; }
	if (memcmp(&lineStr.str[charIndex], lineCommentSyntax.str, lineCommentSyntax.size) != 0) { return false; }
	charIndex += lineCommentSyntax.size;
	
	// Does it look like a center line of a header? (with '|' and spaces on either side)
	if (lineStr.size - charIndex >= 5 &&
		lineStr.str[charIndex] == '|' && lineStr.str[charIndex+1] == ' ' &&
		lineStr.str[lineStr.size-2] == ' ' && lineStr.str[lineStr.size-1] == '|')
	{
		charIndex += 2;
		
		while (charIndex < lineStr.size && lineStr.str[charIndex] == ' ') { charIndex++; }
		if (charIndex >= lineStr.size-2) { return false; }
		
		if (titleOut != nullptr) { titleOut->str = &lineStr.str[charIndex]; }
		while (charIndex < lineStr.size-1 && IsValidTitleChar(lineStr.str[charIndex])) { charIndex++; if (titleOut != nullptr) { titleOut->size++; } }
		if (charIndex < lineStr.size-2) { return false; }
		
		if (titleOut != nullptr)
		{
			// We probably slurped up the whitespace on the right-side of the header. Let's chop that off again
			while (titleOut->size > 0 && titleOut->str[titleOut->size-1] == ' ') { titleOut->size--; }
		}
		return true;
	}
	// Does it look like a top/bottom line of a header? Where it has corner chars '+' and has a single repeated char in-between?
	else if (lineStr.size - charIndex >= 3 &&
		lineStr.str[charIndex] == '+' && lineStr.str[lineStr.size-1] == '+')
	{
		charIndex++;
		char topChar = lineStr.str[charIndex];
		while (charIndex < lineStr.size-1 && lineStr.str[charIndex] == topChar) { charIndex++; }
		if (charIndex < lineStr.size-1) { return false; } //not all the chars were the same topChar
		return true;
	}
	else { return false; }
}
bool FindFullHeaderAroundLine(u64 line, Str8 lineCommentSyntax, Arena* lineArena, u64* firstLineOut, Str8* titleOut, Str8* indentationOut)
{
	ScratchBegin1(scratch, lineArena);
	u64 numLinesInFile = ed_line_count(Ctx);
	for (u64 lineIndex = (line > 2 ? line-2 : 0); lineIndex <= line; lineIndex++)
	{
		if (lineIndex + 2 > numLinesInFile) { continue; } //TODO: Equal to?
		
		Str8 firstLineTitle = Str8_Empty;
		EditorOffsetRange firstLineRange = ZEROED;
		ed_byte_range_at_line(Ctx, lineIndex + 0, &firstLineRange);
		Str8 firstLineStr = Str8_Empty;
		ed_string_at_range(scratch, Ctx, &firstLineRange, &firstLineStr);
		if (!IsStrPartOfHeader(firstLineStr, lineCommentSyntax, &firstLineTitle, indentationOut) || firstLineTitle.size > 0) { continue; }
		
		Str8 middleLineTitle = Str8_Empty;
		EditorOffsetRange middleLineRange = ZEROED;
		ed_byte_range_at_line(Ctx, lineIndex + 1, &middleLineRange);
		Str8 middleLineStr = Str8_Empty;
		ed_string_at_range(scratch, Ctx, &middleLineRange, &middleLineStr);
		if (!IsStrPartOfHeader(middleLineStr, lineCommentSyntax, &middleLineTitle, nullptr) || middleLineTitle.size == 0) { continue; }
		
		Str8 lastLineTitle = Str8_Empty;
		EditorOffsetRange lastLineRange = ZEROED;
		ed_byte_range_at_line(Ctx, lineIndex + 2, &lastLineRange);
		Str8 lastLineStr = Str8_Empty;
		ed_string_at_range(scratch, Ctx, &lastLineRange, &lastLineStr);
		if (!IsStrPartOfHeader(lastLineStr, lineCommentSyntax, &lastLineTitle, nullptr) || lastLineTitle.size > 0) { continue; }
		
		SetOptionalOutPntr(firstLineOut, lineIndex);
		SetOptionalOutPntr(titleOut, AllocStr8(lineArena, middleLineTitle));
		ScratchEnd(scratch);
		return true;
	}
	ScratchEnd(scratch);
	return false;
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
	EditorOffsetArray newCursors = ZEROED;
	newCursors.size = 0;
	newCursors.array = AllocArray(u64, scratch, cursors.size);
	
	for (u64 cIndex = 0; cIndex < cursors.size; cIndex++)
	{
		EditorCursorRange cursor = cursors.array[cIndex];
		Str8 cursorContents = ZEROED;
		ed_string_at_range(scratch, Ctx, &cursor.sel, &cursorContents);
		u64 cursorLine = ed_line_at_offset(Ctx, Min2(cursor.sel.first_off, cursor.sel.last_off));
		
		Str8 headerText = Str8_Empty;
		Str8 indentationStr = Str8_Empty;
		u64 headerTopLine = 0;
		if (FindFullHeaderAroundLine(cursorLine, lineCommentSyntax, scratch, &headerTopLine, &headerText, &indentationStr))
		{
			// Replace the entirety of the 3-lines with just the headerText
			EditorOffsetRange topLineRange = ZEROED;
			ed_byte_range_at_line(Ctx, headerTopLine, &topLineRange);
			EditorOffsetRange bottomLineRange = ZEROED;
			ed_byte_range_at_line(Ctx, headerTopLine+2, &bottomLineRange);
			EditorOffsetRange headerRange = {
				.first_off=Min2(topLineRange.first_off, topLineRange.last_off),
				.last_off=Max2(bottomLineRange.first_off, bottomLineRange.last_off),
			};
			
			NotifyPrint_I("Collapsing header on lines %llu-%llu to \"%S\"", headerTopLine, headerTopLine+2, headerText);
			
			newCursors.array[newCursors.size] = headerRange.last_off;
			newCursors.size++;
			
			EditorReplaceData* replacePntr = &batchReplace.array[replaceIndex];
			replacePntr->range = headerRange;
			replacePntr->buf = JoinStringsInArena(scratch, indentationStr, headerText);
			
			replaceIndex++;
		}
		else if (cursorContents.size > 0)
		{
			EditorOffsetRange cursorLineRange = ZEROED;
			ed_byte_range_at_line(Ctx, cursorLine, &cursorLineRange);
			Str8 cursorLineStr = Str8_Empty;
			ed_string_at_range(scratch, Ctx, &cursorLineRange, &cursorLineStr);
			indentationStr = StrSlice(cursorLineStr, 0, 0);
			while (indentationStr.size < cursorLineStr.size && (cursorLineStr.str[indentationStr.size] == ' ' || cursorLineStr.str[indentationStr.size] == '\t')) { indentationStr.size++; }
			
			// Create the header
			Str8 headerLines = ConvertTitleToHeader(scratch, cursorContents, indentationStr, lineCommentSyntax, width, cornerChar, topChar, bottomChar, leftChar, rightChar);
			
			newCursors.array[newCursors.size] = Max2(cursor.sel.first_off, cursor.sel.last_off);
			newCursors.size++;
			
			batchReplace.array[replaceIndex].range = cursor.sel;
			batchReplace.array[replaceIndex].buf = headerLines;
			replaceIndex++;
		}
	}
	
	if (replaceIndex > 0)
	{
		batchReplace.size = replaceIndex;
		EditorBatchEdit batchEdit = ZEROED;
		ed_edit_batch_begin(Ctx, &batchEdit);
		ed_edit_batch_replace(&batchEdit, &batchReplace);
		ed_edit_batch_end(Ctx, &batchEdit);
		
		//Fixup the cursor locations based on replacements
		for (u64 cIndex = 0; cIndex < newCursors.size; cIndex++)
		{
			u64* cursorPntr = &newCursors.array[cIndex];
			u64 cursorOriginalOffset = *cursorPntr;
			for (u64 rIndex = 0; rIndex < batchReplace.size; rIndex++)
			{
				EditorReplaceData* replacePntr = &batchReplace.array[rIndex];
				u64 replaceMin = Min2(replacePntr->range.first_off, replacePntr->range.last_off);
				u64 replaceMax = Max2(replacePntr->range.first_off, replacePntr->range.last_off);
				u64 replaceLength = replaceMax - replaceMin;
				if (cursorOriginalOffset >= replaceMin)
				{
					i64 bufferLengthDiff = (i64)replacePntr->buf.size - (i64)(replaceMax - replaceMin);
					if (bufferLengthDiff != 0)
					{
						NotifyPrint_I("Replacement[%llu] changes buffer by %s%lld", rIndex, bufferLengthDiff >= 0 ? "+" : "", bufferLengthDiff);
						bool cursorIsInside = (cursorOriginalOffset > replaceMin && cursorOriginalOffset < replaceMax);
						if (cursorIsInside)
						{
							u64 cursorInReplaceOffset = (cursorOriginalOffset - replaceMin);
							bool isCursorOnRight = (cursorInReplaceOffset >= replaceLength/2);
							if (isCursorOnRight) { *cursorPntr += replaceLength - cursorInReplaceOffset; }
							else { *cursorPntr -= cursorInReplaceOffset; }
						}
						else
						{
							*cursorPntr += bufferLengthDiff;
						}
					}
				}
			}
			NotifyPrint_I("Cursor[%llu] went from %llu->%llu after %llu replacement%s",
				cIndex,
				cursorOriginalOffset, *cursorPntr,
				batchReplace.size, (batchReplace.size == 1) ? "" : "s"
			);
		}
		
		EditorCmd command = ZEROED;
		command.cmd = ED_MCDropCursors;
		ed_push_command(Ctx, &command);
		command.cmd = ED_NavMoveCursorTo;
		command.byte_offsets.size = 1;
		command.byte_offsets.array = &newCursors.array[0];
		ed_push_command(Ctx, &command);
		if (newCursors.size > 1)
		{
			command.cmd = ED_MCCreateCursors;
			command.byte_offsets.size = newCursors.size-1;
			command.byte_offsets.array = &newCursors.array[1];
			ed_push_command(Ctx, &command);
		}
		
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
