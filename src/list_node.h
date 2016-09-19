
#ifndef _LIST_NODE_H_                                                                   
#define _LIST_NODE_H_

#include "stddef.h"


typedef struct list_node {
    struct list_node *next, *prev;
    void *entity;
}list_node;

void list_head_init(list_node *head);

void list_node_init(list_node *node);

int list_is_empty(const list_node *head);

int list_is_last(const list_node *node, const list_node *head);

void list_add(list_node *node, list_node *head);

void list_add_tail(list_node *node, list_node *head);

void list_del(list_node *node);

void list_move(list_node *node, list_node *head);

void list_move_tail(list_node *node, list_node *head);

#define list_for_each(pos, head) \
    for (pos = (head)->next; NULL != pos->next && pos != (head); \
            pos = pos->next) 

#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; NULL != pos->prev && pos != (head); \
            pos = pos->prev)

#define list_for_each_safe(pos, next_entry, head) \
    for (pos = (head)->next, next_entry = pos->next; \
            NULL != pos->next && pos != (head); \
            pos = next_entry, next_entry = pos->next)

#define list_for_each_prev_safe(pos, prev, head) \
    for (pos = (head)->prev, prev = pos->prev; \
            NULL != pos->prev && pos != (head); \
            pos = prev, prev = pos->prev)

#endif /* _LIST_NODE_H_ */
