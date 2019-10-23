/*
   Copyright (C) Seiko Epson Corporation 2009.
 
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free  Software Foundation, Inc., 
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blend-watermark.h"

#pragma pack (2)
typedef struct {
	unsigned short Type; 
	unsigned int Size; 
	unsigned short Reserved1; 
	unsigned short Reserved2; 
	unsigned int OffBits; 
} BITMAPFILEHEADER; 

typedef struct { 
	unsigned int Size;
	unsigned int Width; 
	unsigned int Height; 
	unsigned short Planes; 
	unsigned short BitCount; 
	unsigned int Compression; 
	unsigned int SizeImage; 
	unsigned int XPelsPerMeter; 
	unsigned int YPelsPerMeter; 
	unsigned int ClrUsed; 
	unsigned int ClrImportant; 
} BITMAPINFOHEADER; 

typedef struct {
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Reserved;
} RGBQUAD;

#pragma pack()

static void print_wbf_file_header(BITMAPFILEHEADER* bf)
{
	debuglog(("BITMAPFILEHEADER"));
	debuglog(("\tType      : %d", bf->Type));
	debuglog(("\tSize      : %d", bf->Size));
	debuglog(("\tReserved1 : %d", bf->Reserved1));
	debuglog(("\tReserved2 : %d", bf->Reserved2));
	debuglog(("\tOffBits   : %d", bf->OffBits));
}

static void print_wbf_info_header(BITMAPINFOHEADER* bi)
{
	debuglog(("BITMAPFILEHEADER"));
	debuglog(("\tSize          : %d", bi->Size));
	debuglog(("\tWidth         : %d", bi->Width));
	debuglog(("\tHeight        : %d", bi->Height));
	debuglog(("\tPlanes        : %d", bi->Planes));
	debuglog(("\tBitCount      : %d", bi->BitCount));
	debuglog(("\tCompression   : %d", bi->Compression));
	debuglog(("\tSizeImage     : %d", bi->SizeImage));
	debuglog(("\tXPelsPerMeter : %d", bi->XPelsPerMeter));
	debuglog(("\tYPelsPerMeter : %d", bi->YPelsPerMeter));
	debuglog(("\tClrUsed       : %d", bi->ClrUsed));
	debuglog(("\tClrImportant  : %d", bi->ClrImportant));
}

static void wbf_setbit(char *bits, int index)
{
	unsigned char *setbit_byte = (unsigned char *)bits + (index / 8);
	*setbit_byte |= (1 << (index % 8));
}

static int wbf_getbit(char *bits, int index)
{
	unsigned char *getbit_byte = (unsigned char *)bits + (index / 8);
	return (*getbit_byte & (1 << (index % 8))) ? 1 : 0;
}

static int wbf_decompress_RLE4(FILE *stream, EpsSize size, char **decompressedPixels)
{
	int error = 0;
	int y;
	int x;

	int pixelCount;
	int color;
	int i;
	int offset_x;
	int offset_y;
	unsigned char data;

	y = size.height - 1;
	x = 0;
	while (y >= 0) {
		data = fgetc(stream);
		if (data == 0) { /* Escape code */
			data = fgetc(stream);
			if (data == 0) { /* End of Line */
				x = 0;
				y--;
			} else if (data == 1) { /* End of Image */
				break;
			} else if (data == 2) { /* Data offset */
				offset_x = fgetc(stream);
				offset_y = fgetc(stream);
				x += offset_x;
				y -= offset_y;
			} else { /* In-contigious pixels */
				pixelCount = data;
				for (i = 0; i < pixelCount; i++) {
					if ((i & 1) == 1) {
						color = (data & 0x0F);
					} else {
						data = fgetc(stream);
						color = (data & 0xF0) >> 4;
					}
					if (color == 0) { /* black */
						wbf_setbit(decompressedPixels[y], x);
					}
					x++;
				}

				if (((pixelCount & 3) == 1) || ((pixelCount & 3) == 2)) {
					fgetc(stream); /* eatup padding byte */
				}
			}
		} else { /* Contigious pixels */
			pixelCount = data;
			data = fgetc(stream);
			for (i = 0; i < pixelCount; i++) {
				color = (i & 1) ? (data & 0x0F) : ((data >> 4) & 0x0F);
				if (color == 0) { /* black */
					wbf_setbit(decompressedPixels[y], x);
				}
				x++;
			}
		}
	}

	return y;
}

typedef struct wbf_stream_private_data_s
{
	EpsSize imageSize;
	char **decompressedPixels;
	RGBQUAD palettes [16];
	FILE* stream;
} wbf_stream_private_data_t;

void * wbfReaderOpen(FILE* fstream, EpsSize *size)
{
	const int pixels_per_byte = 8;

	wbf_stream_private_data_t *data = NULL;
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	int error = EPS_BLEND_SOURCE_ERROR;
	int decompressed_lines;
	int read_bytes;
	int bytes_per_line;
	int i;
	char dummy;

	do {
		data = (wbf_stream_private_data_t *) eps_malloc(sizeof(wbf_stream_private_data_t));
		if (data == NULL) {
			break;
		}

		data->stream = fstream;

		read_bytes = fread(&bf, 1, sizeof(bf), data->stream);
		if (read_bytes <= 0) {
			debuglog(("No more file header read."));
			break;
		}
		print_wbf_file_header(&bf);

		if (bf.Type != 19778) {
			debuglog(("Invalid BMP file header."));
			break;
		}

		read_bytes = fread(&bi, sizeof(bi), 1, data->stream);
		if (read_bytes <= 0) {
			debuglog(("No more info header read."));
			break;
		}
		print_wbf_info_header(&bi);
		
		data->imageSize.width = bi.Width;
		data->imageSize.height = bi.Height;

		for (i = 0; i < bi.ClrUsed; i++) {
			fread(&data->palettes[i], sizeof(RGBQUAD), 1, data->stream);
			bf.OffBits -= sizeof(RGBQUAD);
		}	

		bytes_per_line = ((data->imageSize.width + (pixels_per_byte - 1)) / pixels_per_byte);
		debuglog(("Bytes per line : %d", bytes_per_line));

		data->decompressedPixels = (char **) eps_malloc(sizeof(char *) * data->imageSize.height);
		if (data->decompressedPixels == NULL) {
			break;
		}

		for (i = 0; i < data->imageSize.height; i++) {
			data->decompressedPixels[i] = NULL;
		}

		for (i = 0; i < data->imageSize.height; i++) {
			data->decompressedPixels[i] = (char *) eps_malloc(bytes_per_line);
			if (data->decompressedPixels[i] == NULL) {
				break;
			}
			memset(data->decompressedPixels[i], 0x00, bytes_per_line);
		}

		if (i < data->imageSize.height) {
			break;
		}

		wbf_decompress_RLE4(data->stream, data->imageSize, data->decompressedPixels);

		size->width = data->imageSize.width;
		size->height = data->imageSize.height;

		error = EPS_BLEND_SOURCE_OK;

	} while (0);

	if (error == EPS_BLEND_SOURCE_ERROR && data) {
		eps_free(data);
		data = NULL;
	}

	return data;
}

int wbfReaderClose(void *wbf_handle)
{
	wbf_stream_private_data_t* data = (wbf_stream_private_data_t*)wbf_handle;
	int i;
	if (data) {
		if (data->decompressedPixels) {
			for (i = 0; i < data->imageSize.height; i++) {
				if (data->decompressedPixels[i]) {
					eps_free(data->decompressedPixels[i]);
					data->decompressedPixels[i] = NULL;
				}
			}
			eps_free(data->decompressedPixels);
			data->decompressedPixels = NULL;
		}

		eps_free(data);
		data = NULL;
	}

	return EPS_BLEND_SOURCE_OK;
}

int wbfReaderIsBlackPixel(void *wbf_handle, EpsPoint point)
{
	wbf_stream_private_data_t* data = (wbf_stream_private_data_t*)wbf_handle;
	int answer = 0;
	if (data && data->decompressedPixels) {
		if ((point.y < data->imageSize.height) && (point.x < data->imageSize.width)) {
			answer = wbf_getbit(data->decompressedPixels[point.y], point.x);
		}
	}

	return answer;
}
