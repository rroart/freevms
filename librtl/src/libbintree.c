// $Id$
// $Locker$

// Author. Roar Thronæs.

// This is mainly a modified libavl

#include <descrip.h>
#include <libdef.h>
#include <ssdef.h>
#include <starlet.h>

/*****************************************************************************

    avl.h - Source code for the AVL-tree library.

    Copyright (C) 1998  Michael H. Buselli <cosine@cosine.org>
    Copyright (C) 2000-2002  Wessel Dankers <wsl@nl.linux.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

    Augmented AVL-tree. Original by Michael H. Buselli <cosine@cosine.org>.

    Modified by Wessel Dankers <wsl@nl.linux.org> to add a bunch of bloat to
    the sourcecode, change the interface and squash a few bugs.
    Mail him if you find new bugs.

*****************************************************************************/

#ifndef _AVL_H
#define _AVL_H

#define AVL_DEPTH

/* We need either depths, counts or both (the latter being the default) */
#if !defined(AVL_DEPTH) && !defined(AVL_COUNT)
#define AVL_DEPTH
#define AVL_COUNT
#endif

/* User supplied function to compare two items like strcmp() does.
 * For example: cmp(a,b) will return:
 *   -1  if a < b
 *    0  if a = b
 *    1  if a > b
 */
typedef int (*avl_compare_t)(const void *, const void *);

/* User supplied function to delete an item when a node is free()d.
 * If NULL, the item is not free()d.
 */
typedef void (*avl_freeitem_t)(void *);

typedef struct avl_node_t
{
#ifdef DO_REST
    struct avl_node_t *next;
    struct avl_node_t *prev;
    struct avl_node_t *parent;
#endif
    struct avl_node_t *left;
    struct avl_node_t *right;
#ifdef AVL_COUNT
    unsigned int count;
#endif
#ifdef AVL_DEPTH
    unsigned short depth;
#endif
    void *item;
} avl_node_t;

#ifdef DO_REST
typedef struct avl_tree_t
{
    avl_node_t *head;
    avl_node_t *tail;
    avl_node_t *top;
    avl_compare_t cmp;
    avl_freeitem_t freeitem;
} avl_tree_t;
#endif

/* Initializes a new tree for elements that will be ordered using
 * the supplied strcmp()-like function.
 * Returns the value of avltree (even if it's NULL).
 * O(1) */
#ifdef DO_REST
extern avl_tree_t *avl_init_tree(avl_tree_t *avltree, avl_compare_t, avl_freeitem_t);

/* Allocates and initializes a new tree for elements that will be
 * ordered using the supplied strcmp()-like function.
 * Returns NULL if memory could not be allocated.
 * O(1) */
extern avl_tree_t *avl_alloc_tree(avl_compare_t, avl_freeitem_t);

/* Frees the entire tree efficiently. Nodes will be free()d.
 * If the tree's freeitem is not NULL it will be invoked on every item.
 * O(n) */
extern void avl_free_tree(avl_tree_t *);

/* Reinitializes the tree structure for reuse. Nothing is free()d.
 * Compare and freeitem functions are left alone.
 * O(1) */
extern void avl_clear_tree(avl_tree_t *);

/* Free()s all nodes in the tree but leaves the tree itself.
 * If the tree's freeitem is not NULL it will be invoked on every item.
 * O(n) */
extern void avl_free_nodes(avl_tree_t *);

/* Initializes memory for use as a node. Returns NULL if avlnode is NULL.
 * O(1) */
extern avl_node_t *avl_init_node(avl_node_t *avlnode, void *item);

/* Insert an item into the tree and return the new node.
 * Returns NULL and sets errno if memory for the new node could not be
 * allocated or if the node is already in the tree (EEXIST).
 * O(lg n) */
extern avl_node_t *avl_insert(avl_tree_t *, void *item);

/* Insert a node into the tree and return it.
 * Returns NULL if the node is already in the tree.
 * O(lg n) */
extern avl_node_t *avl_insert_node(avl_tree_t *, avl_node_t *);

/* Insert a node in an empty tree. If avlnode is NULL, the tree will be
 * cleared and ready for re-use.
 * If the tree is not empty, the old nodes are left dangling.
 * O(1) */
extern avl_node_t *avl_insert_top(avl_tree_t *, avl_node_t *avlnode);

/* Insert a node before another node. Returns the new node.
 * If old is NULL, the item is appended to the tree.
 * O(lg n) */
extern avl_node_t *avl_insert_before(avl_tree_t *, avl_node_t *old, avl_node_t *new);

/* Insert a node after another node. Returns the new node.
 * If old is NULL, the item is prepended to the tree.
 * O(lg n) */
extern avl_node_t *avl_insert_after(avl_tree_t *, avl_node_t *old, avl_node_t *new);

/* Deletes a node from the tree. Returns immediately if the node is NULL.
 * The item will not be free()d regardless of the tree's freeitem handler.
 * This function comes in handy if you need to update the search key.
 * O(lg n) */
extern void avl_unlink_node(avl_tree_t *, avl_node_t *);

/* Deletes a node from the tree. Returns immediately if the node is NULL.
 * If the tree's freeitem is not NULL, it is invoked on the item.
 * If it is, returns the item.
 * O(lg n) */
extern void *avl_delete_node(avl_tree_t *, avl_node_t *);

/* Searches for an item in the tree and deletes it if found.
 * If the tree's freeitem is not NULL, it is invoked on the item.
 * If it is, returns the item.
 * O(lg n) */
extern void *avl_delete(avl_tree_t *, const void *item);

/* If exactly one node is moved in memory, this will fix the pointers
 * in the tree that refer to it. It must be an exact shallow copy.
 * Returns the pointer to the old position.
 * O(1) */
extern avl_node_t *avl_fixup_node(avl_tree_t *, avl_node_t *new);

/* Searches for a node with the key closest (or equal) to the given item.
 * If avlnode is not NULL, *avlnode will be set to the node found or NULL
 * if the tree is empty. Return values:
 *   -1  if the returned node is smaller
 *    0  if the returned node is equal or if the tree is empty
 *    1  if the returned node is greater
 * O(lg n) */
extern int avl_search_closest(const avl_tree_t *, const void *item, avl_node_t **avlnode, avl_node_t **parent, void ** user_data_address);

/* Searches for the item in the tree and returns a matching node if found
 * or NULL if not.
 * O(lg n) */
extern avl_node_t *avl_search(const avl_tree_t *, const void *item);

#ifdef AVL_COUNT
/* Returns the number of nodes in the tree.
 * O(1) */
extern unsigned int avl_count(const avl_tree_t *);

/* Searches a node by its rank in the list. Counting starts at 0.
 * Returns NULL if the index exceeds the number of nodes in the tree.
 * O(lg n) */
extern avl_node_t *avl_at(const avl_tree_t *, unsigned int);

/* Returns the rank of a node in the list. Counting starts at 0.
 * O(lg n) */
extern unsigned int avl_index(const avl_node_t *);
#endif
#endif

#endif

/*****************************************************************************

    avl.c - Source code for the AVL-tree library.

    Copyright (C) 1998  Michael H. Buselli <cosine@cosine.org>
    Copyright (C) 2000-2002  Wessel Dankers <wsl@nl.linux.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

    Augmented AVL-tree. Original by Michael H. Buselli <cosine@cosine.org>.

    Modified by Wessel Dankers <wsl@nl.linux.org> to add a bunch of bloat to
    the sourcecode, change the interface and squash a few bugs.
    Mail him if you find new bugs.

*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#if 0
#include "avl.h"
#endif

static void avl_rebalance(void **, avl_node_t *, avl_node_t *);

#ifdef AVL_COUNT
#define NODE_COUNT(n)  ((n) ? (n)->count : 0)
#define L_COUNT(n)     (NODE_COUNT((n)->left))
#define R_COUNT(n)     (NODE_COUNT((n)->right))
#define CALC_COUNT(n)  (L_COUNT(n) + R_COUNT(n) + 1)
#endif

#ifdef AVL_DEPTH
#define NODE_DEPTH(n)  ((n) ? (n)->depth : 0)
#define L_DEPTH(n)     (NODE_DEPTH((n)->left))
#define R_DEPTH(n)     (NODE_DEPTH((n)->right))
#define CALC_DEPTH(n)  ((L_DEPTH(n)>R_DEPTH(n)?L_DEPTH(n):R_DEPTH(n)) + 1)
#endif

#ifndef AVL_DEPTH
/* Also known as ffs() (from BSD) */
static int lg(unsigned int u)
{
    int r = 1;
    if(!u) return 0;
    if(u & 0xffff0000)
    {
        u >>= 16;
        r += 16;
    }
    if(u & 0x0000ff00)
    {
        u >>= 8;
        r += 8;
    }
    if(u & 0x000000f0)
    {
        u >>= 4;
        r += 4;
    }
    if(u & 0x0000000c)
    {
        u >>= 2;
        r += 2;
    }
    if(u & 0x00000002) r++;
    return r;
}
#endif

static int avl_check_balance(avl_node_t *avlnode)
{
#ifdef AVL_DEPTH
    int d;
    d = R_DEPTH(avlnode) - L_DEPTH(avlnode);
    return d<-1?-1:d>1?1:0;
#else
    /*	int d;
     *	d = lg(R_COUNT(avlnode)) - lg(L_COUNT(avlnode));
     *	d = d<-1?-1:d>1?1:0;
     */
#ifdef AVL_COUNT
    int pl, r;

    pl = lg(L_COUNT(avlnode));
    r = R_COUNT(avlnode);

    if(r>>pl+1)
        return 1;
    if(pl<2 || r>>pl-2)
        return 0;
    return -1;
#else
#error No balancing possible.
#endif
#endif
}

#ifdef AVL_COUNT
unsigned int avl_count(const avl_tree_t *avltree)
{
    return NODE_COUNT(avltree->top);
}

avl_node_t *avl_at(const avl_tree_t *avltree, unsigned int index)
{
    avl_node_t *avlnode;
    unsigned int c;

    avlnode = avltree->top;

    while(avlnode)
    {
        c = L_COUNT(avlnode);

        if(index < c)
        {
            avlnode = avlnode->left;
        }
        else if(index > c)
        {
            avlnode = avlnode->right;
            index -= c+1;
        }
        else
        {
            return avlnode;
        }
    }
    return NULL;
}

unsigned int avl_index(const avl_node_t *avlnode)
{
    avl_node_t *next;
    unsigned int c;

    c = L_COUNT(avlnode);

    while((next = avlnode->parent))
    {
        if(avlnode == next->right)
            c += L_COUNT(next) + 1;
        avlnode = next;
    }

    return c;
}
#endif

int avl_search_closest(void **treehead, int (*user_compare_routine)(), const void *item, avl_node_t **avlnode, avl_node_t **parent, void **user_data_address)
{
    avl_node_t *node;
    int c;

    if(!avlnode)
        avlnode = &node;

    node = *treehead;
    *parent = 0;

    if(!node)
        return *avlnode = NULL, 0;

    for(;;)
    {
        c = user_compare_routine(item, node, user_data_address);

        if(c < 0)
        {
            if(node->left)
                *parent = node,
                 node = node->left;
            else
                return *avlnode = node, -1;
        }
        else if(c > 0)
        {
            if(node->right)
                *parent = node,
                 node = node->right;
            else
                return *avlnode = node, 1;
        }
        else
        {
            return *avlnode = node, 0;
        }
    }
}

#ifdef DO_REST
/*
 * avl_search:
 * Return a pointer to a node with the given item in the tree.
 * If no such item is in the tree, then NULL is returned.
 */
avl_node_t *avl_search(const avl_tree_t *avltree, const void *item)
{
    avl_node_t *node;
    return avl_search_closest(avltree, item, &node) ? NULL : node;
}

avl_tree_t *avl_init_tree(avl_tree_t *rc, avl_compare_t cmp, avl_freeitem_t freeitem)
{
    if(rc)
    {
        rc->head = NULL;
        rc->tail = NULL;
        rc->top = NULL;
        rc->cmp = cmp;
        rc->freeitem = freeitem;
    }
    return rc;
}

avl_tree_t *avl_alloc_tree(avl_compare_t cmp, avl_freeitem_t freeitem)
{
    return avl_init_tree(malloc(sizeof(avl_tree_t)), cmp, freeitem);
}

void avl_clear_tree(avl_tree_t *avltree)
{
    avltree->top = avltree->head = avltree->tail = NULL;
}

void avl_free_nodes(avl_tree_t *avltree)
{
    avl_node_t *node, *next;
    avl_freeitem_t freeitem;

    freeitem = avltree->freeitem;

    for(node = avltree->head; node; node = next)
    {
        next = node->next;
        if(freeitem)
            freeitem(node->item);
        free(node);
    }

    return avl_clear_tree(avltree);
}

/*
 * avl_free_tree:
 * Free all memory used by this tree.  If freeitem is not NULL, then
 * it is assumed to be a destructor for the items referenced in the avl_
 * tree, and they are deleted as well.
 */
void avl_free_tree(avl_tree_t *avltree)
{
    avl_free_nodes(avltree);
    free(avltree);
}
#endif

static void avl_clear_node(avl_node_t *newnode)
{
    newnode->left = newnode->right = NULL;
#ifdef AVL_COUNT
    newnode->count = 1;
#endif
#ifdef AVL_DEPTH
    newnode->depth = 1;
#endif
}

#ifdef DO_REST
avl_node_t *avl_init_node(avl_node_t *newnode, void *item)
{
    if(newnode)
    {
        /*		avl_clear_node(newnode); */
        newnode->item = item;
    }
    return newnode;
}
#endif

avl_node_t *avl_insert_top(void **treehead, avl_node_t *newnode)
{
    *treehead = newnode;
    newnode->left = newnode->right = newnode->depth = 0;
    return LIB$_NORMAL;
    return newnode;
}

avl_node_t *avl_insert_before(void ** treehead, avl_node_t *node, avl_node_t *parent, avl_node_t *newnode)
{
#if 0
    if(!node)
        return avltree->tail
               ? avl_insert_after(treehead, avltree->tail, newnode)
               : avl_insert_top(treehead, newnode);
#endif

#if 0
    if(node->left)
        return avl_insert_after(treehead, node->prev, newnode);
#endif

#if 0
    avl_clear_node(newnode);

    newnode->next = node;
    newnode->parent = node;

    newnode->prev = node->prev;
    if(node->prev)
        node->prev->next = newnode;
    else
        avltree->head = newnode;
    node->prev = newnode;
#endif

    node->left = newnode;
    avl_rebalance(treehead, node, parent);
    return LIB$_NORMAL;
    return newnode;
}

avl_node_t *avl_insert_after(void **treehead, avl_node_t *node, avl_node_t *parent, avl_node_t *newnode)
{
#if 0
    if(!node)
        return avltree->head
               ? avl_insert_before(treehead, avltree->head, newnode)
               : avl_insert_top(treehead, newnode);
#endif

#if 0
    if(node->right)
        return avl_insert_before(treehead, node->next, newnode);
#endif

#if 0
    avl_clear_node(newnode);

    newnode->prev = node;
    newnode->parent = node;

    newnode->next = node->next;
    if(node->next)
        node->next->prev = newnode;
    else
        avltree->tail = newnode;
    node->next = newnode;
#endif

    node->right = newnode;
    avl_rebalance(treehead, node, parent);
    return LIB$_NORMAL;
    return newnode;
}

static int avl_insert_node(void ** treehead, unsigned int * flags, int (*user_compare_routine)(), void ** new_node, void ** user_data_address)
{
//avl_tree_t *avltree, avl_node_t *newnode)
    avl_node_t *node;
    avl_node_t *parent;

    if(*treehead == 0)
        return avl_insert_top(treehead, new_node);

    switch(avl_search_closest(treehead, user_compare_routine, &((avl_node_t *)new_node)->item, &node, &parent, user_data_address))
    {
    case -1:
        return avl_insert_before(treehead, node, parent, new_node);
    case 1:
        return avl_insert_after(treehead, node, parent, new_node);
    }

    return LIB$_KEYALRINS;
    return LIB$_NORMAL;
    return NULL;
}

#ifdef DO_REST
/*
 * avl_insert:
 * Create a new node and insert an item there.
 * Returns the new node on success or NULL if no memory could be allocated.
 */
avl_node_t *avl_insert(avl_tree_t *avltree, void *item)
{
    avl_node_t *newnode;
    newnode = avl_init_node(malloc(sizeof(avl_node_t)), item);
    if(newnode)
    {
        if(avl_insert_node(avltree, newnode))
            return newnode;
        free(newnode);
        errno = EEXIST;
    }
    return NULL;
}

/*
 * avl_unlink_node:
 * Removes the given node.  Does not delete the item at that node.
 * The item of the node may be freed before calling avl_unlink_node.
 * (In other words, it is not referenced by this function.)
 */
void avl_unlink_node(avl_tree_t *avltree, avl_node_t *avlnode)
{
    avl_node_t *parent;
    avl_node_t **superparent;
    avl_node_t *subst, *left, *right;
    avl_node_t *balnode;

    if(avlnode->prev)
        avlnode->prev->next = avlnode->next;
    else
        avltree->head = avlnode->next;

    if(avlnode->next)
        avlnode->next->prev = avlnode->prev;
    else
        avltree->tail = avlnode->prev;

    parent = avlnode->parent;

    superparent = parent
                  ? avlnode == parent->left ? &parent->left : &parent->right
              : &avltree->top;

    left = avlnode->left;
    right = avlnode->right;
    if(!left)
    {
        *superparent = right;
        if(right)
            right->parent = parent;
        balnode = parent;
    }
    else if(!right)
    {
        *superparent = left;
        left->parent = parent;
        balnode = parent;
    }
    else
    {
        subst = avlnode->prev;
        if(subst == left)
        {
            balnode = subst;
        }
        else
        {
            balnode = subst->parent;
            balnode->right = subst->left;
            if(balnode->right)
                balnode->right->parent = balnode;
            subst->left = left;
            left->parent = subst;
        }
        subst->right = right;
        subst->parent = parent;
        right->parent = subst;
        *superparent = subst;
    }

    avl_rebalance(avltree, balnode);
}

void *avl_delete_node(avl_tree_t *avltree, avl_node_t *avlnode)
{
    void *item = NULL;
    if(avlnode)
    {
        item = avlnode->item;
        avl_unlink_node(avltree, avlnode);
        if(avltree->freeitem)
            avltree->freeitem(item);
        free(avlnode);
    }
    return item;
}

void *avl_delete(avl_tree_t *avltree, const void *item)
{
    return avl_delete_node(avltree, avl_search(avltree, item));
}

avl_node_t *avl_fixup_node(avl_tree_t *avltree, avl_node_t *newnode)
{
    avl_node_t *oldnode = NULL, *node;

    if(!avltree || !newnode)
        return NULL;

    node = newnode->prev;
    if(node)
    {
        oldnode = node->next;
        node->next = newnode;
    }
    else
    {
        avltree->head = newnode;
    }

    node = newnode->next;
    if(node)
    {
        oldnode = node->prev;
        node->prev = newnode;
    }
    else
    {
        avltree->tail = newnode;
    }

    node = newnode->parent;
    if(node)
    {
        if(node->left == oldnode)
            node->left = newnode;
        else
            node->right = newnode;
    }
    else
    {
        oldnode = avltree->top;
        avltree->top = newnode;
    }

    return oldnode;
}
#endif

// TODO: mix together rebalance, search and insert?
/*
 * avl_rebalance:
 * Rebalances the tree if one side becomes too heavy.  This function
 * assumes that both subtrees are AVL-trees with consistant data.  The
 * function has the additional side effect of recalculating the count of
 * the tree at this node.  It should be noted that at the return of this
 * function, if a rebalance takes place, the top of this subtree is no
 * longer going to be the same node.
 */
void avl_rebalance_not(void **treehead, avl_node_t *avlnode, avl_node_t *orig_parent)
{
    avl_node_t *child;
    avl_node_t *gchild;
    avl_node_t *parent;
    avl_node_t **superparent;

    parent = avlnode;

    while(avlnode)
    {
        parent = orig_parent;

        superparent = parent
                      ? avlnode == parent->left ? &parent->left : &parent->right
              : treehead;

        switch(avl_check_balance(avlnode))
        {
        case -1:
            child = avlnode->left;
#ifdef AVL_DEPTH
            if(L_DEPTH(child) >= R_DEPTH(child))
            {
#else
#ifdef AVL_COUNT
            if(L_COUNT(child) >= R_COUNT(child))
            {
#else
#error No balancing possible.
#endif
#endif
                avlnode->left = child->right;
#if 0
                if(avlnode->left)
                    avlnode->left->parent = avlnode;
#endif
                child->right = avlnode;
#if 0
                avlnode->parent = child;
#endif
                *superparent = child;
#if 0
                child->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
#endif
            }
            else
            {
                gchild = child->right;
                avlnode->left = gchild->right;
#if 0
                if(avlnode->left)
                    avlnode->left->parent = avlnode;
#endif
                child->right = gchild->left;
#if 0
                if(child->right)
                    child->right->parent = child;
#endif
                gchild->right = avlnode;
#if 0
                if(gchild->right)
                    gchild->right->parent = gchild;
#endif
                gchild->left = child;
#if 0
                if(gchild->left)
                    gchild->left->parent = gchild;
#endif
                *superparent = gchild;
#if 0
                gchild->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
                gchild->count = CALC_COUNT(gchild);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
                gchild->depth = CALC_DEPTH(gchild);
#endif
            }
            break;
        case 1:
            child = avlnode->right;
#ifdef AVL_DEPTH
            if(R_DEPTH(child) >= L_DEPTH(child))
            {
#else
#ifdef AVL_COUNT
            if(R_COUNT(child) >= L_COUNT(child))
            {
#else
#error No balancing possible.
#endif
#endif
                avlnode->right = child->left;
#if 0
                if(avlnode->right)
                    avlnode->right->parent = avlnode;
#endif
                child->left = avlnode;
#if 0
                avlnode->parent = child;
#endif
                *superparent = child;
#if 0
                child->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
#endif
            }
            else
            {
                gchild = child->left;
                avlnode->right = gchild->left;
#if 0
                if(avlnode->right)
                    avlnode->right->parent = avlnode;
#endif
                child->left = gchild->right;
#if 0
                if(child->left)
                    child->left->parent = child;
#endif
                gchild->left = avlnode;
#if 0
                if(gchild->left)
                    gchild->left->parent = gchild;
#endif
                gchild->right = child;
#if 0
                if(gchild->right)
                    gchild->right->parent = gchild;
#endif
                *superparent = gchild;
#if 0
                gchild->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
                gchild->count = CALC_COUNT(gchild);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
                gchild->depth = CALC_DEPTH(gchild);
#endif
            }
            break;
        default:
#ifdef AVL_COUNT
            avlnode->count = CALC_COUNT(avlnode);
#endif
#ifdef AVL_DEPTH
            avlnode->depth = CALC_DEPTH(avlnode);
#endif
        }
        avlnode = parent;
    }
}

void avl_rebalance(void **treehead, avl_node_t *avlnode, avl_node_t *orig_parent)
{
    avl_node_t *child;
    avl_node_t *gchild;
    avl_node_t *parent;
    avl_node_t **superparent;

    parent = avlnode;

    if (avlnode->left)
        avl_rebalance(treehead, avlnode->left, avlnode);

    if (avlnode->right)
        avl_rebalance(treehead, avlnode->right, avlnode);

    {
        parent = orig_parent;

        superparent = parent
                      ? avlnode == parent->left ? &parent->left : &parent->right
              : treehead;

        switch(avl_check_balance(avlnode))
        {
        case -1:
            child = avlnode->left;
#ifdef AVL_DEPTH
            if(L_DEPTH(child) >= R_DEPTH(child))
            {
#else
#ifdef AVL_COUNT
            if(L_COUNT(child) >= R_COUNT(child))
            {
#else
#error No balancing possible.
#endif
#endif
                avlnode->left = child->right;
#if 0
                if(avlnode->left)
                    avlnode->left->parent = avlnode;
#endif
                child->right = avlnode;
#if 0
                avlnode->parent = child;
#endif
                *superparent = child;
#if 0
                child->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
#endif
            }
            else
            {
                gchild = child->right;
                avlnode->left = gchild->right;
#if 0
                if(avlnode->left)
                    avlnode->left->parent = avlnode;
#endif
                child->right = gchild->left;
#if 0
                if(child->right)
                    child->right->parent = child;
#endif
                gchild->right = avlnode;
#if 0
                if(gchild->right)
                    gchild->right->parent = gchild;
#endif
                gchild->left = child;
#if 0
                if(gchild->left)
                    gchild->left->parent = gchild;
#endif
                *superparent = gchild;
#if 0
                gchild->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
                gchild->count = CALC_COUNT(gchild);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
                gchild->depth = CALC_DEPTH(gchild);
#endif
            }
            break;
        case 1:
            child = avlnode->right;
#ifdef AVL_DEPTH
            if(R_DEPTH(child) >= L_DEPTH(child))
            {
#else
#ifdef AVL_COUNT
            if(R_COUNT(child) >= L_COUNT(child))
            {
#else
#error No balancing possible.
#endif
#endif
                avlnode->right = child->left;
#if 0
                if(avlnode->right)
                    avlnode->right->parent = avlnode;
#endif
                child->left = avlnode;
#if 0
                avlnode->parent = child;
#endif
                *superparent = child;
#if 0
                child->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
#endif
            }
            else
            {
                gchild = child->left;
                avlnode->right = gchild->left;
#if 0
                if(avlnode->right)
                    avlnode->right->parent = avlnode;
#endif
                child->left = gchild->right;
#if 0
                if(child->left)
                    child->left->parent = child;
#endif
                gchild->left = avlnode;
#if 0
                if(gchild->left)
                    gchild->left->parent = gchild;
#endif
                gchild->right = child;
#if 0
                if(gchild->right)
                    gchild->right->parent = gchild;
#endif
                *superparent = gchild;
#if 0
                gchild->parent = parent;
#endif
#ifdef AVL_COUNT
                avlnode->count = CALC_COUNT(avlnode);
                child->count = CALC_COUNT(child);
                gchild->count = CALC_COUNT(gchild);
#endif
#ifdef AVL_DEPTH
                avlnode->depth = CALC_DEPTH(avlnode);
                child->depth = CALC_DEPTH(child);
                gchild->depth = CALC_DEPTH(gchild);
#endif
            }
            break;
        default:
#ifdef AVL_COUNT
            avlnode->count = CALC_COUNT(avlnode);
#endif
#ifdef AVL_DEPTH
            avlnode->depth = CALC_DEPTH(avlnode);
#endif
        }
        avlnode = parent;
    }
}

// my own part

static int traverse_tree (long * treehead , int (*user_action_procedure)(), void * user_data_address)
{
    int sts;
    if (treehead == 0)
        return LIB$_NORMAL;
    sts = traverse_tree(treehead[0], user_action_procedure, user_data_address);
    if ((sts & 1) == 0)
        return sts;
    sts = user_action_procedure (treehead, user_data_address);
    if ((sts & 1) == 0)
        return sts;
    return traverse_tree(treehead[1], user_action_procedure, user_data_address);
}
// static long Alloc_node(struct Tree_element* Rec,
// struct Full_node** Ret_addr, void* Dummy)

// static long Compare_node_3(struct Tree_element* Rec, struct Full_node* Node,	void* Dummy)

int lib$insert_tree (void ** treehead, void * symbol, unsigned int * flags, int (*user_compare_routine)(), int (*user_allocation_procedure)(), void ** new_node, void ** user_data)
{
    int sts;
    sts = user_allocation_procedure(symbol, new_node, user_data);
    if ((sts & 1) == 0)
        return LIB$_INSVIRMEM;
    sts = avl_insert_node(treehead, flags, user_compare_routine, *new_node, user_data);
    return sts;
}

int lib$lookup_tree (void ** treehead, unsigned int symbol, int (*user_compare_routine)(), void ** new_node)
{
    avl_node_t *parent;
    if (avl_search_closest(treehead, user_compare_routine, symbol, new_node, &parent, 0))
        return LIB$_KEYNOTFOU;
    else
        return LIB$_NORMAL;
}

int lib$traverse_tree (void ** treehead , int (*user_action_procedure)(), void * user_data_address)
{
    return traverse_tree(*treehead, user_action_procedure, user_data_address);
}

