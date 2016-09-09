/*********************************************
* Filename: rapid_timer.c
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月08日 星期四 15时04分06秒
*********************************************/

#include "rapid_timer.h"  
#include "timer_node.h"
#include "string.h"
#include "errno.h"

#define MAGIC_NUM           1024

size_t scheme_size(rapid_timer* rt) {
    
    if (NULL == rt) {
     
        printf("rapid_timer is NULL, scheme_size failed\n");
        return -1;
    }

    switch(rt->scheme_id) {

        case UNSORTED_LIST:
            return rt->scheme->size;

    }

    return 0;
}

int scheme_init(rapid_timer* rt) {

    if (NULL == rt) {
     
        printf("rapid_timer is NULL, scheme_init failed\n");
        return -1;
    }

    void* scheme_mem = rt->mem + sizeof(rapid_timer);
    size_t scheme_mem_size = rt->men_size - sizeof(rapid_timer);

    switch(rt->scheme_id) {

        case UNSORTED_LIST:

            rt->scheme = unsorted_list_init(scheme_mem, scheme_mem_size);
            
            if (NULL == rt->scheme) {
                return -1;
            }

            break;
        default:
            break;
    }

    return 0;
};

int timer_node_init(rapid_timer* rt) {

    if (NULL == rt) {
     
        printf("rapid_timer is NULL, timer_node_init failed\n");
        return -1;
    }

    void* tn_mem = rt->mem + sizeof(rapid_timer) + scheme_size(rt);
    size_t tn_mem_size = rt->mem_size - sizeof(rapid_timer) - scheme_size(rt);

    if (tn_mem_size < sizeof(timer_node)) {

        printf("timer_node mem_size not enough, timer_node_init failed\n");
        return -1;
    }

    int node_index = 0;

    while ((node_index + 1)* sizeof(timer_node) < tn_mem_size) {

        timer_node* tn = tn_mem + node_index * sizeof(timer_node);
        memset(tn, 0x0, sizeof(timer_node));
        list_add_tail(&tn->list_node, &rt->free_timer_nodes);

        ++node_index;
    }

    return 0;
}

rapid_timer* rapid_timer_init(uint32_t scheme_id, uint32_t max_interval, 
                              uint32_t accuracy, 
                              void* mem, size_t mem_size, 
                              bool reuse) {

    if (scheme_id < LIST || scheme_id > HIERARCHICAL_WHEEL) {

        printf("scheme_id=%d error\n", scheme_id);
        return NULL;
    }

    int ret;
    rapid_timer* rt;
    void* timer_mem = NULL;
    size_t timer_size;

    if (NULL != mem) {
        
        if (mem_size < sizeof(rapid_timer)) {

            printf("mem_size=%d less than rapid_timer\n", mem_size);
            return NULL;
        }

        timer_mem = mem;
        timer_size = mem_size;

        if (reuse) {

            rt = (rapid_timer*)mem;

            if (MAGIC_NUM != rt->magic_num || 
                    scheme_id != rt->scheme_id || 
                    mem != rt->mem ||
                    men_size != rt->men_size) {

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
    rt->max_interval = max_interval;
    rt->accuracy = accuracy;
    rt->mem = timer_mem;
    rt->men_size = timer_size;
    rt->sequence = 0;

    INIT_LIST_HEAD(&rt->free_timer_nodes);

    memset(rt->err_msg, 0x0, 1024);

    ret = scheme_init(rt);

    if (0 != ret) {

        if (NULL == men) {
            free(timer_mem);
        }

        return NULL;
    }

    ret = timer_node_init(rt);

    if (0 != ret) {

        if (NULL == men) {
            free(timer_mem);
        }

        return NULL;
    }

    return rt;
}


