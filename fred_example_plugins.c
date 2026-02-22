/*
File:   fred_example_plugins.c
Author: Taylor Robbins
Date:   02\21\2026
Description: 
	** Contains the example plugins that the template generated
*/

// +==============================+
// |          say_hello           |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Says hello", "Says hello in each feed.", say_hello)
{
	String8 str = str8_lit("Hello, world!");
	feed_queue_warning(str);
	feed_queue_error(str);
	Temp scratch = scratch_begin(nullptr);
	String8 msg = str8_fmt(scratch.arena, "Hello! This is an int '%d'. This is a char '%c'. This is a float '%f'. This is a string '%S'", 42, 'C', 3.1415, str);
	feed_queue_info(msg);
	scratch_end(scratch);
}

// +==============================+
// |         message_demo         |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Message demo", "Demos a formatted message to each feed.", message_demo)
{
	Temp scratch = scratch_begin(nullptr);
	String8 str = str8_lit("String");
	String8 msg = str8_fmt(scratch.arena, "Hello! This is an int '%d'. This is a char '%c'. This is a float '%f'. This is a string '%S'", 42, 'C', 3.1415, str);
	feed_queue_info(msg);
	feed_queue_warning(msg);
	feed_queue_error(msg);
	scratch_end(scratch);
}

// +==============================+
// |        replace_w_fred        |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Replace word with FRED", "Replaces the next word with 'FRED'", replace_w_fred)
{
	EditorCmd cmd = {0};
	cmd.cmd = ED_NavWordRight;
	cmd.flags = ED_FLG_UpdateSelection;
	ed_push_command(ctx, &cmd);
	cmd.cmd = ED_InsInsert;
	cmd.flags = 0;
	cmd.buf = str8_lit("FRED");
	ed_push_command(ctx, &cmd);
}

// +==============================+
// |       show_selections        |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Show selections", "Emits a message for each cursor selection", show_selections)
{
	Temp scratch = scratch_begin(nullptr);
	String8Array strings;
	ed_cursor_selections(scratch.arena, ctx, &strings);
	String8 msg = {0};
	for EachIndex(i, strings.size) {
		String8 str = strings.strs[i];
		if (str.size != 0) {
			msg = str8_fmt(scratch.arena, "[%d] = '%S'", i, str);
		}
		else {
			msg = str8_fmt(scratch.arena, "[%d] = empty", i);
		}
		feed_queue_info(msg);
	}
	scratch_end(scratch);
}

// +==============================+
// |          snake_case          |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Change selection to snake case", "For each selection if it is PascalCase or camelCase, change it to snake_case", snake_case)
{
	Temp scratch = scratch_begin(nullptr);
	EditorCmd cmd = {0};
	String8Array strings;
	ed_cursor_selections(scratch.arena, ctx, &strings);
	// First, let's allocate our result array which will be inserted as a command.  The strings here will have lengths +1 for each capital letter.
	String8Array ins_buf = {0};
	ins_buf.size = strings.size;
	ins_buf.strs = push_array(scratch.arena, String8, strings.size);
	for EachIndex(i, strings.size) {
		String8 str = strings.strs[i];
		for EachIndex(j, str.size) {
			// Skip the first index because we only care about internal characters.
			if (j != 0
					&& str.str[j] >= 'A'
					&& str.str[j] <= 'Z') {
				ins_buf.strs[i].size += 2;
			}
			else {
				ins_buf.strs[i].size += 1;
			}
		}
	}
	// Now we can compute the result.
	const u32 delta = 'a' - 'A';
	char ins_char = 0;
	for EachIndex(i, strings.size) {
		String8 str = strings.strs[i];
		ins_buf.strs[i].str = push_array_no_zero(scratch.arena, char, ins_buf.strs[i].size);
		char* r_buf = ins_buf.strs[i].str;
		for EachIndex(j, str.size) {
			ins_char = str.str[j];
			if (j != 0
					&& str.str[j] >= 'A'
					&& str.str[j] <= 'Z') {
				*r_buf++ = '_';
				ins_char = str.str[j] + delta;
			}
			else {
					if (j == 0
							&& str.str[j] >= 'A'
							&& str.str[j] <= 'Z') {
						ins_char = str.str[j] + delta;
					}
					else {
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
	scratch_end(scratch);
}

// +==============================+
// |        page_up_center        |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Page up and center", "Moves the screen a page up and centers the screen", page_up_center)
{
	EditorCmd cmd = {0};
	cmd.cmd = ED_NavPageUp;
	ed_push_command(ctx, &cmd);
	cmd.cmd = ED_NavCenterCameraCursor;
	ed_push_command(ctx, &cmd);
}

// +==============================+
// |       page_down_center       |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Page down and center", "Moves the screen a page down and centers the screen", page_down_center)
{
	EditorCmd cmd = {0};
	cmd.cmd = ED_NavPageDown;
	ed_push_command(ctx, &cmd);
	cmd.cmd = ED_NavCenterCameraCursor;
	ed_push_command(ctx, &cmd);
}

// +==============================+
// |        trim_and_save         |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Trim file and save", "", trim_and_save)
{
	EditorCmd cmd = {0};
	cmd.cmd = ED_SpecTrimLineEndings;
	ed_push_command(ctx, &cmd);
	cmd.cmd = ED_SaveRequestSave;
	ed_push_command(ctx, &cmd);
}

// +==============================+
// |          line_info           |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Report line info", "Displays info about the current line.", line_info)
{
		Temp scratch = scratch_begin(nullptr);
		// Get the cursors first.
		EditorCursorArray cursors = {0};
		EditorCursorRange cursor_rng;
		u64 line;
		EditorOffsetRange line_rng;
		String8 msg;
		ed_cursor_ranges(scratch.arena, ctx, &cursors);
		for EachIndex(i, cursors.size)
		{
				cursor_rng = cursors.array[i];
				line = ed_line_at_offset(ctx, cursor_rng.cursor_off);
				ed_byte_range_at_line(ctx, line, &line_rng);
				msg = str8_fmt(scratch.arena,
					"Cursor{%I64d} | Line{%I64d} | LineRng{%I64d, %I64d}",
					cursor_rng.cursor_off,
					line,
					line_rng.first_off, line_rng.last_off
				);
				feed_queue_info(msg);
		}
		scratch_end(scratch);
}

// +==============================+
// |        repl_all_nums         |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Replace selection with numbers", "Replaces the current selection with ascending numbers for each subsequent occurrence.", repl_all_nums)
{
	Temp scratch = scratch_begin(nullptr);
	// Get the cursors first.
	EditorCursorArray cursors = {0};
	ed_cursor_ranges(scratch.arena, ctx, &cursors);
	String8 repl_str = {0};
	int good = 1;
	if (cursors.size != 1)
	{
		String8 msg = str8_lit("Operation only supported with a single cursor");
		feed_queue_warning(msg);
		good = 0;
	}

	// The range containing the original selection.
	EditorOffsetRange off_rng;
	if (good)
	{
		EditorCursorRange rng = cursors.array[0];
		off_rng.first_off = rng.sel.first_off;
		off_rng.last_off = rng.sel.last_off;
		ed_string_at_range(scratch.arena, ctx, &off_rng, &repl_str);
		good = repl_str.size != 0;
	}

	// Now we're going to try and extract a 'starting' point for the numbering based on the selection.
	// We do this by the following format:
	// SELECTION_STARTNUM
	// e.g.: 'FOOBAR_10'
	// This would replace every instance of 'FOOBAR' with an ascending number starting at 10.
	// Since the first instance will have extra characters, we save the first replacement string with
	// 'initial_repl_str'.
	String8 initial_repl_str = repl_str;
	String8 sliced;
	u64 count_start = 0;
	if (good)
	{
		// First, let's see if there's an '_'.
		u32 found_int = 0;
		for EachIndex(i, repl_str.size)
		{
			if (repl_str.str[i] == '_')
			{
				sliced = str8_substr(repl_str, .off = i + 1);
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
			repl_str = str8_substr(repl_str, .len = repl_str.size - sliced.size - 1);
			count_start = u64_from_str8(sliced, 10);
		}
	}

	if (good)
	{
		EditorBatchEdit batch;
		// First, find every instance of the string above.
		EditorFindResults find_results = {0};
		ed_find_all(scratch.arena, ctx, &repl_str, 0, &find_results);
		// Allocate enough space for the batch.
		EditorBatchReplace repl_op = {0};
		repl_op.size = find_results.size;
		repl_op.array = push_array(scratch.arena, EditorReplaceData, repl_op.size);
		// Fill.
		for EachIndex(i, repl_op.size)
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
			repl_op.array[i].buf = str8_fmt(scratch.arena, "%I64d", count_start);
			++count_start;
		}
		// Iterate the lines and perform the batch replacement.
		ed_edit_batch_begin(ctx, &batch);
		ed_edit_batch_replace(&batch, &repl_op);
		ed_edit_batch_end(ctx, &batch);
	}
	scratch_end(scratch);
}

// +==============================+
// |        toggle_comment        |
// +==============================+
DEF_PLUGIN_EDITOR_HOOK("Toggle C-style comment line(s)", "Adds a C-style comment to selected lines or current line.", toggle_comment)
{
	Temp scratch = scratch_begin(nullptr);
	// Get the cursors first.
	EditorCursorArray cursors = {0};
	ed_cursor_ranges(scratch.arena, ctx, &cursors);
	String8 repl_str = {0};
	int good = 1;
	if (cursors.size != 1)
	{
		String8 msg = str8_lit("Operation only supported with a single cursor");
		feed_queue_warning(msg);
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
		String8 line_txt;
		ed_string_at_range(scratch.arena, ctx, &line_rng, &line_txt);
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
			EditorBatchInsert ins = {0};
			ins.size = (end_line - start_line) + 1;
			ins.array = push_array(scratch.arena, EditorInsertData, ins.size);
			u64 idx = 0;
			for (u64 line = start_line; line <= end_line; ++line)
			{
				ed_byte_range_at_line(ctx, line, &line_rng);
				ins.array[idx].off = line_rng.first_off;
				ins.array[idx].buf = str8_lit("//");
				++idx;
			}
			ed_edit_batch_insert(&batch, &ins);
		}
		else
		{
			// Note: Lines are inclusive ranges.
			EditorBatchRemove rm = {0};
			rm.size = (end_line - start_line) + 1;
			rm.array = push_array(scratch.arena, EditorOffsetRange, rm.size);
			u64 idx = 0;
			for (u64 line = start_line; line <= end_line; ++line)
			{
				ed_byte_range_at_line(ctx, line, &line_rng);
				ed_string_at_range(scratch.arena, ctx, &line_rng, &line_txt);
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
	scratch_end(scratch);
}
