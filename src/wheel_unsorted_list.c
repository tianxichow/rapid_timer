
#include "wheel_unsorted_list.h"
#include "timer_node.h"

wheel_unsorted_list * wul;


int wheel_unsorted_list_init(void* mem, size_t mem_size) {

    if (NULL == mem) {
        return -1;
    }

    if (mem_size < sizeof(wheel_unsorted_list)) {

        printf("mem_size=%lu less than wheel_unsorted_list_init\n", mem_size);
        return -1;
    }

    wul = (wheel_unsorted_list*)mem;

    int i = 0;

    while (i < WHEEL_SLOT_NUMS) {
        list_head_init(&wul->wheel[i]);
        ++i;
    }

    wul->list_nodes = 0;

    return 0;
}

int wheel_unsorted_list_start(list_node *node) {
    
    if (NULL == wul) {
        return -1;
    }

    list_add_tail(node, &wul->wheel[get_slot(node, WHEEL_SLOT_NUMS)]);
    return 0;
}

int wheel_unsorted_list_stop(list_node *node) {

    if (NULL == wul) {
        return -1;
    }

    list_del(node);
    return 0;
}

list_node* wheel_unsorted_list_get(uint64_t last_timestamp, 
                                    uint64_t now_timestamp) {

    if (NULL == wul) {
        return NULL;
    }

    uint32_t last_slot = last_timestamp % WHEEL_SLOT_NUMS;
    uint32_t this_slot = now_timestamp % WHEEL_SLOT_NUMS;
    uint32_t gap = now_timestamp - last_timestamp;
    
    if (gap > this_slot) {
        this_slot = (last_slot - 1) % WHEEL_SLOT_NUMS;
    } 

    printf("%u %u\n", last_slot, this_slot);

    list_node *node;
    list_node *next;

    while (last_slot != this_slot) {

        //printf("check slot %u\n", last_slot);

        list_for_each_safe(node, next, &wul->wheel[last_slot]) {
        
            if (!is_expire_node(node, now_timestamp)) {
                continue;
            }

            wheel_unsorted_list_stop(node);
            return node;
        }

        ++last_slot;

        if (WHEEL_SLOT_NUMS == last_slot) {
            last_slot = 0;
        }

    }

    return NULL;
}

const struct scheme_operations wheel_unsorted_list_operations = {

    .scheme_init = wheel_unsorted_list_init,
    .scheme_start = wheel_unsorted_list_start,
    .scheme_stop = wheel_unsorted_list_stop,
    .scheme_get = wheel_unsorted_list_get,
    .size = sizeof(wheel_unsorted_list),

};

