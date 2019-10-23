# Copyright (C) Seiko Epson Corporation 2009.

SUBDIRS = memory raster pagemanager filteropt

INCLUDES = \
	-I../include \
	-I./raster \
	-I./memory \
	-I./pagemanager \
	-I./filteropt

bindir = $(CUPS_SERVER_DIR)/filter

bin_PROGRAMS = epson_inkjet_printer_filter

epson_inkjet_printer_filter_LDADD = \
	@DL_LIBS@ \
	@STDCPP_LIBS@ \
	@CUPS_IMAGE_LIBS@ \
	./memory/libmemory.la \
	./pagemanager/libpagemanager.la \
	./filteropt/libfilteropt.la \
	./raster/libraster.la

epson_inkjet_printer_filter_SOURCES = \
	main.c \
	debuglog.h \
	raster_to_epson.c raster_to_epson.h

noinst_HEADERS = \
	debuglog.h
