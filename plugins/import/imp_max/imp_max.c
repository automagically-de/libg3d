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

#include <string.h>

#include <g3d/types.h>
#include <g3d/context.h>
#include <g3d/object.h>
#include <g3d/material.h>
#include <g3d/stream.h>

#include "imp_max_chunks.h"



static gboolean max_read_subfile(G3DContext *context, G3DModel *model,
	G3DStream *stream, const gchar *subfile);
static gboolean max_read_chunk(MaxGlobalData *global, gint32 *nb,
	guint32 level, gint32 parentid, gpointer object, guint32 *l2cnt,
	GNode *tree);
static MaxChunk *max_get_chunk_desc(guint16 id, gint32 parentid,
	gboolean container);

static const gchar *max_subfiles[] = {
#if 0
	"Config",
	"VideoPostQueue",
	"ScriptedCustAttribDefs",
	"DllDirectory",
	"ClassDirectory",
	"ClassDirectory2",
	"ClassDirectory3",
	"ClassData",
#endif
	"Scene",
	NULL
};

typedef enum {
	MAX_ROOT_NODE,
	MAX_L2_NODE,
	MAX_CNT_NODE,
	MAX_DATA_NODE
} MaxNodeType;

typedef struct {
	MaxNodeType type;
	gchar *name;
	guint32 cnt2034;
	guint32 val2034;
	gint32 data2034;
	GSList *children;
} MaxNode;

typedef struct {
	guint32 l2id;
	guint32 id;
	gchar *text;
} MaxTreeItem;

gboolean plugin_load_model_from_stream(G3DContext *context, G3DStream *stream,
	G3DModel *model)
{
	gboolean retval = FALSE;
	G3DMaterial *material;
	const gchar **subfile = max_subfiles;

	/* create default material */
	material = g3d_material_new();
	material->name = g_strdup("default material");
	model->materials = g_slist_append(model->materials, material);

	/* debugging material */
	material = g3d_material_new();
	material->r = 1.0;
	material->g = 0.2;
	material->b = 0.1;
	material->name = g_strdup("debugging material");
	model->materials = g_slist_append(model->materials, material);

	while(*subfile) {
		retval = max_read_subfile(context, model, stream, *subfile);
		subfile ++;
	}

	g3d_context_update_progress_bar(context, 0.0, FALSE);

	return retval;
}

gchar *plugin_description(void)
{
	return g_strdup("import plugin for 3D Studio MAX files (EXPERIMENTAL)\n");
}

gchar **plugin_extensions(void)
{
	return g_strsplit("max:gmax", ":", 0);
}

/*****************************************************************************
 * max specific
 *****************************************************************************/

static void max_walk_tree(GNode *tree, guint32 level)
{
	GNode *node;
	MaxTreeItem *mtitem;
	static const gchar *padding = "                                    ";

	mtitem = (MaxTreeItem *)tree->data;

	g_debug("\\%s(%u)[0x%04X][0x%04X] %s",
		padding + (strlen(padding) - level), level,
		mtitem->l2id, mtitem->id, mtitem->text);

	for(node = tree->children; node != NULL; node = node->next) {
		max_walk_tree(node, level + 1);
	}
}

static gboolean max_read_subfile(G3DContext *context, G3DModel *model,
	G3DStream *stream, const gchar *subfile)
{
	G3DStream *ssf;
	MaxGlobalData *global;
	gint32 fsize;
	guint32 l2cnt = 0;
	MaxTreeItem *rootitem;
	GNode *tree;

	rootitem = g_new0(MaxTreeItem, 1);
	rootitem->text = g_strdup("ROOT");
	tree = g_node_new(rootitem);

	ssf = g3d_stream_open_structured_file_from_stream(stream, subfile);
	if(ssf == NULL) {
		g_warning("MAX: failed to open '%s' in structured file '%s'",
			subfile, stream->uri);
		return FALSE;
	}

	fsize = g3d_stream_size(ssf);

	g_debug("\\%s (%d bytes)", subfile, fsize);

	global = g_new0(MaxGlobalData, 1);
	global->context = context;
	global->model = model;
	global->stream = ssf;
	global->padding = "                                               ";
	global->subfile = subfile;

	while(max_read_chunk(global, &fsize, 1 /* level */, IDNONE, NULL, &l2cnt,
		tree));

	g_debug("MAX tree:");
	max_walk_tree(tree, 0);

	g_free(global);
	g3d_stream_close(ssf);

	return TRUE;
}

static GNode *max_find_node(GNode *tree, guint32 id)
{
	GNode *node, *found;
	MaxTreeItem *mtitem;

	mtitem = (MaxTreeItem *)tree->data;
	if(mtitem->l2id == id)
		return tree;

	for(node = tree->children; node != NULL; node = node->next) {
		found = max_find_node(node, id);
		if(found != NULL)
			return found;
	}

	return NULL;
}

static gboolean max_read_chunk(MaxGlobalData *global, gint32 *nb,
	guint32 level, gint32 parentid, gpointer object, guint32 *l2cnt,
	GNode *tree)
{
	guint16 id;
	guint32 length;
	gboolean container;
	MaxChunk *chunk;
	MaxLocalData *local;
	MaxTreeItem *mtitem;
	GNode *pnode, *node;

	if(nb && (*nb < 6))
		return FALSE;

	id = g3d_stream_read_int16_le(global->stream);
	length = g3d_stream_read_int32_le(global->stream);
	container = (length & 0x80000000);
	length &= 0x7FFFFFFF;

	if(nb && (length > *nb))
		return FALSE;
	if(nb)
		*nb -= length;

	if((level == 2) && l2cnt)
		(*l2cnt) ++;

	chunk = max_get_chunk_desc(id, parentid, container);

#if DEBUG > 0
	g_debug("\\%s(%d)[0x%04X][0x%04X][%c%c] %s -- %d (%d) bytes @ 0x%08x",
		(global->padding + (strlen(global->padding) - level)), level,
		id, (l2cnt ? *l2cnt : 0xFFFF), (container ? 'c' : ' '),
		(chunk && chunk->callback) ? 'f' : ' ',
		chunk ? chunk->desc : "unknown",
		length - 6, length,
		(guint32)g3d_stream_tell(global->stream) - 6);
#endif

	node = tree;
	if(level == 2) {
		pnode = max_find_node(tree, id);
		if(pnode != NULL) {
			mtitem = g_new0(MaxTreeItem, 1);
			mtitem->l2id = *l2cnt;
			mtitem->id = id;
			mtitem->text = g_strdup("L2ITEM");
			node = g_node_append_data(pnode, mtitem);
		} else {
			mtitem = g_new0(MaxTreeItem, 1);
			mtitem->l2id = 0xFFFF;
			mtitem->id = id;
			mtitem->text = g_strdup_printf("OUTOFTREE: 0x%04X", id);
			node = g_node_append_data(tree, mtitem);
		}
	} else if(level > 2) {
		mtitem = g_new0(MaxTreeItem, 1);
		mtitem->l2id = 0xFFFF;
		mtitem->id = id;
		mtitem->text = g_strdup_printf("REGITEM: 0x%04X: %s",
			id, chunk ? chunk->desc : "unknown");
		node = g_node_append_data(tree, mtitem);
	}

	local = g_new0(MaxLocalData, 1);
	local->id = id;
	local->parentid = parentid;
	local->nb = length - 6;
	local->level = level + 1;
	local->object = object;

#if 0
	if(chunk && chunk->callback)
		chunk->callback(global, local);
#endif
	if(container/* && (level < 2)*/)
		while(local->nb > 0)
			if(!max_read_chunk(global, &(local->nb), level + 1, id,
				local->object, l2cnt, node))
				return FALSE;

	if(local->nb > 0)
		g3d_stream_seek(global->stream, local->nb, G_SEEK_CUR);

	g_free(local);

	g3d_context_update_interface(global->context);

	if(level < 3)
		g3d_context_update_progress_bar(global->context,
			(gfloat)g3d_stream_tell(global->stream) /
			(gfloat)g3d_stream_size(global->stream),
			TRUE);

	return TRUE;
}

static MaxChunk *max_get_chunk_desc(guint16 id, gint32 parentid,
	gboolean container)
{
	MaxChunk *chunk, *chunks;
	gint32 i;

	if(container)
		chunks = max_cnt_chunks;
	else
		chunks = max_chunks;

	for(i = 0, chunk = &(chunks[i]); chunk->id != IDNONE;
		i ++, chunk = &(chunks[i])) {
		if((chunk->parentid == IDSOME) || (parentid == chunk->parentid) ||
			(parentid == IDSOME) ||
			((chunk->parentid == IDROOT) && ID_IS_ROOT(parentid)) ||
			((chunk->parentid == IDGEOM) && ID_IS_GEOM(parentid)) ||
			((chunk->parentid == IDMATG) && ID_IS_MATG(parentid)) ||
			((chunk->parentid == IDFILE) && ID_IS_FILE(parentid))) {
			if(chunk->id == id)
				return chunk;
		} /* parentid */
	}
	return NULL;
}
