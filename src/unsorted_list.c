
#include "unsorted_list.h"
#include "timer_node.h"

void* unsorted_list_init(void* mem, size_t mem_size) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(unsorted_list)) {

        printf("mem_size=%lu less than unsorted_list\n", mem_size);
        return NULL;
    }

    unsorted_list* ul = (unsorted_list*)mem;

    list_head_init(&ul->head);
    ul->list_nodes = 0;

    return ul;
}

int unsorted_list_start(void* scheme, list_node *node) {

    if (NULL == scheme) {
        return -1;
    }

    unsorted_list* ul = (unsorted_list*)scheme;

    list_add_tail(node, &ul->head);
    return 0;
}

int unsorted_list_stop(void* scheme, list_node *node) {

    if (NULL == scheme) {
        return -1;
    }
    
    list_del(node);
    return 0;
}

int unsorted_list_get(void* scheme, uint64_t last_timestamp, 
                      uint64_t now_timestamp, list_node* expire_head) {

    if (NULL == scheme) {
        return -1;
    } 

    unsorted_list* ul = (unsorted_list*)scheme;

    list_node *node;
    list_node *next;

    list_for_each_safe(node, next, &ul->head) {
        
        if (!is_expire_node(node, now_timestamp)) {
            continue;
        }

        list_move_tail(node, expire_head);
    }

    return 0;
}

const struct scheme_operations unsorted_list_operations = {

    .scheme_init = unsorted_list_init,
    .scheme_start = unsorted_list_start,
    .scheme_stop = unsorted_list_stop,
    .scheme_get = unsorted_list_get,
    .size = sizeof(unsorted_list),

};

