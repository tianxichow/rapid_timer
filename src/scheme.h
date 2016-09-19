
#ifndef _SCHEME_H_
#define _SCHEME_H_

#include <stdint.h>
#include <sys/time.h> 

#include "list_node.h"


#define WHEEL_SLOT_BITS                 12
#define WHELL_ROOT_SLOT_BITS            16
#define WHEEL_SLOT_NUMS                 (1 << WHEEL_SLOT_BITS)
#define WHELL_ROOT_SLOT_NUMS            (1 << WHELL_ROOT_SLOT_BITS)
#define WHEEL_SLOT_MARSK                (WHEEL_SLOT_NUMS - 1)
#define WHELL_ROOT_SLOT_MARKS           (WHELL_ROOT_SLOT_NUMS - 1)

struct scheme_operations {

    void *(*scheme_init)(void *mem, size_t mem_size);
    int (*scheme_start)(void *scheme, list_node *node);
    int (*scheme_stop)(void *scheme, list_node *node);
    int (*scheme_get)(void *scheme, uint64_t last_timestamp, 
                      uint64_t now_timestamp, list_node *expire_head);
    size_t size;
};



#endif /* _SCHEME_H_ */


