/* $Id$ */

/*
    libg3d - 3D object loading library

    Copyright (C) 2005, 2006  Markus Dahms <mad@automagically.de>

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

#include <stdio.h>
#include <string.h>

#include <g3d/types.h>
#include <g3d/material.h>
#include <g3d/texture.h>
#include <g3d/stream.h>
#include <g3d/iff.h>

#include "imp_md2_normals.h"

#define MD2_SKINNAMELEN 64

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	guint32 idid, idver, skinwidth, skinheight, framesize;
	guint32 numskins, numverts, numtexs, numfaces, numglcmds, numframes;
	guint32 offskins, offtexs, offfaces, offframes, offglcmds, offend;
	gfloat *texco = NULL, *normals;
	gchar **skinnames = NULL;
	int i;
	G3DObject *object;
	G3DMaterial *material;
	G3DImage *image = NULL;
	static guint32 tex_id = 1;

	idid = g3d_stream_read_int32_be(stream);
	if(idid != G3D_IFF_MKID('I','D','P','2')) {
		g_printerr("file '%s' is not a .md2 file\n", stream->uri);
		return FALSE;
	}

	idver = g3d_stream_read_int32_le(stream);
	if(idver != 8) {
		g_printerr("file '%s' has wrong version (%d)\n", stream->uri, idver);
#define CLOSE_ON_WRONG_VERSION
#ifdef CLOSE_ON_WRONG_VERSION
		return FALSE;
#endif
	}

	object = g_new0(G3DObject, 1);
	object->name = g_strdup("Q2Object");
	material = g3d_material_new();
	object->materials = g_slist_append(object->materials, material);
	model->objects = g_slist_append(model->objects, object);

	skinwidth  = g3d_stream_read_int32_le(stream);
	skinheight = g3d_stream_read_int32_le(stream);
	framesize  = g3d_stream_read_int32_le(stream);
	numskins   = g3d_stream_read_int32_le(stream);
	numverts   = g3d_stream_read_int32_le(stream);
	numtexs    = g3d_stream_read_int32_le(stream);
	numfaces   = g3d_stream_read_int32_le(stream);
	numglcmds  = g3d_stream_read_int32_le(stream);
	numframes  = g3d_stream_read_int32_le(stream);

	object->vertex_count = numverts;
	object->vertex_data = g_new0(gfloat, numverts * 3);
	normals = g_new0(gfloat, numverts * 3);

	offskins   = g3d_stream_read_int32_le(stream);
	offtexs    = g3d_stream_read_int32_le(stream);
	offfaces   = g3d_stream_read_int32_le(stream);
	offframes  = g3d_stream_read_int32_le(stream);
	offglcmds  = g3d_stream_read_int32_le(stream);
	offend     = g3d_stream_read_int32_le(stream);

	if(numskins > 0) {
		skinnames = g_new0(gchar *, numskins);
		for(i = 0; i < numskins; i ++) {
			skinnames[i] = g_new0(gchar, MD2_SKINNAMELEN);
			g3d_stream_read(stream, skinnames[i], 1, MD2_SKINNAMELEN);
#if DEBUG > 0
			g_printerr("skin #%d: %s\n", i + 1, skinnames[i]);
#endif
		}

		image = g3d_texture_load_cached(context, model, skinnames[0]);
		if(image == NULL)
			image = g3d_texture_load_cached(context, model, "tris0.bmp");
		if(image) {
			image->tex_id = tex_id;
			image->tex_env = G3D_TEXENV_DECAL;
			tex_id ++;
		}
	}

	g3d_stream_seek(stream, offframes, G_SEEK_SET);
	/* vertices per frame */
#if DEBUG > 0
	g_printerr("numframes: %d\n", numframes);
#endif
	for(i = 0; i < numframes; i ++) {
		gfloat s0,s1,s2, t0,t1,t2;
		gchar fname[16];
		guint32 j;

		s0 = g3d_stream_read_float_le(stream); /* scale */
		s1 = g3d_stream_read_float_le(stream);
		s2 = g3d_stream_read_float_le(stream);
		t0 = g3d_stream_read_float_le(stream); /* translate */
		t1 = g3d_stream_read_float_le(stream);
		t2 = g3d_stream_read_float_le(stream);
		g3d_stream_read(stream, fname, 1, 16); /* frame name*/

		for(j = 0; j < numverts; j ++) {
			gfloat x,y,z;
			guint32 v,n;

			v = g3d_stream_read_int8(stream);
			x = (gfloat)v * s0 + t0;
			v = g3d_stream_read_int8(stream);
			y = (gfloat)v * s1 + t1;
			v = g3d_stream_read_int8(stream);
			z = (gfloat)v * s2 + t2;
			n = g3d_stream_read_int8(stream);
			if(i == 0) {
				object->vertex_data[j * 3 + 0] = x;
				object->vertex_data[j * 3 + 1] = y;
				object->vertex_data[j * 3 + 2] = z;

				normals[j * 3 + 0] = md2_normals[n * 3 + 0];
				normals[j * 3 + 1] = md2_normals[n * 3 + 1];
				normals[j * 3 + 2] = md2_normals[n * 3 + 2];
			}
		}
	}

	g3d_stream_seek(stream, offtexs, G_SEEK_SET);
	/* texture coordinates */
	if(numtexs > 0) {
		texco = g_new0(gfloat, numtexs * 2);
		for(i = 0; i < numtexs; i ++) {
			texco[i * 2 + 0] = g3d_stream_read_int16_le(stream) /
				(gfloat)skinwidth;
			texco[i * 2 + 1] = g3d_stream_read_int16_le(stream) /
				(gfloat)skinheight;
		}
	}

	/* faces */
	for(i = 0; i < numfaces; i ++) {
		G3DFace *face;
		guint32 i;
		guint16 index;

		face = g_new0(G3DFace, 1);
		object->faces = g_slist_append(object->faces, face);
		face->material = material;
		face->vertex_count = 3;
		face->vertex_indices = g_new0(guint32, 3);
		face->tex_vertex_data = g_new0(gfloat, 3 * 2);
		face->normals = g_new0(gfloat, 3 * 3);
		face->flags |= G3D_FLAG_FAC_NORMALS;

		if(image)
		{
			face->flags |= G3D_FLAG_FAC_TEXMAP;
			face->tex_image = image;
		}

		for(i = 0; i < 3; i ++)
		{
			face->vertex_indices[i] = g3d_stream_read_int16_le(stream);
			face->normals[i * 3 + 0] =
				- normals[face->vertex_indices[i] * 3 + 0];
			face->normals[i * 3 + 1] =
				- normals[face->vertex_indices[i] * 3 + 1];
			face->normals[i * 3 + 2] =
				- normals[face->vertex_indices[i] * 3 + 2];
		}

		for(i = 0; i < 3; i ++)
		{
			index = g3d_stream_read_int16_le(stream);
			face->tex_vertex_data[i * 2 + 0] = texco[index * 2 + 0];
			face->tex_vertex_data[i * 2 + 1] = texco[index * 2 + 1];
		}
	}


	/* free skin names */
	if(skinnames)
	{
		for(i = 0; i < numskins; i ++)
			g_free(skinnames[i]);
		g_free(skinnames);
	}

	if(texco)
		g_free(texco);
	if(normals)
		g_free(normals);

	return TRUE;
}

gchar *plugin_description(G3DContext *context)
{
	return g_strdup(
		"import plugin for ID Software's Quake II models\n");
}

gchar **plugin_extensions(G3DContext *context)
{
	return g_strsplit("md2", ":", 0);
}

