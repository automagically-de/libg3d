#include <g3d/types.h>
#include <g3d/stream-scanner.h>
#include <g3d/debug.h>

#include "imp_vrml.h"

typedef enum {
	VRML_V1_SCOPE_DEFAULT,
	VRML_V1_SCOPE_VALUE,
	VRML_V1_SCOPE_LIST,
	VRML_V1_SCOPE_DEF,
	VRML_V1_SCOPE_USE,

	VRML_V1_SCOPE_UNKNOWN
} VrmlScope;

typedef struct {
	const gchar *name;
	VrmlScope scope;
	VrmlScope toscope;
	guint32 n_values;
	guint32 flags;
} VrmlSymbol;

static VrmlSymbol vrml_symbols[] = {
	{ "Cone",             VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Coordinate3",      VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Cube",             VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Cylinder",         VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Group",            VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "IndexedFaceSet",   VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "IndexedLineSet",   VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Info",             VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Material",         VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "MaterialBinding",  VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "MatrixTransform",  VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Normal",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "NormalBinding",    VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Separator",        VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "ShapeHints",       VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Sphere",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "SpotLight",        VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Texture2",         VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Texture2Transform",VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Transform",        VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },
	{ "Translation",      VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEFAULT, 0 },

	{ "ambientColor",     VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "bottomRadius",     VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "center",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "color",            VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "coordIndex",       VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "creaseAngle",      VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "cutOffAngle",      VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "depth",            VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "diffuseColor",     VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "direction",        VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "dropOffRate",      VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "emissiveColor",    VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "faceType",         VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "filename",         VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "height",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "intensity",        VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "location",         VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "materialIndex",    VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "matrix",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   16 },
	{ "normalIndex",      VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "on",               VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "point",            VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3, 1 },
	{ "radius",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "rotation",         VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   4 },
	{ "scaleFactor",      VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   2 },
	{ "shapeType",        VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "shininess",        VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "specularColor",    VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "string",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "textureCoordIndex",VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "transparency",     VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "value",            VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "vector",           VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   3 },
	{ "vertexOrdering",   VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },
	{ "width",            VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_VALUE,   1 },

	{ "DEF",              VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_DEF,     0 },
	{ "USE",              VRML_V1_SCOPE_DEFAULT, VRML_V1_SCOPE_USE,     0 },
	{ NULL }
};

static void vrml_v1_init_gscanner(VrmlGlobal *global, GScanner *gscanner)
{
	gint32 i;

	gscanner->config->cset_identifier_nth =
		G_CSET_a_2_z "_0123456789:+" G_CSET_A_2_Z G_CSET_LATINS G_CSET_LATINC;

	global->scope = VRML_V1_SCOPE_DEFAULT;
	g_scanner_set_scope(gscanner, VRML_V1_SCOPE_DEFAULT);

	for(i = 0; vrml_symbols[i].name != NULL; i ++) {
		g_scanner_scope_add_symbol(gscanner,
			vrml_symbols[i].scope, vrml_symbols[i].name,
			&(vrml_symbols[i]));
	}
}

static void vcnt_dec(VrmlGlobal *global, GScanner *gscanner)
{
	if(global->scope == VRML_V1_SCOPE_VALUE) {
		if(global->value_counter == 0)
#if DEBUG > 0
			g_debug("vcnt: %d", global->value_counter);
#endif
		if(global->value_counter > 0)
			global->value_counter --;
		if(!global->in_list && (global->value_counter == 0)) {
			global->scope = VRML_V1_SCOPE_DEFAULT;
			g_scanner_set_scope(gscanner, global->scope);
		}
	}
}

static void set_num(VrmlProperty *property, GTokenType tt,
	gint32 i_list, gint32 i_val, gint32 vi, G3DFloat vf)
{
	g_return_if_fail(property != NULL);
	g_return_if_fail(i_list >= 0);

	property->n_items = i_list + 1;
	if((i_list + i_val) == 0) {
		property->type = tt;
	}

	g_return_if_fail(tt == property->type);

	switch(tt) {
		case G_TOKEN_INT:
#if DEBUG > 1
			g_debug("realloc %d * %d * INT", property->n_items,
				property->n_per_value);
#endif
			property->u.ip = g_realloc(property->u.ip,
				property->n_items * property->n_per_value * sizeof(gint32));
			property->u.ip[i_list * property->n_per_value + i_val] = vi;
			break;
		case G_TOKEN_FLOAT:
			property->u.fp = g_realloc(property->u.fp,
				property->n_items * property->n_per_value * sizeof(G3DFloat));
			property->u.fp[i_list * property->n_per_value + i_val] = vf;
			break;
		default:
			break;
	}
}

static gboolean vrml_v1_handler(GScanner *gscanner, gpointer user_data)
{
	GTokenType tt;
	VrmlSymbol *symbol = NULL;
	VrmlGlobal *global = user_data;
	VrmlNode *node = NULL;
	VrmlProperty *property;

	while(!g_scanner_eof(gscanner)) {
		tt = g_scanner_get_next_token(gscanner);
	
#if DEBUG > 1
		g_debug("token type: %d", tt);
#endif

		switch(tt) {
			case G_TOKEN_SYMBOL:
				symbol = gscanner->value.v_symbol;
				global->property = NULL;
				global->scope = symbol->toscope;
				global->value_counter = symbol->n_values;
				global->value_reload = global->value_counter;
				g_scanner_set_scope(gscanner, global->scope);
				if(global->scope == VRML_V1_SCOPE_DEFAULT) {
#if DEBUG > 0
					g_debug("\\%s[%s] %s", debug_pad(global->level),
						symbol->name,
						(global->defname ? global->defname : "(unnamed)"));
#endif
					global->current_group = symbol->name;
				} else if(global->scope == VRML_V1_SCOPE_VALUE) {
#if DEBUG > 0
					g_debug("\\%s[%s] %d value(s)", debug_pad(global->level),
						symbol->name, global->value_counter);
#endif
					global->current_option = symbol->name;
					global->i_list = 0;
					global->i_val = 0;
					if(node) {
						property = g_new0(VrmlProperty, 1);
						property->n_per_value = symbol->n_values;
						g_hash_table_insert(node->properties,
							(gchar *)symbol->name, property);
						global->property = property;
					}
					gscanner->config->int_2_float = (symbol->flags & 1);
				}
				if(global->defname) {
					g_free(global->defname);
					global->defname = NULL;
				}
				break;
			case G_TOKEN_IDENTIFIER:
				if(global->scope == VRML_V1_SCOPE_DEF) {
					if(global->defname) {
#if DEBUG > 0
						g_debug("FIXME: DEF name already set (%s)",
							global->defname);
#endif
						g_free(global->defname);
					}
					global->scope = VRML_V1_SCOPE_DEFAULT;
					g_scanner_set_scope(gscanner, global->scope);
					global->defname = g_strdup(gscanner->value.v_identifier);
				} else if(global->scope == VRML_V1_SCOPE_USE) {
					global->scope = VRML_V1_SCOPE_DEFAULT;
					g_scanner_set_scope(gscanner, global->scope);
				} else if(global->scope == VRML_V1_SCOPE_VALUE) {
					;
				} else {
					g_debug("[%d] identifier: %s", global->level,
						gscanner->value.v_identifier);
				}
				vcnt_dec(global, gscanner);
				break;
			case G_TOKEN_LEFT_CURLY:
				global->level ++;
				node = g_new0(VrmlNode, 1);
				node->name = symbol->name;
				node->properties = g_hash_table_new(g_str_hash, g_str_equal);
				g_queue_push_head(global->stack, node);
				break;
			case G_TOKEN_RIGHT_CURLY:
				node = g_queue_pop_head(global->stack);
				if(node) {
					g_hash_table_destroy(node->properties);
				}
				global->level --;
				break;
			case G_TOKEN_LEFT_BRACE:
#if DEBUG > 0
				g_debug("|%s[", debug_pad(global->level + 1));
#endif
				global->list_item_count = 1;
				global->in_list = TRUE;
				break;
			case G_TOKEN_RIGHT_BRACE:
#if DEBUG > 0
				g_debug("|%s] (%d x %d)", debug_pad(global->level + 1),
					global->list_item_count, global->value_reload);
#endif
				global->in_list = FALSE;
				global->scope = VRML_V1_SCOPE_DEFAULT;
				g_scanner_set_scope(gscanner, global->scope);
				break;
			case G_TOKEN_COMMA:
				if(global->in_list) {
					global->list_item_count ++;
					if(global->value_counter > 0) {
						g_warning("VRML: %s: wrong parameter count? (%d/%d)",
							global->current_option, global->value_counter,
							global->value_reload);
					}
					global->value_counter = global->value_reload;
					global->i_list ++;
					global->i_val = 0;
				}
				break;
			case '-':
				global->negate_next = TRUE;
				break;

			case G_TOKEN_STRING:
				g_debug("string: %s", gscanner->value.v_string);
				vcnt_dec(global, gscanner);
				global->i_val ++;
				break;

			case G_TOKEN_INT:
				property = global->property;
				set_num(property, G_TOKEN_INT, global->i_list, global->i_val,
					gscanner->value.v_int *
					(global->negate_next ? -1 : 1),
					0.0);
				global->negate_next = FALSE;
				vcnt_dec(global, gscanner);
				global->i_val ++;
				break;

			case G_TOKEN_FLOAT:
				property = global->property;
				set_num(property, G_TOKEN_FLOAT, global->i_list, global->i_val,
					0.0,
					gscanner->value.v_float *
					(global->negate_next ? -1.0 : 1.0));
				global->negate_next = FALSE;
				vcnt_dec(global, gscanner);
				global->i_val ++;
				break;

			case G_TOKEN_EOF:
#if DEBUG > 1
				g_debug("EOF");
#endif
				return TRUE;

			default:
				g_warning("unhandled token type: %d (%c) @ %d:%d",
					tt, (tt & 0x7f),
					gscanner->line, gscanner->position);
		}
	}
	return TRUE;
}

gboolean vrml_v1_scan(VrmlGlobal *global)
{
	G3DStreamScanner *scanner;
	GScanner *gscanner;

	scanner = g3d_stream_scanner_new(global->stream);
	g3d_stream_scanner_set_handler(scanner, vrml_v1_handler, global);
	gscanner = g3d_stream_scanner_get_gscanner(scanner);
	vrml_v1_init_gscanner(global, gscanner);
#if 1
	g3d_stream_scanner_run(scanner);
#endif
	return FALSE;
}
