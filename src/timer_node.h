/*********************************************
* Filename: timer_node.h
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月08日 星期四 16时23分27秒
*********************************************/

#ifndef _TIMER_NODE_H_
#define _TIMER_NODE_H_

#include <sys/time.h>
#include "string.h"
#include "stdint.h"
#include "stdbool.h"

#include "list_node.h"

typedef struct timer_node {

    uint32_t id;                        // timer_node index 
    uint32_t seq;                       // sequence number
    struct timeval interval;            // interval info
    struct timeval expire;              
    bool is_repeate;
    int (*action_handler)(const void*);
    void* action_data;

    list_node node;
    //struct rb_node tree_node;

}timer_node;

void timer_node_init(timer_node* tn);

bool is_expire_node(const list_node* node, struct timeval* now_timestamp);

uint32_t get_slot(const list_node* node, uint32_t slot_nums);

#endif


