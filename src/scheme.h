/*********************************************
* Filename: scheme.h
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月12日 星期一 14时37分16秒
*********************************************/

#ifndef _SCHEME_H_
#define _SCHEME_H_

#include <sys/time.h> 
#include "list_node.h"

struct scheme_operations {

    int (*scheme_init)(void* mem, size_t mem_size);
    int (*scheme_start)(list_node* node);
    int (*scheme_stop)(list_node* node);
    list_node* (*scheme_get)(struct timeval* last_timestamp,
                             struct timeval* now_timestamp);
    size_t size;
};



#endif


