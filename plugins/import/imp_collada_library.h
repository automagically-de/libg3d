/* $Id:$ */

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

#ifndef IMP_COLLADA_LIBRARY_H
#define IMP_COLLADA_LIBRARY_H

#include <g3d/types.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

typedef struct {
	xmlXPathContextPtr context;
	GHashTable *materials;
	GHashTable *geometries;

	G3DModel *model;
} ColladaLibrary;

ColladaLibrary *col_library_init(xmlDocPtr xdoc, G3DModel *model);
void col_library_free(ColladaLibrary *library);

#endif /* IMP_COLLADA_LIBRARY_H */
