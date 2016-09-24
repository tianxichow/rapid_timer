
#include "wheel_unsorted_list.h"
#include "timer_node.h"



void *wheel_unsorted_list_init(void *mem, size_t mem_size, uint64_t now_timestamp) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(wheel_unsorted_list)) {

        printf("mem_size=%lu less than wheel_unsorted_list_init\n", mem_size);
        return NULL;
    }

    wheel_unsorted_list *wul = (wheel_unsorted_list *)mem;

    int i = 0;

    while (i < WHEEL_SLOT_NUMS) {
        list_head_init(&wul->wheel[i]);
        ++i;
    }

    wul->list_nodes = 0;

    return wul;
}

int wheel_unsorted_list_start(void *scheme, timer_node *node) {
    
    if (NULL == scheme) {
        return -1;
    }

    wheel_unsorted_list *wul = (wheel_unsorted_list *)scheme;

    list_add_tail(&node->list_entry, &wul->wheel[node->expire % WHEEL_SLOT_NUMS]);
    return 0;
}

int wheel_unsorted_list_stop(void *scheme, timer_node *node) {

    if (NULL == scheme) {
        return -1;
    }
    
    list_del(&node->list_entry);
    return 0;
}

int wheel_unsorted_list_get(void *scheme, uint64_t last_timestamp, 
                            uint64_t now_timestamp, list_node *expire_head) {

    if (NULL == scheme) {
        return -1;
    }
    
    wheel_unsorted_list *wul = (wheel_unsorted_list *)scheme;

    uint32_t last_slot = last_timestamp % WHEEL_SLOT_NUMS;
    uint32_t this_slot = now_timestamp % WHEEL_SLOT_NUMS;
    uint32_t gap = now_timestamp - last_timestamp;
    
    if (gap > this_slot) {
        this_slot = (last_slot - 1) % WHEEL_SLOT_NUMS;
    } 

    printf("%u %u\n", last_slot, this_slot);

    list_node *entry;
    list_node *next_entry;

    while (last_slot != this_slot) {

        //printf("check slot %u\n", last_slot);

        list_for_each_safe(entry, next_entry, &wul->wheel[last_slot]) {
        
            if (!is_expire_node(entry, now_timestamp)) {
                continue;
            }

            list_move_tail(entry, expire_head);
        }

        ++last_slot;

        if (WHEEL_SLOT_NUMS == last_slot) {
            last_slot = 0;
        }

    }

    return 0;
}

const struct scheme_operations wheel_unsorted_list_operations = {

    .scheme_init = wheel_unsorted_list_init,
    .scheme_start = wheel_unsorted_list_start,
    .scheme_stop = wheel_unsorted_list_stop,
    .scheme_get = wheel_unsorted_list_get,
    .size = sizeof(wheel_unsorted_list),

};

