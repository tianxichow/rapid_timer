
#include "stdio.h"

#include "timer_node.h"

void timer_node_init(timer_node *node) {
   
    int node_index = node->id;
    memset(node, 0x0, sizeof(timer_node));
    node->id = node_index;
    node->list_entry.entity = node;
    node->rb_entry.entity = node;
}

bool is_expire_node(const list_node *node, uint64_t now_timestamp) {
    return ((timer_node*)node->entity)->expire <= now_timestamp;
}

uint32_t get_slot(const list_node *node, uint32_t slot_nums) {
    return ((timer_node *)node->entity)->expire % slot_nums;
}

bool timer_node_later_than(const list_node *node, const list_node *check_node) {
    return ((timer_node *)node->entity)->expire > ((timer_node*)check_node->entity)->expire;
}

uint64_t timer_node_interval(const list_node *node) {
    return ((timer_node *)node->entity)->interval;
}

uint64_t timer_node_expire(const list_node *node) {
    return ((timer_node *)node->entity)->expire;
}

