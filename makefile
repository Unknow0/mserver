PROG=mserver
CC=gcc
CFLAGS=-g
LDFLAGS=-lcfg -llogger -pthread -lgstreamer-1.0 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lbcm_host -lglib-2.0 -lcontainer -L/usr/local/lib/ -ljson

SRC=$(wildcard *.c)
OBJECTS=$(SRC:.c=.o)

all:build
build: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(PROG) $^

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean

clean:
	rm -f $(OBJECTS)
	rm -f $(PROG)
