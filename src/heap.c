#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "heap.h"

#define get_parent(self) ((self-1)/2)
#define get_lchild(self) (self*2+1)
#define get_rchild(self) (self*2+2)

Heap *heap_new(size_t init_size, int (*get_value)(void *a)) {
	Heap *heap = malloc(sizeof(Heap));
	assert(heap != NULL);
	heap->data = malloc (sizeof(void *) * init_size);
	assert(heap->data != NULL);
	heap->size = init_size;
	heap->n_elems = 0;
	heap->get_value = get_value;
	return heap;
}

void heap_insert(Heap *heap, void *data) {
	if(heap->size == heap->n_elems) { //full
		heap->size *= 2;
		heap->data = realloc(heap->data, sizeof(void *) * heap->size);
		assert(heap->data != NULL); //malloc could very likely fail
	}
	heap->data[heap->n_elems++] = data;
	size_t idx = heap->n_elems-1;
	void **array = heap->data;
	
	while(idx != 0 && (*heap->get_value)(array[idx]) < (*heap->get_value)(array[get_parent(idx)])) {
		void *tmp = array[idx];
		array[idx] = array[get_parent(idx)];
		array[get_parent(idx)] = tmp;
		idx = get_parent(idx);
	}
}

void *heap_remove_min(Heap *heap) {
	void *value = heap->data[0];
	heap->n_elems--;
	heap->data[0] = heap->data[heap->n_elems];
	
	size_t idx = 0;
	void **array = heap->data;
	while(idx <= heap->n_elems) {
		//exit it we're a leaf already
		if(get_lchild(idx) >= heap->n_elems) break;
		size_t swap;
		if(get_rchild(idx) >= heap->n_elems) {
			//check if lchild should be swapped.
			if( (*heap->get_value)(array[idx]) > (*heap->get_value)(array[get_rchild(idx)])) {
				swap = get_rchild(idx);
			} else {
				break; //done;
			}
		} else {
			swap = (*heap->get_value)(array[get_rchild(idx)]) < (*heap->get_value)(array[get_lchild(idx)]) ? 
				get_rchild(idx) : get_lchild(idx);
			if( (*heap->get_value)(array[idx]) > (*heap->get_value)(array[swap]))  break;
		}
		void *tmp = array[idx];
		array[idx] = array[swap];
		array[swap] = tmp;
		idx = swap;
	}
	return value;
}

void heap_free(Heap *heap) {
	free(heap->data);
	free(heap);
}

