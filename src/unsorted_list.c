
#include "unsorted_list.h"
#include "timer_node.h"

unsorted_list* unsorted_list_init(void* mem, size_t mem_size) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(unsorted_list)) {

        printf("mem_size=%lu less than unsorted_list\n", mem_size);
        return NULL;
    }

    unsorted_list *ul = (unsorted_list*)mem;

    list_head_init(&ul->head);
    ul->list_nodes = 0;

    return ul;
}

int unsorted_list_start(unsorted_list *ul, list_node *node) {

    list_add_tail(node, &ul->head);
    return 0;
}

int unsorted_list_stop(list_node *node) {

    list_del(node);
    return 0;
}

list_node* unsorted_list_get(unsorted_list* ul, 
                             struct timeval* last_timestamp, 
                             struct timeval* now_timestamp) {

    if (NULL == ul || NULL == last_timestamp || 
        NULL == now_timestamp) {
            return NULL;
    }    

    list_node *node;
    list_node *next;

    list_for_each_safe(node, next, &ul->head) {
        
        if (!is_expire_node(node, now_timestamp)) {
            continue;
        }

        unsorted_list_stop(node);
        return node;
    }

    return NULL;
}
