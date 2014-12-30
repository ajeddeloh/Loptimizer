CC = gcc
CFLAGS = -ggdb -Wall -Wextra -pedantic -std=c99 

all : optimizer

optimizer : gate.o expression.o
	$(CC) $(CFLAGS) -o optimizer opt7400.c gate.o expression.o

expression.o :
	$(CC) $(CFLAGS) -c expression.c

gate.o :
	$(CC) $(CFLAGS) -c gate.c

clean:
	rm *.o
	rm optimizer
