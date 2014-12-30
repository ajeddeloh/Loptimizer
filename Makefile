CC = gcc
CFLAGS = -ggdb -Wall -Wextra -pedantic -std=c99 

all : optimizer

optimizer : gate.o expression.o
	$(CC) $(CFLAGS) -o optimizer opt7400.c gate.o expression.o

%.o : %.c
	$(CC) $(CFLAGS) -c $<


clean:
	rm *.o
	rm optimizer
