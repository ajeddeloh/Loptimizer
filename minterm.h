#ifndef MINTERM_H
#define MINTERM_H

#include <stdint.h>

typedef uint64_t uint64_t;
//typedef minterm_uint64_t* Minterm;

//todo make this faster
int get_hamming_dist(uint64_t *a, uint64_t *b);

void minterm_print(uint64_t *m);

void minterm_init(size_t n_inputs);

uint64_t *minterm_new();

void minterm_set_bit(uint64_t *m, size_t bit);

void minterm_do_operation(uint64_t *res, uint64_t *a, uint64_t *b, char op);

void minterm_cpy(uint64_t *dst, uint64_t *src); 

#endif 
