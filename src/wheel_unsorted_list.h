#ifndef _WHEEL_UNSORTED_LIST_H_
#define _WHEEL_UNSORTED_LIST_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"
#include "scheme.h"

typedef struct wheel_unsorted_list {

    list_node wheel[WHEEL_SLOT_NUMS];
    uint32_t last_slot;
    uint32_t list_nodes;
}wheel_unsorted_list;

extern const struct scheme_operations wheel_unsorted_list_operations; 

void *wheel_unsorted_list_init(void *mem, size_t mem_size);

int wheel_unsorted_list_start(void *scheme, timer_node *node);

int wheel_unsorted_list_stop(void *scheme, timer_node *node);

int wheel_unsorted_list_get(void *scheme, uint64_t last_timestamp, 
                            uint64_t now_timestamp, list_node *expire_head);




#endif /* _WHEEL_UNSORTED_LIST_H_ */
