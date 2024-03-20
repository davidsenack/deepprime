CC = gcc
CFLAGS = -O3 -march=native -flto -fopenmp
LIBS = -lgmp -lmpi -lm

all: main test_main

main: src/main.c
	$(CC) $(CFLAGS) -o main src/main.c $(LIBS)

test_main: src/test_main.c
	$(CC) $(CFLAGS) -o test_main src/test_main.c $(LIBS)

clean:
	rm -f main test_main
