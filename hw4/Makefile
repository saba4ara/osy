.PHONY: all clean

CC ?= gcc
EXTRA_CFLAGS ?= -fsanitize=address -fno-omit-frame-pointer
CFLAGS = -Wall -g -O2 -pthread $(EXTRA_CFLAGS)
LDFLAGS = -pthread $(EXTRA_CFLAGS)

all: factory

factory: factory.o
	$(CC) $(LDFLAGS) -o $@ $^

factory.o: factory.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf factory factory.o *.zip *.tar

zip: factory.c
	zip factory.zip factory.c Makefile
