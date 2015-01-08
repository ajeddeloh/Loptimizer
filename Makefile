CC = clang
SRC_DIR = src

OCTO_DIR = $(SRC_DIR)/Octothorpe
INCLUDE = -I$(OCTO_DIR)/include/ 
CFLAGS = -ggdb -Wall -Wextra -Werror -pedantic -std=c99 $(INCLUDE) -march=native -mtune=native -O3

OBJECTS = gate.o expression.o minterm.o heap.o $(OCTO_DIR)/libocto.a

.PHONY : all
all : optimizer

optimizer : $(OBJECTS) $(SRC_DIR)/opt7400.c
	$(CC) $(CFLAGS) -o optimizer $(SRC_DIR)/opt7400.c $(OBJECTS)

$(OCTO_DIR)/libocto.a : 
	make -C $(OCTO_DIR) libocto.a

minterm.o : $(SRC_DIR)/minterm.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/minterm.c
gate.o : $(SRC_DIR)/minterm.h $(SRC_DIR)/gate.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/gate.c
expression.o : $(SRC_DIR)/gate.h $(SRC_DIR)/minterm.h $(SRC_DIR)/expression.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/expression.c
heap.o : $(SRC_DIR)/heap.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/heap.c

.PHONY : clean
clean:
	rm -f optimizer $(OBJECTS)
	make -C $(OCTO_DIR) clean
