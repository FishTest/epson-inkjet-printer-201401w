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
#include <cups/cups.h>
#include "debuglog.h"
#include "memory.h"
#include "filter_option.h"

#define WATERMAKR_OPTION_NAME		"Watermark"

extern ppd_file_t *	PPD;
extern const char *	JobOptions;

typedef struct {
	char	*choice;
	int	value;
} EpsFilterChoice;

typedef struct {
	char		*keyword;
	int		choice_num;
	EpsFilterChoice choiceList[10];
} EpsFilterOption;

static EpsFilterOption filterOptionPageLayout = {
	"PosterPrinting",
	5,
	{
		{"Off", EPS_PAGE_LAYOUT_1x1},
		{"2x1", EPS_PAGE_LAYOUT_2x1},
		{"2x2", EPS_PAGE_LAYOUT_2x2},
		{"3x3", EPS_PAGE_LAYOUT_3x3},
		{"4x4", EPS_PAGE_LAYOUT_4x4}
	}
};
static EpsFilterOption filterOptionRotate180 = {
	"Rotate180",
	2,
	{
		{"Off", EPS_ROTATE180_OFF},
		{"On", EPS_ROTATE180_ON}
		}
};
static EpsFilterOption filterOptionMirrorImage = {
	"MirrorImage",
	2,
	{
		{"Off", EPS_MIRROR_IMAGE_OFF},
		{"On", EPS_MIRROR_IMAGE_ON}
	}
};

static EpsFilterOption filterOptionWatermarkPosition = {
	"PositionWatermark",
	10,
	{
		{"Center",	EPS_PAGE_WATERMARK_POSITION_CENTER},
		{"TopLeft",	EPS_PAGE_WATERMARK_POSITION_TOPLEFT},
		{"Top",		EPS_PAGE_WATERMARK_POSITION_TOP},
		{"TopRight",	EPS_PAGE_WATERMARK_POSITION_TOPRIGHT},
		{"Left",	EPS_PAGE_WATERMARK_POSITION_LEFT},
		{"Right",	EPS_PAGE_WATERMARK_POSITION_RIGHT},
		{"BottomLeft",	EPS_PAGE_WATERMARK_POSITION_BOTTOMLEFT},
		{"Bottom",	EPS_PAGE_WATERMARK_POSITION_BOTTOM},
		{"BottomRight",	EPS_PAGE_WATERMARK_POSITION_BOTTOMRIGHT},
		{"Middle",	EPS_PAGE_WATERMARK_POSITION_CENTER}	/* For Am_In */
	}
};

static EpsFilterOption filterOptionWatermarkDensity = {
	"DensityWatermark",
	6,
	{
		{"Level1", EPS_PAGE_WATERMARK_DENSITY_LEVEL1}, /* Light */
		{"Level2", EPS_PAGE_WATERMARK_DENSITY_LEVEL2},
		{"Level3", EPS_PAGE_WATERMARK_DENSITY_LEVEL3},
		{"Level4", EPS_PAGE_WATERMARK_DENSITY_LEVEL4},
		{"Level5", EPS_PAGE_WATERMARK_DENSITY_LEVEL5}, 
		{"Level6", EPS_PAGE_WATERMARK_DENSITY_LEVEL6} /* Dark */
	}
};

static EpsFilterOption filterOptionWatermarkColor = {
	"ColurWatermark",
	7,
	{
		{"Black", EPS_PAGE_WATERMARK_COLOR_BLACK},
		{"Blue", EPS_PAGE_WATERMARK_COLOR_BLUE},
		{"Lime", EPS_PAGE_WATERMARK_COLOR_LIME},
		{"Aqua", EPS_PAGE_WATERMARK_COLOR_AQUA},
		{"Red", EPS_PAGE_WATERMARK_COLOR_RED},
		{"Fuchsia", EPS_PAGE_WATERMARK_COLOR_FUCHSIA},
		{"Yellow", EPS_PAGE_WATERMARK_COLOR_YELLOW}
	}
};

static EpsFilterOption filterOptionWatermarkSize = {
	"SizeWatermark",
	10,
	{
		{"10", EPS_PAGE_WATERMARK_SIZE_10},
		{"20", EPS_PAGE_WATERMARK_SIZE_20},
		{"30", EPS_PAGE_WATERMARK_SIZE_30},
		{"40", EPS_PAGE_WATERMARK_SIZE_40},
		{"50", EPS_PAGE_WATERMARK_SIZE_50},
		{"60", EPS_PAGE_WATERMARK_SIZE_60},
		{"70", EPS_PAGE_WATERMARK_SIZE_70},
		{"80", EPS_PAGE_WATERMARK_SIZE_80},
		{"90", EPS_PAGE_WATERMARK_SIZE_90},
		{"100", EPS_PAGE_WATERMARK_SIZE_100}
	}
};

ppd_attr_t * get_ppd_attr(const char * name, int isFirst)
{
	ppd_attr_t * attr = NULL;

	if (isFirst) {
		attr = ppdFindAttr(PPD, name, NULL);
	} else {
		attr = ppdFindNextAttr(PPD, name, NULL);
	}

#ifdef DEBUG
	if (attr) {
		debuglog(("PPD Attribute %s -> %s", name, attr->value));
	}
#endif

	return attr;
}

char * get_default_choice (const char *key)
{
	ppd_option_t * option;
	ppd_choice_t * choice;

	option = ppdFindOption (PPD, key);
	if (option == NULL) {
		debuglog(("Failed to get option %s", key));
		return NULL;
	}

	choice = ppdFindChoice (option, option->defchoice);
	if (choice == NULL) {
		debuglog(("Failed to get default choice"));
		return NULL;
	}

	return choice->choice;
}

char * get_option_for_job (const char * key)
{
	const char * cups_optstr = JobOptions;
	cups_option_t * cups_opt_p = NULL;
	char * choice = NULL;
	int cups_optnum = 0;

	if (cups_optstr) {
		cups_optnum = cupsParseOptions (cups_optstr, cups_optnum, &cups_opt_p);
	}

	if (cups_optstr && cups_optnum) {
		choice = (char *) cupsGetOption (key, cups_optnum, cups_opt_p);
	}

	return choice;
}

static int get_filter_option(int *value, EpsFilterOption option)
{
	int	i;
	char	*choice;
	int	error;
	
	error = 0;

	
	choice = (char *) get_option_for_job (option.keyword);
	if (choice == NULL) {
		choice = get_default_choice (option.keyword);
	}
	if (choice == NULL) {
		error = 1;
		return error;
	}
	
	for (i = 0; i < option.choice_num; i++) {
		if (strcmp(choice, option.choiceList[i].choice) == 0) {
			*value = option.choiceList[i].value;
			debuglog(("Option=%s Choice=%s", option.keyword, choice));
			break;
		}
	}

	return error;
}
 
int setup_filter_option (EpsFilterPrintOption *filterPrintOption)
{
	debuglog(("TRACE IN"));
	
	char		*choice;
	int		value;
	int		isFirst;
	ppd_attr_t	*attr = NULL;
	int		error;

	error = 0;
	filterPrintOption->pageLayout = EPS_PAGE_LAYOUT_1x1;
	filterPrintOption->rotate180 = EPS_ROTATE180_OFF;
	filterPrintOption->mirrorImage = EPS_MIRROR_IMAGE_OFF;

	filterPrintOption->useWatermark = 0;
	filterPrintOption->watermarkPosition = EPS_PAGE_WATERMARK_POSITION_CENTER;
	filterPrintOption->watermarkDensity = EPS_PAGE_WATERMARK_DENSITY_LEVEL4;
	filterPrintOption->watermarkColor = EPS_PAGE_WATERMARK_COLOR_RED;
	filterPrintOption->size_ratio = EPS_PAGE_WATERMARK_SIZE_70 / 10.0;

	// Page Layout
	error = get_filter_option(&value, filterOptionPageLayout);
	if (!error) {
	  filterPrintOption->pageLayout = value;
	}

	// Rotate180
	error = get_filter_option(&value, filterOptionRotate180);
	if (!error) {
	  filterPrintOption->rotate180 = value;
	}

	// Mirror Image
	error = get_filter_option(&value, filterOptionMirrorImage);
	if (!error) {
	  filterPrintOption->mirrorImage = value;
	}

	// Watermark 
	error = 0;
	choice = (char *) get_option_for_job (WATERMAKR_OPTION_NAME);
	if (choice == NULL) {
		choice = get_default_choice (WATERMAKR_OPTION_NAME);
	}
	if (choice == NULL) {
		error = 1;
	}
	if (!error) {
	  isFirst = 1;
	  attr = NULL;
	  while (attr = get_ppd_attr ("epcgWatermarkData", isFirst)) {		
	    if (strcmp(choice, attr->spec) == 0) {
	      filterPrintOption->useWatermark = 1;
	      strncpy(filterPrintOption->watermarkFilePath, attr->value, 512);
	      break;
	    }
	    isFirst = 0;
	  }	
	  debuglog(("Option=Watermark Choice=%s", choice));
	}

	// Position of Watermark
	error = get_filter_option(&value, filterOptionWatermarkPosition);
	if (!error) {
	  filterPrintOption->watermarkPosition = value;
	}

	// Density of Watermark
	error = get_filter_option(&value, filterOptionWatermarkDensity);
	if (!error) {
	  filterPrintOption->watermarkDensity = value;
	}

	// Colur of Watermark
	error = get_filter_option(&value, filterOptionWatermarkColor);
	if (!error) {
	  filterPrintOption->watermarkColor = value;
	}

	// Size of Watermark
	error = get_filter_option(&value, filterOptionWatermarkSize);
	if (!error) {
	  filterPrintOption->size_ratio = value / 10.0;
	}

	error = 0;
	debuglog(("TRACE OUT=%d", error));

	return error;
}
