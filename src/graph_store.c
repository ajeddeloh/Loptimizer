#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "octo/loa.h"
#include "octo/keygen.h"

#include "minterm.h"
#include "expression.h"
#include "graph_store.h"

#define REHASH_LOAD 0.75

typedef struct GSValue {
	Expression *expr;
	size_t heap_idx;
} GSValue;

static void heap_swap(GraphStore *gs, size_t idx1, size_t idx2);
static void heap_sift_up(GraphStore *gs, size_t elem_idx);
static void heap_sift_down(GraphStore *gs, size_t elem_idx);

static void heap_swap(GraphStore *gs, size_t idx1, size_t idx2) {
	Expression **heap_array = gs->heap;
	uint64_t *key1 = heap_array[idx1]->value;
	uint64_t *key2 = heap_array[idx2]->value;
	((GSValue*)(octo_loa_fetch(key1, gs->ht)))->heap_idx = idx2;
	((GSValue*)(octo_loa_fetch(key2, gs->ht)))->heap_idx = idx1;
	Expression *tmp = heap_array[idx1];
	heap_array[idx1] = heap_array[idx2];
	heap_array[idx2] = tmp;
}

static void heap_sift_up(GraphStore *gs, size_t heap_idx) {
	Expression **arr = gs->heap;
	while(heap_idx > 0) {
		size_t parent = (heap_idx - 1) / 2;
		if( arr[heap_idx]->cost < arr[parent]->cost ) {
			heap_swap(gs, heap_idx, parent);
			heap_idx = parent;
		} else {
			break;
		}
	}
}

static void heap_sift_down(GraphStore *gs, size_t heap_idx) {
	Expression **arr = gs->heap;
	while(heap_idx < gs->heap_n_elems) {
		size_t lchild = heap_idx * 2 + 1;
		if( lchild >= gs->heap_n_elems) break;

		size_t rchild = heap_idx * 2 + 2; 
		if(rchild >= gs->heap_n_elems) {
			if( arr[heap_idx] > arr[lchild] ) {
				heap_swap(gs, heap_idx, lchild);
				heap_idx = lchild;
			} else {
				break;
			}
		} else {
			size_t swp = (arr[lchild]->cost < arr[rchild]->cost) ? lchild : rchild;
			if( arr[heap_idx] > arr[swp]) {
				heap_swap(gs, heap_idx, swp);
				heap_idx = swp;
			} else {
				break;
			}
		}
	}
}
	

GraphStore *graph_store_new(size_t init_size) {
	GraphStore *gs = malloc(sizeof(GraphStore));
	gs->heap = malloc(sizeof(Expression*) * init_size);
	gs->heap_cap = init_size;
	gs->heap_n_elems = 0;
	
	gs->ht_key = octo_keygen();
	gs->ht = octo_loa_init(minterm_get_size(), sizeof(GSValue), init_size, gs->ht_key);
	gs->ht_cap = init_size;
	gs->ht_n_elems = 0;
	
	gs->closed_set = malloc(sizeof(Expression*) * init_size);
	gs->closed_set_cap = init_size;
	gs->closed_set_n_elems = 0;
	return gs;
}

Expression *graph_store_get_expr(GraphStore *gs, uint64_t *key) {
	void *ret = octo_loa_fetch(key, gs->ht);
	if(ret == NULL) {
		exit(EXIT_FAILURE); 
	}
	if(ret == gs->ht) {
		return NULL;
	}
	return ((GSValue*)(ret))->expr;
}

Expression *graph_store_remove_min(GraphStore *gs) {
	Expression *min = gs->heap[0];
	gs->heap[0] = gs->heap[gs->heap_n_elems - 1];
	gs->heap_n_elems--;
	heap_sift_down(gs, 0);
	return min;
}

void graph_store_insert_open(GraphStore *gs, Expression *item) {
	gs->heap[gs->heap_n_elems] = item;
	GSValue to_insert = {.expr=item, .heap_idx=gs->heap_n_elems};
	octo_loa_insert(item->value, &to_insert, gs->ht);
	gs->ht_n_elems++;
	heap_sift_up(gs, gs->heap_n_elems);
	gs->heap_n_elems++;

	if( gs->heap_n_elems+1 >= gs->heap_cap) {
		gs->heap_cap *= 2;
		gs->heap = realloc(gs->heap, gs->heap_cap * sizeof(Expression*));
		assert(gs->heap != NULL);
	}

	if( gs->ht_n_elems > gs->ht_cap * REHASH_LOAD) {
		gs->ht_cap *= 2;
		gs->ht = octo_loa_rehash(gs->ht, minterm_get_size(), sizeof(GSValue), gs->ht_cap, gs->ht_key);
		assert(gs->ht != NULL);
	}
}

void graph_store_insert_closed(GraphStore *gs, Expression *item) {
	gs->closed_set[gs->closed_set_n_elems++] = item;
	if(gs->closed_set_n_elems == gs->closed_set_cap) {
		gs->closed_set_cap *= 2;
		gs->closed_set = realloc(gs->closed_set, gs->closed_set_cap * sizeof(Expression*));
		assert(gs->closed_set != NULL);
	}
}

void graph_store_update_priority(GraphStore *gs, Expression *item) {
	size_t idx = ((GSValue*)(octo_loa_fetch(item->value, gs->ht)))->heap_idx;
	heap_sift_up(gs, idx);
} 

void graph_store_free(GraphStore *gs) {
	free(gs->heap);
	free(gs->closed_set);
	free(gs->ht_key);
	octo_loa_free(gs->ht);
	free(gs);
}	
	
