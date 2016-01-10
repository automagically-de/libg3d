#include <g3d/image.h>
#include <g3d/stream.h>

#include "dxtn.h"

static void decode_rgb565(guint16 c, gfloat *r, gfloat *g, gfloat *b)
{
	*r = ((c & 0xF800) >> 11) / 32.0;
	*g = ((c & 0x07E0) >> 5)  / 64.0;
	*b = (c & 0x001F)         / 32.0;
}

gboolean decode_dxtn(G3DImage *image, G3DStream *stream, guint32 dxtn)
{
	gint32 x, y, i, j;
	guint32 index, width, height;
	gfloat r, g, b, r1, r2, g1, g2, b1, b2;
	guint8 line, v2;
	guint8 *pixeldata;

	width = g3d_image_get_width(image);
	height = g3d_image_get_height(image);
	pixeldata = g3d_image_get_pixels(image);

	for(y = 0; y < height; y += 4) {
		for(x = 0; x < width; x += 4) {
			/* DXT5 */
			if (dxtn == 5) {
				guint8 a0 = g3d_stream_read_int8(stream);
				guint8 a1 = g3d_stream_read_int8(stream);
				guint64 bytes = g3d_stream_read_int32_le(stream);
				guint64 part2 = g3d_stream_read_int16_le(stream);
				bytes |= part2 << 32;

				for (j = 0; j < 4; j ++) {
					for (i = 0; i < 4; i ++) {
						index = ((height - 4) - y + 3 - j) * width + x + i;
						v2 = bytes & 0x7;
						bytes >>= 3;
						switch (v2) {
						case 0:
							pixeldata[index * 4 + 3] = a0;
							break;
						case 1:
							pixeldata[index * 4 + 3] = a1;
							break;
						case 2:
						case 3:
						case 4:
						case 5:
							if (a0 > a1)
								pixeldata[index * 4 + 3] = ((8 - v2) * a0 + (v2 - 1) * a1) / 7;
							else
								pixeldata[index * 4 + 3] = ((6 - v2) * a0 + (v2 - 1) * a1) / 5;
							break;
						case 6:
							if (a0 > a1)
								pixeldata[index * 4 + 3] = (2 * a0 + 5 * a1) / 7;
							else
								pixeldata[index * 4 + 3] = 0;
							break;
						case 7:
							if (a0 > a1)
								pixeldata[index * 4 + 3] = (1 * a0 + 6 * a1) / 7;
							else
								pixeldata[index * 4 + 3] = 0xff;
							break;
						}
					}
				}
			}

			decode_rgb565(g3d_stream_read_int16_le(stream), &r1, &g1, &b1);
			decode_rgb565(g3d_stream_read_int16_le(stream), &r2, &g2, &b2);

			for(j = 0; j < 4; j ++) {
				line = g3d_stream_read_int8(stream);
				for(i = 0; i < 4; i ++) {
					v2 = line & 0x3;
					line >>= 2;
					r = r1 + ((r2 - r1) / 3.0) * v2;
					g = g1 + ((g2 - g1) / 3.0) * v2;
					b = b1 + ((b2 - b1) / 3.0) * v2;
					index = ((height - 4) - y + 3 - j) * width + x + i;
					pixeldata[index * 4 + 0] = r * 255.0;
					pixeldata[index * 4 + 1] = g * 255.0;
					pixeldata[index * 4 + 2] = b * 255.0;
					if (dxtn == 1)
						pixeldata[index * 4 + 3] = 0xFF;
				}
			}
		}
	}

	return TRUE;
}

