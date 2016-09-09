/*********************************************
* Filename: tapid_timer.h
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月08日 星期四 10时08分58秒
*********************************************/


#ifndef RAPID_TIMER_H_
#define RAPID_TIMER_H_

#include "stdint.h"

#define UNSORTED_LIST                       1
#define SORTED_LIST                         2
#define RBTREE                              3
#define SIMPLE_WHEEL                        4
#define HASHED_WHEEL_UNSORTED_LIST          5
#define HASHED_WHEEL_SORTED_LIST            6
#define HIERARCHICAL_WHEEL                  7

#define uint32_t                            timer_id
#define DEFAULT_TIMER_NUMS                  1000


typedef struct rapid_timer {

    int magic_num;

    int scheme_id;
    int max_interval;
    int accuracy;
    void* mem;
    int men_size;

    uint32_t sequence;
    void* scheme;
 
    struct list_head free_timer_nodes;

    char err_msg[1024];

}rapid_timer;

rapid_timer* rapid_timer_init(uint32_t scheme_id, uint32_t max_interval, 
                              uint32_t accuracy, 
                              void* mem, size_t mem_size, 
                              bool reuse);

int rapid_timer_start(rapid_timer rt, uint32_t interval, bool is_repeate, 
                     int (*action_func)(const void*), void* action_data, 
                     timer_id& id);

int repid_timer_stop(rapid_timer rt, timer_id id);

int repid_timer_tick(rapid_timer rt, uint32_t now_timestamp);


#endif
