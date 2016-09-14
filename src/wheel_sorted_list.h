#ifndef _WHEEL_SORTED_LIST_H_
#define _WHEEL_SORTED_LIST_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"
#include "scheme.h"

typedef struct wheel_sorted_list {

    list_node wheel[WHEEL_SLOT_NUMS];
    uint32_t last_slot;
    uint32_t list_nodes;
}wheel_sorted_list;

extern const struct scheme_operations wheel_sorted_list_operations; 

void* wheel_sorted_list_init(void* mem, size_t mem_size);

int wheel_sorted_list_start(void* scheme, list_node *node);

int wheel_sorted_list_stop(void* scheme, list_node *node);

int wheel_sorted_list_get(void* scheme, uint64_t last_timestamp, 
                          uint64_t now_timestamp, list_node* expire_head);




#endif
