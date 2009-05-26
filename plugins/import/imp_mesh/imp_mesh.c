/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005-2009  Markus Dahms <mad@automagically.de>

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

#include <string.h>

#include <g3d/types.h>
#include <g3d/stream.h>
#include <g3d/debug.h>
#include <g3d/face.h>
#include <g3d/material.h>

#include "imp_mesh.h"

static void mesh_debug_chunk(MeshGlobal *global, MeshLocal *local);
static gchar *mesh_read_to_0a(MeshGlobal *global);
static gboolean mesh_read_container(MeshGlobal *global, MeshLocal *local);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar *version;
	guint16 headerid;
	MeshGlobal *global;
	MeshLocal *local;

	headerid = g3d_stream_read_int16_le(stream);
	if(headerid != 0x1000) {
		g_warning("%s is not a valid mesh file", g3d_stream_get_uri(stream));
		return FALSE;
	}

	global = g_new0(MeshGlobal, 1);
	global->context = context;
	global->stream = stream;
	global->model = model;

	global->defmat = g3d_material_new();
	global->defmat->name = g_strdup("(default material)");

	local = g_new0(MeshLocal, 1);
	local->nb = g3d_stream_size(global->stream);
	local->nb -= 2;

	version = mesh_read_to_0a(global);
	if(!version || strncmp(version, "[MeshSerializer_v", 17)) {
		if(version)
			g_free(version);
		g_free(global);
		g_warning("%s is not a valid mesh file", g3d_stream_get_uri(stream));
		return FALSE;
	}

	global->major = version[17] - '0';
	global->minor = (version[19] - '0') * 10 + (version[20] - '0');
	local->nb -= (strlen(version) + 1);
	g_free(version);

	g_debug("version: %d.%d", global->major, global->minor);

	if(!mesh_read_container(global, local)) {
		g_free(global);
		g_free(local);
		g_warning("%s: error reading mesh", g3d_stream_get_uri(stream));
		return FALSE;
	}

	g_free(local);
	g_free(global);

	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Ogre mesh files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("mesh", ":", 0);
}

/*****************************************************************************/

static void mesh_debug_chunk(MeshGlobal *global, MeshLocal *local)
{
#if DEBUG > 0
	g_debug("\\%s[0x%04x] @ 0x%08x (%d bytes)", debug_pad(local->level),
		local->chunkid,
		(guint32)g3d_stream_tell(global->stream) - 6,
		local->nb);
#endif
}

static gchar *mesh_read_to_0a(MeshGlobal *global)
{
	gchar *buffer = g_new0(gchar, 1000), *rbuf;
	gint32 i;
	
	for(i = 0; i < 1000; i ++) {
		buffer[i] = g3d_stream_read_int8(global->stream);
		if(buffer[i] == '\x0a') {
			buffer[i] = '\0';
			rbuf = g_strdup(buffer);
			g_free(buffer);
			return rbuf;
		}
	}
	g_free(buffer);
	return NULL;
}

/* MESH */
static gboolean mesh_read_0x3000(MeshGlobal *global, MeshLocal *local)
{
	g3d_stream_read_int8(global->stream); /* boolean skelAnim */
	local->nb -= 1;

	local->object = g_new0(G3DObject, 1);
	global->model->objects = g_slist_append(global->model->objects,
		local->object);

	return mesh_read_container(global, local);
}

/* SUBMESH */
static gboolean mesh_read_0x4000(MeshGlobal *global, MeshLocal *local)
{
	gchar *matname;
	gboolean shared_verts, index_32bit;
	gint32 i;

	matname = mesh_read_to_0a(global);
	local->nb -= (strlen(matname) + 1);
#if DEBUG > 0
	g_debug("|%smatname: %s", debug_pad(local->level + 1), matname);
#endif

	shared_verts = g3d_stream_read_int8(global->stream);
	local->nindices = g3d_stream_read_int32_le(global->stream);
	index_32bit = g3d_stream_read_int8(global->stream);
	local->nb -= 6;

	local->indices = g_new0(guint32, local->nindices);

	for(i = 0; i < local->nindices; i ++) {
		if(index_32bit) {
			local->indices[i] = g3d_stream_read_int32_le(global->stream);
			local->nb -= 4;
		} else {
			local->indices[i] = g3d_stream_read_int16_le(global->stream);
			local->nb -= 2;
		}
	}

	return mesh_read_container(global, local);
}

/* SUBMESH_OPERATION */
static gboolean mesh_read_0x4010(MeshGlobal *global, MeshLocal *local)
{
	g3d_stream_read_int16_le(global->stream); /* operation type */
	local->nb -= 2;

	return TRUE;
}

/* GEOMETRY */
static gboolean mesh_read_0x5000(MeshGlobal *global, MeshLocal *local)
{
	guint32 vcount;

	vcount = g3d_stream_read_int32_le(global->stream);
	local->nb -= 4;

	if(local->object) {
		local->object->vertex_count = vcount;
		local->object->vertex_data = g_new0(G3DVector, 3 * vcount);
	}

	return mesh_read_container(global, local);
}

/* GEOMETRY_VERTEX_DECLARATION */
static gboolean mesh_read_0x5100(MeshGlobal *global, MeshLocal *local)
{
	return mesh_read_container(global, local);
}

/* GEOMETRY_VERTEX_ELEMENT */
static gboolean mesh_read_0x5110(MeshGlobal *global, MeshLocal *local)
{
	g3d_stream_read_int16_le(global->stream); /* source */
	g3d_stream_read_int16_le(global->stream); /* type */
	g3d_stream_read_int16_le(global->stream); /* semantic */
	g3d_stream_read_int16_le(global->stream); /* offset */
	g3d_stream_read_int16_le(global->stream); /* index */
	local->nb -= 10;

	return TRUE;
}

/* GEOMETRY_VERTEX_BUFFER */
static gboolean mesh_read_0x5200(MeshGlobal *global, MeshLocal *local)
{
	g3d_stream_read_int16_le(global->stream); /* bind index */
	g3d_stream_read_int16_le(global->stream); /* vertex size */
	local->nb -= 4;

	return mesh_read_container(global, local);
}

/* GEOMETRY_VERTEX_BUFFER_DATA */
static gboolean mesh_read_0x5210(MeshGlobal *global, MeshLocal *local)
{
	gint32 i, j;
	guint32 n;
	guint8 fperv = 8;

	g_return_val_if_fail(local->object != NULL, TRUE);

	n = local->nb / (4 * 8);
	if(local->object->vertex_count < n) {
		n = local->nb / (4 * 12);
		fperv = 12;
		if(local->object->vertex_count < n) {
			g_warning("0x5210: n = %u, o = %p (%d)", n,
				(gpointer)local->object,
				local->object->vertex_count);
			return TRUE;
		}
	}

	for(i = 0; i < n; i ++) {
		for(j = 0; j < 3; j ++) {
			local->object->vertex_data[i * 3 + j] =
				g3d_stream_read_float_le(global->stream);
		}
		for(j = 0; j < 3; j ++) {
			/* shared? */
			g3d_stream_read_float_le(global->stream);
		}
		for(j = 0; j < 2; j ++) {
			/* tex vertex */
			g3d_stream_read_float_le(global->stream);
		}
		if(fperv == 12) {
			for(j = 0; j < 4; j ++) {
				/* unknown */
				g3d_stream_read_float_le(global->stream);
			}
		}
		local->nb -= fperv * 4;
	}

	if(local->nb) {
		g3d_stream_skip(global->stream, local->nb);
		local->nb = 0;
	}

	return TRUE;
}

/* MESH_BOUNDS */
static gboolean mesh_read_0x9000(MeshGlobal *global, MeshLocal *local)
{
	g3d_stream_read_float_le(global->stream); /* minx */
	g3d_stream_read_float_le(global->stream); /* miny */
	g3d_stream_read_float_le(global->stream); /* minz */
	g3d_stream_read_float_le(global->stream); /* maxx */
	g3d_stream_read_float_le(global->stream); /* maxy */
	g3d_stream_read_float_le(global->stream); /* maxz */
	g3d_stream_read_float_le(global->stream); /* radius */
	local->nb -= 28;
	return TRUE;
}

/* SUBMESH_NAME_TABLE */
static gboolean mesh_read_0xA000(MeshGlobal *global, MeshLocal *local)
{
	return mesh_read_container(global, local);
}

/* SUBMESH_NAME_TABLE_ELEMENT */
static gboolean mesh_read_0xA100(MeshGlobal *global, MeshLocal *local)
{
	guint16 index;
	gchar *name;

	index = g3d_stream_read_int16_le(global->stream);
	name = mesh_read_to_0a(global);
	local->nb -= (strlen(name) + 1 + 2);
#if DEBUG > 0
	g_debug("|%s[%02d] %s", debug_pad(local->level + 1), index, name);
#endif

	return TRUE;
}

/* EDGE_LISTS */
static gboolean mesh_read_0xB000(MeshGlobal *global, MeshLocal *local)
{
	return mesh_read_container(global, local);
}

/* EDGE_LIST_LOD */
static gboolean mesh_read_0xB100(MeshGlobal *global, MeshLocal *local)
{
	G3DFace *face;
	guint16 index;
	gboolean manual, closed;
	guint32 ntris, nedgegrps;
	gint32 i;

	g_return_val_if_fail(local->object != NULL, TRUE);

	index = g3d_stream_read_int16_le(global->stream);
	manual = g3d_stream_read_int8(global->stream);
	local->nb -= 3;

	if(!manual) {
		closed = g3d_stream_read_int8(global->stream);
		ntris = g3d_stream_read_int32_le(global->stream);
		nedgegrps = g3d_stream_read_int32_le(global->stream);
		local->nb -= 9;

		for(i = 0; i < ntris; i ++) {
			g3d_stream_read_int32_le(global->stream); /* index set */
			g3d_stream_read_int32_le(global->stream); /* vertex set */

			face = g3d_face_new_tri(global->defmat,
				g3d_stream_read_int32_le(global->stream),
				g3d_stream_read_int32_le(global->stream),
				g3d_stream_read_int32_le(global->stream));
			local->object->faces = g_slist_prepend(local->object->faces, face);

			g3d_stream_read_int32_le(global->stream); /* shared[0] */
			g3d_stream_read_int32_le(global->stream); /* shared[1] */
			g3d_stream_read_int32_le(global->stream); /* shared[2] */
			g3d_stream_read_float_le(global->stream); /* normal[0] */
			g3d_stream_read_float_le(global->stream); /* normal[1] */
			g3d_stream_read_float_le(global->stream); /* normal[2] */
			g3d_stream_read_float_le(global->stream); /* normal[3] */
			local->nb -= 48;
		}

		return mesh_read_container(global, local);
	}
	return TRUE;
}

/* EDGE_GROUP */
static gboolean mesh_read_0xB110(MeshGlobal *global, MeshLocal *local)
{
	guint32 nedges;
	gint32 i;

	g3d_stream_read_int32_le(global->stream); /* vertex set */
	g3d_stream_read_int32_le(global->stream); /* tri start */
	g3d_stream_read_int32_le(global->stream); /* tri count */
	nedges = g3d_stream_read_int32_le(global->stream);
	local->nb -= 16;
	for(i = 0; i < nedges; i ++) {
		g3d_stream_read_int32_le(global->stream); /* triindex[0] */
		g3d_stream_read_int32_le(global->stream); /* triindex[1] */
		g3d_stream_read_int32_le(global->stream); /* vertindex[0] */
		g3d_stream_read_int32_le(global->stream); /* vertindex[1] */
		g3d_stream_read_int32_le(global->stream); /* sharedindex[0] */
		g3d_stream_read_int32_le(global->stream); /* sharedindex[1] */
		g3d_stream_read_int8(global->stream); /* degenerate */
		local->nb -= 25;
	}
	return TRUE;
}

static gboolean mesh_read_container(MeshGlobal *global, MeshLocal *local)
{
	MeshLocal *sublocal;
	gboolean retval = TRUE;

	while(local->nb >= 6) {
		sublocal = g_new0(MeshLocal, 1);
		sublocal->level = local->level + 1;
		sublocal->object = local->object;
		sublocal->indices = local->indices;
		sublocal->nindices = sublocal->nindices;
		sublocal->chunkid = g3d_stream_read_int16_le(global->stream);
		sublocal->nb = g3d_stream_read_int32_le(global->stream) - 6;

		if((sublocal->chunkid == 0) || (sublocal->nb < 0)) {
			/* FIXME: still something wrong... */
			local->nb = 0;
			g_free(sublocal);
			return TRUE;
		}

		local->nb -= 6;
		local->nb -= sublocal->nb;

		mesh_debug_chunk(global, sublocal);
#if DEBUG > 2
		g_debug("%s([%d]: %d bytes remaining)", debug_pad(local->level + 1),
			local->level, local->nb);
#endif

		switch(sublocal->chunkid) {
			case 0x3000: /* MESH */
				retval = mesh_read_0x3000(global, sublocal);
				break;
			case 0x4000: /* SUBMESH */
				retval = mesh_read_0x4000(global, sublocal);
				break;
			case 0x4010: /* SUBMESH_OPERATION */
				retval = mesh_read_0x4010(global, sublocal);
				break;
			case 0x5000: /* GEOMETRY */
				retval = mesh_read_0x5000(global, sublocal);
				break;
			case 0x5100: /* GEOMETRY_VERTEX_DECLARATION */
				retval = mesh_read_0x5100(global, sublocal);
				break;
			case 0x5110: /* GEOMETRY_VERTEX_ELEMENT */
				retval = mesh_read_0x5110(global, sublocal);
				break;
			case 0x5200: /* GEOMETRY_VERTEX_BUFFER */
				retval = mesh_read_0x5200(global, sublocal);
				break;
			case 0x5210: /* GEOMETRY_VERTEX_BUFFER_DATA */
				retval = mesh_read_0x5210(global, sublocal);
				break;
			case 0x9000: /* MESH_BOUNDS */
				retval = mesh_read_0x9000(global, sublocal);
				break;
			case 0xA000: /* SUBMESH_NAME_TABLE */
				retval = mesh_read_0xA000(global, sublocal);
				break;
			case 0xA100: /* SUBMESH_NAME_TABLE_ELEMENT */
				retval = mesh_read_0xA100(global, sublocal);
				break;
			case 0xB000: /* EDGE_LISTS */
				retval = mesh_read_0xB000(global, sublocal);
				break;
			case 0xB100: /* EDGE_LIST_LOD */
				retval = mesh_read_0xB100(global, sublocal);
				break;
			case 0xB110: /* EDGE_GROUP */
				retval = mesh_read_0xB110(global, sublocal);
				break;
				
			default:
#if DEBUG > 0
				g_debug("skipping 0x%04x chunk", sublocal->chunkid);
#endif
		}
		if(sublocal->nb > 0) {
#if DEBUG > 0
				g_debug("skipping %d bytes (%d)", sublocal->nb, local->level);
#endif
			g3d_stream_skip(global->stream, sublocal->nb);
		}

		if(sublocal->indices && !local->indices)
			g_free(sublocal->indices);
		g_free(sublocal);

		if(retval == FALSE)
			return FALSE;
	}
	return TRUE;
}
