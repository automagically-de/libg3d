#include <g3d/types.h>
#include <g3d/stream-scanner.h>
#include <g3d/debug.h>

#include "imp_vrml_v2_chunks.h"

static void vrml_v2_init_gscanner(VrmlGlobal *global, GScanner *gscanner)
{
	gint32 i;

	gscanner->config->case_sensitive = TRUE;
	gscanner->config->cset_identifier_nth =
		G_CSET_a_2_z "_0123456789:+" G_CSET_A_2_Z G_CSET_LATINS G_CSET_LATINC;

	global->scope = VRML_V2_SCOPE_DEFAULT;
	g_scanner_set_scope(gscanner, VRML_V2_SCOPE_DEFAULT);

	for(i = 0; vrml_v2_symbols[i].name != NULL; i ++) {
		g_scanner_scope_add_symbol(gscanner,
			VRML_V2_SCOPE_DEFAULT, vrml_v2_symbols[i].name,
			&(vrml_v2_symbols[i]));
	}
}

static gboolean vrml_v2_handler(GScanner *gscanner, gpointer user_data)
{
	GTokenType tt;
	VrmlGlobal *global = user_data;
	VrmlV2Data *v2data = global->opaque;
	VrmlV2Symbol *symbol;

	while(!g_scanner_eof(gscanner)) {
		tt = g_scanner_get_next_token(gscanner);
		switch(tt) {
			case G_TOKEN_SYMBOL:
				symbol = gscanner->value.v_symbol;
#if DEBUG > 0
				g_debug("\\%s[%s]", debug_pad(global->level), symbol->name);
#endif
				break;

			case G_TOKEN_IDENTIFIER:
#if DEBUG > 0
				g_debug("[%d/%d] unhandled identifier: %s",
					global->level, v2data->listlevel,
					gscanner->value.v_identifier);
#endif
				break;

			case G_TOKEN_LEFT_CURLY:
				global->level ++;
				break;

			case G_TOKEN_RIGHT_CURLY:
				global->level --;
				break;

			case G_TOKEN_LEFT_BRACE:
				v2data->listlevel ++;
				break;

			case G_TOKEN_RIGHT_BRACE:
				v2data->listlevel --;
				break;

			case G_TOKEN_COMMA:
				break;

			case '-':
				global->negate_next = TRUE;
				break;

			case G_TOKEN_STRING:
			case G_TOKEN_INT:
			case G_TOKEN_FLOAT:
				break;

			case G_TOKEN_EOF:
#if DEBUG > 0
				g_debug("EOF");
#endif
				global->line_offset += gscanner->line - 1;
				return TRUE;

			default:
				g_warning("unhandled token type: %d (%c) @ %d:%d",
					tt, (tt & 0x7f),
					global->line_offset + gscanner->line, gscanner->position);
		}
	}
	return FALSE;
}

gboolean vrml_v2_scan(VrmlGlobal *global)
{
	G3DStreamScanner *scanner;
	GScanner *gscanner;
	VrmlV2Data *v2data;
	gboolean retval;

	scanner = g3d_stream_scanner_new(global->stream);
	g3d_stream_scanner_set_handler(scanner, vrml_v2_handler, global);
	gscanner = g3d_stream_scanner_get_gscanner(scanner);

	v2data = g_new0(VrmlV2Data, 1);
	v2data->liststack = g_queue_new();
	v2data->objectstack = g_queue_new();
	global->opaque = v2data;

	vrml_v2_init_gscanner(global, gscanner);
	retval =  g3d_stream_scanner_run(scanner);

	g_queue_free(v2data->liststack);
	g_queue_free(v2data->objectstack);
	g_free(v2data);

	return retval;
}

