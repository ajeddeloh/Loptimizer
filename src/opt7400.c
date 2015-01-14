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
#include <stdint.h>
#include <stdbool.h>

#include "octo/loa.h"
#include "octo/keygen.h"

#include "expression.h"
#include "minterm.h"
#include "gate.h"
#include "graph_store.h"

int main(int argc, char *argv[]) {
	if(argc < 4) {
		printf("ERROR: not enough args\n");
		exit( EXIT_FAILURE );
	}
	int n_inputs = atoi(argv[1]);
	if(n_inputs == 0) {
		printf("number of inputs invalid\n");
		exit( EXIT_FAILURE );
	}
	
	minterm_init(n_inputs);
	uint64_t *goal = minterm_new();

	char *tmp = strtok(argv[2], ",");
	while(tmp != NULL) {
		int pos = atoi(tmp);
		minterm_set_bit(goal, pos);
		tmp = strtok(NULL, ",");
	}
	minterm_print(goal);

	glob_t globbuf;
	for(int i = 3; i < argc; i++) {
		int app = (i==3) ? 0 : GLOB_APPEND;
		glob(argv[i], GLOB_NOSORT | app, NULL, &globbuf);
	}
	Gate **gates = calloc(globbuf.gl_pathc, sizeof(Gate*));
	int n_gates = 0;
	for(int i = 0; (unsigned int)i < globbuf.gl_pathc; i++) {
		Gate *g = gate_parse(globbuf.gl_pathv[i]);
		if(g != NULL) {
			printf("%d - %s %zu: %s\n", n_gates, g->name, g->n_inputs, g->operation);
			gates[n_gates++] = g;
		}
	}
	globfree(&globbuf);
	gates = realloc(gates, sizeof(Gate *) * n_gates);

	GraphStore *graph = graph_store_new(n_inputs*2);

	for(int i = 0; i < n_inputs; i++) {
		Expression *e = expr_new_from_input(i, n_inputs, goal);
		graph_store_insert_open(graph, e);
	}
	printf("Added initial values\n");

	while(graph->heap_n_elems != 0) {
		Expression *min = graph_store_remove_min(graph);
	
		printf("%d ",min->cost);
		minterm_print(min->value);
		
		//exit cond
		if(min->hamm_dist == 0) {
			printf("Found it\n");
			expr_print_soln(min, 0);
			break;
		}
		//add to closed set
		graph_store_insert_closed(graph, min);
		for(int i = 0; i < n_gates; i++) {
			Gate *gate = gates[i];
			if(graph->closed_set_n_elems < gate->n_inputs) continue;
			
			//indices of elements of the closed set to generate new open set elems
			size_t *idxs = NULL;
			gate_generate_indices(gate, &idxs, graph->closed_set_n_elems);

			while (idxs != NULL) {
				Expression **children = malloc(sizeof(Expression*) * gate->n_inputs);
				//generate this batch's children
				for(size_t j = 0; j < gate->n_inputs; j++) {
					children[j] = graph->closed_set[idxs[j]];
				}
			
				//figure out what to add next
				gate_generate_indices(gate, &idxs, graph->closed_set_n_elems);
				
				//create new expression to add
				Expression *to_add = expr_new_from_expr(gates[i], goal, children);
				
				//check if a better version exists or if we're better than something existing
				Expression *openset_test = graph_store_get_expr(graph, to_add->value);

				if(openset_test != NULL && openset_test->cost <= to_add->cost) {	
					expr_free(to_add);
					continue;
				}
				if(openset_test != NULL)  {
					free(openset_test->children);
					openset_test->children = children;
					to_add->children = NULL; //so they can be freed
					openset_test->cost = to_add->cost;
					expr_free(to_add);
					graph_store_update_priority(graph, openset_test);
					continue;
				}
				
				graph_store_insert_open(graph, to_add);
			}
		} 

	}

	for(int i = 0; i < n_gates; i++) {
		gate_free(gates[i]);
	}
	free(gates);
	free(goal);
	graph_store_free(graph);
	return EXIT_SUCCESS;
}
