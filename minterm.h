#ifndef MINTERM_H
#define MINTERM_H

#include <stdint.h>

/*
 * Minterms are stored as an array of uint64_t's. The bit at position n
 * indictates if n is a minterm. Its basically a truth table. Since all
 * minterms are the same size, the size of the minterms is stored statically
 * and need to be set with minterm_init before calling any of the other 
 * minterm functions
 */

//todo make this faster
int get_hamming_dist(const uint64_t *a, const uint64_t *b);

void minterm_print(const uint64_t *m);

void minterm_init(size_t n_inputs);

uint64_t *minterm_new();

void minterm_set_bit(uint64_t *m, size_t bit);

void minterm_do_operation(uint64_t *res, uint64_t *a, uint64_t *b, char op);

void minterm_cpy(uint64_t *dst, const uint64_t *src); 

#endif 
