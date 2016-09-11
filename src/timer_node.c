
#include "timer_node.h"

void timer_node_init(timer_node* tn) {
   
    int node_index = tn->id;
    memset(tn, 0x0, sizeof(timer_node));
    tn->id = node_index;
    tn->node.entity = tn;
}

bool is_expire_node(const list_node* node, struct timeval* now_timestamp) {
  
    struct timeval* expire = &((timer_node*)node->entity)->expire;
   
    if (expire->tv_sec < now_timestamp->tv_sec) {
        return true;
    }
  
    if (expire->tv_sec == now_timestamp->tv_sec && expire->tv_usec < now_timestamp->tv_usec) {
        return true;
    }
  
    return false;
}

uint32_t get_slot(const list_node* node, uint32_t slot_nums) {

    struct timeval* expire = &((timer_node*)node->entity)->expire;

    return ((expire->tv_sec % slot_nums) * 1000000 + expire->tv_usec) % slot_nums;
}
