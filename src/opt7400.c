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
#include "heap.h"

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

	
	uint8_t *key = octo_keygen();
	size_t all_expr_n_elems = 0;
	size_t all_expr_size = n_inputs*2;
	Heap *openset_q = heap_new(all_expr_size, (int (*)(void*)) expr_get_est_cost); 
	octo_dict_loa_t *all_expr_ht = octo_loa_init(minterm_get_size(), sizeof(Expression*), all_expr_size, key);
	
	Expression **closed_set = calloc(10, sizeof(Expression*));
	size_t closed_set_size = 10;
	size_t closed_set_n_elems = 0;

	for(int i = 0; i < n_inputs; i++) {
		Expression *e = expr_new_from_input(i, n_inputs, goal);
		heap_insert(openset_q, e);
		octo_loa_insert(e->value, &e, all_expr_ht);
		all_expr_n_elems++;
	}
	printf("Added initial values\n");

	while(all_expr_n_elems != 0) {
		Expression *min = heap_remove_min(openset_q);
		//octo_loa_delete(min, all_expr_ht);
		//all_expr_n_elems--;
		
		printf("%d ",min->cost);
		minterm_print(min->value);
		
		//exit cond
		if(min->hamm_dist == 0) {
			printf("Found it\n");
			break;
		}
		//add to closed set
		closed_set[closed_set_n_elems++] = min;
		if(closed_set_n_elems == closed_set_size) { //resize
			closed_set_size *= 2;
			closed_set = realloc(closed_set, sizeof(Expression *) * closed_set_size);
		}
		for(int i = 0; i < n_gates; i++) {
			int gate_inputs = gates[i]->n_inputs;
			int *idxs = calloc(gates[i]->n_inputs, sizeof(int));
			idxs[0] = closed_set_n_elems-1;
			//generate all permutations
			while(idxs[gate_inputs-1] != (int)closed_set_n_elems) {
				Expression **children = malloc(sizeof(Expression*) * gate_inputs);
				//for(int j = 0; j < gate_inputs; j++) printf("%d\n",idxs[j]);
				//generate this batch's children
				for(int j = 0; j < gate_inputs; j++) {
					children[j] = closed_set[idxs[j]];
				}
				//figure out what to add next
				idxs[1] ++;
				for(int j = 1; j < gate_inputs-1 && idxs[j] == (int)closed_set_n_elems; j++) {
					idxs[j] = 0;
					idxs[j+1]++;
				}
				
				//create new expression to add
				Expression *to_add = expr_new_from_expr(gates[i], goal, children);
				
				//check if a better version exists or if we're better than something existing
				Expression **openset_test = octo_loa_fetch(to_add->value, all_expr_ht);
				if(openset_test == NULL) exit(1);
				Expression **NOT_FOUND = (Expression**)all_expr_ht;
				
				if(openset_test != NOT_FOUND && (*openset_test)->cost <= to_add->cost) {	
					expr_free(to_add);
					continue;
				}
				if(openset_test != NOT_FOUND)  {
					printf("oldc: %d, newc: %d\n", (*openset_test)->cost, to_add->cost);
					minterm_print((*openset_test)->value);
					minterm_print(to_add->value);
					free((*openset_test)->children);
					(*openset_test)->children = children;
					to_add->children = NULL; //so they can be freed
					(*openset_test)->cost = to_add->cost;
					expr_free(to_add);
					continue;
				}
				
				heap_insert(openset_q, to_add);
				octo_loa_insert(to_add->value, &to_add, all_expr_ht);
				all_expr_n_elems++;
				if(all_expr_n_elems > all_expr_size * 3 / 4) {
					all_expr_size *= 2;
					all_expr_ht = octo_loa_rehash(all_expr_ht, minterm_get_size(), sizeof(Expression*),
							all_expr_size, key);
				}

					
			}
			free(idxs);	
		} 

	}
	
	

	for(int i = 0; i < n_gates; i++) {
		gate_free(gates[i]);
	}
	free(gates);
	free(goal);
	free(key);
	heap_free(openset_q);
	octo_loa_free(all_expr_ht);
	free(closed_set);
	return EXIT_SUCCESS;
}
