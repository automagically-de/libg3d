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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <locale.h>

#include <g3d/context.h>
#include <g3d/types.h>
#include <g3d/material.h>
#include <g3d/stream.h>

static G3DObject *obj_createobject(GSList **olist, const gchar *name);
static gboolean obj_tryloadmat(G3DModel *model, const gchar *filename);
static G3DMaterial *obj_usemat(G3DModel *model, const gchar *matname);

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model, gpointer user_data)
{
	gchar line[2048], oname[128], matname[128], matfile[1024];
	gchar *filename;
	G3DObject *object = NULL;
	G3DMaterial *material = NULL;
	gfloat pcnt, prev_pcnt = 0.0;
	gdouble x,y,z;
	guint32 num_v, v_off = 1, v_cnt = 0;

	setlocale(LC_NUMERIC, "C");
	filename = g3d_stream_get_uri(stream);

	strncpy(matfile, filename, strlen(filename) - 3);
	matfile[strlen(filename)-3] = '\0';
	strcat(matfile, "mtl");
	obj_tryloadmat(model, matfile);

	object = obj_createobject(&(model->objects), "(default)");

	while(!g3d_stream_eof(stream))
	{
		memset(line, 0, 2048);
		g3d_stream_read_line(stream, line, 2048);
		/* remove leading and trailing whitespace characters */
		g_strstrip(line);
		if(strlen(line) > 0)
		{
			switch(line[0])
			{
				case '#':
				case '\n': continue; break;

				case 'g': /* group */
					if(strlen(line) == 1)
					{
					}
					else if(sscanf(line, "g %s", oname) == 1)
					{
						material = obj_usemat(model, oname);
#if 0
						object = obj_createobject(&(model->objects), oname);
#endif
#if 0
						v_off += v_cnt;
						v_cnt = 1;
#endif
					}
					else g_warning("parse error in line: %s", line);
					break;

				case 'l': /* line */
					break;

				case 'o': /* object */
#if 0
					if(strlen(line) == 1)
					{
						object = obj_createobject(
							object ? &(object->objects) : &(model->objects),
							"(unnamed object)");
						v_off += v_cnt;
						v_cnt = 1;
					}
					else if(sscanf(line, "o %s", oname) == 1)
					{
						object = obj_createobject(
							object ? &(object->objects) : &(model->objects),
							oname);
						v_off += v_cnt;
						v_cnt = 1;
					}
					else
					{
						g_warning("[OBJ] parse error in line: %s", line);
					}
#endif
					break;

				case 'v': /* vertex */
					if(strncmp(line, "vn ", 3) == 0)
					{
						/* normal ? */
					}
					else if(strncmp(line, "vt ", 3) == 0)
					{
						/* ?? */
					}
					else if(sscanf(line, "v %lf %lf %lf", &x, &y, &z) == 3)
					{
						if(object == NULL)
						{
							object = obj_createobject(
								&(model->objects), "(noname)");
						}
						object->vertex_count++;
						object->vertex_data = g_realloc(object->vertex_data,
							object->vertex_count * 3 * sizeof(gfloat));
						object->vertex_data[v_cnt*3+0] = x;
						object->vertex_data[v_cnt*3+1] = y;
						object->vertex_data[v_cnt*3+2] = z;
						v_cnt++;
					}
					else g_warning("parse error in line: %s", line);
					break;

				case 'f': /* face */
					if(strncmp("f ", line, 2) == 0)
					{
						G3DFace *face;
						gchar **vertex, **vstrs = g_strsplit(line, " ", 0);
						int i;

						num_v = 0;
						if(object == NULL) {
							g_warning("error: face before object");
							return FALSE;
						}
						face = g_new0(G3DFace, 1);
						if(material != NULL)
							face->material = material;
						else face->material =
							g_slist_nth_data(object->materials, 0);

						/* find number of vertices in line */
						vertex = vstrs;
						while(*vertex != NULL) { num_v++; vertex++; }
						face->vertex_count = num_v - 1;

						/* next one if # of vertices < 3 */
						if(face->vertex_count < 3)
							continue;

						/* read vertices */
						face->vertex_indices = g_new0(guint32, num_v - 1);
						for(i = 1; i < num_v; i ++) {
							gint32 index = strtol(vstrs[i], NULL, 10);

							if(index < 0)
								face->vertex_indices[i - 1] =
									v_cnt + index - v_off + 1;
							else
								face->vertex_indices[i - 1] = index - v_off;
						}
						g_strfreev(vstrs);
						object->faces = g_slist_prepend(object->faces, face);
					}
					else
						g_warning("parse error in line: %s", line);
					break;

				case 'u': /* usemat? */
				case 'm':
				case 's':
					if(sscanf(line, "usemtl %s", matname) == 1) {
						material = obj_usemat(model, matname);
					} else if(sscanf(line, "mtllib %s", matfile) == 1) {
						/* loads external material library */
						if(obj_tryloadmat(model, matfile) != TRUE)
							g_warning("error loading material library '%s'",
								matfile);
					}
					break;
				default:
#if DEBUG > 0
					g_debug("unknown type of line: %s", line);
#endif
					break;
			}
		}

#if 1
		pcnt = (gfloat)g3d_stream_tell(stream) /
			(gfloat)g3d_stream_size(stream);
		if((pcnt - prev_pcnt) > 0.01) {
			prev_pcnt = pcnt;
			g3d_context_update_progress_bar(context, pcnt, TRUE);
		}
#endif
		g3d_context_update_interface(context);
	} /* !eof(stream) */
	return TRUE;
}

gchar *plugin_description(void)
{
	return g_strdup(
		"Import plugin for Maya .obj files\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("obj", ":", 0);
}

/*****************************************************************************/

G3DObject *obj_createobject(GSList **olist, const char *name)
{
	G3DObject *object;
	G3DMaterial *material = g3d_material_new();

	object = g_new0(G3DObject, 1);
	object->name = g_strdup(name);
	*olist = g_slist_append(*olist, object);
	object->materials = g_slist_append(object->materials, material);
	return object;
}

/*****************************************************************************/
/* material file ops                                                         */
/*****************************************************************************/

int obj_tryloadmat(G3DModel *model, const char *filename)
{
	FILE *f;
	G3DMaterial *material = NULL;

	f = fopen(filename, "r");
	if(f == NULL) {
#if DEBUG > 1
		g_warning("obj_tryloadmat: loading '%s' failed: %s", filename,
			strerror(errno));
#endif
		return FALSE;
	}
#if DEBUG > 0
	g_debug("loading material library %s", filename);
#endif
	while(!feof(f)) {
		char line[2048];
		float r,g,b, t1,t2, ni;
		int tf, ns, il;

		fgets(line, 2048, f);
		if(strlen(line))
		{
			char mname[128];

			if(line[0] == '#') continue;   /* comments */
			if(line[0] == '\n') continue;  /* empty lines */

			if(sscanf(line, "newmtl %s", mname) == 1)
			{
				/* new material */
				material = g3d_material_new();
				material->name = g_strdup(mname);
				model->materials = g_slist_append(model->materials, material);
			}
			else if(sscanf(line, " Kd %f %f %f", &r, &g, &b) == 3)
			{
				/* material color? */
				if(material != NULL)
				{
					material->r = r;
					material->g = g;
					material->b = b;
				}
			}
			else if(sscanf(line, " Ks %f %f %f", &r, &g, &b) == 3)
			{
				/* ?? */
			}
			else if(sscanf(line, " Tf %f %f %d", &t1, &t2, &tf) == 3)
			{
				/* transparency ?? */
				if(material != NULL)
				{
					if(tf == 1) material->a = 1.0 - t1;
				}
			}
			else if(sscanf(line, " Ns %d Ni %f", &ns, &ni) == 2)
			{
				/* ?? */
			}
			else if(sscanf(line, " illum %d", &il) == 1)
			{
				/* ?? */
			}
			else {
#if DEBUG > 0
				g_warning("unknown type of line: %s", line);
#endif
			}
		}
	} /* !feof */
	return TRUE;
}

G3DMaterial *obj_usemat(G3DModel *model, const gchar *matname)
{
	/* sets new active material from named list */
	GSList *mlist = model->materials;
	while(mlist != NULL)
	{
		G3DMaterial *mat = (G3DMaterial*)mlist->data;
		if(strcmp(matname, mat->name) == 0)
		{
			return mat;
		}
		mlist = mlist->next;
	}

	return NULL;
}

