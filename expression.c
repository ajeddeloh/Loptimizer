#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "expression.h"
#include "gate.h"
#include "minterm.h"

Expression *mkExpression(Gate *gate, Expression **children, Minterm goal) {
	Expression *e = malloc(sizeof(Expression));
	e->gate = gate;
	e->children = children;
	
	//calculate this guys value
	char *op = gate->operation;
	int stackTop = -1;
	Minterm *stack = malloc(sizeof(Minterm)*strlen(op));
	while( *op != '\0' ) {
		switch (*op) {
		case '!': 
			stack[stackTop] = ~stack[stackTop];
			break;
		case '&':
		case '*':
			stack[stackTop-1] = stack[stackTop] & stack[stackTop-1];
			stackTop--;
			break;
		case '|':
		case '+':
			stack[stackTop-1] = stack[stackTop] | stack[stackTop-1];
			stackTop--;
			break;
		case '^':
			stack[stackTop-1] = stack[stackTop] ^ stack[stackTop-1];
			stackTop--;
			break;
		default:
			assert(*op - 'A' < gate->n_inputs); 
			int val = children[*op - 'A']->value;
			stack[++stackTop] = val;
			break;
		}
		op++;
	}
	assert(stackTop == 0); //error parsing expression
	e->value = stack[stackTop];
	free(stack);

	e->cost = 0;
	for(int i = 0; i < gate->n_inputs; i++) {
		e->cost += children[i]->cost;
	}
	e->cost++;
	
	e->hamm_dist = get_hamming_dist(e->value, goal);	

	return e;
}

