//usage: opt7400 [options]  <number of inputs> minterms gatefiles gatefiles gatefiles
//minterms should be expressed as a list of integers, comma seperated (no spaces):
//to run with gate files: and2.gate, or2.gate, and inv.gate for f(a,c,b,d)=m(1,3,5,11)
//you would run
// opt7400 4 1,3,5,11,12 and2.gate or2.gate, inv.gate
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include "gate.h"

int main(int argc, char *argv[]) {
	if(argc < 4) {
		printf("ERROR: not enough args\n");
		exit( EXIT_FAILURE );
	}
	int num_inputs = atoi(argv[1]);
	if(num_inputs == 0) {
		printf("number of inputs invalid\n");
		exit( EXIT_FAILURE );
	}

	minterm goal = 0;

	char *tmp = strtok(argv[2], ",");
	while(tmp != NULL) {
		int pos = atoi(tmp);
		goal |= 1 << pos;
		tmp = strtok(NULL, ",");
	}
	printf("%llu\n",goal);

	glob_t globbuf;
	for(int i = 3; i < argc; i++) {
		int app = (i==3) ? 0 : GLOB_APPEND;
		glob(argv[i], GLOB_NOSORT | app, NULL, &globbuf);
	}
	Gate **gates = malloc(sizeof(Gate *) * globbuf.gl_pathc);
	int num_gates = 0;
	for(int i = 0; (unsigned int)i < globbuf.gl_pathc; i++) {
		Gate *g = parseGate(globbuf.gl_pathv[i]);
		if(g != NULL) {
			printf("%d - %s %d: %s\n", num_gates, g->name, g->n_inputs, g->operation);
			gates[num_gates++] = g;
		}
	}
	globfree(&globbuf);


	gates = realloc(gates, sizeof(Gate *) * num_gates);
	minterm args[2] = {goal, goal};
	minterm res = eval(gates[2], args);
	printf("%llu\n",res);


	//cleanup
	for(int i = 0; i < num_gates; i++) {
		freeGate(gates[i]);
	}
	free(gates);
	return EXIT_SUCCESS;
}
