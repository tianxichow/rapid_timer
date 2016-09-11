
#ifndef _UNSORTED_LIST_H_
#define _UNSORTED_LIST_H_

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include <sys/time.h>

#include "list_node.h"

typedef struct unsorted_list {

    list_node head;
    uint32_t list_nodes;
}unsorted_list;

unsorted_list* unsorted_list_init(void* mem, size_t mem_size);

int unsorted_list_start(unsorted_list *ul, list_node *node);

int unsorted_list_stop(list_node *node);

list_node* unsorted_list_get(unsorted_list* ul, 
                             struct timeval* last_timestamp, 
                             struct timeval* now_timestamp);


#endif
