#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "expression.h"
#include "gate.h"
#include "minterm.h"

Expression *expr_new(Gate *gate, Expression **children, minterm_chunk *goal) {
	Expression *e = malloc(sizeof(Expression));
	e->gate = gate;
	e->children = children;
	
	//calculate this guys value
	char *op = gate->operation;
	int stack_top = -1;
	minterm_chunk *stack[strlen(op)];
	while( *op != '\0' ) {
		switch (*op) {
		case '!': 
			minterm_do_operation(stack[stack_top], stack[stack_top], stack[stack_top], *op);
//			stack[stack_top] = ~stack[stack_top];
			break;
		case '&':
		case '*':
		case '|':
		case '+':
		case '^':
			minterm_do_operation(stack[stack_top-1],stack[stack_top], stack[stack_top-1], *op);
//			stack[stack_top-1] = stack[stack_top] ^ stack[stack_top-1];
			minterm_free(stack[stack_top]);
			stack_top--;
			break;
		default:
			assert(*op - 'A' < gate->n_inputs); 
			minterm_chunk *val = children[*op - 'A']->value;
			stack[++stack_top] = minterm_new();
			minterm_cpy(stack[stack_top], val);
			break;
		}
		op++;
	}
	assert(stack_top == 0); //error parsing expression
	e->value = stack[stack_top];

	e->cost = 0;
	for(int i = 0; i < gate->n_inputs; i++) {
		e->cost += children[i]->cost;
	}
	e->cost++;
	
	e->hamm_dist = get_hamming_dist(e->value, goal);	

	return e;
}

Expression *expr_new_from_input(int input_idx, size_t n_inputs, minterm_chunk *goal) {
	Expression *e = malloc(sizeof(Expression));
	e->gate = NULL;
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
	minterm_free(e->value);
	free(e);
}

int expr_get_est_cost(Expression *e) {
	return e->cost*10 + e->hamm_dist;
}
