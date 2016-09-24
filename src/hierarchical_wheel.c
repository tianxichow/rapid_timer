
#include "hierarchical_wheel.h"
#include "timer_node.h"

static int cascade(hierarchical_wheel *hw, 
                   list_node *wheel_head, int wheel_index) {

    int index = (hw->last_timestamp >> 
                 (WHELL_ROOT_SLOT_BITS + (wheel_index - 2) * WHEEL_SLOT_BITS)) & 
                 WHEEL_SLOT_MARSK;

    list_node *entry;
    list_node *next_entry;

    list_for_each_safe(entry, next_entry, wheel_head + index) {
        hierarchical_wheel_stop(hw, entry->entity);
        hierarchical_wheel_start(hw, entry->entity);
    }

    return index;
}

void *hierarchical_wheel_init(void *mem, size_t mem_size, uint64_t now_timestamp) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(hierarchical_wheel)) {

        printf("mem_size=%lu less than size of hierarchical_wheel\n", mem_size);
        return NULL;
    }

    hierarchical_wheel *hw = (hierarchical_wheel *)mem;

    int i = 0;
    
    while (i < WHELL_ROOT_SLOT_NUMS) {
        list_head_init(hw->hw1 + i);
        ++i;
    }
    
    i = 0;

    while (i < WHEEL_SLOT_NUMS) {
        list_head_init(hw->hw2 + i);
        list_head_init(hw->hw3 + i);
        list_head_init(hw->hw4 + i);
        list_head_init(hw->hw5 + i);
        ++i;
    }

    hw->list_nodes = 0;
    hw->last_timestamp = now_timestamp;
    printf("last_timestamp %llu\n", hw->last_timestamp);


    return hw;
}

int hierarchical_wheel_start(void *scheme, timer_node *node) {
    
    if (NULL == scheme) {
        return -1;
    }

    hierarchical_wheel *hw = (hierarchical_wheel *)scheme;

    uint64_t expire = node->expire;
    int64_t interval = expire - hw->last_timestamp;
    
    list_node *head_node = NULL;

    if (interval <= 0) {
        head_node = hw->hw1 + (hw->last_timestamp & WHELL_ROOT_SLOT_MARKS);
    } else if (interval < WHELL_ROOT_SLOT_NUMS) {
        int i = expire & WHELL_ROOT_SLOT_MARKS;
        head_node = hw->hw1 + i;
    } else if (interval < 1LL << (WHELL_ROOT_SLOT_BITS + WHEEL_SLOT_BITS)) {
        int i = (expire >> WHELL_ROOT_SLOT_BITS) & WHEEL_SLOT_MARSK;
        head_node = hw->hw2 + i;
    } else if (interval < 1LL << (WHELL_ROOT_SLOT_BITS + 2 * WHEEL_SLOT_BITS)) {
        int i = (expire >> (WHELL_ROOT_SLOT_BITS + WHEEL_SLOT_BITS)) & 
                                                            WHEEL_SLOT_MARSK;
        head_node = hw->hw3 + i;
    } else if (interval < 1LL << (WHELL_ROOT_SLOT_BITS + 3 * WHEEL_SLOT_BITS)) {
        int i = (expire >> (WHELL_ROOT_SLOT_BITS + 2  *WHEEL_SLOT_BITS)) & 
                                                            WHEEL_SLOT_MARSK;
        head_node = hw->hw4 + i;
    } else {
        int i = (expire >> (WHELL_ROOT_SLOT_BITS + 3 * WHEEL_SLOT_BITS)) & 
                                                            WHEEL_SLOT_MARSK;
        head_node = hw->hw5 + i;
    }

    list_add_tail(&node->list_entry, head_node);

    return 0;
}

int hierarchical_wheel_stop(void *scheme, timer_node *node) {

    if (NULL == scheme) {
        return -1;
    }
    
    list_del(&node->list_entry);
    return 0;
}

int hierarchical_wheel_get(void *scheme, uint64_t last_timestamp, 
                            uint64_t now_timestamp, list_node *expire_head) {

    if (NULL == scheme) {
        return -1;
    }
    
    hierarchical_wheel *hw = (hierarchical_wheel *)scheme;
    
    list_node *entry;
    list_node *next_entry;

    do {

        int index = hw->last_timestamp & WHELL_ROOT_SLOT_MARKS;

        if (!index && !cascade(hw, hw->hw2, 2) &&
                      !cascade(hw, hw->hw3, 3) &&
                      !cascade(hw, hw->hw4, 4) &&
                      !cascade(hw, hw->hw5, 5)) {};
        
        list_for_each_safe(entry, next_entry, hw->hw1 + index) {
            list_move_tail(entry, expire_head);
        }

    } while (++hw->last_timestamp <= now_timestamp) ;

    hw->last_timestamp = now_timestamp;

    return 0;
}

const struct scheme_operations hierarchical_wheel_operations = {

    .scheme_init = hierarchical_wheel_init,
    .scheme_start = hierarchical_wheel_start,
    .scheme_stop = hierarchical_wheel_stop,
    .scheme_get = hierarchical_wheel_get,
    .size = sizeof(hierarchical_wheel),

};

