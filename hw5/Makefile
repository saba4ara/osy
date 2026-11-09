.PHONY: all run clean zip

CC := gcc
NAME = hexconv
GNU_V ?= gnu23
CFLAGS += -ffreestanding -fno-stack-protector -nostdlib -nostdinc -static -Wall -Wextra -pedantic -g -O2 -std=$(GNU_V)

all: $(NAME)

$(NAME): $(NAME).o
	$(CC) $(CFLAGS) $< -o $@

$(NAME).o: $(NAME).c
	$(CC) $(CFLAGS) $< -c -o $@

run: $(NAME)
	./$(NAME) 

clean:
	rm -rf $(NAME) $(NAME).o *.zip *.tar

zip:
	zip $(NAME).zip $(NAME).c

