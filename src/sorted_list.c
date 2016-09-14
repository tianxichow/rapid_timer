
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

int sorted_list_start(void* scheme, list_node *node) {

    if (NULL == scheme) {
        return -1;
    }

    sorted_list *sl = (sorted_list*)scheme;

	list_node *check_node;

	list_for_each(check_node, &sl->head) {
		if (!timer_node_later_than(node, check_node)) {
			list_add_tail(node, check_node);
			return 0;
		}
	}

    list_add_tail(node, &sl->head);

    return 0;
}

int sorted_list_stop(void* scheme, list_node *node) {

    if (NULL == scheme) {
        return -1;
    }

    list_del(node);
    return 0;
}

int sorted_list_get(void* scheme, uint64_t last_timestamp, 
                    uint64_t now_timestamp, list_node* expire_head) {

    if (NULL == scheme) {
        return -1;
    } 
    
    sorted_list *sl = (sorted_list*)scheme;

    list_node *node;
    list_node *next;

    list_for_each_safe(node, next, &sl->head) {
        
        if (!is_expire_node(node, now_timestamp)) {
            continue;
        }

        list_move_tail(node, expire_head);
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

