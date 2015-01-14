#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "gate.h"

static bool validate_operation(const char *operation, size_t n_inputs);

static bool validate_operation(const char *operation, size_t n_inputs) {
	if(operation == NULL && n_inputs == 0) {
		return true; //input gate
	}
	int stack_size = 0;
	while(*operation != '\0') {
		switch(*operation) {
		case '!':
			if(stack_size == 0) return false;
			break;
		case '+':
		case '|':
		case '&':
		case '*':
		case '^':
			if(stack_size < 2) return false;
			stack_size --;
			break;
		default:
			if(!isupper(*operation)) return false;
			if(*operation - 'A' >= (int)n_inputs) return false;
			stack_size++;
			break;
		}
		operation++;
	}
	return true;
}

Gate *gate_new (const char* name, int n_inputs, const char *operation, size_t n_gates) {
	Gate *gate = malloc( sizeof(Gate) );
	gate->n_inputs = n_inputs;
	if( operation != NULL) {
		gate->operation = strdup(operation);
	}
	gate->name = strdup(name);
	gate->n_gates = n_gates;
	return gate;
}

void gate_free(Gate* gate) {
	free(gate->operation);
	free(gate->name);
	free(gate);
}

//gate file format:
//#comments
//n_inputs
//n_gates
//operation string
//optional special properties (e.g. associative)
Gate *gate_parse(char *path) {
	FILE *fp = fopen(path, "r");
	if(fp == NULL) {
		perror(path);
		return NULL;
	}	
	char *line = NULL;
	size_t n = 0;
	size_t n_inputs = 0;
	size_t n_gates = 0;
	char *name = NULL;
	char *operation = NULL;
	bool error = false;
	while( getline(&line, &n, fp) != -1) {
		*strchr(line, '\n') = '\0'; //replace newline with null terminator
		if( line[0] == '#' || strlen(line) == 0 ) {
			continue;
		}
		if(name == NULL) {
			name = strdup(line);
		} else if(n_inputs == 0) {
			int n = atol(line);
			if(n == 0) {
				error = true;
				break;
			}
			n_inputs = n;
		} else if(n_gates == 0) {
			int n = atol(line);
			if(n == 0) {
				error = true;
				break;
			}
			n_gates = n;
		} else if (operation == NULL) {
			if(!validate_operation(line, n_inputs)) {
				error = true;
				break;
			}
			operation = strdup(line);
		} else { //error
		}	
	}
	if(error) {
		printf("Invalid format in %s\n",path);
		free(line);
		free(name);
		free(operation);
		fclose(fp);
		return NULL;
	}
	Gate *g = gate_new(name, n_inputs, operation, n_gates);
	free(name);
	free(line);
	free(operation);
	fclose(fp);
	return g;
}

//pass NULL for g to get next val, will set indices to NULL and free the indices at the last
//index
//only works with associative gates for now
void gate_generate_indices(Gate *g, size_t **indices, size_t closed_set_size) {
	size_t newidx = closed_set_size-1;
	if(*indices == NULL) {
		*indices = calloc(g->n_inputs, sizeof(size_t));
		(*indices)[0] = newidx;
		return;
	}
	if(g->n_inputs == 1) {
		free(*indices);
		*indices = NULL;
		return;
	}
	(*indices)[1] ++;
	for(size_t i = 1; i < g->n_inputs && (*indices)[i] == closed_set_size; i++) {
		if(i == g->n_inputs - 1) {
			free(*indices);
			*indices = NULL;
			return;
		}
		(*indices)[i] = 0;
		(*indices)[i+1]++;
	}
}



