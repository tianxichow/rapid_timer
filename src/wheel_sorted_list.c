
#include "wheel_sorted_list.h"
#include "timer_node.h"



void* wheel_sorted_list_init(void* mem, size_t mem_size) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(wheel_sorted_list)) {

        printf("mem_size=%lu less than wheel_sorted_list_init\n", mem_size);
        return NULL;
    }

    wheel_sorted_list* wsl = (wheel_sorted_list*)mem;

    int i = 0;

    while (i < WHEEL_SLOT_NUMS) {
        list_head_init(&wsl->wheel[i]);
        ++i;
    }

    wsl->list_nodes = 0;

    return wsl;
}

int wheel_sorted_list_start(void* scheme, list_node *node) {
    
    if (NULL == scheme) {
        return -1;
    }

    wheel_sorted_list* wsl = (wheel_sorted_list*)scheme;

    list_node* check_node;
    list_node* head_node = &wsl->wheel[get_slot(node, WHEEL_SLOT_NUMS)];

	list_for_each(check_node, head_node) {
		if (!timer_node_later_than(node, check_node)) {
			list_add_tail(node, check_node);
			return 0;
		}
	}

    list_add_tail(node, head_node);
    return 0;
}

int wheel_sorted_list_stop(void* scheme, list_node *node) {

    if (NULL == scheme) {
        return -1;
    }
    
    list_del(node);
    return 0;
}

int wheel_sorted_list_get(void* scheme, uint64_t last_timestamp, 
                            uint64_t now_timestamp, list_node* expire_head) {

    if (NULL == scheme) {
        return -1;
    }
    
    wheel_sorted_list* wsl = (wheel_sorted_list*)scheme;

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

        list_for_each_safe(node, next, &wsl->wheel[last_slot]) {
        
            if (!is_expire_node(node, now_timestamp)) {
                continue;
            }

            list_move_tail(node, expire_head);
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

