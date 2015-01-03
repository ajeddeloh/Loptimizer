#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minterm.h"

static size_t n_bits = -1;
static size_t n_chunks = -1;

static inline size_t get_chunk_idx(size_t bit_n);
static uint64_t chunk_do_operation(uint64_t a, uint64_t b, char op);
static void chunk_print(uint64_t a, size_t start_at);

int get_hamming_dist(uint64_t *a, uint64_t *b) {
	(void)a;
	(void)b;
/*	uint64_t *diff = a^b;
#if __GNUC__ >= 4 //todo, add clang to this or something, iirc clang can do it too
	return __builtin_popcountll(diff);
#else
#error
	int cnt = 0;
	for(size_t i = 0; i < 8*sizeof(Minterm); i++) {
		cnt += 1ull & diff;
		diff = diff >> 1;
	}
	return cnt;
#endif*/
	return 0;
}

void minterm_print(uint64_t *m) {
	if(n_bits < sizeof(uint64_t)*8) {
		uint64_t shifted = *m << ((sizeof(uint64_t)*8)-n_bits);
		chunk_print(shifted, (sizeof(uint64_t)*8)-n_bits);
	}else {
		for(size_t i = 0; i < n_chunks; i++) {
			chunk_print(m[i],0);
		}
	}
	printf("\n");
}

static void chunk_print(uint64_t c, size_t start_at) {
	uint64_t mask = 1llu << (8 * sizeof(uint64_t) - 1);
	for(size_t i = start_at; i < 8*sizeof(uint64_t); i++) {
		if( mask & c ) {
			putc('1', stdout);
		} else {
			putc('0', stdout);
		}
		c = c << 1;
	}
}

void minterm_init(size_t n_inputs) {
	n_bits = 1 << n_inputs;
	n_chunks = 1 + (n_bits-1) / (sizeof(uint64_t)*8);
}

uint64_t *minterm_new() {
	uint64_t *m = malloc(sizeof(uint64_t) * n_chunks);
	memset(m, 0, sizeof(uint64_t) *n_chunks);
	return m;
}

static inline size_t get_chunk_idx(size_t bit_n) {
	return bit_n / (sizeof(uint64_t) * 8);
}

void minterm_set_bit(uint64_t *m, size_t bit) {
	m[get_chunk_idx(bit)] |= 1llu << (bit%(sizeof(uint64_t)*8));
}

void minterm_do_operation(uint64_t *res, uint64_t *a, uint64_t *b, char op) {
	for(size_t i = 0; i < n_chunks; i++) {
		res[i] = chunk_do_operation(a[i], b[i], op);
	}
}
		
static uint64_t chunk_do_operation(uint64_t a, uint64_t b, char op) {
	switch (op) {
	case '!':
		return ~a;
	case '+':
	case '|':
		return a | b;
	case '*':
	case '&':
		return a & b;
	case '^':
		return a ^ b;
	default:
		fprintf(stderr, "Error: unexpected operator: %c\n", op);
		exit(EXIT_FAILURE);
	}
}

void minterm_cpy(uint64_t *dst, uint64_t *src) {
	memcpy(dst, src, n_chunks * sizeof(uint64_t));
}

