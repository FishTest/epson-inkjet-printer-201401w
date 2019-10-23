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
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "blend-source.h"

/* implementation (blend-watermark-wbf-reader.c) */
extern void * wbfReaderOpen(FILE* fstream, EpsSize *size);
extern int wbfReaderClose(void *wbf_handle);
extern int wbfReaderIsBlackPixel(void *wbf_handle, EpsPoint point);

static int WatermarkOpen(void *privateData, const char* sourcePath, EpsSize size, EpsColor color);
static int WatermarkBlendingPixels(void *privateData, unsigned char* pixelBuf, int bytesPixelBuf, int pixelCount);
static int WatermarkClose(void *privateData);
static void WatermarkPrivateFinalize(void *privateData);

typedef struct WatermarkPrivateData {
	FILE *wbfFile;
	void *wbfReaderHandle;
	EpsSize wbfImageSize;
	EpsColor watermarkColor;
	EpsRect fitPageBounds;
	float scaleRatio;
	int rasterLineIndex;
	int blendingLineIndex;
} WatermarkPrivateData;

int blend_watermark_initialize_instance(EpsBlendSource *instance)
{
	int error = EPS_BLEND_SOURCE_OK;
	WatermarkPrivateData *privateData = (WatermarkPrivateData *) eps_malloc(sizeof(WatermarkPrivateData));
	if (privateData) {
		instance->open = WatermarkOpen;
		instance->blendingPixels = WatermarkBlendingPixels;
		instance->close = WatermarkClose;
		instance->finalize = WatermarkPrivateFinalize;
		instance->privateData = privateData;
	} else {
		error = EPS_BLEND_SOURCE_ERROR;
	}

	return error;
}

static int WatermarkOpen(void *privateData, const char* sourcePath, EpsSize size, EpsColor color)
{
	float x_scale;
	float y_scale;

	debuglog(("sourcePath : %s", sourcePath));
	debuglog(("size (%d, %d)", size.width, size.height));
	debuglog(("color (%f, %f, %f, %f)", color.red, color.green, color.blue, color.alpha));

	int error = EPS_BLEND_SOURCE_ERROR;
	WatermarkPrivateData *data = (WatermarkPrivateData *) privateData;

	do {
		if (sourcePath == NULL) {
			break;
		}

		data->wbfFile = fopen(sourcePath, "rb");
		if (data->wbfFile == NULL) {
			debuglog(("failed to open : %s -> %s", sourcePath, strerror(errno)));
			
			break;
		}

		data->wbfReaderHandle = wbfReaderOpen(data->wbfFile, &data->wbfImageSize);
		if (data->wbfReaderHandle == NULL) {
			debuglog(("failed to open wbfReader"));
			break;
		}

		x_scale = (float) size.width / (float) data->wbfImageSize.width;
		y_scale = (float) size.height / (float) data->wbfImageSize.height;
		data->scaleRatio = x_scale < y_scale ? x_scale : y_scale;
		debuglog(("scale (%.2f, %.2f) -> %.2f", x_scale, y_scale, data->scaleRatio));

		data->fitPageBounds.size.width = (float) data->wbfImageSize.width * data->scaleRatio;
		data->fitPageBounds.size.height = (float) data->wbfImageSize.height * data->scaleRatio;
		data->fitPageBounds.origin.x = (size.width - data->fitPageBounds.size.width) / 2;
		data->fitPageBounds.origin.y = (size.height - data->fitPageBounds.size.height) / 2;

		debuglog(("fit page bounds origin (%d, %d)", data->fitPageBounds.origin.x, data->fitPageBounds.origin.y));
		debuglog(("fit page bounds size   (%d, %d)", data->fitPageBounds.size.width, data->fitPageBounds.size.height));

		data->rasterLineIndex = 0;
		data->blendingLineIndex = 0;
		data->watermarkColor = color;

		error = EPS_BLEND_SOURCE_OK;
	} while (0);

	return error;
}

static int WatermarkBlendingPixels(void *privateData, unsigned char* pixelBuf, int bytesPixelBuf, int pixelCount)
{
	float alpha;
	float valueA;
	float valueB;
	float value;
	float color[3];
	int i;
	int k;

	WatermarkPrivateData *data = (WatermarkPrivateData *) privateData;
	int bytesPerPixel = bytesPixelBuf / pixelCount;
	unsigned char *pixelPtr = pixelBuf;
	EpsPoint point = epsMakePoint(0, 0);
	int black = 0;

	if (is_current_raster_in_blending_bounds(data->rasterLineIndex++, data->fitPageBounds)) {
		point.y = (float)data->blendingLineIndex++ / data->scaleRatio;
		color[0] = data->watermarkColor.red;
		color[1] = data->watermarkColor.green;
		color[2] = data->watermarkColor.blue;
		alpha = data->watermarkColor.alpha;

		for (i = 0; i < pixelCount; i++) {
			point.x = (float) i / data->scaleRatio;
			black = wbfReaderIsBlackPixel(data->wbfReaderHandle, point);
			for (k = 0; k < bytesPerPixel; k++) {
				if (black) {
					valueA = ((float) pixelPtr[k]) / 255.f;
					valueB = color[k];
					value = ((1.0 - alpha) * valueA) + (alpha * valueB);
					pixelPtr[k] = (unsigned char) (value * 255.f);
				}
			}
			pixelPtr += bytesPerPixel;
		}
	}

	return EPS_BLEND_SOURCE_OK;
}

static int WatermarkClose(void *privateData)
{
	int error = EPS_BLEND_SOURCE_ERROR;
	WatermarkPrivateData *data = (WatermarkPrivateData *) privateData;
	do {
		if (data->wbfReaderHandle) {
			wbfReaderClose(data->wbfReaderHandle);
			data->wbfReaderHandle = NULL;
		}

		if (data->wbfFile) {
			fclose(data->wbfFile);
			data->wbfFile = NULL;
		}

		error = EPS_BLEND_SOURCE_OK;

	} while (0);

	return error;
}

void WatermarkPrivateFinalize(void *privateData)
{
	if (privateData) {
		eps_free(privateData);
		privateData = NULL;
	}
}

