/* $Id:$ */

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
#ifndef _IMP_LDRAW_TYPES_H
#define _IMP_LDRAW_TYPES_H

#include <glib.h>

#include <g3d/types.h>
#include <g3d/stream.h>

typedef struct {
	gchar *name;
	gchar *filename;
	G3DObject *object;
	G3DStream *stream;
	gboolean failed;
	gboolean master;
} LDrawPart;

typedef struct {
	G3DContext *context;
	gchar *libdir;
	GHashTable *partdb;
	GSList *partlist;
	GHashTable *colordb;
	GSList *colorlist;
} LDrawLibrary;

#endif /* _IMP_LDRAW_TYPES_H */
