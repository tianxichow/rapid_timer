#include "rb_node.h"

static void __rb_rotate_left(rb_node *node, rb_root *root) {
    
    rb_node *right = node->rb_right;
    rb_node *parent = node->rb_parent;

    if ((node->rb_right = right->rb_left)) {
        right->rb_left->rb_parent = node;
    }

    right->rb_left = node;
    right->rb_parent = parent;

    if (parent) {

        if (node == parent->rb_left) {
            parent->rb_left = right;
        } else {
            parent->rb_right = right;
        }	
    } else {
        root->child = right;
    }
        
    node->rb_parent = right;
    return;
}

static void __rb_rotate_right(rb_node *node, rb_root *root) {

    rb_node *left = node->rb_left;
    rb_node *parent = node->rb_parent;

    if ((node->rb_left = left->rb_right)) {
        left->rb_right->rb_parent = node;
    }

    left->rb_right = node;
    left->rb_parent = parent;

    if (parent) {

        if (node == parent->rb_right) {
            parent->rb_right = left;
        } else {
            parent->rb_left = left;
        }	
    } else {
        root->child = left;
    }
        
    node->rb_parent = left;

    return;
}

void rb_insert_color(rb_node *node, rb_root *root) {

    rb_node *parent;
    rb_node *gparent;

    while ((parent = node->rb_parent) && rb_is_red(parent)) {
        // node and parent is red
        gparent = parent->rb_parent;

        if (parent == gparent->rb_left) {
            {
                register rb_node *uncle = gparent->rb_right;
                
                if (uncle && rb_is_red(uncle)) {

                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_right == node) {

                register rb_node *tmp;
                __rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_right(gparent, root);

        } else {
            {
                register rb_node *uncle = gparent->rb_left;
                
                if (uncle && rb_is_red(uncle)) {

                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_left == node) {

                register rb_node *tmp;
                __rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_left(gparent, root);
        }
    }

    rb_set_black(root->child);

    return;
}

static void __rb_erase_color(rb_node *node, rb_node *parent, rb_root *root) {
    
    rb_node *other;

    while ((!node || rb_is_black(node)) && node != root->child) {

        if (parent->rb_left == node) {

            other = parent->rb_right;
            
            if (rb_is_red(other)) {

                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_left(parent, root);
                other = parent->rb_right;
            }

            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right))) {

                rb_set_red(other);
                node = parent;
                parent = node->rb_parent;

            } else {

                if (!other->rb_right || rb_is_black(other->rb_right)) {
                    
                    rb_set_black(other->rb_left);
                    rb_set_red(other);
                    __rb_rotate_right(other, root);
                    other = parent->rb_right;
                }
                
                other->color = parent->color;
                rb_set_black(parent);
                rb_set_black(other->rb_right);
                __rb_rotate_left(parent, root);
                node = root->child;
                break;
            }
        } else {

            other = parent->rb_left;
            
            if (rb_is_red(other)) {

                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_right(parent, root);
                other = parent->rb_left;
            }

            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right))) {

                rb_set_red(other);
                node = parent;
                parent = node->rb_parent;
            } else {
                
                if (!other->rb_left || rb_is_black(other->rb_left)) {

                    rb_set_black(other->rb_right);
                    rb_set_red(other);
                    __rb_rotate_left(other, root);
                    other = parent->rb_left;
                }

                other->color = parent->color;
                rb_set_black(parent);
                rb_set_black(other->rb_left);
                __rb_rotate_right(parent, root);
                node = root->child;
                break;
            }
        }
    }

    if (node) {
        rb_set_black(node);
    }

    return;	
}

void rb_erase(rb_node *node, rb_root *root) {

    rb_node *child, *parent;
    int color;

    if (!node->rb_left) {
        child = node->rb_right;
    } else if (!node->rb_right) {
        child = node->rb_left;
    } else {

        rb_node *old = node, *left;
        node = node->rb_right;

        while ((left = node->rb_left) != NULL) {
            node = left;
        }
            
        if (old->rb_parent) {

            if (old->rb_parent->rb_left == old) {
                old->rb_parent->rb_left = node;
            } else {
                old->rb_parent->rb_right = node;
            }		
        } else {
            root->child = node;
        }
            
        child = node->rb_right;
        parent = node->rb_parent;
        color = node->color;

        if (parent == old) {
            parent = node;
        } else {

            if (child) {
                child->rb_parent = parent;
            }
                
            parent->rb_left = child;

            node->rb_right = old->rb_right;
            old->rb_right->rb_parent = node;
        }

        node->parent_color = old->parent_color;
        node->rb_left = old->rb_left;
        old->rb_left->rb_parent = node;

        if (color == RB_BLACK) {
            __rb_erase_color(child, parent, root);
        }

        return;
    }

    parent = node->rb_parent;
    color = node->color;

    if (child) {
        child->rb_parent = parent;
    }

    if (parent) {

        if (parent->rb_left == node) {
            parent->rb_left = child;
        } else {
            parent->rb_right = child;
        }		
    } else {
        root->child = child;
    }
        
    if (color == RB_BLACK) {
        __rb_erase_color(child, parent, root);
    }

    return;	
}

rb_node *rb_first(const rb_root *root) {
    
    rb_node	*n;
    n = root->child;

    if (!n) {
        return NULL;
    }
        
    while (n->rb_left) {
        n = n->rb_left;
    }
        
    return n;
}

rb_node *rb_last(const rb_root *root) {

    rb_node	*n;

    n = root->child;

    if (!n) {
        return NULL;
    }
        
    while (n->rb_right) {
        n = n->rb_right;
    }
        
    return n;
}

rb_node *rb_next(const rb_node *node) {

    rb_node *parent;

    if (node->rb_parent == node)
        return NULL;

    if (node->rb_right) {

        node = node->rb_right; 

        while (node->rb_left) {
            node=node->rb_left;
        }
            
        return (rb_node *)node;
    }

    while ((parent = node->rb_parent) && node == parent->rb_right) {
        node = parent;
    }
        
    return parent;
}

rb_node *rb_prev(const rb_node *node)
{
    rb_node *parent;

    if (node->rb_parent == node) {
        return NULL;
    }
 
    if (node->rb_left) {

        node = node->rb_left; 

        while (node->rb_right) {
            node=node->rb_right;
        }
            
        return (rb_node *)node;
    }

    while ((parent = node->rb_parent) && node == parent->rb_left) {
        node = parent;
    }
        
    return parent;
}

void rb_replace_node(rb_node *victim, rb_node *new, rb_root *root) {

    rb_node *parent = victim->rb_parent;

    if (parent) {
        if (victim == parent->rb_left) {
            parent->rb_left = new;
        } else {
            parent->rb_right = new;
        }		
    } else {
        root->child = new;
    }

    if (victim->rb_left) {
        victim->rb_left->rb_parent = new;
    }
        
    if (victim->rb_right) {
        victim->rb_right->rb_parent = new;
    }

    *new = *victim;

    return;
}