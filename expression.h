#ifndef EXPRESSION_H
#define EXPRESSION_H

typedef struct Expression {
	Minterm value;
	Gate *operation;
	Expression *children;
} Expression;

Expression *mkExpression(Gate *gate, Expression *children);

#endif
