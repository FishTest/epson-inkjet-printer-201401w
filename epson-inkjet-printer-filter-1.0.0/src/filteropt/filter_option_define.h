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
#ifndef __EPS_FILTER_OPTION_DEFINE_H__

#define __EPS_FILTER_OPTION_DEFINE_H__


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum  {
	EPS_PAGE_LAYOUT_1x1 = 0,
	EPS_PAGE_LAYOUT_2x1,
	EPS_PAGE_LAYOUT_2x2,
	EPS_PAGE_LAYOUT_3x3,
	EPS_PAGE_LAYOUT_4x4
} EpsPageLayout;

typedef enum  {
	EPS_ROTATE180_OFF = 0,
	EPS_ROTATE180_ON
} EpsRotate180;

typedef enum  {
	EPS_MIRROR_IMAGE_OFF = 0,
	EPS_MIRROR_IMAGE_ON
} EpsMirrorImage;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __EPS_FILTER_OPTION_DEFINE_H__ */
