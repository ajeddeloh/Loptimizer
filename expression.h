#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdint.h>

#include "gate.h"

typedef struct Expression {
	uint64_t *value;
	const Gate *gate;
	struct Expression **children;
	int cost; //in gates for now, (todo: make in terms of ICs)
	int hamm_dist;
} Expression;

//children array is not copied. 
Expression *expr_new_from_expr(const Gate *gate, const uint64_t *goal, Expression **children);

Expression *expr_new_from_input(int input_idx, size_t n_inputs, uint64_t *goal);

void expr_free(Expression *e);

int expr_get_est_cost(Expression *e);

#endif
