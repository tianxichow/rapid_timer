#ifndef _RB_TREE_H_
#define _RB_TREE_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"
#include "rb_node.h"
#include "scheme.h"

typedef struct rb_tree {

    rb_root root;
    uint32_t rb_nodes;
}rb_tree;

extern const struct scheme_operations rb_tree_operations;

void *rb_tree_init(void *mem, size_t mem_size);

int rb_tree_start(void *scheme, timer_node *node);

int rb_tree_stop(void *scheme, timer_node *node);

int rb_tree_get(void *scheme, uint64_t last_timestamp, 
                uint64_t now_timestamp, list_node *expire_head);

#endif /* _RB_TREE_H_ */