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
	} else {
		gate->operation = NULL;
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
	Gate *gate = calloc(1, sizeof(Gate));

	while( getline(&line, &n, fp) != -1) {
		*strchr(line, '\n') = '\0'; //replace newline with null terminator
		if( line[0] == '#' || strlen(line) == 0 ) {
			continue;
		}
		char *key = strtok(line, " =");
		char *value = strtok(NULL, " =");
		
		if( strcmp(key, "name") == 0) {
			gate->name = strdup(value);
		} else if( strcmp(key, "inputs") == 0) {
			gate->n_inputs = atol(value);
		} else if( strcmp(key, "gatesperic") == 0) {
			gate->n_gates = atol(value);
		} else if( strcmp(key, "operation") == 0) {
			gate->operation = strdup(value);
		} else if( strcmp(key, "flags") == 0) {
			char *flag = strtok(value, ", ");
			while(flag != NULL) {
				if( strcmp(flag, "symmetric") == 0) {
					gate->symmetric = true;
				} else if( strcmp(flag, "repeatable") == 0) {
					gate->repeatable = true;
				} else {
					printf("Error parsing %s: unknown flag %s, ignoring flag\n", path, flag);
				}
				flag = strtok(NULL, " ,");
			}
		} else {
			printf("Error parsing %s: unknown key %s, ignoring key\n", path, value);
		}			
	}
	free(line);
	fclose(fp);
	
	//check for correctness	

	//was name specified?
	if(gate->name == NULL) {
		printf("Gate name not specified for %s. This gate will be ignored\n", path);
		gate_free(gate);
		return NULL;
	}

	//was number of inputs specified and valid?
	if( gate->n_inputs == 0) {
		printf("Unspecified or invalid number of inputs for %s. This gate will be ignored\n", path);
		gate_free(gate);
		return NULL;
	}

	//was the number of gates per ic specified?
	if( gate->n_gates == 0) {
		printf("Unspecified or invalid number of gates per ic for %s. This gate will be ignored\n", path);
		gate_free(gate);
		return NULL;
	}

	//was the operation specified?
	if( gate->operation == NULL) {
		printf("Unspecified operation for %s. This gate will be ignored\n", path);
		gate_free(gate);
		return NULL;
	}

	//was the operation specified valid postfix?
	if( !validate_operation(gate->operation, gate->n_inputs) ) {
		printf("Invalid operation for %s. This gate will be ignore\n", path);
		gate_free(gate);
		return NULL;
	}
	
	//everything passed its checks, we're all good!
	return gate;
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



