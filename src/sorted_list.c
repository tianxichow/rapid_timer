
#include "sorted_list.h"
#include "timer_node.h"

sorted_list *sl;

int sorted_list_init(void* mem, size_t mem_size) {

    if (NULL == mem) {
        return -1;
    }

    if (mem_size < sizeof(sorted_list)) {

        printf("mem_size=%lu less than sorted_list\n", mem_size);
        return -1;
    }

    sl = (sorted_list*)mem;

    list_head_init(&sl->head);
    sl->list_nodes = 0;

    return 0;
}

int sorted_list_start(list_node *node) {

    if (NULL == sl) {
        return -1;
    }

	list_node *check_node;
    list_node *next;

	list_for_each_safe(check_node, next, &sl->head) {
		if (!timer_node_later_than(node, check_node)) {
			list_add_tail(node, check_node);
			break;
		}
	}

    return 0;
}

int sorted_list_stop(list_node *node) {

    if (NULL == sl) {
        return -1;
    }

    list_del(node);
    return 0;
}

list_node* sorted_list_get(uint64_t last_timestamp, uint64_t  now_timestamp){

    if (NULL == sl) {
        return NULL;
    } 

    list_node *node;
    list_node *next;

    list_for_each_safe(node, next, &sl->head) {
        
        if (!is_expire_node(node, now_timestamp)) {
            continue;
        }

        sorted_list_stop(node);
        return node;
    }

    return NULL;
}

const struct scheme_operations sorted_list_operations = {

    .scheme_init = sorted_list_init,
    .scheme_start = sorted_list_start,
    .scheme_stop = sorted_list_stop,
    .scheme_get = sorted_list_get,
    .size = sizeof(sorted_list),

};

