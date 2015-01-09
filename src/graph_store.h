#include <stdlib.h>
#include <stdint.h>

#include "octo/loa.h"

typedef struct GraphStore {
	Expression **heap;
	size_t heap_n_elems;
	size_t heap_cap;
	uint8_t *ht_key;
	octo_dict_loa_t *ht;
	size_t ht_n_elems;
	size_t ht_cap;
	Expression **closed_set;
	size_t closed_set_n_elems;
	size_t closed_set_cap;
} GraphStore;

GraphStore *graph_store_new(size_t init_size);

Expression *graph_store_get_expr(GraphStore *gs, uint64_t *key);

Expression *graph_store_remove_min(GraphStore *gs);

void graph_store_insert_open(GraphStore *gs, Expression *item);

void graph_store_insert_closed(GraphStore *gs, Expression *item);

void graph_store_update_priority(GraphStore *gs, Expression *item);


