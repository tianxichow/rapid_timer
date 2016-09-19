
#include "sorted_list.h"
#include "timer_node.h"


void* sorted_list_init(void* mem, size_t mem_size) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(sorted_list)) {

        printf("mem_size=%lu less than sorted_list\n", mem_size);
        return NULL;
    }

    sorted_list* sl = (sorted_list*)mem;

    list_head_init(&sl->head);
    sl->list_nodes = 0;

    return sl;
}

int sorted_list_start(void* scheme, timer_node *node) {

    if (NULL == scheme) {
        return -1;
    }

    sorted_list *sl = (sorted_list*)scheme;
    list_node *entry = node->list_entry;

	list_node *check_entry;

	list_for_each(check_entry, &sl->head) {
		if (!timer_node_later_than(entry, check_entry)) {
			list_add_tail(entry, check_entry);
			return 0;
		}
	}

    list_add_tail(entry, &sl->head);

    return 0;
}

int sorted_list_stop(void* scheme, timer_node *node) {

    if (NULL == scheme) {
        return -1;
    }

    list_del(node->list_entry);
    return 0;
}

int sorted_list_get(void* scheme, uint64_t last_timestamp, 
                    uint64_t now_timestamp, list_node* expire_head) {

    if (NULL == scheme) {
        return -1;
    } 
    
    sorted_list *sl = (sorted_list*)scheme;

    list_node *entry;
    list_node *next_entry;

    list_for_each_safe(entry, next_entry, &sl->head) {
        
        if (!is_expire_node(entry, now_timestamp)) {
            continue;
        }

        list_move_tail(entry, expire_head);
    }

    return 0;
}

const struct scheme_operations sorted_list_operations = {

    .scheme_init = sorted_list_init,
    .scheme_start = sorted_list_start,
    .scheme_stop = sorted_list_stop,
    .scheme_get = sorted_list_get,
    .size = sizeof(sorted_list),

};

