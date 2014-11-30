#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mserver], [1.0])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([tools])

AM_INIT_AUTOMAKE([foreign])

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

AC_ARG_ENABLE([memleak], AS_HELP_STRING([--enable-memleak], [enable memleak test]))
AS_IF([test "x${enable_memleak}" = "xyes"], [
	AM_CONDITIONAL([MEMLEAK], true)
	AC_DEFINE([MEMLEAK], [1], [Who care])
	], [
	AM_CONDITIONAL([MEMLEAK], false)
	])

# Checks for libraries.
AC_CHECK_LIB([bcm_host], [bcm_host_init])
AC_CHECK_LIB([json-c], [json_object_new_object], [], 
	AC_MSG_ERROR([install json-c: https://github.com/json-c/json-c/])
	)

PKG_CHECK_MODULES([DEPS], [libavdevice libavformat libavcodec libavutil libavfilter libffplay logger libcfg container])

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
