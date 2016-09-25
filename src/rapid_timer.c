
#include "rapid_timer.h"  
#include "string.h"
#include "errno.h"
#include <sys/time.h>

#include "unsorted_list.h"
#include "sorted_list.h"
#include "rb_tree.h"
#include "wheel_unsorted_list.h"
#include "wheel_sorted_list.h"
#include "hierarchical_wheel.h"

#define MAGIC_NUM           1024
#define USEC_PER_SEC        1000000ull

const struct scheme_operations *sops[MAX_SCHEME_NUMS] = {
    &unsorted_list_operations,          // UNSORTED_LIST
    &sorted_list_operations,            // SORTED_LIST
    &rb_tree_operations,                // RB_TREE
    &wheel_unsorted_list_operations,    // WHEEL_UNSORTED_LIST
    &wheel_sorted_list_operations,      // WHEEL_SORTED_LIST
    &hierarchical_wheel_operations      // HIERARCHICAL_WHEEL
};

static inline uint64_t __timeval_to_u64(struct timeval *timestamp) {

    uint64_t timestamp_sce = timestamp->tv_sec;
    return (timestamp_sce * USEC_PER_SEC) + timestamp->tv_usec;
}

static inline uint64_t __reduction(struct timeval *timestamp, uint32_t accuracy) {
    return __timeval_to_u64(timestamp) / accuracy;
}

int free_nodes_init(rapid_timer *rt) {

    if (NULL == rt) {
     
        printf("rapid_timer is NULL, free_nodes_init failed\n");
        return -1;
    }

    timer_node *timer_nodes = rt->mem + sizeof(rapid_timer) + rt->sops->size;
    size_t tn_mem_size = rt->mem_size - sizeof(rapid_timer) - rt->sops->size;

    if (tn_mem_size < sizeof(timer_node)) {

        printf("timer_node mem_size not enough, free_nodes_init failed\n");
        return -1;
    }

    int node_index = 0;

    while ((node_index + 1) * sizeof(timer_node) < tn_mem_size) {

        timer_node *node = &timer_nodes[node_index];
        node->id = node_index;
        timer_node_init(node);
        list_add_tail(&node->list_entry, &rt->free_timer_nodes);

        ++node_index;
    }

    rt->timer_node_nums = node_index;
    //printf("%d timer_node init\n", node_index);
    return 0;
}

rapid_timer *rapid_timer_init(uint32_t scheme_id, uint32_t accuracy, 
                              void *mem, size_t mem_size, int persist_type,
                              struct timeval *now_timestamp) {

    if (scheme_id < UNSORTED_LIST || scheme_id > HIERARCHICAL_WHEEL) {

        printf("scheme_id=%d error\n", scheme_id);
        return NULL;
    }

    if (NULL == now_timestamp) {

         printf("now_timestamp is null\n");
        return NULL;       
    }

    int ret = 0;
    rapid_timer *rt = NULL;
    void *timer_mem = NULL;
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
        
        timer_size = sizeof(rapid_timer) + sops[scheme_id]->size + DEFAULT_TIMER_NUMS * sizeof(timer_node);

        timer_mem = malloc(timer_size);

        if (NULL == timer_mem) {

            printf("malloc error: %s\n", strerror(errno));
            return NULL;
        }

    }

    rt = (rapid_timer*)timer_mem;

    rt->magic_num = MAGIC_NUM;
    rt->scheme_id = scheme_id;
    rt->sops = sops[scheme_id];
    rt->accuracy = accuracy;
    rt->mem = timer_mem;
    rt->mem_size = timer_size;
    rt->sequence = 0;

    list_head_init(&rt->free_timer_nodes);

    memset(rt->err_msg, 0x0, 1024);

    void *scheme_mem = rt->mem + sizeof(rapid_timer);
    size_t scheme_mem_size = rt->mem_size - sizeof(rapid_timer);

    rt->scheme = rt->sops->scheme_init(scheme_mem,  scheme_mem_size, 
                                       __reduction(now_timestamp, rt->accuracy));
            
    if (NULL == rt->scheme) {

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


int rapid_timer_start(rapid_timer *rt, struct timeval *now_timestamp, 
                      struct timeval *interval, bool is_repeate, 
                      int (*action_handler)(const void*, timer_id), 
                      void *action_data, timer_id *id) {

    if (NULL == rt || NULL == now_timestamp || NULL == interval) {
     
        printf("rapid_timer is NULL, rapid_timer_start failed\n");
        return -1;
    }

    if (list_is_empty(&rt->free_timer_nodes)) {
        
        printf("free_timer_nodes is empty\n");
        return -1; 
    }

    // get free timer_node
    list_node *list_entry = rt->free_timer_nodes.next;
    list_del(list_entry);

    // fill timer_node
    timer_node *node = list_entry->entity;
    node->seq = rt->sequence++;
    node->interval = __reduction(interval, rt->accuracy);
    node->expire = __reduction(now_timestamp, rt->accuracy) + node->interval;
    node->is_repeate = is_repeate;
    node->action_handler = action_handler;
    node->action_data = action_data;

    int ret = 0;

    ret = rt->sops->scheme_start(rt->scheme, node);

    if (0 != ret) {
     
        timer_node_init(node);
        list_add_tail(&node->list_entry, &rt->free_timer_nodes);
        return -1;
    }

    if (NULL != id) {
        *id = node->id;
    }
    
    return 0;
}

int repid_timer_stop(rapid_timer *rt, timer_id id) {

    if (NULL == rt) {
     
        printf("rapid_timer is NULL, repid_timer_stop failed\n");
        return -1;
    }

    if (id > rt->timer_node_nums) {

        printf("id=%u error, repid_timer_stop failed\n", id);
        return -1;
    }

    timer_node *node = &rt->timer_nodes[id];

    rt->sops->scheme_stop(rt->scheme, node);
    timer_node_init(node);
    list_add_tail(&node->list_entry, &rt->free_timer_nodes);

    return 0;
}

int repid_timer_tick(rapid_timer *rt, struct timeval *now_timestamp) {

    if (NULL == rt || NULL == now_timestamp) {
     
        printf("rapid_timer is NULL, repid_timer_stop failed\n");
        return -1;
    }

    int ret = 0;

    uint64_t last = __reduction(&rt->last_tick, rt->accuracy);
    uint64_t now = __reduction(now_timestamp, rt->accuracy);

    list_node expire_head;
    list_head_init(&expire_head);
    
    list_node *entry;
    list_node *next_entry;
    
    ret = rt->sops->scheme_get(rt->scheme, last, now, &expire_head);

    list_for_each_safe(entry, next_entry, &expire_head) {
        
        list_del(entry);
        timer_node *node = (timer_node *)entry->entity;

        if (NULL != node->action_handler) {
            node->action_handler(node->action_data, node->id);
        } 

        if (node->is_repeate) {
    
            node->seq = rt->sequence++;
            node->expire = now + node->interval;

            //printf("start %llu %llu %llu\n", node->expire, now, node->interval);

            ret = rt->sops->scheme_start(rt->scheme, node);
            
            if (0 != ret) {
                timer_node_init(node);
                list_add_tail(entry, &rt->free_timer_nodes);
                return -1;
            }

        } else {
            timer_node_init(node);
            list_add_tail(entry, &rt->free_timer_nodes);
        }
    }

    rt->last_tick.tv_sec = now_timestamp->tv_sec;
    rt->last_tick.tv_usec = now_timestamp->tv_usec;

    return 0;
}
