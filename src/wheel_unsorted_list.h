#ifndef _WHEEL_UNSORTED_LIST_H_
#define _WHEEL_UNSORTED_LIST_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"

#define WHEEL_SLOT_NUMS    1 << 8

typedef struct wheel_unsorted_list {

    list_node wheel[WHEEL_SLOT_NUMS];
    uint32_t last_slot;
    uint32_t list_nodes;

}wheel_unsorted_list;

wheel_unsorted_list* wheel_unsorted_list_init(void* mem, size_t mem_size);

int wheel_unsorted_list_start(wheel_unsorted_list *wul, list_node *node);

int wheel_unsorted_list_stop(list_node *node);

list_node* wheel_unsorted_list_get(wheel_unsorted_list* wul, 
                                   struct timeval* last_timestamp, 
                                   struct timeval* now_timestamp);




#endif