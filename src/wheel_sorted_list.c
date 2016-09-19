
#include "wheel_sorted_list.h"
#include "timer_node.h"



void *wheel_sorted_list_init(void *mem, size_t mem_size) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(wheel_sorted_list)) {

        printf("mem_size=%lu less than wheel_sorted_list_init\n", mem_size);
        return NULL;
    }

    wheel_sorted_list *wsl = (wheel_sorted_list *)mem;

    int i = 0;

    while (i < WHEEL_SLOT_NUMS) {
        list_head_init(&wsl->wheel[i]);
        ++i;
    }

    wsl->list_nodes = 0;

    return wsl;
}

int wheel_sorted_list_start(void *scheme, timer_node *node) {
    
    if (NULL == scheme) {
        return -1;
    }

    wheel_sorted_list *wsl = (wheel_sorted_list *)scheme;
    list_node *entry = node->list_entry;

    list_node *check_entry;
    list_node *head_entry = &wsl->wheel[node->expire % WHEEL_SLOT_NUMS];

	list_for_each(check_entry, head_entry) {
		if (!timer_node_later_than(entry, check_entry)) {
			list_add_tail(entry, check_entry);
			return 0;
		}
	}

    list_add_tail(entry, head_entry);
    return 0;
}

int wheel_sorted_list_stop(void *scheme, list_node *node) {

    if (NULL == scheme) {
        return -1;
    }
    
    list_del(node->list_entry);
    return 0;
}

int wheel_sorted_list_get(void *scheme, uint64_t last_timestamp, 
                            uint64_t now_timestamp, list_node *expire_head) {

    if (NULL == scheme) {
        return -1;
    }
    
    wheel_sorted_list *wsl = (wheel_sorted_list *)scheme;

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

        list_for_each_safe(entry, next_entry, &wsl->wheel[last_slot]) {
        
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

const struct scheme_operations wheel_sorted_list_operations = {

    .scheme_init = wheel_sorted_list_init,
    .scheme_start = wheel_sorted_list_start,
    .scheme_stop = wheel_sorted_list_stop,
    .scheme_get = wheel_sorted_list_get,
    .size = sizeof(wheel_sorted_list),

};

