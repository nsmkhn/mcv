CC=cc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb

mcv: main.c
	$(CC) $(CFLAGS) main.c -o mcv

clean:
	rm -f mcv *.ppm
