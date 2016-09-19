
#ifndef _SORTED_LIST_H_
#define _SORTED_LIST_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"
#include "scheme.h"

typedef struct sorted_list {

    list_node head;
    uint32_t list_nodes;
}sorted_list;

extern const struct scheme_operations sorted_list_operations;

void *sorted_list_init(void *mem, size_t mem_size);

int sorted_list_start(void *scheme, timer_node *node);

int sorted_list_stop(void *scheme, timer_node *node);

int sorted_list_get(void *scheme, uint64_t last_timestamp, 
                    uint64_t now_timestamp, list_node *expire_head);

#endif /* _SORTED_LIST_H_ */
