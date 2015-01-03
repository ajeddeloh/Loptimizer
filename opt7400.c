//usage: opt7400 [options]  <number of inputs> Minterms gatefiles gatefiles gatefiles
//Minterms should be expressed as a list of integers, comma seperated (no spaces):
//to run with gate files: and2.gate, or2.gate, and inv.gate for f(a,c,b,d)=m(1,3,5,11)
//you would run
// opt7400 4 1,3,5,11,12 and2.gate or2.gate, inv.gate
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include "expression.h"
#include "minterm.h"
#include "gate.h"
#include "heap.h"

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
	
	minterm_init(num_inputs);
	uint64_t *goal = minterm_new();

	char *tmp = strtok(argv[2], ",");
	while(tmp != NULL) {
		int pos = atoi(tmp);
		//goal |= 1 << pos;
		minterm_set_bit(goal, pos);
		tmp = strtok(NULL, ",");
	}
	minterm_print(goal);

	glob_t globbuf;
	for(int i = 3; i < argc; i++) {
		int app = (i==3) ? 0 : GLOB_APPEND;
		glob(argv[i], GLOB_NOSORT | app, NULL, &globbuf);
	}
	Gate **gates = malloc(sizeof(Gate *) * globbuf.gl_pathc);
	int num_gates = 0;
	for(int i = 0; (unsigned int)i < globbuf.gl_pathc; i++) {
		Gate *g = gate_parse(globbuf.gl_pathv[i]);
		if(g != NULL) {
			printf("%d - %s %zu: %s\n", num_gates, g->name, g->n_inputs, g->operation);
			gates[num_gates++] = g;
		}
	}
	globfree(&globbuf);
	gates = realloc(gates, sizeof(Gate *) * num_gates);
	
	Heap *queue = heap_new(10, (int (*)(void*)) expr_get_est_cost); 
	(void)queue; //stfu compiler
	
	Expression *a = expr_new_from_input(0,num_inputs,goal);
	minterm_print(a->value);
	Expression *b = expr_new_from_input(1,num_inputs,goal);
	minterm_print(b->value);
	Expression *c = expr_new_from_input(2,num_inputs,goal);
	minterm_print(c->value);
	Expression *d = expr_new_from_input(3,num_inputs,goal);
	minterm_print(d->value);
	Expression *children[2] = {a,b};
	Expression *aANDb = expr_new_from_expr(gates[2],children, goal);
	minterm_print(aANDb->value); 
	//cleanup
	expr_free(a);
	expr_free(b);
	expr_free(c);
	expr_free(d);
	expr_free(aANDb);
	for(int i = 0; i < num_gates; i++) {
		gate_free(gates[i]);
	}
	free(gates);
	free(goal);
	heap_free(queue);
	return EXIT_SUCCESS;
}
