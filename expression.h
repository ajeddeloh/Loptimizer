#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "gate.h"

typedef struct Expression {
	Minterm value;
	Gate *gate;
	struct Expression **children;
} Expression;

Expression *mkExpression(Gate *gate, Expression **children);

#endif
