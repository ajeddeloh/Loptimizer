#ifndef GATE_H
#define GATE_H

#include "minterm.h"

typedef struct Gate {
	char *name;
	int n_inputs;
	int n_gates;
	char *operation; //in postfix
} Gate;

Gate *gate_parse(char *path);

Gate *gate_new(const char* name, int n_inputs, const char *operation, int n_gates);

void gate_free(Gate *gate);


#endif
