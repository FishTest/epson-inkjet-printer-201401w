#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(epson-inkjet-printer-filter, 1.0.0, epson@localdomain)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/raster_to_epson.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])

# Define flags
AC_ARG_ENABLE(debug,
        AC_HELP_STRING([--enable-debug],[enable debug]),
        [ if test "x$enable_debug" = "xyes" ; then
                AC_DEFINE([DEBUG], 1, [SET DEBUG = 1])
        fi], 
        [RELEASE_BUILD=yes]
)

if test "x$RELEASE_BUILD" = "xyes" ; then
        CFLAGS=-O2
        CXXFLAGS=-O2
fi

if test "x${prefix}" = "xNONE"; then
	AC_MSG_ERROR([*** '--prefix=/PATH' missing, please specify a prefix PATH ***])
fi

BUILD_MACHINE_ARCH=`uname -m`
if test "x$BUILD_MACHINE_ARCH" = "xx86_64"; then
	LSB_ARCH=64
fi

if test "x$CC" = "x/opt/lsb/bin/lsbcc"; then
	CUPS_SERVER_DIR=${prefix}/cups/lib
	CUPS_LIBS="/opt/lsb/lib"$LSB_ARCH"/libcups.so /opt/lsb/lib"$LSB_ARCH"/libm.so"
	CUPS_IMAGE_LIBS="/opt/lsb/lib"$LSB_ARCH"/libcupsimage.so /opt/lsb/lib"$LSB_ARCH"/libcups.so /opt/lsb/lib"$LSB_ARCH"/libjpeg.so"
	DL_LIBS="/opt/lsb/lib"$LSB_ARCH"/libdl.so"
	STDCPP_LIBS="/opt/lsb/lib"$LSB_ARCH"/libstdc++.so"
else
	CUPS_SERVER_DIR=${prefix}/lib/cups
	CUPS_LIBS='-lcups -lm'
	CUPS_IMAGE_LIBS='-lcupsimage -lcups -ljpeg -lm'
	DL_LIBS='-ldl'
	STDCPP_LIBS='-lstdc++'
fi

AC_SUBST(CUPS_SERVER_DIR)
AC_SUBST(CUPS_LIBS)
AC_SUBST(CUPS_IMAGE_LIBS)
AC_SUBST(DL_LIBS)
AC_SUBST(STDCPP_LIBS)

CORE_LIBRARY_PATH=${prefix}/lib"$LSB_ARCH"
CORE_RESOURCE_PATH=${prefix}/resource

AC_SUBST(CORE_LIBRARY_PATH)
AC_SUBST(CORE_RESOURCE_PATH)
AC_DEFINE_UNQUOTED(CORE_LIBRARY_PATH, "$CORE_LIBRARY_PATH", CORE_LIBRARY_PATH)
AC_DEFINE_UNQUOTED(CORE_RESOURCE_PATH, "$CORE_RESOURCE_PATH", CORE_RESOURCE_PATH)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset strdup])

AC_CONFIG_FILES([
                Makefile
                src/Makefile
                src/raster/Makefile
                src/raster/blendSource/Makefile
                src/memory/Makefile
                src/pagemanager/Makefile
                src/filteropt/Makefile
])
AC_OUTPUT
