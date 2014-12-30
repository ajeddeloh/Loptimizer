#ifndef GATE_H
#define GATE_H

#include "minterm.h"

typedef struct Gate {
	char *name;
	int n_inputs;
	int n_gates;
	char *operation; //in postfix
} Gate;

Gate *parseGate(char *path);

Gate *mkGate(const char* name, int n_inputs, const char *operation, int n_gates);

void freeGate(Gate *gate);


#endif
