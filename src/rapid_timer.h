
#ifndef _RAPID_TIMER_H_
#define _RAPID_TIMER_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"

#include "list_node.h"
#include "timer_node.h"
#include "scheme.h"

// for scheme
#define UNSORTED_LIST                       0
#define SORTED_LIST                         1
#define RB_TREE                             2
#define WHEEL_UNSORTED_LIST                 3
#define WHEEL_SORTED_LIST                   4
#define HIERARCHICAL_WHEEL                  5
#define MAX_SCHEME_NUMS                     6

// for persist_type
#define PROCESS_PERSIST                     0
#define KERNEL_PERSIST                      1

// for is_repeate
#define ONESHOT                             0
#define REPEATE                             1


#define timer_id                            uint32_t 
#define DEFAULT_TIMER_NUMS                  1000



typedef struct rapid_timer {

    int magic_num;

    int scheme_id;
    uint32_t accuracy;
    void *mem;
    int mem_size;

    timer_node *timer_nodes;
    uint32_t timer_node_nums;
    list_node free_timer_nodes;
    uint32_t sequence;

    struct timeval last_tick;

    void *scheme;
    const struct scheme_operations *sops;

    char err_msg[1024];

}rapid_timer;

rapid_timer *rapid_timer_init(uint32_t scheme_id, uint32_t accuracy, 
                              void *mem, size_t mem_size, int persist_type, 
                              struct timeval *now_timestamp);

int rapid_timer_start(rapid_timer *rt, struct timeval *now_timestamp, 
                      struct timeval *interval, bool is_repeate, 
                      int (*action_handler)(const void*, timer_id), 
                      void *action_data, timer_id *id);

int repid_timer_stop(rapid_timer *rt, timer_id id);

int repid_timer_tick(rapid_timer *rt, struct timeval *now_timestamp);


#endif /* _RAPID_TIMER_H_ */
