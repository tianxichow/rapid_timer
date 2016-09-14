/*********************************************
* Filename: rapid_timer.c
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月08日 星期四 15时04分06秒
*********************************************/

#include "rapid_timer.h"  
#include "string.h"
#include "errno.h"
#include <sys/time.h>

#include "unsorted_list.h"
#include "sorted_list.h"
#include "wheel_unsorted_list.h"
#include "wheel_sorted_list.h"

#define MAGIC_NUM           1024
#define USEC_PER_SEC        1000000ull



static inline uint64_t __timeval_to_u64(struct timeval* timestamp) {

    uint64_t timestamp_sce = timestamp->tv_sec;
    return (timestamp_sce * USEC_PER_SEC) + timestamp->tv_usec;
}

static inline uint64_t __reduction(struct timeval* timestamp, uint32_t accuracy) {
    return __timeval_to_u64(timestamp) / accuracy;
}

int scheme_init(rapid_timer* rt) {

    if (NULL == rt) {
     
        printf("rapid_timer is NULL, scheme_init failed\n");
        return -1;
    }

    int ret = 0;

    void *scheme_mem = rt->mem + sizeof(rapid_timer);
    size_t scheme_mem_size = rt->mem_size - sizeof(rapid_timer);

    switch(rt->scheme_id) {

        case UNSORTED_LIST:
            rt->sops = &unsorted_list_operations;
            break;
        case SORTED_LIST:
            rt->sops = &sorted_list_operations;
            break;
        case WHEEL_UNSORTED_LIST:
            rt->sops = &wheel_unsorted_list_operations;
            break;
        case WHEEL_SORTED_LIST:
            rt->sops = &wheel_sorted_list_operations;
            break;
        default:
            break;
    }
            
    rt->scheme = rt->sops->scheme_init(scheme_mem,  scheme_mem_size);
            
    if (0 != ret) {
        return -1;
    }

    return 0;
};

int free_nodes_init(rapid_timer* rt) {

    if (NULL == rt) {
     
        printf("rapid_timer is NULL, free_nodes_init failed\n");
        return -1;
    }

    timer_node* timer_nodes = rt->mem + sizeof(rapid_timer) + rt->sops->size;
    size_t tn_mem_size = rt->mem_size - sizeof(rapid_timer) - rt->sops->size;

    if (tn_mem_size < sizeof(timer_node)) {

        printf("timer_node mem_size not enough, free_nodes_init failed\n");
        return -1;
    }

    int node_index = 0;

    while ((node_index + 1)* sizeof(timer_node) < tn_mem_size) {

        timer_node* tn = &timer_nodes[node_index];
        tn->id = node_index;
        timer_node_init(tn);
        list_add_tail(&tn->node, &rt->free_timer_nodes);

        ++node_index;
    }

    rt->timer_node_nums = node_index;
    printf("%d timer_node init\n", node_index);
    return 0;
}

rapid_timer* rapid_timer_init(uint32_t scheme_id, uint32_t accuracy, 
                              void* mem, size_t mem_size, int persist_type) {

    if (scheme_id < UNSORTED_LIST || scheme_id > HIERARCHICAL_WHEEL) {

        printf("scheme_id=%d error\n", scheme_id);
        return NULL;
    }

    int ret;
    rapid_timer* rt = NULL;
    void* timer_mem = NULL;
    size_t timer_size;

    if (NULL != mem) {
        
        if (mem_size < sizeof(rapid_timer)) {

            printf("mem_size=%lu less than rapid_timer\n", mem_size);
            return NULL;
        }

        timer_mem = mem;
        timer_size = mem_size;

        if (KERNEL_PERSIST == persist_type) {

            rt = (rapid_timer*)mem;

            if (MAGIC_NUM != rt->magic_num || scheme_id != rt->scheme_id || 
                mem != rt->mem || mem_size != rt->mem_size) {

                printf("reuse failed\n");
                return NULL;
            }

            return rt;
        }

    } else {
        
        timer_size = sizeof(rapid_timer) + DEFAULT_TIMER_NUMS * sizeof(timer_node);

        timer_mem = malloc(timer_size);

        if (NULL == timer_mem) {

            printf("malloc error: %s\n", strerror(errno));
            return NULL;
        }

    }

    rt = (rapid_timer*)timer_mem;

    rt->magic_num = MAGIC_NUM;
    rt->scheme_id = scheme_id;
    rt->accuracy = accuracy;
    rt->mem = timer_mem;
    rt->mem_size = timer_size;
    rt->sequence = 0;

    list_head_init(&rt->free_timer_nodes);

    memset(rt->err_msg, 0x0, 1024);

    ret = scheme_init(rt);

    if (0 != ret) {

        if (NULL == mem) {
            free(timer_mem);
        }

        return NULL;
    }

    ret = free_nodes_init(rt);

    if (0 != ret) {

        if (NULL == mem) {
            free(timer_mem);
        }

        return NULL;
    }

    return rt;
}


int rapid_timer_start(rapid_timer* rt, struct timeval* now_timestamp, 
                      struct timeval* interval, bool is_repeate, 
                      int (*action_handler)(const void*), void* action_data, 
                      timer_id *id) {

    if (NULL == rt || NULL == now_timestamp || NULL == interval) {
     
        printf("rapid_timer is NULL, rapid_timer_start failed\n");
        return -1;
    }

    if (list_is_empty(&rt->free_timer_nodes)) {
        
        printf("free_timer_nodes is empty\n");
        return -1; 
    }

    // get free node
    list_node* node = rt->free_timer_nodes.next;
    list_del(node);

    // fill timer_node
    timer_node* tn = node->entity;
    tn->seq = rt->sequence++;
    tn->interval = __reduction(interval, rt->accuracy);
    tn->expire = __reduction(now_timestamp, rt->accuracy) + tn->interval;
    tn->is_repeate = is_repeate;
    tn->action_handler = action_handler;
    tn->action_data = action_data;

    int ret = 0;

    ret = rt->sops->scheme_start(rt->scheme, node);

    if (0 != ret) {
     
        timer_node_init(tn);
        list_add_tail(&tn->node, &rt->free_timer_nodes);
        return -1;
    }

    if (NULL != id) {
        *id = tn->id;
    }
    
    return 0;
}

int repid_timer_stop(rapid_timer* rt, timer_id id) {

    if (NULL == rt) {
     
        printf("rapid_timer is NULL, repid_timer_stop failed\n");
        return -1;
    }

    if (id > rt->timer_node_nums) {

        printf("id=%u error, repid_timer_stop failed\n", id);
        return -1;
    }

    timer_node* tn = &rt->timer_nodes[id];

    rt->sops->scheme_stop(rt->scheme, &tn->node);
    timer_node_init(tn);
    list_add_tail(&tn->node, &rt->free_timer_nodes);

    return 0;
}

int repid_timer_tick(rapid_timer* rt, struct timeval* now_timestamp) {

    if (NULL == rt || NULL == now_timestamp) {
     
        printf("rapid_timer is NULL, repid_timer_stop failed\n");
        return -1;
    }

    int ret = 0;

    uint64_t last = __reduction(&rt->last_tick, rt->accuracy);
    uint64_t now = __reduction(now_timestamp, rt->accuracy);

    list_node expire_head;
    list_head_init(&expire_head);
    
    list_node* node;
    list_node* next;
    
    ret = rt->sops->scheme_get(rt->scheme, last, now, &expire_head);

    list_for_each_safe(node, next, &expire_head) {
        
        timer_node* tn = (timer_node*)node->entity;

        if (NULL != tn->action_handler) {
            tn->action_handler(tn->action_data);
        } 

        if (tn->is_repeate) {
    
            tn->seq = rt->sequence++;
            tn->expire = now + tn->interval;

            printf("%lu %lu %lu\n", tn->expire, now, tn->interval);

            ret = rt->sops->scheme_start(rt->scheme, node);
            
            if (0 != ret) {
                timer_node_init(tn);
                list_add_tail(&tn->node, &rt->free_timer_nodes);
                return -1;
            }

        } else {
            timer_node_init(tn);
            list_add_tail(&tn->node, &rt->free_timer_nodes);
        }
    }

    rt->last_tick.tv_sec = now_timestamp->tv_sec;
    rt->last_tick.tv_usec = now_timestamp->tv_usec;

    return 0;
}
