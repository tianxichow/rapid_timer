/*********************************************
* Filename: list_node.c
* Author: PIzhou
* Mail: yuanzhou@outlook.com
* CreateTime: 2016年09月09日 星期五 16时28分06秒
*********************************************/

#include "list_node.h"

static inline void __list_add(list_node *node, list_node *prev, list_node *next) {
	
    next->prev = node;
	node->next = next;
	node->prev = prev;
	prev->next = node;
}

int list_is_empty(const list_node *head) {
    
    list_node* next = head->next;
    return (next == head) && (next == head->prev);
}

int list_is_last(const list_node *node, const list_node *head) {
    return node->next == head;
}

void list_add(list_node *node, list_node *head) {
    __list_add(node, head, head->next);
}

void list_add_tail(list_node *node, list_node *head) {
    __list_add(node, head->prev, head); 
}

static inline void __list_del(list_node * prev, list_node * next)
{
	next->prev = prev;
	prev->next = next;
}

void list_del(list_node *node) {

	__list_del(node->prev, node->next);
	node->next = NULL;
	node->prev = NULL;
}

void list_move(list_node *node, list_node *head) {
	
    __list_del(node->prev, node->next);
	list_add(node, head);
}

