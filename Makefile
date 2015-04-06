CC = gcc
SRC_DIR = src

OCTO_DIR = $(SRC_DIR)/Octothorpe
INCLUDE = -I$(OCTO_DIR)/include/ 
CFLAGS = -ggdb -pg -Wall -Wextra -Werror -pedantic -std=c99 $(INCLUDE) -march=native -mtune=native -O3 

HEADERS = $(wildcard $(SRC_DIR)/*.h)
OBJECTS = $(HEADERS:.h=.o) $(OCTO_DIR)/libocto.a


.PHONY : all
all : optimizer

optimizer : $(OBJECTS) $(SRC_DIR)/opt7400.c
	echo $(HEADERS)
	echo $(OBJECTS)
	$(CC) $(CFLAGS) -o optimizer $(SRC_DIR)/opt7400.c $(OBJECTS)

$(OCTO_DIR)/libocto.a : 
	make -C $(OCTO_DIR) libocto.a

.o: $(HEADERS)
	$(CC) $(CFLAGS) -c $<
#minterm.o : $(HEADERS) $(SRC_DIR)/minterm.c
#	$(CC) $(CFLAGS) -c $(SRC_DIR)/minterm.c
#gate.o : $(HEADERS) $(SRC_DIR)/gate.c
#	$(CC) $(CFLAGS) -c $(SRC_DIR)/gate.c
#expression.o : $(HEADERS) $(SRC_DIR)/expression.c
#	$(CC) $(CFLAGS) -c $(SRC_DIR)/expression.c
#graph_store.o : $(HEADERS) $(SRC_DIR)/graph_store.c
#	$(CC) $(CFLAGS) -c $(SRC_DIR)/graph_store.c

.PHONY : clean
clean:
	rm -f optimizer $(OBJECTS)
	make -C $(OCTO_DIR) clean
