/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2010  Markus Dahms <mad@automagically.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <g3d/types.h>
#include <g3d/debug.h>
#include <g3d/stream.h>

#include "javaser.h"

gboolean javaser_read_field_data(G3DStream *stream, JavaserData *global,
	JavaserField *field, guint32 level);
gpointer javaser_read_level(G3DStream *stream, JavaserData *global,
	guint32 level);

gchar *javaser_read_string(G3DStream *stream)
{
	guint16 len;
	gchar *str;

	len = g3d_stream_read_int16_be(stream);
	g_assert(len != 0xFFFF);
	str = g_new0(gchar, len + 1);
	g3d_stream_read(stream, str, len);
	return str;
}

JavaserField *javaser_read_field(G3DStream *stream, JavaserData *global,
	guint32 level)
{
	JavaserField *field;

	field = g_new0(JavaserField, 1);
	field->typecode = g3d_stream_read_int8(stream);
	field->name = javaser_read_string(stream);
	g_debug("%sfield: %s (%c)", debug_pad(level),
		field->name, field->typecode);
	switch (field->typecode) {
		case TC_FIELD_ARRAY:
		case TC_FIELD_OBJECT:
			javaser_read_field_data(stream, global, field, level + 1);
			break;
		default:
			break;
	}
	return field;
}

gboolean javaser_new_handle(JavaserData *global, gpointer obj,
	const gchar *typehint, guint32 level)
{
	global->handles = g_slist_append(global->handles, obj);
	g_debug("%sassigned handle %d to new %s", debug_pad(level),
		g_slist_length(global->handles) - 1, typehint);
	return TRUE;
}

gboolean javaser_read_field_data(G3DStream *stream, JavaserData *global,
	JavaserField *field, guint32 level)
{

	field->datatypecode = g3d_stream_read_int8(stream);
	switch (field->datatypecode) {
		case TC_REFERENCE:
			field->data.reference = g3d_stream_read_int32_be(stream);
#if DEBUG > 1
			g_debug("%sref: 0x%08x", debug_pad(level), field->data.reference);
#endif
			return TRUE;
		case TC_STRING:
			/* newHandle */
			/* (utf) */
			field->data.className = javaser_read_string(stream);
			javaser_new_handle(global, field->data.className, "STRING", level);
#if DEBUG > 1
			g_debug("%sstring: %s", debug_pad(level), field->data.className);
#endif
			return TRUE;
		default:
			g_error("javaser_read_field_data: unexpected tc=%d",
				field->datatypecode);
			return FALSE;
	}
}

gboolean javaser_read_class_data(G3DStream *stream, JavaserData *global,
	JavaserObject *object, guint32 level)
{
	JavaserField *field;
	JavaserProperty *property;
	GSList *fitem;
	guint32 u;
	guint64 llu;

	g_assert(object->jclass);
	g_assert(object->jclass->superclass == NULL);
	g_assert(object->jclass->flags & SC_SERIALIZABLE);

	g_debug("%sproperties (%s):", debug_pad(level), object->jclass->name);
	for (fitem = object->jclass->fields; fitem != NULL; fitem = fitem->next) {
		field = fitem->data;
		g_debug("%sproperty '%s' (%c)", debug_pad(level + 1), field->name,
			field->typecode);
	}

	for (fitem = object->jclass->fields; fitem != NULL; fitem = fitem->next) {
		field = fitem->data;
		property = g_new0(JavaserProperty, 1);
		property->name = field->name;
		switch (field->typecode) {
			case TC_FIELD_BOOLEAN:
				g_value_init(&(property->value), G_TYPE_BOOLEAN);
				g_value_set_boolean(&(property->value),
					g3d_stream_read_int8(stream));
				g_debug("%s'%s' = %s", debug_pad(level), field->name,
					g_value_get_boolean(&(property->value)) ? "true":"false");
				break;
			case TC_FIELD_INTEGER:
				u = g3d_stream_read_int32_be(stream);
				g_value_init(&(property->value), G_TYPE_UINT);
				g_value_set_uint(&(property->value), u);
				g_debug("%s'%s' = 0x%08x (%u)", debug_pad(level), field->name,
					u, u);
				break;
			case TC_FIELD_LONG:
				llu = g3d_stream_read_int32_be(stream);
				llu <<= 32;
				llu += g3d_stream_read_int32_be(stream);
				g_value_init(&(property->value), G_TYPE_INT64);
				g_value_set_int64(&(property->value), llu);
#if DEBUG > 0
				g_debug("%s'%s' = 0x%08x%08x", debug_pad(level), field->name,
					(guint32)(llu >> 32), (guint32)(llu & 0xFFFFFFFF));
#endif
				break;
			case TC_FIELD_FLOAT:
				g_value_init(&(property->value), G_TYPE_FLOAT);
				g_value_set_float(&(property->value),
					g3d_stream_read_float_be(stream));
				g_debug("%s'%s' = %0.4f", debug_pad(level), field->name,
					g_value_get_float(&(property->value)));
				break;
			case TC_FIELD_OBJECT:
				g_debug("%s'%s' = [object]", debug_pad(level), field->name);
				g_value_init(&(property->value), G_TYPE_POINTER);
				g_value_set_pointer(&(property->value),
					javaser_read_level(stream, global, level + 1));
				break;
			case TC_FIELD_ARRAY:
				g_debug("%s'%s' = [array]", debug_pad(level), field->name);
				g_value_init(&(property->value), G_TYPE_POINTER);
				g_value_set_pointer(&(property->value),
					javaser_read_level(stream, global, level + 1));
				break;
			default:
				g_error("unhandled property type: 0x%02X @ 0x%08lx",
					field->typecode,
					g3d_stream_tell(stream));
				return FALSE;
		}
	}

	if (object->jclass->flags & SC_WRITE_METHOD) {
		/* objectAnnotation */
		javaser_read_level(stream, global, level + 1);
	}
	return TRUE;
}

gpointer javaser_read_level(G3DStream *stream, JavaserData *global,
	guint32 level)
{
	JavaserObject *object;
	JavaserClass *jclass;
	JavaserArray *array;
	JavaserField *field;
	gint32 i;
	guint32 ref;
	guint16 nfields, len;
	guint8 tc, *buffer;
	gpointer current = NULL;

	while (!g3d_stream_eof(stream)) {
		tc = g3d_stream_read_int8(stream);
		g_debug("%stc: 0x%02x @0x%08lx", debug_pad(level), tc,
			g3d_stream_tell(stream));
		switch (tc) {
			case TC_CLASSDESC:
				jclass = g_new0(JavaserClass, 1);
				/* className */
				jclass->name = javaser_read_string(stream);
				g_debug("%sclass name: %s", debug_pad(level), jclass->name);
				/* serialVersionUID */
				g3d_stream_read_int32_be(stream);
				g3d_stream_read_int32_be(stream);
				/* newHandle */
				javaser_new_handle(global, jclass, "CLASS", level);
				/* classDescInfo */
				/* - classDescFlags */
				jclass->flags = g3d_stream_read_int8(stream);
				g_debug("%sclass flags: 0x%02x", debug_pad(level),
					jclass->flags);
				/* - fields */
				nfields = g3d_stream_read_int16_be(stream);
				for (i = 0; i < nfields; i ++) {
					jclass->fields = g_slist_append(jclass->fields,
						javaser_read_field(stream, global, level + 1));
				}
				global->classes = g_slist_append(global->classes, jclass);
				/* - classAnnotation */
				javaser_read_level(stream, global, level + 1);
				/* - superClassDesc */
				jclass->superclass =
					javaser_read_level(stream, global, level + 1);
				return jclass;
			case TC_BLOCKDATA:
				len = g3d_stream_read_int8(stream);
				buffer = g_new0(guint8, len);
				g3d_stream_read(stream, buffer, len);
				g_free(buffer);
				g_debug("%sblockdata of %d bytes", debug_pad(level), len);
				break;
			case TC_ENDBLOCKDATA:
				return NULL;
			case TC_NULL:
				return NULL;
			case TC_OBJECT:
				object = g_new0(JavaserObject, 1);
				object->properties = g_hash_table_new(g_str_hash, g_str_equal);
				global->objects = g_slist_append(global->objects, object);
				global->current_object = object;
				/* classDesc */
				object->jclass = javaser_read_level(stream, global, level + 1);
				/* newHandle */
				javaser_new_handle(global, object, "OBJECT", level);
				/* classData[] */
				javaser_read_class_data(stream, global, object, level + 1);
				return object;
			case TC_REFERENCE:
				ref = g3d_stream_read_int32_be(stream);
				if (ref & 0x7E0000) {
					ref &= 0xFFFF;
					current = g_slist_nth_data(global->handles, ref);
					if (!current) {
						g_debug("%sbroken reference %d", debug_pad(level),
							ref);
						return NULL;
					}
					g_debug("%sreference to 0%p (%d)", debug_pad(level),
						current, ref);
					return current;
				}
				g_debug("%sinvalid reference (0x%08x)", debug_pad(level), ref);
				return NULL;
			case TC_ENUM:
				/* classDesc */
				current = javaser_read_level(stream, global, level + 1);
				/* newHandle */
				/* enumConstName */
				field = g_new0(JavaserField, 1);
				javaser_read_field_data(stream, global, field, level + 1);
				g_debug("%senum %s", debug_pad(level), field->data.className);
				javaser_new_handle(global, field, "ENUM", level);
				return NULL;
			case TC_ARRAY:
				array = g_new0(JavaserArray, 1);
				/* classDesc */
				array->jclass = javaser_read_level(stream, global, level + 1);
				/* newHandle */
				javaser_new_handle(global, array, "ARRAY", level);
				/* size */
				array->nitems = g3d_stream_read_int32_be(stream);
				g_debug("%sarray with %d items", debug_pad(level),
					array->nitems);
				/* values */
				for (i = 0; i < array->nitems; i ++) {
					object = g_new0(JavaserObject, 1);
					object->jclass = array->jclass;
					object->properties = g_hash_table_new(
						g_str_hash, g_str_equal);
					array->items = g_slist_append(array->items, object);
					javaser_read_class_data(stream, global, object, level + 1);
				}
				return array;
			case TC_STRING:
				current = javaser_read_string(stream);
				javaser_new_handle(global, current, "STRING", level);
				g_debug("%sstring: %s", debug_pad(level), (gchar *)current);
				return current;
			default:
				/* unhandled tc */
				g_error("unhandled tc=0x%02x", tc);
				return NULL;
		}
	}
	return NULL;
}

JavaserClass *javaser_parse(G3DStream *stream)
{
	JavaserData *global;
	guint16 magic, version;

	magic = g3d_stream_read_int16_be(stream);
	if (magic != 0xACED) {
		g_error("java serialization magic not 0xACED (0x%04X)", magic);
		return NULL;
	}
	version = g3d_stream_read_int16_be(stream);
	if (version != 5) {
		g_error("only java serialization version 5 supported");
		return NULL;
	}

	global = g_new0(JavaserData, 1);
	javaser_read_level(stream, global, 0);

	return NULL;
}


