#ifndef GATE_H
#define GATE_H

#include "minterm.h"

typedef struct Gate {
	char *name;
	size_t n_inputs;
	size_t n_gates;
	char *operation; //in postfix
} Gate;

Gate *gate_parse(char *path);

Gate *gate_new(const char* name, int n_inputs, const char *operation, size_t n_gates);

void gate_generate_indices(Gate *g, size_t **indices, size_t closed_set_size);

void gate_free(Gate *gate);


#endif
