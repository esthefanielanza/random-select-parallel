CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -Werror -std=c99 -fopenmp
LIBS = -lm

all: ep2 clean

ep2:  ep2.o
	$(CC) $(CFLAGS) main -o ep2 $(LIBS)

sequential.o: ep2.c
	$(CC) $(CFLAGS) -c ep2.c


clean:
	rm *.o

