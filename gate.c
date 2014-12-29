#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gate.h"

Gate *mkGate (const char* name, int n_inputs, const char *operation, int n_gates) {
	Gate *gate = malloc( sizeof(Gate) );
	gate->n_inputs = n_inputs;
	gate->operation = strdup(operation);
	gate->name = strdup(name);
	gate->n_gates = n_gates;
	return gate;
}

void freeGate(Gate* gate) {
	free(gate->operation);
	free(gate->name);
	free(gate);
}

//gate file format:
//#comments
//n_inputs
//n_gates
//operation string

Gate *parseGate(char *path) {
	FILE *fp = fopen(path, "r");
	if(fp == NULL) {
		perror(path);
		return NULL;
	}	
	char *line = NULL;
	size_t n = 0;
	int n_inputs = -1;
	int n_gates = -1;
	char *name = NULL;
	while( getline(&line, &n, fp) != -1) {
		*strchr(line, '\n') = '\0'; //replace newline with null terminator
		if( line[0] == '#' || strlen(line) == 0 ) {
			continue;
		}
		if(name == NULL) {
			name = strdup(line);
		} else if(n_inputs == -1) {
			int n = atol(line);
			if(n == 0) break;
			n_inputs = n;
		} else if(n_gates == -1) {
			int n = atol(line);
			if(n == 0) break;
			n_gates = n;
		} else {
			Gate *g = mkGate(name, n_inputs, line, n_gates);
			free(name);
			free(line);
			fclose(fp);
			return g;
		}
	}
	printf("Invalid format in %s\n",path);
	free(line);
	free(name);
	fclose(fp);
	return NULL;
}
			
