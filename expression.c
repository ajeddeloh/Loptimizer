#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "expression.h"
#include "gate.h"
#include "minterm.h"

Expression *expr_new(Gate *gate, Expression **children, Minterm goal) {
	Expression *e = malloc(sizeof(Expression));
	e->gate = gate;
	e->children = children;
	
	//calculate this guys value
	char *op = gate->operation;
	int stack_top = -1;
	Minterm *stack = malloc(sizeof(Minterm)*strlen(op));
	while( *op != '\0' ) {
		switch (*op) {
		case '!': 
			stack[stack_top] = ~stack[stack_top];
			break;
		case '&':
		case '*':
			stack[stack_top-1] = stack[stack_top] & stack[stack_top-1];
			stack_top--;
			break;
		case '|':
		case '+':
			stack[stack_top-1] = stack[stack_top] | stack[stack_top-1];
			stack_top--;
			break;
		case '^':
			stack[stack_top-1] = stack[stack_top] ^ stack[stack_top-1];
			stack_top--;
			break;
		default:
			assert(*op - 'A' < gate->n_inputs); 
			int val = children[*op - 'A']->value;
			stack[++stack_top] = val;
			break;
		}
		op++;
	}
	assert(stack_top == 0); //error parsing expression
	e->value = stack[stack_top];
	free(stack);

	e->cost = 0;
	for(int i = 0; i < gate->n_inputs; i++) {
		e->cost += children[i]->cost;
	}
	e->cost++;
	
	e->hamm_dist = get_hamming_dist(e->value, goal);	

	return e;
}

int expr_get_est_cost(Expression *e) {
	return e->cost*10 + e->hamm_dist;
}
