#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gate.h"

Gate *gate_new (const char* name, int n_inputs, const char *operation, size_t n_gates) {
	Gate *gate = malloc( sizeof(Gate) );
	gate->n_inputs = n_inputs;
	gate->operation = strdup(operation);
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
	while( getline(&line, &n, fp) != -1) {
		*strchr(line, '\n') = '\0'; //replace newline with null terminator
		if( line[0] == '#' || strlen(line) == 0 ) {
			continue;
		}
		if(name == NULL) {
			name = strdup(line);
		} else if(n_inputs == 0) {
			int n = atol(line);
			if(n == 0) break;
			n_inputs = n;
		} else if(n_gates == 0) {
			int n = atol(line);
			if(n == 0) break;
			n_gates = n;
		} else if (operation == NULL) {
			printf("%s\n",line);
			operation = strdup(line);
		} else { //error
			printf("Invalid format in %s\n",path);
			free(line);
			free(name);
			free(operation);
			fclose(fp);
			return NULL;
		}	
	}
	Gate *g = gate_new(name, n_inputs, operation, n_gates);
	free(name);
	free(line);
	free(operation);
	fclose(fp);
	return g;
}
			
