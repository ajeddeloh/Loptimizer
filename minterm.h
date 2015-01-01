#ifndef MINTERM_H
#define MINTERM_H

typedef unsigned long long minterm_chunk;
//typedef minterm_minterm_chunk* Minterm;

//todo make this faster
int get_hamming_dist(minterm_chunk *a, minterm_chunk *b);

void minterm_print(minterm_chunk *m);

void minterm_init(size_t n_inputs);

minterm_chunk *minterm_new();

void minterm_set_bit(minterm_chunk *m, size_t bit);

void minterm_do_operation(minterm_chunk *res, minterm_chunk *a, minterm_chunk *b, char op);

void minterm_cpy(minterm_chunk *dst, minterm_chunk *src); 

void minterm_free(minterm_chunk *m);

#endif 
