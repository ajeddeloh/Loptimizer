#ifndef BITFIELD_H
#define BITFIELD_H

#define ULL_BITS (8*sizeof(unsigned long long))

typedef struct Bitfield {
	int n_bits;
	int n_ulls;
	unsigned long long *field;
} Bitfield;

void setBit(Bitfield *b, int bit);

void printBitfield(Bitfield *b);

void mkBitfield (Bitfield *b, int log_size);

void doOperation( char operation, Bitfield *result, Bitfield *left, Bitfield *right);

int equals(Bitfield *first, Bitfield *second);

#endif
