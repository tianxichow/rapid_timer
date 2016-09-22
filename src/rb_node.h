#ifndef _RB_NODE_H_
#define _RB_NODE_H_

#include "stdlib.h"

#define	RB_RED		0
#define	RB_BLACK	1

typedef struct rb_node {
    struct rb_node *rb_parent;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
    void *entity;
    char parent_color:1;
    char color:1;
} rb_node;

typedef struct rb_root
{
    rb_node *child;
}rb_root;


#define rb_is_red(r)				!(r)->color
#define rb_is_black(r)				(r)->color
#define rb_set_red(r)  				((r)->color = RB_RED)
#define rb_set_black(r)  			((r)->color = RB_BLACK)

#define RB_EMPTY_ROOT(root)	((root)->child == NULL)
#define RB_EMPTY_NODE(node)	((node)->rb_parent == node)
#define RB_CLEAR_NODE(node)	((node)->rb_parent = node)

extern void rb_insert_color(rb_node *, rb_root *);
extern void rb_erase(rb_node *, rb_root *);

extern rb_node *rb_next(const rb_node *);
extern rb_node *rb_prev(const rb_node *);
extern rb_node *rb_first(const rb_root *);
extern rb_node *rb_last(const rb_root *);

extern void rb_replace_node(rb_node *victim, rb_node *new, rb_root *root);

static inline void rb_link_node(rb_node *node, rb_node *parent, 
                                               rb_node **rb_link) {

    node->rb_parent = parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

#endif	/* _RB_NODE_H_ */