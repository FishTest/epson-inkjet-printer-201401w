# Copyright (C) Seiko Epson Corporation 2009.

INCLUDES = \
	-I../ \
	-I../../ \
	-I../../memory

AM_CFLAGS = -fsigned-char

noinst_LTLIBRARIES = libblendSource.la

libblendSource_la_LIBADD = \
	../../memory/libmemory.la 

libblendSource_la_SOURCES = \
	blend-watermark.c blend-watermark-wbf-reader.c \
	blend-source.c blend-source.h

noinst_HEADERS = \
	blend-source.h
