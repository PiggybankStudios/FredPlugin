/*
File:   example_commands.c
Author: Taylor Robbins
Date:   02\21\2026
Description: 
	** Contains the example plugins that the template generated
*/

// +==============================+
// |          say_hello           |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Say hello", "Says hello in each feed.", say_hello)
{
	WithContext(ctx)
	{
		Notify_W("Hello, world!");
		Notify_E("Hello, world!");
		Str8 helloWorld = StrLit("Hello, world!");
		NotifyPrint_I("Hello! This is an int '%d'. This is a char '%c'. This is a float '%f'. This is a string '%S'", 42, 'C', 3.1415, helloWorld);
	}
}

// +==============================+
// |      replace_word_fred       |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Replace word with FRED", "Replaces the next word with 'FRED'", replace_word_fred)
{
	WithContext(ctx)
	{
		EditorCmd cmd = ZEROED;
		cmd.cmd = ED_NavWordRight;
		cmd.flags = ED_FLG_UpdateSelection;
		ed_push_command(ctx, &cmd);
		cmd.cmd = ED_InsInsert;
		cmd.flags = 0;
		cmd.buf = StrLit("FRED");
		ed_push_command(ctx, &cmd);
	}
}

// +==============================+
// |       print_selections       |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Print Selections", "Emits a message for each cursor selection", print_selections)
{
	WithContext(ctx)
	{
		ScratchBegin(scratch);
		Str8Array strings;
		ed_cursor_selections(scratch, ctx, &strings);
		for (u64 i = 0; i < strings.size; i++)
		{
			Str8 str = strings.strs[i];
			if (str.size != 0) { NotifyPrint_I("[%d] = '%S'", i, str); }
			else { NotifyPrint_I("[%d] = empty", i); }
		}
		ScratchEnd(scratch);
	}
}

// +==============================+
// |          snake_case          |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Change selection to snake case", "For each selection if it is PascalCase or camelCase, change it to snake_case", snake_case)
{
	WithContext(ctx)
	{
		ScratchBegin(scratch);
		EditorCmd cmd = ZEROED;
		Str8Array strings;
		ed_cursor_selections(scratch, ctx, &strings);
		// First, let's allocate our result array which will be inserted as a command.  The strings here will have lengths +1 for each capital letter.
		Str8Array ins_buf = ZEROED;
		ins_buf.size = strings.size;
		ins_buf.strs = AllocArray(Str8, scratch, strings.size);
		for (u64 i = 0; i < strings.size; i++)
		{
			Str8 str = strings.strs[i];
			for (u64 j = 0; j < str.size; j++)
			{
				// Skip the first index because we only care about internal characters.
				if (j != 0 && str.str[j] >= 'A' && str.str[j] <= 'Z')
				{
					ins_buf.strs[i].size += 2;
				}
				else
				{
					ins_buf.strs[i].size += 1;
				}
			}
		}
		// Now we can compute the result.
		const u32 delta = 'a' - 'A';
		char ins_char = 0;
		for (u64 i = 0; i < strings.size; i++)
		{
			Str8 str = strings.strs[i];
			ins_buf.strs[i].str = AllocArrayNoZero(char, scratch, ins_buf.strs[i].size);
			char* r_buf = ins_buf.strs[i].str;
			for (u64 j = 0; j < str.size; j++)
			{
				ins_char = str.str[j];
				if (j != 0 && str.str[j] >= 'A' && str.str[j] <= 'Z')
				{
					*r_buf++ = '_';
					ins_char = str.str[j] + delta;
				}
				else
				{
						if (j == 0 && str.str[j] >= 'A' && str.str[j] <= 'Z')
						{
							ins_char = str.str[j] + delta;
						}
						else
						{
							ins_char = str.str[j];
						}
				}
				*r_buf++ = ins_char;
			}
		}
		// Now we can compose the command.
		cmd.cmd = ED_MCInsertGroup;
		cmd.buffers = ins_buf;
		ed_push_command(ctx, &cmd);
		ScratchEnd(scratch);
	}
}

// +==============================+
// |        page_up_center        |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Page up and center", "Moves the screen a page up and centers the screen", page_up_center)
{
	WithContext(ctx)
	{
		EditorCmd cmd = ZEROED;
		cmd.cmd = ED_NavPageUp;
		ed_push_command(ctx, &cmd);
		cmd.cmd = ED_NavCenterCameraCursor;
		ed_push_command(ctx, &cmd);
	}
}

// +==============================+
// |       page_down_center       |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Page down and center", "Moves the screen a page down and centers the screen", page_down_center)
{
	WithContext(ctx)
	{
		EditorCmd cmd = ZEROED;
		cmd.cmd = ED_NavPageDown;
		ed_push_command(ctx, &cmd);
		cmd.cmd = ED_NavCenterCameraCursor;
		ed_push_command(ctx, &cmd);
	}
}

// +==============================+
// |        trim_and_save         |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Trim file and save", "", trim_and_save)
{
	WithContext(ctx)
	{
		EditorCmd cmd = ZEROED;
		cmd.cmd = ED_SpecTrimLineEndings;
		ed_push_command(ctx, &cmd);
		cmd.cmd = ED_SaveRequestSave;
		ed_push_command(ctx, &cmd);
	}
}

// +==============================+
// |          line_info           |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Report line info", "Displays info about the current line.", line_info)
{
	WithContext(ctx)
	{
		ScratchBegin(scratch);
		// Get the cursors first.
		EditorCursorArray cursors = ZEROED;
		EditorCursorRange cursor_rng;
		u64 line;
		EditorOffsetRange line_rng;
		ed_cursor_ranges(scratch, ctx, &cursors);
		for (u64 i = 0; i < cursors.size; i++)
		{
				cursor_rng = cursors.array[i];
				line = ed_line_at_offset(ctx, cursor_rng.cursor_off);
				ed_byte_range_at_line(ctx, line, &line_rng);
				NotifyPrint_I("Cursor{%I64d} | Line{%I64d} | LineRng{%I64d, %I64d}",
					cursor_rng.cursor_off,
					line,
					line_rng.first_off, line_rng.last_off
				);
		}
		ScratchEnd(scratch);
	}
}

// +==============================+
// |        repl_all_nums         |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Replace selection with numbers", "Replaces the current selection with ascending numbers for each subsequent occurrence.", repl_all_nums)
{
	WithContext(ctx)
	{
		ScratchBegin(scratch);
		// Get the cursors first.
		EditorCursorArray cursors = ZEROED;
		ed_cursor_ranges(scratch, ctx, &cursors);
		Str8 repl_str = ZEROED;
		int good = 1;
		if (cursors.size != 1)
		{
			Notify_W("Operation only supported with a single cursor");
			good = 0;
		}
		
		// The range containing the original selection.
		EditorOffsetRange off_rng;
		if (good)
		{
			EditorCursorRange rng = cursors.array[0];
			off_rng.first_off = rng.sel.first_off;
			off_rng.last_off = rng.sel.last_off;
			ed_string_at_range(scratch, ctx, &off_rng, &repl_str);
			good = repl_str.size != 0;
		}
		
		// Now we're going to try and extract a 'starting' point for the numbering based on the selection.
		// We do this by the following format:
		// SELECTION_STARTNUM
		// e.g.: 'FOOBAR_10'
		// This would replace every instance of 'FOOBAR' with an ascending number starting at 10.
		// Since the first instance will have extra characters, we save the first replacement string with
		// 'initial_repl_str'.
		Str8 initial_repl_str = repl_str;
		Str8 sliced;
		u64 count_start = 0;
		if (good)
		{
			// First, let's see if there's an '_'.
			u32 found_int = 0;
			for (u64 i = 0; i < repl_str.size; i++)
			{
				if (repl_str.str[i] == '_')
				{
					sliced = StrSliceFrom(repl_str, i+1);
					// Only support base-10 for now.
					if (sliced.size != 0 && str8_is_integer(sliced, 10))
					{
						found_int = 1;
						break;
					}
				}
			}
			if (found_int)
			{
				// Also remove '1' for the '_' character.
				repl_str = StrSlice(repl_str, 0, repl_str.size - sliced.size - 1);
				count_start = u64_from_str8(sliced, 10);
			}
		}
		
		if (good)
		{
			EditorBatchEdit batch;
			// First, find every instance of the string above.
			EditorFindResults find_results = ZEROED;
			ed_find_all(scratch, ctx, &repl_str, 0, &find_results);
			// Allocate enough space for the batch.
			EditorBatchReplace repl_op = ZEROED;
			repl_op.size = find_results.size;
			repl_op.array = AllocArray(EditorReplaceData, scratch, repl_op.size);
			// Fill.
			for (u64 i = 0; i < repl_op.size; i++)
			{
				// Since the first selection can be different, we will special case it.
				if (find_results.array[i].first_off == off_rng.first_off)
				{
					repl_op.array[i].range = off_rng;
				}
				else
				{
					repl_op.array[i].range = find_results.array[i];
				}
				repl_op.array[i].buf = PrintInArena(scratch, "%I64d", count_start);
				++count_start;
			}
			// Iterate the lines and perform the batch replacement.
			ed_edit_batch_begin(ctx, &batch);
			ed_edit_batch_replace(&batch, &repl_op);
			ed_edit_batch_end(ctx, &batch);
		}
		ScratchEnd(scratch);
	}
}

// +==============================+
// |        toggle_comment        |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Toggle C-style comment line(s)", "Adds a C-style comment to selected lines or current line.", toggle_comment)
{
	WithContext(ctx)
	{
		ScratchBegin(scratch);
		// Get the cursors first.
		EditorCursorArray cursors = ZEROED;
		ed_cursor_ranges(scratch, ctx, &cursors);
		Str8 repl_str = ZEROED;
		int good = 1;
		if (cursors.size != 1)
		{
			Notify_W("Operation only supported with a single cursor");
			good = 0;
		}
		
		u64 start_line;
		u64 end_line;
		u32 add_comment = 0;
		if (good)
		{
			// Find the line at the cursor selection.  Note: The selection might also be empty, but that's OK.
			start_line = ed_line_at_offset(ctx, cursors.array[0].sel.first_off);
			end_line = ed_line_at_offset(ctx, cursors.array[0].sel.last_off);
			// Pull the first line range and decide if we need to add a comment or remove one.
			EditorOffsetRange line_rng;
			ed_byte_range_at_line(ctx, start_line, &line_rng);
			Str8 line_txt;
			ed_string_at_range(scratch, ctx, &line_rng, &line_txt);
			u32 slash_count = 0;
			for (u64 i = 0; i < line_txt.size && i < 2; ++i)
			{
				slash_count += line_txt.str[i] == '/';
			}
			add_comment = slash_count != 2;
			
			// Create batch to iterate lines which either removes the comment or adds one.
			// Note: For selected lines which do not already have a comment, we will leave
			// them alone.
			EditorBatchEdit batch;
			ed_edit_batch_begin(ctx, &batch);
			if (add_comment)
			{
				// Note: Lines are inclusive ranges.
				EditorBatchInsert ins = ZEROED;
				ins.size = (end_line - start_line) + 1;
				ins.array = AllocArray(EditorInsertData, scratch, ins.size);
				u64 idx = 0;
				for (u64 line = start_line; line <= end_line; ++line)
				{
					ed_byte_range_at_line(ctx, line, &line_rng);
					ins.array[idx].off = line_rng.first_off;
					ins.array[idx].buf = StrLit("//");
					++idx;
				}
				ed_edit_batch_insert(&batch, &ins);
			}
			else
			{
				// Note: Lines are inclusive ranges.
				EditorBatchRemove rm = ZEROED;
				rm.size = (end_line - start_line) + 1;
				rm.array = AllocArray(EditorOffsetRange, scratch, rm.size);
				u64 idx = 0;
				for (u64 line = start_line; line <= end_line; ++line)
				{
					ed_byte_range_at_line(ctx, line, &line_rng);
					ed_string_at_range(scratch, ctx, &line_rng, &line_txt);
					if (line_txt.size > 1 && line_txt.str[0] == '/' && line_txt.str[1] == '/')
					{
						rm.array[idx].first_off = line_rng.first_off;
						rm.array[idx].last_off = line_rng.first_off + 2;
					}
					++idx;
				}
				ed_edit_batch_remove(&batch, &rm);
			}
			ed_edit_batch_end(ctx, &batch);
		}
		ScratchEnd(scratch);
	}
}
