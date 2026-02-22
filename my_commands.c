
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
