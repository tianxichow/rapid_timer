
#ifndef _HIERARCHICAL_WHEEL_H_
#define _HIERARCHICAL_WHEEL_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"
#include "scheme.h"

typedef struct hierarchical_wheel {
    
    list_node hw1[WHELL_ROOT_SLOT_NUMS];
    list_node hw2[WHEEL_SLOT_NUMS];
    list_node hw3[WHEEL_SLOT_NUMS];
    list_node hw4[WHEEL_SLOT_NUMS];
    list_node hw5[WHEEL_SLOT_NUMS];

    uint32_t list_nodes;
    uint32_t last_timestamp;

}hierarchical_wheel;


extern const struct scheme_operations hierarchical_wheel_operations; 

void *hierarchical_wheel_init(void *mem, size_t mem_size);

int hierarchical_wheel_start(void *scheme, timer_node *node);

int hierarchical_wheel_stop(void *scheme, timer_node *node);

int hierarchical_wheel_get(void *scheme, uint64_t last_timestamp, 
                           uint64_t now_timestamp, list_node *expire_head);


#endif /* _HIERARCHICAL_WHEEL_H_ */


