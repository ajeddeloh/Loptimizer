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

Expression *expr_new_from_input(int input_idx, minterm_chunk *goal);

int expr_get_est_cost(Expression *e);

#endif
