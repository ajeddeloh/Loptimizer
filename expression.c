#include "expression.h"
#include "gate.h"

Expression *mkExpressoin(Gate *gate, Expression *children) {
	Expression *e = malloc(sizeof(Expression));
	e->gate = gate;
	e->children = children;
	minterm *child_vals = malloc(sizeof(minterm) * gate->n_inputs);
	//todo: move gate's eval func here, its the only place it'll get called
	return e;
}

