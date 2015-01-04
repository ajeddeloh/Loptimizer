CC = clang
INCLUDE = -I./Octothorpe/include/
CFLAGS = -ggdb -Wall -Wextra -pedantic -std=c99 -Werror $(INCLUDE)

objects = gate.o expression.o minterm.o heap.o Octothorpe/libocto.a

all : optimizer

optimizer : $(objects) opt7400.c
	$(CC) $(CFLAGS) -o optimizer opt7400.c $(objects)

minterm.o : minterm.h
gate.o : minterm.h gate.h
expression.o : gate.h minterm.h expression.h
heap.o : heap.h

.PHONY : clean
clean:
	rm optimizer $(objects)
