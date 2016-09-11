
#include "wheel_unsorted_list.h"
#include "timer_node.h"

wheel_unsorted_list* wheel_unsorted_list_init(void* mem, size_t mem_size) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(wheel_unsorted_list)) {

        printf("mem_size=%lu less than wheel_unsorted_list_init\n", mem_size);
        return NULL;
    }

    wheel_unsorted_list *wul = (wheel_unsorted_list*)mem;

    for (int i = 0;i < WHEEL_SLOT_NUMS; ++i) {
        list_head_init(&wul->wheel[i]);
    }

    wul->list_nodes = 0;

    return wul;
}

int wheel_unsorted_list_start(wheel_unsorted_list *wul, list_node *node) {

    list_add_tail(node, &wul->wheel[get_slot(node, WHEEL_SLOT_NUMS)]);
    return 0;
}

int wheel_unsorted_list_stop(list_node *node) {

    list_del(node);
    return 0;
}

list_node* wheel_unsorted_list_get(wheel_unsorted_list* wul, 
                                   struct timeval* last_timestamp, 
                                   struct timeval* now_timestamp) {

    if (NULL == wul || NULL == last_timestamp || 
        NULL == now_timestamp) {
            return NULL;
    }    

    uint32_t last_slot = ((last_timestamp->tv_sec % WHEEL_SLOT_NUMS) * 1000000 + 
                            last_timestamp->tv_usec) % WHEEL_SLOT_NUMS;
    uint32_t this_slot = ((now_timestamp->tv_sec % WHEEL_SLOT_NUMS) * 1000000 + 
                            now_timestamp->tv_usec) % WHEEL_SLOT_NUMS;   

    list_node *node;
    list_node *next;

    while (last_slot <= this_slot) {

        list_for_each_safe(node, next, &wul->wheel[last_slot]) {
        
            if (!is_expire_node(node, now_timestamp)) {
                continue;
            }

            wheel_unsorted_list_stop(node);
            return node;
        }

        last_slot++;
    }

    return NULL;
    
}
