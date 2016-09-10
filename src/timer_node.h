/*********************************************
* Filename: timer_node.h
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月08日 星期四 16时23分27秒
*********************************************/

#ifndef _TIMER_NODE_H_
#define _TIMER_NODE_H_

#include "time.h"
#include "string.h"

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

void timer_node_init(timer_node* tn) {
    int node_index = tn->id;
    memset(tn, 0x0, sizeof(timer_node));
    tn->id = node_index;
    tn->node.entity = tn;
}

bool is_expire_node(const list_node* node, struct timeval* now_timestamp) {
    struct timeval* expire = &((timer_node*)node->entity)->expire;
    if (expire->tv_sec > now_timestamp->tv_sec) {
        return true;
    }
    if (expire->tv_usec > now_timestamp->tv_usec) {
        return true;
    }
    return false;
}


#endif


