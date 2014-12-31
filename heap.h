#ifndef HEAP_H
#define HEAP_H

//simple binary heap

typedef struct Heap {
	void **data;
	size_t size;
	size_t n_elems;
	int (*cmp)(void *a, void*b);
} Heap;

Heap *heap_new(size_t init_size, int (*cmp)(void *a, void *b));

void heap_insert(Heap *heap, void *data);

void *heap_remove_min(Heap *heap);

void heap_free(Heap *heap);
#endif

