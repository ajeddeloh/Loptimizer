CC = clang
CFLAGS = -ggdb -Wall -Wextra -pedantic -std=c99 -Werror

objects = gate.o expression.o minterm.o heap.o

all : optimizer

optimizer : $(objects)
	$(CC) $(CFLAGS) -o optimizer opt7400.c $(objects)

minterm.o : minterm.h
gate.o : minterm.h gate.h
expression.o : gate.h minterm.h expression.h
heap.o : heap.h

.PHONY : clean
clean:
	rm optimizer $(objects)
