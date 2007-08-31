/* $Id: imp_maya.c 75 2006-11-06 10:52:56Z mmmaddd $ */

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

#ifndef _IMP_TA_3DO_PALETTE_H
#define _IMP_TA_3DO_PALETTE_H

#include <glib.h>

static guint8 ta_3do_palette[768] = {
		0x00, 0x00, 0x00,
		0x80, 0x00, 0x00,
		0x00, 0x80, 0x00,
		0x80, 0x80, 0x00,
		0x00, 0x00, 0x80,
		0x80, 0x00, 0x80,
		0x00, 0x80, 0x80,
		0x80, 0x80, 0x80,
		0xC0, 0xDC, 0xC0,
		0x54, 0x54, 0xFC,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0xFF, 0xEB, 0xF3,
		0xEB, 0xC7, 0xD3,
		0xD7, 0xA3, 0xB3,
		0xC3, 0x87, 0x97,
		0xAF, 0x6F, 0x7F,
		0x9B, 0x5B, 0x63,
		0x8B, 0x47, 0x4F,
		0x7B, 0x3B, 0x47,
		0x6F, 0x33, 0x3B,
		0x63, 0x2B, 0x33,
		0x57, 0x23, 0x2B,
		0x4B, 0x1B, 0x27,
		0x3B, 0x17, 0x1F,
		0x2F, 0x0F, 0x17,
		0x23, 0x0B, 0x0F,
		0x17, 0x07, 0x0B,
		0x73, 0xFF, 0xDF,
		0x57, 0xE7, 0xBF,
		0x43, 0xCF, 0x9F,
		0x2F, 0xB7, 0x83,
		0x1F, 0x9F, 0x67,
		0x13, 0x8B, 0x4F,
		0x0F, 0x77, 0x3F,
		0x0B, 0x6B, 0x37,
		0x07, 0x5F, 0x2F,
		0x07, 0x53, 0x2B,
		0x00, 0x47, 0x27,
		0x00, 0x3F, 0x23,
		0x00, 0x33, 0x1B,
		0x00, 0x27, 0x17,
		0x00, 0x1B, 0x0F,
		0x00, 0x13, 0x0B,
		0xE3, 0xEF, 0xFF,
		0xC7, 0xDF, 0xE7,
		0xAF, 0xCF, 0xCB,
		0x93, 0xB7, 0xA7,
		0x7F, 0x9F, 0x83,
		0x6B, 0x87, 0x67,
		0x5F, 0x6F, 0x53,
		0x5F, 0x63, 0x47,
		0x5B, 0x57, 0x3B,
		0x53, 0x43, 0x33,
		0x47, 0x3B, 0x2B,
		0x3B, 0x33, 0x23,
		0x2F, 0x2B, 0x1B,
		0x23, 0x1F, 0x13,
		0x17, 0x13, 0x0F,
		0x0B, 0x0B, 0x07,
		0xFB, 0xFB, 0xD7,
		0xDF, 0xDF, 0xB7,
		0xC3, 0xC3, 0x9B,
		0xAB, 0xAB, 0x83,
		0x93, 0x93, 0x6F,
		0x77, 0x77, 0x57,
		0x63, 0x63, 0x43,
		0x53, 0x53, 0x33,
		0x43, 0x43, 0x23,
		0x33, 0x33, 0x17,
		0x23, 0x23, 0x0F,
		0x1B, 0x1B, 0x07,
		0x17, 0x17, 0x07,
		0x13, 0x13, 0x00,
		0x0F, 0x0F, 0x00,
		0x0B, 0x0B, 0x00,
		0xFB, 0xFB, 0xFB,
		0xEB, 0xEB, 0xEB,
		0xDB, 0xDB, 0xDB,
		0xCB, 0xCB, 0xCB,
		0xBB, 0xBB, 0xBB,
		0xAB, 0xAB, 0xAB,
		0x9B, 0x9B, 0x9B,
		0x8B, 0x8B, 0x8B,
		0x7B, 0x7B, 0x7B,
		0x6B, 0x6B, 0x6B,
		0x5B, 0x5B, 0x5B,
		0x4B, 0x4B, 0x4B,
		0x3B, 0x3B, 0x3B,
		0x2B, 0x2B, 0x2B,
		0x1F, 0x1F, 0x1F,
		0x0F, 0x0F, 0x0F,
		0xEB, 0xF3, 0xFF,
		0xCB, 0xE3, 0xFF,
		0xAF, 0xCF, 0xFF,
		0x97, 0xB3, 0xFF,
		0x7B, 0x97, 0xFF,
		0x67, 0x7F, 0xFF,
		0x53, 0x6B, 0xEF,
		0x3F, 0x5B, 0xE3,
		0x33, 0x4B, 0xD7,
		0x23, 0x3B, 0xCB,
		0x17, 0x2F, 0xAF,
		0x0F, 0x27, 0x97,
		0x07, 0x1F, 0x7B,
		0x07, 0x17, 0x63,
		0x00, 0x0F, 0x47,
		0x00, 0x0B, 0x2F,
		0xE3, 0xF7, 0xFF,
		0xBF, 0xDB, 0xE7,
		0x9F, 0xBF, 0xCF,
		0x83, 0xA7, 0xB7,
		0x6B, 0x8F, 0xA3,
		0x53, 0x77, 0x8B,
		0x3F, 0x5F, 0x73,
		0x2F, 0x4B, 0x5F,
		0x27, 0x3F, 0x57,
		0x23, 0x37, 0x4F,
		0x1F, 0x2F, 0x47,
		0x1B, 0x27, 0x3F,
		0x17, 0x1F, 0x37,
		0x13, 0x1B, 0x2F,
		0x0F, 0x13, 0x27,
		0x0B, 0x0F, 0x1F,
		0xD7, 0xEF, 0xFF,
		0xBB, 0xE3, 0xEF,
		0x9B, 0xCB, 0xDF,
		0x83, 0xB7, 0xCF,
		0x6B, 0xA3, 0xC3,
		0x53, 0x8F, 0xB3,
		0x3F, 0x7B, 0xA3,
		0x2F, 0x6B, 0x97,
		0x23, 0x5B, 0x87,
		0x1B, 0x4B, 0x77,
		0x13, 0x3F, 0x67,
		0x0B, 0x33, 0x57,
		0x07, 0x27, 0x47,
		0x00, 0x1B, 0x37,
		0x00, 0x13, 0x27,
		0x00, 0x0B, 0x1B,
		0xFF, 0xE7, 0xFF,
		0xE7, 0xC7, 0xEB,
		0xD3, 0xAB, 0xD7,
		0xBB, 0x93, 0xC3,
		0xA7, 0x7B, 0xB3,
		0x8F, 0x63, 0x9F,
		0x77, 0x4B, 0x8F,
		0x63, 0x3B, 0x7F,
		0x4F, 0x2B, 0x6F,
		0x43, 0x1F, 0x63,
		0x37, 0x17, 0x57,
		0x2B, 0x0F, 0x47,
		0x1F, 0x07, 0x3B,
		0x13, 0x00, 0x2B,
		0x0B, 0x00, 0x1F,
		0x07, 0x00, 0x13,
		0xD7, 0xFF, 0xA7,
		0xAB, 0xE7, 0x7F,
		0x83, 0xD3, 0x5B,
		0x67, 0xBF, 0x3F,
		0x4B, 0xAB, 0x2B,
		0x43, 0x97, 0x2B,
		0x37, 0x87, 0x27,
		0x2F, 0x77, 0x1B,
		0x2B, 0x67, 0x13,
		0x23, 0x5B, 0x0F,
		0x1F, 0x4F, 0x0B,
		0x1B, 0x43, 0x07,
		0x17, 0x33, 0x00,
		0x0F, 0x27, 0x00,
		0x0B, 0x1B, 0x00,
		0x07, 0x0F, 0x00,
		0xFF, 0xE3, 0x9F,
		0xE3, 0xC7, 0x73,
		0xCB, 0xAF, 0x53,
		0xB3, 0x97, 0x3F,
		0x9B, 0x83, 0x2F,
		0x83, 0x6F, 0x23,
		0x6B, 0x5B, 0x17,
		0x53, 0x47, 0x0F,
		0x4B, 0x3B, 0x0B,
		0x43, 0x33, 0x07,
		0x3B, 0x2B, 0x07,
		0x37, 0x23, 0x00,
		0x2F, 0x1B, 0x00,
		0x27, 0x13, 0x00,
		0x1F, 0x0F, 0x00,
		0x1B, 0x0B, 0x00,
		0xFF, 0xFF, 0xA3,
		0xFB, 0xF3, 0x83,
		0xF7, 0xE3, 0x67,
		0xF3, 0xD3, 0x4F,
		0xEF, 0xBB, 0x33,
		0xEF, 0xA7, 0x1B,
		0xEB, 0x8F, 0x13,
		0xE7, 0x7B, 0x0F,
		0xDF, 0x4F, 0x07,
		0xD7, 0x23, 0x00,
		0xBF, 0x1F, 0x00,
		0xA7, 0x1B, 0x00,
		0x93, 0x17, 0x00,
		0x7B, 0x13, 0x00,
		0x63, 0x13, 0x00,
		0x4F, 0x0F, 0x00,
		0xFF, 0xFF, 0x00,
		0xFF, 0xBF, 0x00,
		0xFF, 0x83, 0x00,
		0xFF, 0x47, 0x00,
		0xD3, 0x2B, 0x00,
		0xAB, 0x17, 0x00,
		0x7F, 0x07, 0x00,
		0x57, 0x00, 0x00,
		0xDF, 0xCB, 0xFF,
		0xBB, 0x9F, 0xDF,
		0x9B, 0x77, 0xBF,
		0x7F, 0x57, 0x9F,
		0x67, 0x3B, 0x7F,
		0x4B, 0x23, 0x5F,
		0x33, 0x13, 0x3F,
		0x1B, 0x07, 0x1F,
		0xD3, 0xDB, 0xFF,
		0x87, 0x9F, 0xF7,
		0x43, 0x6F, 0xEF,
		0x17, 0x47, 0xE7,
		0x0B, 0x2B, 0xBB,
		0x07, 0x17, 0x8F,
		0x00, 0x07, 0x63,
		0x00, 0x00, 0x37,
		0x7B, 0xFF, 0x77,
		0x53, 0xDF, 0x4F,
		0x33, 0xBF, 0x2B,
		0x1B, 0x9F, 0x13,
		0x1B, 0x7F, 0x0B,
		0x17, 0x5F, 0x07,
		0x13, 0x3F, 0x00,
		0x0B, 0x1F, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0xFF, 0xFB, 0xF0,
		0xA0, 0xA0, 0xA4,
		0x80, 0x80, 0x80,
		0xFF, 0x00, 0x00,
		0x00, 0xFF, 0x00,
		0xFF, 0xFF, 0x00,
		0x00, 0x00, 0xFF,
		0xFF, 0x00, 0xFF,
		0x00, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF};

#endif /* _IMP_TA_3DO_PALETTE_H */