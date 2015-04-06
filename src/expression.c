#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "expression.h"
#include "gate.h"
#include "minterm.h"

Expression *expr_new_from_expr(const Gate *gate, const uint64_t *goal, Expression **children) {
	Expression *e = malloc(sizeof(Expression));
	e->gate = gate;
	e->children = children;
	
	//calculate this guys value
	char *op = gate->operation;
	int stack_top = -1;
	uint64_t *stack[strlen(op)];
	uint64_t *val;
	while( *op != '\0' ) {
		switch (*op) {
		case '!': 
			minterm_do_operation(stack[stack_top], stack[stack_top], stack[stack_top], *op);
			break;
		case '&':
		case '*':
		case '|':
		case '+':
		case '^':
			minterm_do_operation(stack[stack_top-1],stack[stack_top], stack[stack_top-1], *op);
			free(stack[stack_top]);
			stack_top--;
			break;
		default:
			val = children[*op - 'A']->value;
			stack[++stack_top] = minterm_new();
			minterm_cpy(stack[stack_top], val);
			break;
		}
		op++;
	}
	assert(stack_top == 0); //error parsing expression
	e->value = stack[stack_top];

	e->cost = 0;
    bool dup = false;
	for(size_t i = 0; i < gate->n_inputs; i++) {
        for(size_t j = 0; j < i; j++) {
            if(children[i] == children[j]) {
                dup = true;
                break;
            }
        }
        if (!dup) {
    		e->cost += children[i]->cost;
        }
	}
	e->cost++;
	e->hamm_dist = get_hamming_dist(e->value, goal);	

	return e;
}

Expression *expr_new_from_input(int input_idx, size_t n_inputs, uint64_t *goal) {
	Expression *e = malloc(sizeof(Expression));
	char gatestr[] = "A";
	gatestr[0]+=input_idx;
	e->gate = gate_new(gatestr, 0, NULL, 0);
	e->children = NULL;
	e->cost = 0;
	e->value = minterm_new();
	size_t n_bits = 1 << n_inputs;
	input_idx = n_inputs - input_idx - 1;
	for(size_t i = 0; i < n_bits; i++) {
		if( (i>>input_idx) % 2 ) {
			minterm_set_bit(e->value, i);
		}
	}
	e->hamm_dist = get_hamming_dist(e->value, goal);
	return e;
}

void expr_free(Expression *e) {
	if(e->gate->operation == NULL) { //created with expr_new_from_input
		gate_free((Gate *)e->gate);
	}
	free(e->children);
	free(e->value);
	free(e);
}

int expr_get_est_cost(Expression *e) {
	return e->cost;
}

static void expr_print_get_list(Expression *e, Expression ***to_print, int *n_to_print, int *to_print_cap) {
	for(int i = 0; i < (*n_to_print); i++) {
		if((*to_print)[i] == e) return;
	}
	(*to_print)[*n_to_print] = e;
	(*n_to_print)++;
	if( *n_to_print == *to_print_cap) { //resize if necesary
		*to_print_cap *= 2;
		*to_print = realloc((*to_print), *to_print_cap * sizeof(Expression));
	}
	for(size_t child = 0; child < e->gate->n_inputs; child++) {
		expr_print_get_list(e->children[child], to_print, n_to_print, to_print_cap);
	}
}
	
void expr_print_soln(Expression *e) {
	int to_print_cap = 10;
	int n_to_print = 0;
	Expression **to_print = malloc(to_print_cap * sizeof(Expression));
	expr_print_get_list(e, &to_print, &n_to_print, &to_print_cap);
	for(int i = 0; i < n_to_print; i++) {
		Expression *tmp = to_print[i];
		printf("HD:%d, %d <= %s(", tmp->hamm_dist, i, tmp->gate->name);
		for(size_t j = 0; j < tmp->gate->n_inputs; j++) {
			int k;
			for(k = 0; k < n_to_print; k++) {
				if(to_print[k] == tmp->children[j]) break;
			}
			printf("%d, ", k);
		}
		printf(")\n");
	}

	free(to_print);
}
