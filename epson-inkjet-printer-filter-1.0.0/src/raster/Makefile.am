# Copyright (C) Seiko Epson Corporation 2009.
#
SUBDIRS = blendSource

INCLUDES = \
	-I../ \
	-I../memory \
	-I./blendSource

AM_CFLAGS = -fsigned-char

noinst_LTLIBRARIES = libraster.la

libraster_la_LIBADD = \
	./blendSource/libblendSource.la \
	../memory/libmemory.la 

libraster_la_SOURCES = \
	mirror.c \
	fetch-pool.c \
	raster-helper.c \
	raster.c \
	reverse.c \
	blend.c \
	scale.c 

noinst_HEADERS = \
	mirror.h \
	fetch-pool.h \
	raster-helper.h \
	raster.h \
	reverse.h \
	blend.h \
	scale.h
