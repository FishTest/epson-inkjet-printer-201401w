# Copyright (C) Seiko Epson Corporation 2009.
#
INCLUDES = \
	-I.. \
	-I../../include \
	-I../memory \
	-I../filteropt \
	-I../raster

noinst_LTLIBRARIES = libpagemanager.la

libpagemanager_la_LIBADD = \
	../memory/libmemory.la \
	../raster/libraster.la 

libpagemanager_la_SOURCES = \
	subpage.c subpage.h \
	subpagemanager.c subpagemanager.h \
	pagemanager.c pagemanager.h

noinst_HEADERS = \
	subpage.h \
	subpagemanager.h \
	pagemanager.h
