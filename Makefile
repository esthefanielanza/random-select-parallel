CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -Werror -std=c99 -fopenmp
LIBS = -lm

all: sequential clean

sequential:  sequential.o input.o
	$(CC) $(CFLAGS) sequential.o input.o -o sequential $(LIBS)

sequential.o: sequential.c input.h
	$(CC) $(CFLAGS) -c sequential.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

clean:
	rm *.o

