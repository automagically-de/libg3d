#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <g3d/types.h>
#include <g3d/debug.h>
#include <g3d/stream.h>
#include <g3d/vector.h>
#include <g3d/material.h>

#include "imp_3dm_callbacks.h"
#include "imp_3dm_object_types.h"

typedef struct {
	guint32 otype;
	G3DObject *object;
} TdmObjectRecord;

static TdmObjectTypeInfo *tdm_otype_get_info(guint32 code)
{
	gint32 i;

	if(code == 0xFFFFFFFF)
		return NULL;

	for(i = 0; tdm_object_types[i].code <= code; i ++)
		if(tdm_object_types[i].code == code)
			return &(tdm_object_types[i]);
	return NULL;
}

static gboolean tdm_read_chunk_version(TdmGlobal *global, TdmLocal *local)
{
	guint8 ver = g3d_stream_read_int8(global->stream);

	if(!ver)
		return FALSE;
	local->len -= 1;
	local->major_version = (ver >> 4) & 0x0F;
	local->minor_version = ver & 0x0F;

	return TRUE;
}

/* OpenNURBS class data */
gboolean tdm_cb_0x00027ffc(TdmGlobal *global, TdmLocal *local)
{
	TdmObjectRecord *obj = local->object;
	TdmObjectTypeInfo *tinfo;
	G3DMaterial *material;

	if(!obj) {
		g_warning("tdm_cb_0x00027FFC: expecting object...");
		return TRUE;
	}

	tinfo = tdm_otype_get_info(obj->otype);
	if(!tinfo) {
		g_warning("unknown object type 0x%08x", obj->otype);
		return TRUE;
	}

	local->level ++;
#if DEBUG > 0
	g_debug("\\%so: [0x%08x][%c]: %s", debug_pad(local->level),
		obj->otype,
		tinfo->callback ? 'f' : ' ',
		tinfo->description);
#endif
	obj->object = g_new0(G3DObject, 1);
	obj->object->name = g_strdup_printf("%s object @ 0x%08x",
		tinfo->description, (guint32)g3d_stream_tell(global->stream));
	global->model->objects = g_slist_append(global->model->objects,
		obj->object);

	material = g3d_material_new();
	material->name = g_strdup("(default material)");
	obj->object->materials = g_slist_append(obj->object->materials, material);

	if(tinfo->callback)
		tinfo->callback(global, local);

	return TRUE;
}

/* object record type */
gboolean tdm_cb_0x02000071(TdmGlobal *global, TdmLocal *local)
{
	TdmObjectRecord *obj = local->object;

	if(obj)
		obj->otype = local->data;
	return TRUE;
}

/* object record end */
gboolean tdm_cb_0x0200007f(TdmGlobal *global, TdmLocal *local)
{
	TdmObjectRecord *obj = local->object;

	if(obj) {
		g_free(obj);
	}
	return TRUE;
}

/* object record */
gboolean tdm_cb_0x20000070(TdmGlobal *global, TdmLocal *local)
{
	local->object = g_new0(TdmObjectRecord, 1);
	return TRUE;
}

/* object: mesh */
gboolean tdm_cb_o_0x00000020(TdmGlobal *global, TdmLocal *local)
{
	TdmObjectRecord *obj = local->object;
	G3DFace *face;
	gint32 i, j;
	guint32 vcount, fcount, isize, csize, crc;
	guint8 c;
	G3DStream *stream;
#ifdef HAVE_ZLIB
	G3DStream *zstream = NULL;
	goffset o_start = 0, o_end;
#endif

	if(!tdm_read_chunk_version(global, local))
		return FALSE;
	if((local->major_version != 3) && (local->major_version != 1))
		return TRUE;

	vcount = g3d_stream_read_int32_le(global->stream);
	fcount = g3d_stream_read_int32_le(global->stream);
	local->len -= 8;

	/* packed tex domain */
	g3d_stream_read_double_le(global->stream);
	g3d_stream_read_double_le(global->stream);
	g3d_stream_read_double_le(global->stream);
	g3d_stream_read_double_le(global->stream);
	local->len -= 32;
	/* srf domain */
	g3d_stream_read_double_le(global->stream);
	g3d_stream_read_double_le(global->stream);
	g3d_stream_read_double_le(global->stream);
	g3d_stream_read_double_le(global->stream);
	local->len -= 32;
	/* srf scale */
	g3d_stream_read_double_le(global->stream);
	g3d_stream_read_double_le(global->stream);
	local->len -= 16;
	/* vbox */
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	local->len -= 24;
	/* nbox */
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	local->len -= 24;
	/* tbox */
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	g3d_stream_read_float_le(global->stream);
	local->len -= 16;
	/* closed */
	g3d_stream_read_int32_le(global->stream);
	local->len -= 4;

#if DEBUG > 0
	g_debug("|%svcount = %u, fcount = %u", debug_pad(local->level),
		vcount, fcount);
#endif

	/* mesh parameters */
	c = g3d_stream_read_int8(global->stream);
	local->len -= 1;
	if(c) {
		return TRUE;
		/* TODO */
	}

	/* mesh curvature */
	for(i = 0; i < 4; i ++) {
		c = g3d_stream_read_int8(global->stream);
		local->len -= 1;
		if(c) {
			return TRUE;
			/* TODO */
		}
	}

	/* face array */
	isize = g3d_stream_read_int32_le(global->stream);
	local->len -= 4;
	for(i = 0; i < fcount; i ++) {
		face = g_new0(G3DFace, 1);
		face->material = g_slist_nth_data(obj->object->materials, 0);
		face->vertex_count = 4;
		face->vertex_indices = g_new0(guint32, 4);
		for(j = 0; j < 4; j ++) {
			switch(isize) {
				case 1:
					face->vertex_indices[j] =
						g3d_stream_read_int8(global->stream);
					local->len -= 1;
					break;
				case 2:
					face->vertex_indices[j] =
						g3d_stream_read_int16_le(global->stream);
					local->len -= 2;
					break;
				case 4:
					face->vertex_indices[j] =
						g3d_stream_read_int32_le(global->stream);
					local->len -= 4;
					break;
				default:
					g_warning("unsupported isize: %d", isize);
					return TRUE;
					break;
			} /* switch(isize) */
		} /* j := 0..3 */

		obj->object->faces = g_slist_prepend(obj->object->faces, face);
#if DEBUG > 0
		g_debug("|%s[face %04d] %u, %u, %u, %u", debug_pad(local->level), i,
			face->vertex_indices[0], face->vertex_indices[1],
			face->vertex_indices[2], face->vertex_indices[3]);
#endif
	} /* i := 0..fcount */

	/* vertex stuff */
	if(local->major_version == 1) {
		/* TODO */
		return TRUE;
	}

	if(vcount > 0) {
		csize = g3d_stream_read_int32_le(global->stream);
		local->len -= 4;

		crc = g3d_stream_read_int32_le(global->stream);
		local->len -= 4;

		c = g3d_stream_read_int8(global->stream);
		local->len -= 1;

		if(c) {
			/* compressed data */
#ifndef HAVE_ZLIB
			g_warning("no zlib support, unable to read deflated data");
			return TRUE;
#else
			o_start = g3d_stream_tell(global->stream);
#if DEBUG > 0
			g_debug("opening deflated data @ 0x%08x", (guint32)o_start);
#endif
			g3d_stream_read_int32_le(global->stream); /* tcode */
			csize = g3d_stream_read_int32_le(global->stream) - 4; /* size */
			zstream = g3d_stream_zlib_inflate_stream(global->stream, csize);
			stream = zstream;
#endif /* HAVE_ZLIB */
		} else {
			/* uncompressed data */
			stream = global->stream;
		}

		obj->object->vertex_count = vcount;
		obj->object->vertex_data = g3d_vector_new(3, vcount);
		for(i = 0; i < vcount; i ++) {
			for(j = 0; j < 3; j ++) {
				obj->object->vertex_data[i * 3 + j] =
					g3d_stream_read_float_le(stream);
				if(!c)
					local->len -= 4;
			}
#if DEBUG > 0
			g_debug("vertex %04d: %f, %f, %f", i,
				obj->object->vertex_data[i * 3 + 0],
				obj->object->vertex_data[i * 3 + 1],
				obj->object->vertex_data[i * 3 + 2]);
#endif
		}

		if(c) {
#ifdef HAVE_ZLIB
			g3d_stream_close(zstream);
			o_end = g3d_stream_tell(global->stream);
			local->len -= (o_end - o_start);
#endif /* HAVE_ZLIB */
		}
	} /* vcount > 0 */

	return TRUE;
}
