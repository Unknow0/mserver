ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST=README.md

bin_PROGRAMS=mserver
if !DISABLE_MSEND
bin_PROGRAMS+= msend

msend_SOURCES=src/msend.c
endif


mserver_SOURCES=\
	src/lib.c \
	src/lib_str.c \
	src/player.h \
	src/playlist.h \
	src/server.h \
	src/lib.h \
	src/main.c \
	src/playlist.c \
	src/server.c \
	src/lcg.c

mserver_LDADD=$(DEPS_LIBS)

## TODO
##pkgconfigdir=$(libdir)/pkgconfig
##pkgconfig_DATA=container.pc
## TODO: init script

distclean-local:
	-rm -rf Makefile.in aclocal.m4 autom4te.cache/ config.guess config.sub depcomp install-sh ltmain.sh missing
