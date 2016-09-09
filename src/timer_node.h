/*********************************************
* Filename: timer_node.h
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月08日 星期四 16时23分27秒
*********************************************/

#ifndef _TIMER_NODE_H_
#define _TIMER_NODE_H_

typedef struct timer_node {

    timer_id id; 
    uint32_t interval;
    uint32_t expire;
    bool is_repeate;
    int (*action_func)(const void*);
    void * action_data;

    struct list_head list_node;
    struct rb_node tree_node;

}timer_node;


#endif


