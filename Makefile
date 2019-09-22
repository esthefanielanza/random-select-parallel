CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -Werror -std=c99 -fopenmp
LIBS = -lm

all: ep2 clean

ep2:  ep2.o input.o
	$(CC) $(CFLAGS) ep2.o input.o -o ep2 $(LIBS)

ep2.o: ep2.c input.h
	$(CC) $(CFLAGS) -c ep2.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

clean:
	rm *.o

