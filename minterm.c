#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minterm.h"

static size_t n_bits = -1;
static size_t n_chunks = -1;

static inline size_t get_chunk_idx(size_t bit_n);
static minterm_chunk chunk_do_operation(minterm_chunk a, minterm_chunk b, char op);
static void chunk_print(minterm_chunk a, size_t start_at);

int get_hamming_dist(minterm_chunk *a, minterm_chunk *b) {
	(void)a;
	(void)b;
/*	minterm_chunk *diff = a^b;
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

void minterm_print(minterm_chunk *m) {
	if(n_bits < sizeof(minterm_chunk)*8) {
		minterm_chunk shifted = *m << ((sizeof(minterm_chunk)*8)-n_bits);
		chunk_print(shifted, (sizeof(minterm_chunk)*8)-n_bits);
	}else {
		for(size_t i = 0; i < n_chunks; i++) {
			chunk_print(m[i],0);
		}
	}
	printf("\n");
}

static void chunk_print(minterm_chunk c, size_t start_at) {
	minterm_chunk mask = 1llu << (8 * sizeof(minterm_chunk) - 1);
	for(size_t i = start_at; i < 8*sizeof(minterm_chunk); i++) {
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
	n_chunks = 1 + (n_bits-1) / (sizeof(minterm_chunk)*8);
}

minterm_chunk *minterm_new() {
	minterm_chunk *m = malloc(sizeof(minterm_chunk) * n_chunks);
	memset(m, 0, sizeof(minterm_chunk) *n_chunks);
	return m;
}

static inline size_t get_chunk_idx(size_t bit_n) {
	return bit_n / (sizeof(minterm_chunk) * 8);
}

void minterm_set_bit(minterm_chunk *m, size_t bit) {
	m[get_chunk_idx(bit)] |= 1llu << (bit%(sizeof(minterm_chunk)*8));
}

void minterm_do_operation(minterm_chunk *res, minterm_chunk *a, minterm_chunk *b, char op) {
	for(size_t i = 0; i < n_chunks; i++) {
		res[i] = chunk_do_operation(a[i], b[i], op);
	}
}
		
static minterm_chunk chunk_do_operation(minterm_chunk a, minterm_chunk b, char op) {
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

void minterm_cpy(minterm_chunk *dst, minterm_chunk *src) {
	memcpy(dst, src, n_chunks * sizeof(minterm_chunk));
}
