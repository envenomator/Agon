CC=gcc
CFLAGS=-I.
DEPS = loadstart.h
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: sokobanprep levelreader

sokobanprep: sokobanprep.c
	$(CC) -o sokobanprep sokobanprep.c

levelreader: levelreader.c
	$(CC) -o levelreader levelreader.c
