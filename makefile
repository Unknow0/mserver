PROG=mserver
CC=gcc
CFLAGS=-g
LDFLAGS=-lcfg -llogger -pthread -lgstreamer-1.0 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lbcm_host -lglib-2.0 -lcontainer -L/usr/local/lib/ -ljson
PREFIX=/usr/local/

SRC=$(filter-out msend.c, $(wildcard *.c))
OBJECTS=$(SRC:.c=.o)

all:build geninit
build: $(PROG) msend

$(PROG): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(PROG) $^
msend: msend.c
	$(CC) -o msend $^

geninit: initscript.vim
	ex -u NONE -c "let g:path = '$(PREFIX)/bin/'" -c "source initscript.vim" -c "x" mserver.sh

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean

clean:
	rm -f $(OBJECTS)
	rm -f $(PROG)
