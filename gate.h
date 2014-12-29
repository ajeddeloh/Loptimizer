#ifndef GATE_H
#define GATE_H

typedef unsigned long long minterm ;


typedef struct Gate {
	char *name;
	int n_inputs;
	int n_gates;
	char *operation; //in postfix
} Gate;

Gate *parseGate(char *path);

Gate *mkGate(const char* name, int n_inputs, const char *operation, int n_gates);

void freeGate(Gate *gate);

minterm eval(Gate *gate, minterm *inputs);

#endif
