/*********************************************
* Filename: list_node.h
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月09日 星期五 15时55分19秒
*********************************************/

#ifndef _LIST_NODE_H_                                                                   
#define _LIST_NODE_H_

typedef struct list_node {
    struct list_node *next, *prev;
    void* entity;
}list_node;

#define LIST_HEAD_INIT(head) head = { &(head), &(head) , NULL}

#define LIST_NODE_INIT(node) node = { NULL, NULL, NULL}

int list_is_empty(const list_node *head);

int list_is_last(const list_node *node, const list_node *head);

void list_add(list_node *node, list_node *head);

void list_add_tail(list_node *node, list_node *head);

void list_del(list_node *node);

void list_move(list_node *node, list_node *head);

#define list_for_each(pos, head) \
    for (pos = (head)->next; NULL != pos->next && pos != (head); \
            pos = pos->next) 

#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; NULL != pos->prev && pos != (head); \
            pos = pos->prev)

#define list_for_each_safe(pos, next, head) \
    for (pos = (head)->next, next = pos->next; \
            NULL != pos->next && pos != (head); \
            pos = next, next = pos->next)

#define list_for_each_prev_safe(pos, prev, head) \
    for (pos = (head)->prev, prev = pos->prev; \
            NULL != pos->prev && pos != (head); \
            pos = prev, prev = pos->prev)

#endif 
