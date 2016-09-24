
#ifndef _UNSORTED_LIST_H_
#define _UNSORTED_LIST_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"
#include "scheme.h"

typedef struct unsorted_list {

    list_node head;
    uint32_t list_nodes;
}unsorted_list;

extern const struct scheme_operations unsorted_list_operations;

void *unsorted_list_init(void *mem, size_t mem_size, uint64_t now_timestamp);

int unsorted_list_start(void *scheme, timer_node *node);

int unsorted_list_stop(void *scheme, timer_node *node);

int unsorted_list_get(void *scheme, uint64_t last_timestamp, 
                      uint64_t  now_timestamp, list_node *expire_head);


#endif /* _UNSORTED_LIST_H_ */
