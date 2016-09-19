
#include "rb_tree.h"
#include "timer_node.h"

static inline rb_node *search_rb_node(rb_root *root, uint64_t expire)
{
    rb_node *rb_entry = root->child;
    timer_node *node = NULL;

    while (rb_entry) {

        node = (timer_node *)rb_entry->entity;
        
        if (node->expire < expire) {
            rb_entry = rb_entry->rb_left;
        } else if (node->expire > expire) {
            rb_entry = rb_entry->rb_right;
        } else {
            return rb_entry;
        }
    }

    return NULL;
}


void *rb_tree_init(void *mem, size_t mem_size) {

    if (NULL == mem) {
        return NULL;
    }

    if (mem_size < sizeof(rb_tree)) {

        printf("mem_size=%lu less than rb_tree\n", mem_size);
        return NULL;
    }

    rb_tree* rbt = (rb_tree *)mem;

    RB_ROOT(&rbt);
    rbt->rb_nodes = 0;

    return rbt;
}

int rb_tree_start(void *scheme, timer_node *node) {

    if (NULL == scheme) {
        return -1;
    }

    rb_tree *rbt = (rb_tree *)scheme;
    rb_node *rb_entry = (rb_node *)node->rb_entry;

    rb_node **new_entry = &(rbt->root->child), *parent = NULL;
    timer_node *new = NULL;

    while (*new_entry) {

        new = (timer_node *)new_entry->entity;
        parent = *new_entry;

        if (node->expire < new->expire) {
            new_entry = &((*new_entry)->rb_left);
        } else if (node->expire > new->expire) {
            new_entry = &((*new_entry)->rb_right);    
        } else {
            timer_node* head_node = (timer_node *)((*new_entry)->entity);
            list_add_tail(&node->list_entry, new);
            return 0;
        }
            
    }

    rb_link_node(rb_entry, parent, new_entry);
    rb_insert_color(rb_entry, root);

    list_head_init(&node->list_entry);

    return 0;
}

int rb_tree_stop(void *scheme, timer_node *node) {

    if (NULL == scheme) {
        return -1;
    }

    rb_tree *rbt = (rb_tree *)scheme;
 
    rb_node *rb_entry = search_rb_node(rbt->root, node->expire);

    if (NULL == rb_entry) {
        return -1;
    }

    if (&rb_entry->entity == node) {
        
        list_del(&node->list_entry);
        rb_erase(rb_entry, rbt->root);
        return 0;
    }

    list_node *head = ((timer_node *)(rb_entry->entity))->list_entry;
    list_node *entry;
    list_node *next_entry;

    list_for_each_safe(entry, next_entry, head) {

        if (entry == &node->list_entry) {

            list_del(&node->list_entry);
            rb_erase(rb_entry, rbt->root);
            return 0;
        }

    }

    return 0;
}

int rb_tree_get(void *scheme, uint64_t last_timestamp, 
                    uint64_t now_timestamp, list_node *expire_head) {

    if (NULL == scheme) {
        return -1;
    } 
    
    rb_tree *rbt = (rb_tree *)scheme;

    rb_node *rb_entry = rbt->root->child;
    timer_node *node = NULL;

    for (rb_entry = rb_first(rbt->root); rb_entry; rb_entry = rb_next(rb_entry)) {
        
        node = (timer_node *)rb_entry->entity;

        if (node->expire < now_timestamp) {
            rb_erase(rb_entry, rbt->root);
        }
    }

    list_node *entry;
    list_node *next_entry;
    rb_entry = rb_first(rbt->root);

    while (rb_entry) {

        node = (timer_node *)rb_entry->entity;

        if (node->expire > now_timestamp) {
            break;
        }

        list_for_each_safe(entry, next_entry, &node->list_entry) {
            list_move_tail(entry, expire_head);
        }

        rb_erase(rb_entry, rbt->root);
        list_node_init(&node->list_entry);

        list_add_tail(&node->list_entry, expire_head);

        rb_entry = rb_first(rbt->root);

    }

    return 0;
}

const struct scheme_operations rb_tree_operations = {

    .scheme_init = rb_tree_init,
    .scheme_start = rb_tree_start,
    .scheme_stop = rb_tree_stop,
    .scheme_get = rb_tree_get,
    .size = sizeof(rb_tree),

};

