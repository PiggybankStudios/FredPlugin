
// (EditorCtx* ctx)
DEF_PLUGIN_EDITOR_HOOK("Run 'Taylor' Command", "A test command for experimenting with plugin code in Fred", taylor)
{
	CtxGlobal = ctx;
	NotifyPrint_W("Context pntr: %p", ctx);
}
