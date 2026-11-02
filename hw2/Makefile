.PHONY: all run clean

CC:=gcc
CFLAGS+=-Wall
EXTRA_CFLAGS+=

SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
BINS:=$(SRCS:%.c=%)

all: $(BINS)

$(BINS): $(OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $< -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

run: $(BINS)
	./$(BINS)

clean:
	rm -rf $(OBJS) $(BINS)
