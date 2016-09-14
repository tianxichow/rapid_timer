/*********************************************
* Filename: scheme.h
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月12日 星期一 14时37分16秒
*********************************************/

#ifndef _SCHEME_H_
#define _SCHEME_H_

#include <stdint.h>
#include <sys/time.h> 

#include "list_node.h"

#define WHEEL_SLOT_NUMS    (1 << 8)

struct scheme_operations {

    void* (*scheme_init)(void* mem, size_t mem_size);
    int (*scheme_start)(void* scheme, list_node* node);
    int (*scheme_stop)(void* scheme, list_node* node);
    int (*scheme_get)(void* scheme, uint64_t last_timestamp, 
                      uint64_t now_timestamp, list_node* expire_head);
    size_t size;
};



#endif


