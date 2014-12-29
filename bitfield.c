#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bitfield.h"

void mkBitfield(Bitfield *b, int log_size) {
	b->n_bits = (1 << log_size);
	b->n_ulls = 1 + (b->n_bits-1) / ULL_BITS;
	b->field = malloc(b->n_ulls * sizeof(unsigned long long));
	memset(b->field, 0, b->n_ulls * sizeof(unsigned long long));
}

void printBitfield(Bitfield *b) {
	printf("%d --- \n", b->n_ulls);
	for (int i = b->n_bits-1; i >= 0; i--) {
		int idx = i / ULL_BITS;
		if((b->field[idx] & (1 << (i%ULL_BITS))) == 0) {
			putc('0',stdout);
		} else {
			putc('1',stdout);
		}
	}
	putc('\n',stdout);
}

void setBit(Bitfield *b, int bit) {
	int idx = bit / ULL_BITS;
	b->field[idx] |= 1 << (bit % ULL_BITS);
}

void doOperation(char operation, Bitfield *result, Bitfield *left, Bitfield *right) {
	switch(operation) {
	case '!': 
		for(int i = 0; i < left->n_ulls; i++) {
			result->field[i] = ~left->field[i];
		}
		break;
	case '*':
	case '&':
		for(int i = 0; i < left->n_ulls; i++) {
			result->field[i] = left->field[i] & right->field[i];
		}
		break;
	case '|':
	case '+':
		for(int i = 0; i < left->n_ulls; i++) {
			result->field[i] = left->field[i] & right->field[i];
		}
		break;
	case '^':
		for(int i = 0; i < left->n_ulls; i++) {
			result->field[i] = left->field[i] ^ right->field[i];
		}
		break;
	default:
		printf("Unsupported operator %c\n", operation);
		exit(EXIT_FAILURE);
	}
}	

//0 if equal
int equals(Bitfield *first, Bitfield *second) {
	unsigned long long res = 0;
	if(first->n_ulls == 1) {
		unsigned long long mask = (1 << (first->n_bits+1))-1;
		return (first->field[0] - second->field[0]) & mask;
	} else {
		for(int i = 0; i < first->n_ulls; i++) {
			res |= (first->field[i] - second->field[i]);
		}
	}
	return res;
}
