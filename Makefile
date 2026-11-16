CC=gcc
CFLAGS=-Wall -Wextra -O0 -g -Iinc/
SRC=$(wildcard src/*)
INC=$(wildcard inc/*)
all: link

link: main.c $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

indent:
	indent -linux main.c $(SRC) $(INC)
	rm -rf *.c~ src/*.c~ inc/*.h~

clean:
	rm -rf link out.txt
