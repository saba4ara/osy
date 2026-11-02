.PHONY: all run valgrind clean
.SILENT: run

CC := gcc
CFLAGS := -Wall -g -O2 -pthread
EXTRA_CFLAGS ?= -fsanitize=address -fno-omit-frame-pointer
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
BINS := prod-cons
ARGS :=

all: $(BINS)

$(BINS): $(OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS)  $^ -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $< -c -o $@

run: $(BINS)
	./$(BINS) $(ARGS)

clean:
	rm -rf $(OBJS) $(BINS) *.tar
tar: $(SRCS)
	tar -cvf $(BINS).tar Makefile $(SRCS)

