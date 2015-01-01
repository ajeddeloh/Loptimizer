#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "gate.h"

typedef struct Expression {
	minterm_chunk *value;
	Gate *gate;
	struct Expression **children;
	int cost; //in gates for now, (todo: make in terms of ICs)
	int hamm_dist;
} Expression;

Expression *expr_new(Gate *gate, Expression **children, minterm_chunk *goal);

Expression *expr_new_from_input(int input_idx, size_t n_inputs, minterm_chunk *goal);

void expr_free(Expression *e);

int expr_get_est_cost(Expression *e);

#endif
