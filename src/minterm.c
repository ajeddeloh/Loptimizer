#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minterm.h"

static size_t n_bits = -1;
static size_t n_chunks = -1;

static inline size_t get_chunk_idx(size_t bit_n);
static uint64_t chunk_do_operation(uint64_t a, uint64_t b, char op);
static void chunk_print(uint64_t a, size_t start_at);

static uint64_t mask_lookup[] ={0x0000000000000000,
				0x0000000000000003,
				0x000000000000000F,
				0x00000000000000FF,
				0x000000000000FFFF,
				0x00000000FFFFFFFF,
				0xFFFFFFFFFFFFFFFF };

static uint64_t eq_mask;

//requires __builtin_popcountll be defined, should work on clang/gcc
int get_hamming_dist(const uint64_t *a, const uint64_t *b) {
	if(n_chunks > 1) {
		size_t count = 0;
		for(size_t i = 0; i < n_chunks; i++) {
			count += __builtin_popcountll(a[i]^b[i]);
		}
		return count;
	} else {
		return __builtin_popcountll(eq_mask & (a[0]^b[0]));
	}	
}

void minterm_print(const uint64_t *m) {
	if(n_bits < sizeof(uint64_t)*8) {
		uint64_t shifted = *m << ((sizeof(uint64_t)*8)-n_bits);
		chunk_print(shifted, (sizeof(uint64_t)*8)-n_bits);
	}else {
		for(size_t i = 0; i < n_chunks; i++) {
			chunk_print(m[n_chunks-i-1],0);
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
	if(n_chunks == 1) {
		eq_mask = mask_lookup[n_inputs];
	}
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

void minterm_cpy(uint64_t *dst, const uint64_t *src) {
	memcpy(dst, src, n_chunks * sizeof(uint64_t));
}

size_t minterm_get_size() {
	return sizeof(uint64_t) * n_chunks;
}
