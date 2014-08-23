#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mserver], [1.0])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CC

# TODO
AX_PTHREAD([], AC_MSG_ERROR([pthread not found]))
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

AC_ARG_ENABLE([msend], AS_HELP_STRING([--disable-msend], [disable msend build]))
AM_CONDITIONAL([DISABLE_MSEND], [test "x${enable_msend}" = "xno"])

# Checks for libraries.
AC_CHECK_LIB([bcm_host], [bcm_host_init])
AC_CHECK_LIB([json], [json_object_new_object], [], 
	AC_MSG_ERROR([install json-c: https://github.com/json-c/json-c/])
	)
AC_CHECK_LIB([container], [chunked_list_create], [], 
	AC_MSG_ERROR([install container: https://github.com/Unknow0/libcontainer/])
	)
AC_CHECK_LIB([cfg], [cfg_init], [], 
	AC_MSG_ERROR([install libcfg: https://github.com/Unknow0/libcfg/])
	)
AC_CHECK_LIB([logger], [get_logger], [], 
	AC_MSG_ERROR([install logger: https://github.com/Unknow0/logger/])
	)
AC_CHECK_LIB([gstreamer-1.0], [gst_version], [],
	AC_MSG_ERROR([install gstreamer-1.0 http://gstreamer.freedesktop.org/])
	)

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
AC_CHECK_FUNCS([socket strdup strerror])

AC_CONFIG_FILES([
	Makefile
	])
AC_OUTPUT
