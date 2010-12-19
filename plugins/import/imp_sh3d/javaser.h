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

#ifndef _JAVASER_H
#define _JAVASER_H

#include <glib.h>
#include <glib-object.h>

typedef enum {
	TC_FIELD_BYTE = 'B',
	TC_FIELD_CHAR = 'C',
	TC_FIELD_DOUBLE = 'D',
	TC_FIELD_FLOAT = 'F',
	TC_FIELD_INTEGER = 'I',
	TC_FIELD_LONG = 'J',
	TC_FIELD_SHORT = 'S',
	TC_FIELD_BOOLEAN = 'Z',

	TC_FIELD_ARRAY = '[',
	TC_FIELD_OBJECT = 'L',

	TC_NULL = 0x70,
	TC_REFERENCE = 0x71,
	TC_CLASSDESC = 0x72,
	TC_OBJECT = 0x73,
	TC_STRING = 0x74,
	TC_ARRAY = 0x75,
	TC_CLASS = 0x76,
	TC_BLOCKDATA = 0x77,
	TC_ENDBLOCKDATA = 0x78,
	TC_RESET = 0x79,
	TC_BLOCKDATALONG = 0x7A,
	TC_EXCEPTION = 0x7B,
	TC_LONGSTRING = 0x7C,
	TC_PROXYCLASSDESC = 0x7D,
	TC_ENUM = 0x7E
} JavaserTypecode;

enum {
	SC_WRITE_METHOD = 0x01,
	SC_SERIALIZABLE = 0x02,
	SC_EXTERNALIZABLE = 0x04,
	SC_BLOCK_DATA = 0x08,
	SC_ENUM = 0x10
} JavaserClassFlags;

typedef struct _JavaserClass JavaserClass;

struct _JavaserClass {
	gchar *name;
	guint32 flags;
	GSList *fields;
	JavaserClass *superclass;
};

typedef struct {
	gchar *name;
	GValue value;
} JavaserProperty;

typedef struct {
	JavaserClass *jclass;
	guint32 nitems;
	GSList *items;
} JavaserArray;

typedef struct {
	JavaserClass *jclass;
	GHashTable *properties;
} JavaserObject;

typedef struct {
	gchar *name;
	JavaserTypecode typecode;
	JavaserTypecode datatypecode;
	union {
		gchar *className;
		guint32 reference;
	} data;
} JavaserField;

typedef struct {
	GSList *classes;
	GSList *objects;
	GSList *handles;
	JavaserObject *current_object;
} JavaserData;

JavaserClass *javaser_parse(G3DStream *stream);

#endif /* JAVASER_H */
