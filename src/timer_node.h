
#ifndef _TIMER_NODE_H_
#define _TIMER_NODE_H_

#include <sys/time.h>
#include "string.h"
#include "stdint.h"
#include "stdbool.h"

#include "list_node.h"
#include "rb_node.h"

typedef struct timer_node {

    uint32_t id;                        // timer_node index 
    uint32_t seq;                       // sequence number
    uint64_t interval;           
    uint64_t expire;              
    bool is_repeate;
    int (*action_handler)(const void*);
    void* action_data;

    list_node node;
    rb_node tree_node;

}timer_node;

void timer_node_init(timer_node* tn);

bool is_expire_node(const list_node* node, uint64_t now_timestamp);

uint32_t get_slot(const list_node* node, uint32_t slot_nums);

bool timer_node_later_than(const list_node* node, const list_node* check_node);

uint64_t timer_node_interval(const list_node* node);

uint64_t timer_node_expire(const list_node* node);

#endif /* _TIMER_NODE_H_ */


